// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Misc/AssertionMacros.h"
#include "HAL/UnrealMemory.h"
#include "angelscript.h"
#include "OtterAngleScript.h"

#include <new>
#include <algorithm>
#include <as_datatype.h>
#include <as_typeinfo.h>

namespace
{
	static const asPWORD TARRAY_CACHE = 1001;
	static const asPWORD TARRAY_META = 1002;

	struct FScriptTArrayBuffer
	{
		uint32 MaxElements;
		uint32 NumElements;
		uint8  Data[1];
	};

	struct FScriptTArrayCache
	{
		asIScriptFunction* CmpFunc;
		asIScriptFunction* EqFunc;
		int                CmpFuncReturnCode;
		int                EqFuncReturnCode;
	};

	static void CleanupTypeInfoTArrayCache(asITypeInfo* TypeInfo)
	{
		FScriptTArrayCache* Cache = reinterpret_cast<FScriptTArrayCache*>(TypeInfo->GetUserData(TARRAY_CACHE));
		if (Cache)
		{
			Cache->~FScriptTArrayCache();
			FMemory::Free(Cache);
		}
	}


	struct TArrayMetaData
	{
		bool bIsValid : 1 = false;
		uint32 BytePerElement = 0;
		uint32 Alignment;
		asITypeInfo* TypeInfo;
	};

	static bool TArrayTemplateCallback(asITypeInfo* TypeInfo, bool& bDontGarbageCollect)
	{
		int TypeId = TypeInfo->GetSubTypeId();
		if (TypeId == asTYPEID_VOID)
			return false;

		if ((TypeId & asTYPEID_MASK_OBJECT) && !(TypeId & asTYPEID_OBJHANDLE))
		{
			asITypeInfo* SubType = TypeInfo->GetEngine()->GetTypeInfoById(TypeId);
			asQWORD Flags = SubType->GetFlags();
			// If the subtype is a value type without the POD flag, then it must have a default constructor so that the array can initialize new elements.
			if ((Flags & asOBJ_VALUE) && !(Flags & asOBJ_POD))
			{
				bool bFound = false;
				// Verify that there is a default constructor
				for (asUINT n = 0; n < SubType->GetBehaviourCount(); n++)
				{
					asEBehaviours Beh;
					asIScriptFunction* Func = SubType->GetBehaviourByIndex(n, &Beh);
					if (Beh != asBEHAVE_CONSTRUCT)
						continue;
					if (Func->GetParamCount() == 0)
					{
						bFound = true;
						break;
					}
				}
				if (!bFound)
				{
					// There is no default constructor
					// TODO: Should format the message to give the name of the subtype for better understanding
					TypeInfo->GetEngine()->WriteMessage("TArray", 0, 0, asMSGTYPE_ERROR, "The subtype has no default constructor");
					return false;
				}
			}
			else if (Flags & asOBJ_REF)
			{
				bool bFound = false;

				// If value assignment for ref type has been disabled then the array
				// can be created if the type has a default factory function
				if (!TypeInfo->GetEngine()->GetEngineProperty(asEP_DISALLOW_VALUE_ASSIGN_FOR_REF_TYPE))
				{
					for (asUINT n = 0; n < SubType->GetFactoryCount(); n++)
					{
						asIScriptFunction* Func = SubType->GetFactoryByIndex(n);
						if (Func->GetParamCount() == 0)
						{
							bFound = true;
							break;
						}
					}
				}
				if (!bFound)
				{
					TypeInfo->GetEngine()->WriteMessage("TArray", 0, 0, asMSGTYPE_ERROR, "The subtype has no default factory");
					return false;
				}
			}
			// If the object type is not garbage collected then the array also doesn't need to be
			if (!(Flags & asOBJ_GC))
				bDontGarbageCollect = true;
		}
		else if (!(TypeId & asTYPEID_OBJHANDLE))
		{
			// Arrays with primitives cannot form circular references,
			// thus there is no need to garbage collect them
			bDontGarbageCollect = true;
		}
		else
		{
			asITypeInfo* SubType = TypeInfo->GetEngine()->GetTypeInfoById(TypeId);
			asQWORD Flags = SubType->GetFlags();
			if (!(Flags & asOBJ_GC))
			{
				if (Flags & asOBJ_SCRIPT_OBJECT)
				{
					if (Flags & asOBJ_NOINHERIT)
						bDontGarbageCollect = true;
				}
				else
				{
					bDontGarbageCollect = true;
				}
			}
		}

		auto Metadata = (TArrayMetaData*)TypeInfo->GetUserData(TARRAY_META);
		if (!Metadata)
		{
			Metadata = new TArrayMetaData();
			Metadata->TypeInfo = TypeInfo->GetEngine()->GetTypeInfoById(TypeId);
			auto dt = asCDataType::CreateType(reinterpret_cast<asCTypeInfo*>(Metadata->TypeInfo), true);
			Metadata->BytePerElement = dt.GetSizeInMemoryBytes();
			TypeInfo->SetUserData(Metadata, TARRAY_META);
		}
		return true;
	}

	class FScriptTArray
	{
	public:
		// Default constructor: safe zero state for C++ ABI purposes.
		// AngelScript always calls a CONSTRUCT behavior before scripts use the object.
		FScriptTArray()
			: ObjType(nullptr), Buffer(nullptr), ElementSize(0), SubTypeId(0)
		{
		}

		FScriptTArray(asUINT Length, asITypeInfo* TypeInfo)
		{
			ObjType = TypeInfo;
			ObjType->AddRef();
			Buffer = nullptr;
			Precache();

			ElementSize = (SubTypeId & asTYPEID_MASK_OBJECT)
				? (int)sizeof(asPWORD)
				: ObjType->GetEngine()->GetSizeOfPrimitiveType(SubTypeId);

			if (!CheckMaxSize(Length))
				return;
			CreateBuffer(&Buffer, Length);
		}

		FScriptTArray(asITypeInfo* TypeInfo, void* ListBuf)
		{
			ObjType = TypeInfo;
			ObjType->AddRef();
			Buffer = nullptr;
			Precache();

			asIScriptEngine* Engine = TypeInfo->GetEngine();
			ElementSize = (SubTypeId & asTYPEID_MASK_OBJECT)
				? (int)sizeof(asPWORD)
				: Engine->GetSizeOfPrimitiveType(SubTypeId);

			asUINT Length = *(asUINT*)ListBuf;
			if (!CheckMaxSize(Length)) return;

			if ((TypeInfo->GetSubTypeId() & asTYPEID_MASK_OBJECT) == 0)
			{
				CreateBuffer(&Buffer, Length);
				if (Length > 0)
					FMemory::Memcpy(At(0), (asUINT*)ListBuf + 1, Length * ElementSize);
			}
			else if (TypeInfo->GetSubTypeId() & asTYPEID_OBJHANDLE)
			{
				CreateBuffer(&Buffer, Length);
				if (Length > 0)
					FMemory::Memcpy(At(0), (asUINT*)ListBuf + 1, Length * ElementSize);
				FMemory::Memset((asUINT*)ListBuf + 1, 0, Length * ElementSize);
			}
			else
			{
				CreateBuffer(&Buffer, Length);
				for (asUINT n = 0; n < Length; n++)
				{
					void* Obj = At(n);
					uint8* SrcObj = (uint8*)ListBuf + 4 + n * TypeInfo->GetSubType()->GetSize();
					Engine->AssignScriptObject(Obj, SrcObj, TypeInfo->GetSubType());
				}
			}
		}

		FScriptTArray(const FScriptTArray& Other)
		{
			ObjType = Other.ObjType;
			if (ObjType) ObjType->AddRef();
			Buffer = nullptr;
			ElementSize = Other.ElementSize;
			SubTypeId = Other.SubTypeId;
			if (ObjType)
			{
				CreateBuffer(&Buffer, 0);
				*this = Other;
			}
		}

		~FScriptTArray()
		{
			if (Buffer)
			{
				DeleteBuffer(Buffer);
				Buffer = nullptr;
			}
			if (ObjType)
			{
				ObjType->Release();
				ObjType = nullptr;
			}
		}

		asUINT GetSize() const
		{
			return Buffer ? Buffer->NumElements : 0;
		}

		bool IsEmpty() const
		{
			return GetSize() == 0;
		}

		void Reserve(asUINT MaxElems)
		{
			if (!Buffer || MaxElems <= Buffer->MaxElements)
				return;
			if (!CheckMaxSize(MaxElems))
				return;

			FScriptTArrayBuffer* NewBuffer = reinterpret_cast<FScriptTArrayBuffer*>(
				FMemory::Malloc(sizeof(FScriptTArrayBuffer) - 1 + ElementSize * MaxElems));
			if (NewBuffer)
			{
				NewBuffer->NumElements = Buffer->NumElements;
				NewBuffer->MaxElements = MaxElems;
				FMemory::Memcpy(NewBuffer->Data, Buffer->Data, Buffer->NumElements * ElementSize);
				FMemory::Free(Buffer);
				Buffer = NewBuffer;
			}
			else
			{
				if (asIScriptContext* Ctx = asGetActiveContext())
					Ctx->SetException("Out of memory");
			}
		}

		void Resize(asUINT NumElems)
		{
			if (!CheckMaxSize(NumElems))
				return;
			Resize((int)NumElems - (int)GetSize(), (asUINT)-1);
		}

		void* At(asUINT Index)
		{
			return const_cast<void*>(const_cast<const FScriptTArray*>(this)->At(Index));
		}

		const void* At(asUINT Index) const
		{
			if (!Buffer || Index >= Buffer->NumElements)
			{
				if (asIScriptContext* Ctx = asGetActiveContext())
					Ctx->SetException("TArray index out of bounds");
				return nullptr;
			}
			if ((SubTypeId & asTYPEID_MASK_OBJECT) && !(SubTypeId & asTYPEID_OBJHANDLE))
				return *(void**)(Buffer->Data + ElementSize * Index);
			return Buffer->Data + ElementSize * Index;
		}

		void SetValue(asUINT Index, void* Value)
		{
			void* Ptr = At(Index);
			if (!Ptr) return;

			if ((SubTypeId & ~asTYPEID_MASK_SEQNBR) && !(SubTypeId & asTYPEID_OBJHANDLE))
			{
				ObjType->GetEngine()->AssignScriptObject(Ptr, Value, ObjType->GetSubType());
			}
			else if (SubTypeId & asTYPEID_OBJHANDLE)
			{
				void* Tmp = *(void**)Ptr;
				*(void**)Ptr = *(void**)Value;
				ObjType->GetEngine()->AddRefScriptObject(*(void**)Value, ObjType->GetSubType());
				if (Tmp)
					ObjType->GetEngine()->ReleaseScriptObject(Tmp, ObjType->GetSubType());
			}
			else if (SubTypeId == asTYPEID_BOOL || SubTypeId == asTYPEID_INT8 || SubTypeId == asTYPEID_UINT8)
				*(char*)Ptr = *(char*)Value;
			else if (SubTypeId == asTYPEID_INT16 || SubTypeId == asTYPEID_UINT16)
				*(short*)Ptr = *(short*)Value;
			else if (SubTypeId == asTYPEID_INT32 || SubTypeId == asTYPEID_UINT32 ||
			         SubTypeId == asTYPEID_FLOAT || SubTypeId > asTYPEID_DOUBLE)
				*(int*)Ptr = *(int*)Value;
			else if (SubTypeId == asTYPEID_INT64 || SubTypeId == asTYPEID_UINT64 || SubTypeId == asTYPEID_DOUBLE)
				*(double*)Ptr = *(double*)Value;
		}

		FScriptTArray& operator=(const FScriptTArray& Other)
		{
			if (&Other != this && Other.GetArrayObjectType() == GetArrayObjectType())
			{
				Resize(Other.Buffer ? Other.Buffer->NumElements : 0);
				if (Buffer && Other.Buffer)
					CopyBuffer(Buffer, Other.Buffer);
			}
			return *this;
		}

		bool operator==(const FScriptTArray& Other) const
		{
			if (ObjType != Other.ObjType) return false;
			if (GetSize() != Other.GetSize()) return false;

			asIScriptContext* CmpCtx = nullptr;
			bool bIsNested = false;
			if (SubTypeId & ~asTYPEID_MASK_SEQNBR)
			{
				CmpCtx = asGetActiveContext();
				if (CmpCtx)
				{
					if (CmpCtx->GetEngine() == ObjType->GetEngine() && CmpCtx->PushState() >= 0)
						bIsNested = true;
					else
						CmpCtx = nullptr;
				}
				if (!CmpCtx)
					CmpCtx = ObjType->GetEngine()->CreateContext();
			}

			bool bIsEqual = true;
			FScriptTArrayCache* Cache = reinterpret_cast<FScriptTArrayCache*>(ObjType->GetUserData(TARRAY_CACHE));
			for (asUINT n = 0; n < GetSize(); n++)
			{
				if (!Equals(At(n), Other.At(n), CmpCtx, Cache))
				{
					bIsEqual = false;
					break;
				}
			}

			if (CmpCtx)
			{
				if (bIsNested)
				{
					asEContextState State = CmpCtx->GetState();
					CmpCtx->PopState();
					if (State == asEXECUTION_ABORTED)
						CmpCtx->Abort();
				}
				else
					CmpCtx->Release();
			}
			return bIsEqual;
		}

		void InsertAt(asUINT Index, void* Value)
		{
			if (Index > GetSize())
			{
				if (asIScriptContext* Ctx = asGetActiveContext())
					Ctx->SetException("TArray index out of bounds");
				return;
			}
			Resize(1, Index);
			SetValue(Index, Value);
		}

		void InsertAt(asUINT Index, const FScriptTArray& Arr)
		{
			if (Index > GetSize())
			{
				if (asIScriptContext* Ctx = asGetActiveContext())
					Ctx->SetException("TArray index out of bounds");
				return;
			}
			if (ObjType != Arr.ObjType)
			{
				if (asIScriptContext* Ctx = asGetActiveContext())
					Ctx->SetException("Mismatching TArray types");
				return;
			}
			asUINT Elements = Arr.GetSize();
			Resize((int)Elements, Index);
			if (&Arr != this)
			{
				for (asUINT n = 0; n < Elements; n++)
					SetValue(Index + n, const_cast<void*>(Arr.At(n)));
			}
		}

		void InsertLast(void* Value)
		{
			InsertAt(GetSize(), Value);
		}

		void RemoveAt(asUINT Index)
		{
			if (!Buffer || Index >= Buffer->NumElements)
			{
				if (asIScriptContext* Ctx = asGetActiveContext())
					Ctx->SetException("TArray index out of bounds");
				return;
			}
			Resize(-1, Index);
		}

		void RemoveLast()
		{
			if (Buffer && Buffer->NumElements > 0)
				RemoveAt(Buffer->NumElements - 1);
		}

		void RemoveRange(asUINT Start, asUINT Count)
		{
			if (Count == 0) return;
			if (!Buffer || Start > Buffer->NumElements)
			{
				if (asIScriptContext* Ctx = asGetActiveContext())
					Ctx->SetException("TArray index out of bounds");
				return;
			}
			if (Start + Count > Buffer->NumElements)
				Count = Buffer->NumElements - Start;
			DestructRange(Buffer, Start, Start + Count);
			FMemory::Memmove(
				Buffer->Data + Start * ElementSize,
				Buffer->Data + (Start + Count) * ElementSize,
				(Buffer->NumElements - Start - Count) * ElementSize);
			Buffer->NumElements -= Count;
		}

		void SortAsc()                          { Sort(0, GetSize(), true); }
		void SortAsc(asUINT Start, asUINT Cnt)  { Sort(Start, Cnt, true); }
		void SortDesc()                         { Sort(0, GetSize(), false); }
		void SortDesc(asUINT Start, asUINT Cnt) { Sort(Start, Cnt, false); }

		void Reverse()
		{
			asUINT Size = GetSize();
			if (Size >= 2)
			{
				uint8 Temp[16];
				for (asUINT i = 0; i < Size / 2; i++)
				{
					CopyElement(Temp, GetItemPtr(i));
					CopyElement(GetItemPtr(i), GetItemPtr(Size - i - 1));
					CopyElement(GetItemPtr(Size - i - 1), Temp);
				}
			}
		}

		int Find(const void* Value) const    { return Find(0, Value); }
		int FindByRef(const void* Ref) const { return FindByRef(0, Ref); }

		int Find(asUINT StartAt, const void* Value) const
		{
			FScriptTArrayCache* Cache = nullptr;
			if (SubTypeId & ~asTYPEID_MASK_SEQNBR)
			{
				Cache = reinterpret_cast<FScriptTArrayCache*>(ObjType->GetUserData(TARRAY_CACHE));
				if (!Cache || (!Cache->CmpFunc && !Cache->EqFunc))
				{
					if (asIScriptContext* Ctx = asGetActiveContext())
						Ctx->SetException("Type does not support find - no opEquals or opCmp method");
					return -1;
				}
			}

			asIScriptContext* CmpCtx = nullptr;
			bool bIsNested = false;
			if (SubTypeId & ~asTYPEID_MASK_SEQNBR)
			{
				CmpCtx = asGetActiveContext();
				if (CmpCtx)
				{
					if (CmpCtx->GetEngine() == ObjType->GetEngine() && CmpCtx->PushState() >= 0)
						bIsNested = true;
					else
						CmpCtx = nullptr;
				}
				if (!CmpCtx)
					CmpCtx = ObjType->GetEngine()->CreateContext();
			}

			int Ret = -1;
			asUINT Size = GetSize();
			for (asUINT i = StartAt; i < Size; i++)
			{
				if (Equals(At(i), Value, CmpCtx, Cache))
				{
					Ret = (int)i;
					break;
				}
			}

			if (CmpCtx)
			{
				if (bIsNested)
				{
					asEContextState State = CmpCtx->GetState();
					CmpCtx->PopState();
					if (State == asEXECUTION_ABORTED) CmpCtx->Abort();
				}
				else
					CmpCtx->Release();
			}
			return Ret;
		}

		int FindByRef(asUINT StartAt, const void* Ref) const
		{
			asUINT Size = GetSize();
			if (SubTypeId & asTYPEID_OBJHANDLE)
			{
				Ref = *(const void* const*)Ref;
				for (asUINT i = StartAt; i < Size; i++)
					if (*(const void* const*)At(i) == Ref) return (int)i;
			}
			else
			{
				for (asUINT i = StartAt; i < Size; i++)
					if (At(i) == Ref) return (int)i;
			}
			return -1;
		}

		void Sort(asIScriptFunction* Func, asUINT StartAt, asUINT Count)
		{
			if (Count < 2) return;

			asUINT End = (asQWORD(StartAt) + asQWORD(Count) >= (asQWORD(1) << 32))
				? 0xFFFFFFFF : StartAt + Count;
			if (!Buffer) return;
			if (End > Buffer->NumElements) End = Buffer->NumElements;

			if (StartAt >= Buffer->NumElements)
			{
				if (asIScriptContext* Ctx = asGetActiveContext())
					Ctx->SetException("TArray index out of bounds");
				return;
			}

			asIScriptContext* CmpCtx = asGetActiveContext();
			bool bIsNested = false;
			if (CmpCtx)
			{
				if (CmpCtx->GetEngine() == ObjType->GetEngine() && CmpCtx->PushState() >= 0)
					bIsNested = true;
				else
					CmpCtx = nullptr;
			}
			if (!CmpCtx) CmpCtx = ObjType->GetEngine()->RequestContext();

			for (asUINT i = StartAt; i + 1 < End; i++)
			{
				asUINT Best = i;
				for (asUINT j = i + 1; j < End; j++)
				{
					CmpCtx->Prepare(Func);
					CmpCtx->SetArgAddress(0, At(j));
					CmpCtx->SetArgAddress(1, At(Best));
					int r = CmpCtx->Execute();
					if (r != asEXECUTION_FINISHED) break;
					if (*(bool*)(CmpCtx->GetAddressOfReturnValue()))
						Best = j;
				}
				if (Best != i)
					SwapElements(GetItemPtr(i), GetItemPtr(Best));
			}

			if (CmpCtx)
			{
				if (bIsNested)
				{
					asEContextState State = CmpCtx->GetState();
					CmpCtx->PopState();
					if (State == asEXECUTION_ABORTED) CmpCtx->Abort();
				}
				else
					ObjType->GetEngine()->ReturnContext(CmpCtx);
			}
		}

		asITypeInfo* GetArrayObjectType() const { return ObjType; }

		// GC behaviors — value types with asOBJ_GC only need ENUMREFS and RELEASEREFS.
		void EnumReferences(asIScriptEngine* Engine)
		{
			if (!Buffer || !(SubTypeId & asTYPEID_MASK_OBJECT)) return;

			void** d = (void**)Buffer->Data;
			asITypeInfo* SubType = Engine->GetTypeInfoById(SubTypeId);
			if (SubType->GetFlags() & asOBJ_REF)
			{
				for (asUINT n = 0; n < Buffer->NumElements; n++)
					if (d[n]) Engine->GCEnumCallback(d[n]);
			}
			else if ((SubType->GetFlags() & asOBJ_VALUE) && (SubType->GetFlags() & asOBJ_GC))
			{
				for (asUINT n = 0; n < Buffer->NumElements; n++)
					if (d[n]) Engine->ForwardGCEnumReferences(d[n], SubType);
			}
		}

		void ReleaseAllHandles(asIScriptEngine*)
		{
			Resize(0);
		}

	private:
		asITypeInfo*         ObjType;
		FScriptTArrayBuffer* Buffer;
		int                  ElementSize;
		int                  SubTypeId;

		void Precache()
		{
			SubTypeId = ObjType->GetSubTypeId();
			if (!(SubTypeId & ~asTYPEID_MASK_SEQNBR)) return;

			FScriptTArrayCache* Cache = reinterpret_cast<FScriptTArrayCache*>(ObjType->GetUserData(TARRAY_CACHE));
			if (Cache) return;

			asAcquireExclusiveLock();

			Cache = reinterpret_cast<FScriptTArrayCache*>(ObjType->GetUserData(TARRAY_CACHE));
			if (!Cache)
			{
				Cache = reinterpret_cast<FScriptTArrayCache*>(FMemory::Malloc(sizeof(FScriptTArrayCache)));
				if (!Cache)
				{
					asReleaseExclusiveLock();
					return;
				}
				FMemory::Memset(Cache, 0, sizeof(FScriptTArrayCache));

				bool bMustBeConst = (SubTypeId & asTYPEID_HANDLETOCONST) ? true : false;
				asITypeInfo* SubType = ObjType->GetEngine()->GetTypeInfoById(SubTypeId);
				if (SubType)
				{
					for (asUINT i = 0; i < SubType->GetMethodCount(); i++)
					{
						asIScriptFunction* Func = SubType->GetMethodByIndex(i);
						if (Func->GetParamCount() != 1 || (bMustBeConst && !Func->IsReadOnly()))
							continue;

						asDWORD RetFlags = 0;
						int RetTypeId = Func->GetReturnTypeId(&RetFlags);
						if (RetFlags != asTM_NONE) continue;

						bool bIsCmp = (RetTypeId == asTYPEID_INT32) && (FCStringAnsi::Strcmp(Func->GetName(), "opCmp") == 0);
						bool bIsEq  = (RetTypeId == asTYPEID_BOOL)  && (FCStringAnsi::Strcmp(Func->GetName(), "opEquals") == 0);
						if (!bIsCmp && !bIsEq) continue;

						int ParamTypeId;
						asDWORD ParamFlags;
						Func->GetParam(0, &ParamTypeId, &ParamFlags);

						if ((ParamTypeId & ~(asTYPEID_OBJHANDLE | asTYPEID_HANDLETOCONST)) !=
						    (SubTypeId    & ~(asTYPEID_OBJHANDLE | asTYPEID_HANDLETOCONST)))
							continue;

						if (ParamFlags & asTM_INREF)
						{
							if ((ParamTypeId & asTYPEID_OBJHANDLE) || (bMustBeConst && !(ParamFlags & asTM_CONST)))
								continue;
						}
						else if (ParamTypeId & asTYPEID_OBJHANDLE)
						{
							if (bMustBeConst && !(ParamTypeId & asTYPEID_HANDLETOCONST))
								continue;
						}
						else
							continue;

						if (bIsCmp)
						{
							if (Cache->CmpFunc || Cache->CmpFuncReturnCode)
							{
								Cache->CmpFunc = nullptr;
								Cache->CmpFuncReturnCode = asMULTIPLE_FUNCTIONS;
							}
							else
								Cache->CmpFunc = Func;
						}
						else if (bIsEq)
						{
							if (Cache->EqFunc || Cache->EqFuncReturnCode)
							{
								Cache->EqFunc = nullptr;
								Cache->EqFuncReturnCode = asMULTIPLE_FUNCTIONS;
							}
							else
								Cache->EqFunc = Func;
						}
					}
				}

				if (!Cache->EqFunc  && Cache->EqFuncReturnCode  == 0) Cache->EqFuncReturnCode  = asNO_FUNCTION;
				if (!Cache->CmpFunc && Cache->CmpFuncReturnCode == 0) Cache->CmpFuncReturnCode = asNO_FUNCTION;

				ObjType->SetUserData(Cache, TARRAY_CACHE);
			}
			asReleaseExclusiveLock();
		}

		bool CheckMaxSize(asUINT NumElems)
		{
			asUINT MaxSize = 0xFFFFFFFFul - sizeof(FScriptTArrayBuffer) + 1;
			if (ElementSize > 0)
				MaxSize /= (asUINT)ElementSize;
			if (NumElems > MaxSize)
			{
				if (asIScriptContext* Ctx = asGetActiveContext())
					Ctx->SetException("TArray size too large");
				return false;
			}
			return true;
		}

		void CreateBuffer(FScriptTArrayBuffer** Buf, asUINT NumElems)
		{
			*Buf = reinterpret_cast<FScriptTArrayBuffer*>(
				FMemory::Malloc(sizeof(FScriptTArrayBuffer) - 1 + ElementSize * NumElems));
			if (*Buf)
			{
				(*Buf)->NumElements = NumElems;
				(*Buf)->MaxElements = NumElems;
				ConstructRange(*Buf, 0, NumElems);
			}
			else
			{
				if (asIScriptContext* Ctx = asGetActiveContext())
					Ctx->SetException("Out of memory");
			}
		}

		void DeleteBuffer(FScriptTArrayBuffer* Buf)
		{
			DestructRange(Buf, 0, Buf->NumElements);
			FMemory::Free(Buf);
		}

		void ConstructRange(FScriptTArrayBuffer* Buf, asUINT Start, asUINT End)
		{
			if ((SubTypeId & asTYPEID_MASK_OBJECT) && !(SubTypeId & asTYPEID_OBJHANDLE))
			{
				void** Max = (void**)(Buf->Data + End   * sizeof(void*));
				void** d   = (void**)(Buf->Data + Start * sizeof(void*));
				asIScriptEngine* Engine = ObjType->GetEngine();
				asITypeInfo* SubType = ObjType->GetSubType();
				for (; d < Max; d++)
				{
					*d = Engine->CreateScriptObject(SubType);
					if (!*d)
					{
						FMemory::Memset(d, 0, sizeof(void*) * (Max - d));
						return;
					}
				}
			}
			else
			{
				FMemory::Memset(Buf->Data + Start * ElementSize, 0, (End - Start) * ElementSize);
			}
		}

		void DestructRange(FScriptTArrayBuffer* Buf, asUINT Start, asUINT End)
		{
			if (SubTypeId & asTYPEID_MASK_OBJECT)
			{
				asIScriptEngine* Engine = ObjType->GetEngine();
				void** Max = (void**)(Buf->Data + End   * sizeof(void*));
				void** d   = (void**)(Buf->Data + Start * sizeof(void*));
				for (; d < Max; d++)
					if (*d) Engine->ReleaseScriptObject(*d, ObjType->GetSubType());
			}
		}

		void CopyBuffer(FScriptTArrayBuffer* Dst, FScriptTArrayBuffer* Src)
		{
			asIScriptEngine* Engine = ObjType->GetEngine();
			if (SubTypeId & asTYPEID_OBJHANDLE)
			{
				if (Dst->NumElements > 0 && Src->NumElements > 0)
				{
					int Count = Dst->NumElements > Src->NumElements ? (int)Src->NumElements : (int)Dst->NumElements;
					void** Max = (void**)(Dst->Data + Count * sizeof(void*));
					void** d   = (void**)Dst->Data;
					void** s   = (void**)Src->Data;
					for (; d < Max; d++, s++)
					{
						void* Tmp = *d;
						*d = *s;
						if (*d)  Engine->AddRefScriptObject(*d, ObjType->GetSubType());
						if (Tmp) Engine->ReleaseScriptObject(Tmp, ObjType->GetSubType());
					}
				}
			}
			else
			{
				if (Dst->NumElements > 0 && Src->NumElements > 0)
				{
					int Count = Dst->NumElements > Src->NumElements ? (int)Src->NumElements : (int)Dst->NumElements;
					if (SubTypeId & asTYPEID_MASK_OBJECT)
					{
						void** Max = (void**)(Dst->Data + Count * sizeof(void*));
						void** d   = (void**)Dst->Data;
						void** s   = (void**)Src->Data;
						asITypeInfo* SubType = ObjType->GetSubType();
						for (; d < Max; d++, s++)
							Engine->AssignScriptObject(*d, *s, SubType);
					}
					else
					{
						FMemory::Memcpy(Dst->Data, Src->Data, Count * ElementSize);
					}
				}
			}
		}

		bool Equals(const void* A, const void* B, asIScriptContext* Ctx, FScriptTArrayCache* Cache) const
		{
			if (!(SubTypeId & ~asTYPEID_MASK_SEQNBR))
			{
				switch (SubTypeId)
				{
#define COMPARE(T) *((const T*)A) == *((const T*)B)
				case asTYPEID_BOOL:   return COMPARE(bool);
				case asTYPEID_INT8:   return COMPARE(asINT8);
				case asTYPEID_INT16:  return COMPARE(asINT16);
				case asTYPEID_INT32:  return COMPARE(asINT32);
				case asTYPEID_INT64:  return COMPARE(asINT64);
				case asTYPEID_UINT8:  return COMPARE(uint8);
				case asTYPEID_UINT16: return COMPARE(asWORD);
				case asTYPEID_UINT32: return COMPARE(asDWORD);
				case asTYPEID_UINT64: return COMPARE(asQWORD);
				case asTYPEID_FLOAT:  return COMPARE(float);
				case asTYPEID_DOUBLE: return COMPARE(double);
				default:              return COMPARE(int32);
#undef COMPARE
				}
			}
			else
			{
				if ((SubTypeId & asTYPEID_OBJHANDLE) &&
				    *(const void* const*)A == *(const void* const*)B)
					return true;

				if (Cache && Cache->EqFunc && Ctx)
				{
					if (Ctx->Prepare(Cache->EqFunc) < 0) return false;
					if (SubTypeId & asTYPEID_OBJHANDLE)
					{
						Ctx->SetObject(*((void**)const_cast<void*>(A)));
						Ctx->SetArgObject(0, *((void**)const_cast<void*>(B)));
					}
					else
					{
						Ctx->SetObject(const_cast<void*>(A));
						Ctx->SetArgObject(0, const_cast<void*>(B));
					}
					return Ctx->Execute() == asEXECUTION_FINISHED && Ctx->GetReturnByte() != 0;
				}
				if (Cache && Cache->CmpFunc && Ctx)
				{
					if (Ctx->Prepare(Cache->CmpFunc) < 0) return false;
					if (SubTypeId & asTYPEID_OBJHANDLE)
					{
						Ctx->SetObject(*((void**)const_cast<void*>(A)));
						Ctx->SetArgObject(0, *((void**)const_cast<void*>(B)));
					}
					else
					{
						Ctx->SetObject(const_cast<void*>(A));
						Ctx->SetArgObject(0, const_cast<void*>(B));
					}
					return Ctx->Execute() == asEXECUTION_FINISHED && (int)Ctx->GetReturnDWord() == 0;
				}
			}
			return false;
		}

		bool Less(const void* A, const void* B, bool bAsc)
		{
			if (!bAsc) { const void* Tmp = A; A = B; B = Tmp; }
			if (!(SubTypeId & ~asTYPEID_MASK_SEQNBR))
			{
				switch (SubTypeId)
				{
#define COMPARE(T) *((const T*)A) < *((const T*)B)
				case asTYPEID_BOOL:   return COMPARE(bool);
				case asTYPEID_INT8:   return COMPARE(asINT8);
				case asTYPEID_INT16:  return COMPARE(asINT16);
				case asTYPEID_INT32:  return COMPARE(asINT32);
				case asTYPEID_INT64:  return COMPARE(asINT64);
				case asTYPEID_UINT8:  return COMPARE(uint8);
				case asTYPEID_UINT16: return COMPARE(asWORD);
				case asTYPEID_UINT32: return COMPARE(asDWORD);
				case asTYPEID_UINT64: return COMPARE(asQWORD);
				case asTYPEID_FLOAT:  return COMPARE(float);
				case asTYPEID_DOUBLE: return COMPARE(double);
				default:              return COMPARE(int32);
#undef COMPARE
				}
			}
			return false;
		}

		void Sort(asUINT StartAt, asUINT Count, bool bAsc)
		{
			FScriptTArrayCache* Cache = reinterpret_cast<FScriptTArrayCache*>(ObjType->GetUserData(TARRAY_CACHE));
			if (SubTypeId & ~asTYPEID_MASK_SEQNBR)
			{
				if (!Cache || !Cache->CmpFunc)
				{
					if (asIScriptContext* Ctx = asGetActiveContext())
						Ctx->SetException("Type does not support sort - no opCmp method");
					return;
				}
			}

			if (Count < 2) return;

			int Start = (int)StartAt;
			int End   = (int)(StartAt + Count);
			if (!Buffer || Start >= (int)Buffer->NumElements || End > (int)Buffer->NumElements)
			{
				if (asIScriptContext* Ctx = asGetActiveContext())
					Ctx->SetException("TArray index out of bounds");
				return;
			}

			if (SubTypeId & ~asTYPEID_MASK_SEQNBR)
			{
				asIScriptContext* CmpCtx = asGetActiveContext();
				bool bIsNested = false;
				if (CmpCtx)
				{
					if (CmpCtx->GetEngine() == ObjType->GetEngine() && CmpCtx->PushState() >= 0)
						bIsNested = true;
					else
						CmpCtx = nullptr;
				}
				if (!CmpCtx) CmpCtx = ObjType->GetEngine()->RequestContext();

				struct FLess
				{
					bool               bAsc;
					asIScriptContext*  CmpCtx;
					asIScriptFunction* CmpFunc;
					bool operator()(void* A, void* B) const
					{
						void* A2 = A, *B2 = B;
						if (!bAsc) { A2 = B; B2 = A; }
						if (!A2) return true;
						if (!B2) return false;
						if (!CmpFunc) return false;
						CmpCtx->Prepare(CmpFunc);
						CmpCtx->SetObject(A2);
						CmpCtx->SetArgObject(0, B2);
						return CmpCtx->Execute() == asEXECUTION_FINISHED &&
						       (int)CmpCtx->GetReturnDWord() < 0;
					}
				} Comparator = { bAsc, CmpCtx, Cache ? Cache->CmpFunc : nullptr };

				std::sort((void**)GetItemPtr(Start), (void**)GetItemPtr(End), Comparator);

				if (CmpCtx)
				{
					if (bIsNested)
					{
						asEContextState State = CmpCtx->GetState();
						CmpCtx->PopState();
						if (State == asEXECUTION_ABORTED) CmpCtx->Abort();
					}
					else
						ObjType->GetEngine()->ReturnContext(CmpCtx);
				}
			}
			else
			{
				uint8 Tmp[16];
				for (int i = Start + 1; i < End; i++)
				{
					CopyElement(Tmp, GetItemPtr(i));
					int j = i - 1;
					while (j >= Start && Less(GetDataPtr(Tmp), At(j), bAsc))
					{
						CopyElement(GetItemPtr(j + 1), GetItemPtr(j));
						j--;
					}
					CopyElement(GetItemPtr(j + 1), Tmp);
				}
			}
		}

		void Resize(int Delta, asUINT At)
		{
			if (!Buffer) return;
			if (Delta < 0)
			{
				if (-Delta > (int)Buffer->NumElements)
					Delta = -(int)Buffer->NumElements;
				if (At > Buffer->NumElements + Delta)
					At = Buffer->NumElements + Delta;
			}
			else if (Delta > 0)
			{
				if (!CheckMaxSize(Buffer->NumElements + Delta)) return;
				if (At > Buffer->NumElements)
					At = Buffer->NumElements;
			}
			if (Delta == 0) return;

			if (Buffer->MaxElements < Buffer->NumElements + Delta)
			{
				FScriptTArrayBuffer* NewBuffer = reinterpret_cast<FScriptTArrayBuffer*>(
					FMemory::Malloc(sizeof(FScriptTArrayBuffer) - 1 +
					                ElementSize * (Buffer->NumElements + Delta)));
				if (!NewBuffer)
				{
					if (asIScriptContext* Ctx = asGetActiveContext())
						Ctx->SetException("Out of memory");
					return;
				}
				NewBuffer->NumElements = Buffer->NumElements + Delta;
				NewBuffer->MaxElements = NewBuffer->NumElements;

				FMemory::Memcpy(NewBuffer->Data, Buffer->Data, At * ElementSize);
				if (At < Buffer->NumElements)
					FMemory::Memcpy(
						NewBuffer->Data + (At + Delta) * ElementSize,
						Buffer->Data + At * ElementSize,
						(Buffer->NumElements - At) * ElementSize);

				ConstructRange(NewBuffer, At, At + Delta);
				FMemory::Free(Buffer);
				Buffer = NewBuffer;
			}
			else if (Delta < 0)
			{
				DestructRange(Buffer, At, At - Delta);
				FMemory::Memmove(
					Buffer->Data + At * ElementSize,
					Buffer->Data + (At - Delta) * ElementSize,
					(Buffer->NumElements - (At - Delta)) * ElementSize);
				Buffer->NumElements += Delta;
			}
			else
			{
				FMemory::Memmove(
					Buffer->Data + (At + Delta) * ElementSize,
					Buffer->Data + At * ElementSize,
					(Buffer->NumElements - At) * ElementSize);
				ConstructRange(Buffer, At, At + Delta);
				Buffer->NumElements += Delta;
			}
		}

		uint8* GetItemPtr(int Index) { return Buffer->Data + Index * ElementSize; }

		void* GetDataPtr(void* Buf)
		{
			if ((SubTypeId & asTYPEID_MASK_OBJECT) && !(SubTypeId & asTYPEID_OBJHANDLE))
				return reinterpret_cast<void*>(*(size_t*)Buf);
			return Buf;
		}

		void CopyElement(void* Dst, void* Src) { FMemory::Memcpy(Dst, Src, ElementSize); }

		void SwapElements(void* A, void* B)
		{
			uint8 Tmp[16];
			CopyElement(Tmp, A);
			CopyElement(A, B);
			CopyElement(B, Tmp);
		}
	};


	// ---------------------------------------------------------------------------
	// Value-type CONSTRUCT / DESTRUCT helpers (asCALL_CDECL_OBJLAST)
	// AngelScript passes the pre-allocated memory as the last (object) parameter.
	// ---------------------------------------------------------------------------

	static void TArray_Construct(asITypeInfo* TypeInfo, FScriptArray* Self)
	{
		new (Self) FScriptArray();
	}

	static void TArray_ConstructWithLength(asITypeInfo* TypeInfo, asUINT Length, FScriptArray* Self)
	{
		new (Self) FScriptArray();
	}

	static void TArray_CopyConstruct(asITypeInfo* TypeInfo, const FScriptArray& Other, FScriptArray* Self)
	{
		(void)TypeInfo;
		new (Self) FScriptArray(Other);
	}

	static void TArray_ListConstruct(asITypeInfo* TypeInfo, void* ListBuffer, FScriptArray* Self)
	{
		(void)TypeInfo;
		//new (Self) FScriptArray(TypeInfo, ListBuffer);
	}

	static void TArray_Destruct(FScriptArray* Self)
	{
		Self->~FScriptArray();
	}

	static void TArray_Add(asIScriptGeneric* gen)
	{
		auto Index = gen->GetArgDWord(0);
		auto Array = (FScriptArray*)gen->GetObject();
		auto TypeInfoId = gen->GetObjectTypeId();
		auto TypeInfo = gen->GetEngine()->GetTypeInfoById(TypeInfoId);
		auto Meta = (TArrayMetaData*)TypeInfo->GetUserData(TARRAY_META);
		if (!TypeInfo || !Meta)
		{
			return;
		}
		auto ElementSize = Meta->BytePerElement;
		if (ElementSize == 0)
		{
			return;
		}
		Array->Insert(Index, 1, ElementSize, Meta->Alignment);
	}

	static bool TArray_IsEmpty(const FScriptArray& Arr)
	{
		return Arr.IsEmpty();
	}

	// GC behaviors — value types with asOBJ_GC only need ENUMREFS and RELEASEREFS.
	void TArray_EnumReferences(asIScriptEngine* Engine, FScriptArray& Arr)
	{
		//if (!Buffer || !(SubTypeId & asTYPEID_MASK_OBJECT)) return;

		//void** d = (void**)Buffer->Data;
		//asITypeInfo* SubType = Engine->GetTypeInfoById(SubTypeId);
		//if (SubType->GetFlags() & asOBJ_REF)
		//{
		//	for (asUINT n = 0; n < Buffer->NumElements; n++)
		//		if (d[n]) Engine->GCEnumCallback(d[n]);
		//}
		//else if ((SubType->GetFlags() & asOBJ_VALUE) && (SubType->GetFlags() & asOBJ_GC))
		//{
		//	for (asUINT n = 0; n < Buffer->NumElements; n++)
		//		if (d[n]) Engine->ForwardGCEnumReferences(d[n], SubType);
		//}
	}

	void TArray_ReleaseAllHandles(asIScriptEngine* engine, FScriptArray& Arr)
	{
		//Resize(0);
	}

	// ---------------------------------------------------------------------------
	// For-loop iteration helpers
	// ---------------------------------------------------------------------------

	static asUINT TArray_opForBegin(const FScriptTArray*) { return 0; }
	static bool   TArray_opForEnd  (asUINT Iter, const FScriptTArray* Arr) { return !Arr || Arr->GetSize() <= Iter; }
	static asUINT TArray_opForNext (asUINT Iter, const FScriptTArray*) { return Iter + 1; }
	static asUINT TArray_opForValue1(asUINT Iter, const FScriptTArray*) { return Iter; }
}

void Declare_TArray(asIScriptEngine* Engine)
{

	// Value type: size is fixed (heap buffer is held by pointer inside the struct).
	// asOBJ_GC is kept so elements that are GC objects are enumerated correctly.
	const asDWORD TypeFlags =
		asOBJ_VALUE | asOBJ_GC | asOBJ_TEMPLATE |
		asOBJ_APP_CLASS | asOBJ_APP_CLASS_CONSTRUCTOR | asOBJ_APP_CLASS_DESTRUCTOR |
		asOBJ_APP_CLASS_COPY_CONSTRUCTOR | asOBJ_APP_CLASS_ASSIGNMENT |
		asOBJ_APP_CLASS_MORE_CONSTRUCTORS;

	int Result = Engine->RegisterObjectType("TArray<class T>", sizeof(FScriptArray), TypeFlags);
	check(Result >= 0);
}

void Bind_TArray(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	Engine->SetTypeInfoUserDataCleanupCallback(CleanupTypeInfoTArrayCache, TARRAY_CACHE);

	int Result = 0;

	Result = Engine->RegisterObjectBehaviour("TArray<T>", asBEHAVE_TEMPLATE_CALLBACK,
		"bool f(int&in, bool&out)", asFUNCTION(TArrayTemplateCallback), asCALL_CDECL);
	check(Result >= 0);

	// Default constructor: "TArray<int> arr;"
	Result = Engine->RegisterObjectBehaviour("TArray<T>", asBEHAVE_CONSTRUCT,
		"void f(int&in)",
		asFUNCTION(TArray_Construct), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	// Explicit length constructor: "TArray<int> arr(10);"
	Result = Engine->RegisterObjectBehaviour("TArray<T>", asBEHAVE_CONSTRUCT,
		"void f(int&in, uint length) explicit",
		asFUNCTION(TArray_ConstructWithLength), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	// Copy constructor: "TArray<int> b(a);"
	Result = Engine->RegisterObjectBehaviour("TArray<T>", asBEHAVE_CONSTRUCT,
		"void f(int&in, const TArray<T>&in)",
		asFUNCTION(TArray_CopyConstruct), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	// List constructor: "TArray<int> arr = {1, 2, 3};"
	Result = Engine->RegisterObjectBehaviour("TArray<T>", asBEHAVE_LIST_CONSTRUCT,
		"void f(int&in, int&in) {repeat T}",
		asFUNCTION(TArray_ListConstruct), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	Result = Engine->RegisterObjectBehaviour("TArray<T>", asBEHAVE_DESTRUCT,
		"void f()",
		asFUNCTION(TArray_Destruct), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	//Result = Engine->RegisterObjectMethod("TArray<T>",
	//	"T &opIndex(uint index)",
	//	asMETHODPR(FScriptTArray, At, (asUINT), void*), asCALL_THISCALL);
	//check(Result >= 0);

	//Result = Engine->RegisterObjectMethod("TArray<T>",
	//	"const T &opIndex(uint index) const",
	//	asMETHODPR(FScriptTArray, At, (asUINT) const, const void*), asCALL_THISCALL);
	//check(Result >= 0);

	//Result = Engine->RegisterObjectMethod("TArray<T>",
	//	"uint opForBegin() const",
	//	asFUNCTIONPR(TArray_opForBegin, (const FScriptTArray*), asUINT), asCALL_CDECL_OBJLAST);
	//check(Result >= 0);

	//Result = Engine->RegisterObjectMethod("TArray<T>",
	//	"bool opForEnd(uint) const",
	//	asFUNCTIONPR(TArray_opForEnd, (asUINT, const FScriptTArray*), bool), asCALL_CDECL_OBJLAST);
	//check(Result >= 0);

	//Result = Engine->RegisterObjectMethod("TArray<T>",
	//	"uint opForNext(uint) const",
	//	asFUNCTIONPR(TArray_opForNext, (asUINT, const FScriptTArray*), asUINT), asCALL_CDECL_OBJLAST);
	//check(Result >= 0);

	//Result = Engine->RegisterObjectMethod("TArray<T>",
	//	"const T &opForValue0(uint index) const",
	//	asMETHODPR(FScriptTArray, At, (asUINT) const, const void*), asCALL_THISCALL);
	//check(Result >= 0);

	//Result = Engine->RegisterObjectMethod("TArray<T>",
	//	"uint opForValue1(uint index) const",
	//	asFUNCTIONPR(TArray_opForValue1, (asUINT, const FScriptTArray*), asUINT), asCALL_CDECL_OBJLAST);
	//check(Result >= 0);

	//Result = Engine->RegisterObjectMethod("TArray<T>",
	//	"TArray<T> &opAssign(const TArray<T>&in)",
	//	asMETHOD(FScriptTArray, operator=), asCALL_THISCALL);
	//check(Result >= 0);

	//Result = Engine->RegisterObjectMethod("TArray<T>",
	//	"void Insert(uint index, const T&in value)",
	//	asMETHODPR(FScriptTArray, InsertAt, (asUINT, void*), void), asCALL_THISCALL);
	//check(Result >= 0);

	//Result = Engine->RegisterObjectMethod("TArray<T>",
	//	"void Insert(uint index, const TArray<T>&in arr)",
	//	asMETHODPR(FScriptTArray, InsertAt, (asUINT, const FScriptTArray&), void), asCALL_THISCALL);
	//check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TArray<T>",
		"void Add(const T&in value)",
		asFUNCTION(TArray_Add), asCALL_GENERIC);
	check(Result >= 0);

	//Result = Engine->RegisterObjectMethod("TArray<T>",
	//	"void RemoveAt(uint index)",
	//	asMETHOD(FScriptTArray, RemoveAt), asCALL_THISCALL);
	//check(Result >= 0);

	//Result = Engine->RegisterObjectMethod("TArray<T>",
	//	"void Pop()",
	//	asMETHOD(FScriptTArray, RemoveLast), asCALL_THISCALL);
	//check(Result >= 0);

	//Result = Engine->RegisterObjectMethod("TArray<T>",
	//	"void RemoveAt(uint start, uint count)",
	//	asMETHOD(FScriptTArray, RemoveRange), asCALL_THISCALL);
	//check(Result >= 0);

	//Result = Engine->RegisterObjectMethod("TArray<T>",
	//	"uint Num() const",
	//	asMETHOD(FScriptTArray, GetSize), asCALL_THISCALL);
	//check(Result >= 0);

	//Result = Engine->RegisterObjectMethod("TArray<T>",
	//	"void Reserve(uint length)",
	//	asMETHOD(FScriptTArray, Reserve), asCALL_THISCALL);
	//check(Result >= 0);

	//Result = Engine->RegisterObjectMethod("TArray<T>",
	//	"void SetNum(uint length)",
	//	asMETHODPR(FScriptTArray, Resize, (asUINT), void), asCALL_THISCALL);
	//check(Result >= 0);

	//Result = Engine->RegisterObjectMethod("TArray<T>",
	//	"void sortAsc()",
	//	asMETHODPR(FScriptTArray, SortAsc, (), void), asCALL_THISCALL);
	//check(Result >= 0);

	//Result = Engine->RegisterObjectMethod("TArray<T>",
	//	"void sortAsc(uint startAt, uint count)",
	//	asMETHODPR(FScriptTArray, SortAsc, (asUINT, asUINT), void), asCALL_THISCALL);
	//check(Result >= 0);

	//Result = Engine->RegisterObjectMethod("TArray<T>",
	//	"void sortDesc()",
	//	asMETHODPR(FScriptTArray, SortDesc, (), void), asCALL_THISCALL);
	//check(Result >= 0);

	//Result = Engine->RegisterObjectMethod("TArray<T>",
	//	"void sortDesc(uint startAt, uint count)",
	//	asMETHODPR(FScriptTArray, SortDesc, (asUINT, asUINT), void), asCALL_THISCALL);
	//check(Result >= 0);

	//Result = Engine->RegisterObjectMethod("TArray<T>",
	//	"void reverse()",
	//	asMETHOD(FScriptTArray, Reverse), asCALL_THISCALL);
	//check(Result >= 0);

	//Result = Engine->RegisterObjectMethod("TArray<T>",
	//	"int Find(const T&in if_handle_then_const value) const",
	//	asMETHODPR(FScriptTArray, Find, (const void*) const, int), asCALL_THISCALL);
	//check(Result >= 0);

	//Result = Engine->RegisterObjectMethod("TArray<T>",
	//	"int Find(uint startAt, const T&in if_handle_then_const value) const",
	//	asMETHODPR(FScriptTArray, Find, (asUINT, const void*) const, int), asCALL_THISCALL);
	//check(Result >= 0);

	//Result = Engine->RegisterObjectMethod("TArray<T>",
	//	"int FindByRef(const T&in if_handle_then_const value) const",
	//	asMETHODPR(FScriptTArray, FindByRef, (const void*) const, int), asCALL_THISCALL);
	//check(Result >= 0);

	//Result = Engine->RegisterObjectMethod("TArray<T>",
	//	"int FindByRef(uint startAt, const T&in if_handle_then_const value) const",
	//	asMETHODPR(FScriptTArray, FindByRef, (asUINT, const void*) const, int), asCALL_THISCALL);
	//check(Result >= 0);

	//Result = Engine->RegisterObjectMethod("TArray<T>",
	//	"bool opEquals(const TArray<T>&in) const",
	//	asMETHOD(FScriptTArray, operator==), asCALL_THISCALL);
	//check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TArray<T>",
		"bool IsEmpty() const",
		asFUNCTION(TArray_IsEmpty), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	//Result = Engine->RegisterFuncdef(
	//	"bool TArray<T>::less(const T&in if_handle_then_const a, const T&in if_handle_then_const b)");
	//check(Result >= 0);

	//Result = Engine->RegisterObjectMethod("TArray<T>",
	//	"void Sort(const less &in, uint startAt = 0, uint count = uint(-1))",
	//	asMETHODPR(FScriptTArray, Sort, (asIScriptFunction*, asUINT, asUINT), void), asCALL_THISCALL);
	//check(Result >= 0);

	// Value types with asOBJ_GC only need ENUMREFS and RELEASEREFS (no ref-count behaviors).
	Result = Engine->RegisterObjectBehaviour("TArray<T>", asBEHAVE_ENUMREFS,
		"void f(int&in)", asFUNCTION(TArray_EnumReferences), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	Result = Engine->RegisterObjectBehaviour("TArray<T>", asBEHAVE_RELEASEREFS,
		"void f(int&in)", asFUNCTION(TArray_ReleaseAllHandles), asCALL_CDECL_OBJLAST);
	check(Result >= 0);
}
