// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Misc/AssertionMacros.h"
#include "HAL/UnrealMemory.h"
#include "OtterAngleScript.h"

#include <new>
#include <algorithm>

namespace
{
	static const asPWORD TSET_CACHE = 1002;

	struct FScriptTSetBuffer
	{
		uint32 MaxElements;
		uint32 NumElements;
		uint8  Data[1];
	};

	struct FScriptTSetCache
	{
		asIScriptFunction* EqFunc;
		int                EqFuncReturnCode;
	};

	static void CleanupTypeInfoTSetCache(asITypeInfo* TypeInfo)
	{
		FScriptTSetCache* Cache = reinterpret_cast<FScriptTSetCache*>(TypeInfo->GetUserData(TSET_CACHE));
		if (Cache)
		{
			Cache->~FScriptTSetCache();
			FMemory::Free(Cache);
		}
	}

	static bool TSetTemplateCallback(asITypeInfo* TypeInfo, bool& bDontGarbageCollect)
	{
		int TypeId = TypeInfo->GetSubTypeId();
		if (TypeId == asTYPEID_VOID)
			return false;

		if ((TypeId & asTYPEID_MASK_OBJECT) && !(TypeId & asTYPEID_OBJHANDLE))
		{
			asITypeInfo* SubType = TypeInfo->GetEngine()->GetTypeInfoById(TypeId);
			asQWORD Flags = SubType->GetFlags();
			if ((Flags & asOBJ_VALUE) && !(Flags & asOBJ_POD))
			{
				bool bFound = false;
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
					TypeInfo->GetEngine()->WriteMessage("TSet", 0, 0, asMSGTYPE_ERROR, "The subtype has no default constructor");
					return false;
				}
			}
			else if (Flags & asOBJ_REF)
			{
				bool bFound = false;
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
					TypeInfo->GetEngine()->WriteMessage("TSet", 0, 0, asMSGTYPE_ERROR, "The subtype has no default factory");
					return false;
				}
			}
			if (!(Flags & asOBJ_GC))
				bDontGarbageCollect = true;
		}
		else if (!(TypeId & asTYPEID_OBJHANDLE))
		{
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
		return true;
	}

	class FScriptTSet
	{
	public:
		FScriptTSet()
			: ObjType(nullptr), Buffer(nullptr), ElementSize(0), SubTypeId(0)
		{
		}

		FScriptTSet(asUINT Length, asITypeInfo* TypeInfo)
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

		FScriptTSet(const FScriptTSet& Other)
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

		~FScriptTSet()
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

		bool Contains(const void* Value) const
		{
			return FindIndex(Value) != -1;
		}

		bool Add(const void* Value)
		{
			if (FindIndex(Value) != -1)
				return false;

			asUINT OldSize = GetSize();
			Resize(1, OldSize);
			SetValue(OldSize, Value);
			return true;
		}

		bool Remove(const void* Value)
		{
			int Idx = FindIndex(Value);
			if (Idx < 0)
				return false;

			RemoveIndex((asUINT)Idx);
			return true;
		}

		void Clear()
		{
			if (Buffer)
				Resize(-(int)Buffer->NumElements, 0);
		}

		void RemoveIndex(asUINT Index)
		{
			if (!Buffer || Index >= Buffer->NumElements)
			{
				if (asIScriptContext* Ctx = asGetActiveContext())
					Ctx->SetException("TSet index out of bounds");
				return;
			}

			// Move the last element to fill the hole (swap-and-pop)
			asUINT Last = Buffer->NumElements - 1;
			if (Index < Last)
			{
				if ((SubTypeId & asTYPEID_MASK_OBJECT) && !(SubTypeId & asTYPEID_OBJHANDLE))
				{
					// Value type objects: swap pointers in the slot array
					void** Slot = (void**)(Buffer->Data + Index * sizeof(void*));
					void** LastSlot = (void**)(Buffer->Data + Last * sizeof(void*));
					*Slot = *LastSlot;
					*LastSlot = nullptr;
				}
				else
				{
					FMemory::Memcpy(Buffer->Data + Index * ElementSize, Buffer->Data + Last * ElementSize, ElementSize);
					FMemory::Memset(Buffer->Data + Last * ElementSize, 0, ElementSize);
				}
			}
			Resize(-1, Last);
		}

		void* GetValue(asUINT Index)
		{
			return const_cast<void*>(const_cast<const FScriptTSet*>(this)->GetValue(Index));
		}

		const void* GetValue(asUINT Index) const
		{
			if (!Buffer || Index >= Buffer->NumElements)
			{
				if (asIScriptContext* Ctx = asGetActiveContext())
					Ctx->SetException("TSet index out of bounds");
				return nullptr;
			}
			if ((SubTypeId & asTYPEID_MASK_OBJECT) && !(SubTypeId & asTYPEID_OBJHANDLE))
				return *(void**)(Buffer->Data + ElementSize * Index);
			return Buffer->Data + ElementSize * Index;
		}

		void SetValue(asUINT Index, const void* Value)
		{
			void* Ptr = const_cast<void*>(GetValue(Index));
			if (!Ptr) return;

			if ((SubTypeId & ~asTYPEID_MASK_SEQNBR) && !(SubTypeId & asTYPEID_OBJHANDLE))
			{
				ObjType->GetEngine()->AssignScriptObject(Ptr, const_cast<void*>(Value), ObjType->GetSubType());
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

		FScriptTSet& operator=(const FScriptTSet& Other)
		{
			if (&Other != this && Other.GetSetObjectType() == GetSetObjectType())
			{
				Clear();
				asUINT OtherSize = Other.GetSize();
				for (asUINT i = 0; i < OtherSize; i++)
					Add(Other.GetValue(i));
			}
			return *this;
		}

		bool operator==(const FScriptTSet& Other) const
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
			FScriptTSetCache* Cache = reinterpret_cast<FScriptTSetCache*>(ObjType->GetUserData(TSET_CACHE));
			for (asUINT n = 0; n < GetSize(); n++)
			{
				if (!Other.Contains(GetValue(n)))
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

		asITypeInfo* GetSetObjectType() const { return ObjType; }

		// GC behaviors
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
			Clear();
		}

	private:
		asITypeInfo*         ObjType;
		FScriptTSetBuffer*   Buffer;
		int                  ElementSize;
		int                  SubTypeId;

		void Precache()
		{
			SubTypeId = ObjType->GetSubTypeId();
			if (!(SubTypeId & ~asTYPEID_MASK_SEQNBR)) return;

			FScriptTSetCache* Cache = reinterpret_cast<FScriptTSetCache*>(ObjType->GetUserData(TSET_CACHE));
			if (Cache) return;

			asAcquireExclusiveLock();

			Cache = reinterpret_cast<FScriptTSetCache*>(ObjType->GetUserData(TSET_CACHE));
			if (!Cache)
			{
				Cache = reinterpret_cast<FScriptTSetCache*>(FMemory::Malloc(sizeof(FScriptTSetCache)));
				if (!Cache)
				{
					asReleaseExclusiveLock();
					return;
				}
				FMemory::Memset(Cache, 0, sizeof(FScriptTSetCache));

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

						bool bIsEq = (RetTypeId == asTYPEID_BOOL) && (FCStringAnsi::Strcmp(Func->GetName(), "opEquals") == 0);
						if (!bIsEq) continue;

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

						if (Cache->EqFunc || Cache->EqFuncReturnCode)
						{
							Cache->EqFunc = nullptr;
							Cache->EqFuncReturnCode = asMULTIPLE_FUNCTIONS;
						}
						else
							Cache->EqFunc = Func;
					}
				}

				if (!Cache->EqFunc && Cache->EqFuncReturnCode == 0)
					Cache->EqFuncReturnCode = asNO_FUNCTION;

				ObjType->SetUserData(Cache, TSET_CACHE);
			}
			asReleaseExclusiveLock();
		}

		int FindIndex(const void* Value) const
		{
			FScriptTSetCache* Cache = nullptr;
			if (SubTypeId & ~asTYPEID_MASK_SEQNBR)
			{
				Cache = reinterpret_cast<FScriptTSetCache*>(ObjType->GetUserData(TSET_CACHE));
				if (!Cache || !Cache->EqFunc)
				{
					if (asIScriptContext* Ctx = asGetActiveContext())
						Ctx->SetException("Type does not support set operations - no opEquals method");
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
			for (asUINT i = 0; i < Size; i++)
			{
				if (Equals(GetValue(i), Value, CmpCtx, Cache))
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

		bool Equals(const void* A, const void* B, asIScriptContext* Ctx, FScriptTSetCache* Cache) const
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
			}
			return false;
		}

		bool CheckMaxSize(asUINT NumElems)
		{
			asUINT MaxSize = 0xFFFFFFFFul - sizeof(FScriptTSetBuffer) + 1;
			if (ElementSize > 0)
				MaxSize /= (asUINT)ElementSize;
			if (NumElems > MaxSize)
			{
				if (asIScriptContext* Ctx = asGetActiveContext())
					Ctx->SetException("TSet size too large");
				return false;
			}
			return true;
		}

		void CreateBuffer(FScriptTSetBuffer** Buf, asUINT NumElems)
		{
			*Buf = reinterpret_cast<FScriptTSetBuffer*>(
				FMemory::Malloc(sizeof(FScriptTSetBuffer) - 1 + ElementSize * NumElems));
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

		void DeleteBuffer(FScriptTSetBuffer* Buf)
		{
			DestructRange(Buf, 0, Buf->NumElements);
			FMemory::Free(Buf);
		}

		void ConstructRange(FScriptTSetBuffer* Buf, asUINT Start, asUINT End)
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

		void DestructRange(FScriptTSetBuffer* Buf, asUINT Start, asUINT End)
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
				FScriptTSetBuffer* NewBuffer = reinterpret_cast<FScriptTSetBuffer*>(
					FMemory::Malloc(sizeof(FScriptTSetBuffer) - 1 +
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
	};

	// ---------------------------------------------------------------------------
	// Value-type CONSTRUCT / DESTRUCT helpers
	// ---------------------------------------------------------------------------

	static void TSet_Construct(asITypeInfo* TypeInfo, FScriptTSet* Self)
	{
		new (Self) FScriptTSet(asUINT(0), TypeInfo);
	}

	static void TSet_ConstructWithLength(asITypeInfo* TypeInfo, asUINT Length, FScriptTSet* Self)
	{
		new (Self) FScriptTSet(Length, TypeInfo);
	}

	static void TSet_CopyConstruct(asITypeInfo* TypeInfo, const FScriptTSet& Other, FScriptTSet* Self)
	{
		(void)TypeInfo;
		new (Self) FScriptTSet(Other);
	}

	static void TSet_Destruct(FScriptTSet* Self)
	{
		Self->~FScriptTSet();
	}

	// ---------------------------------------------------------------------------
	// For-loop iteration helpers
	// ---------------------------------------------------------------------------

	static asUINT TSet_opForBegin(const FScriptTSet*) { return 0; }
	static bool   TSet_opForEnd  (asUINT Iter, const FScriptTSet* Set) { return !Set || Set->GetSize() <= Iter; }
	static asUINT TSet_opForNext (asUINT Iter, const FScriptTSet*) { return Iter + 1; }
}

void Declare_TSet(asIScriptEngine* Engine)
{
	const asDWORD TypeFlags =
		asOBJ_VALUE | asOBJ_GC | asOBJ_TEMPLATE |
		asOBJ_APP_CLASS | asOBJ_APP_CLASS_CONSTRUCTOR | asOBJ_APP_CLASS_DESTRUCTOR |
		asOBJ_APP_CLASS_COPY_CONSTRUCTOR | asOBJ_APP_CLASS_ASSIGNMENT |
		asOBJ_APP_CLASS_MORE_CONSTRUCTORS;

	int Result = Engine->RegisterObjectType("TSet<class T>", sizeof(FScriptTSet), TypeFlags);
	check(Result >= 0);

	Result = Engine->RegisterObjectBehaviour("TSet<T>", asBEHAVE_TEMPLATE_CALLBACK,
		"bool f(int&in, bool&out)", asFUNCTION(TSetTemplateCallback), asCALL_CDECL);
	check(Result >= 0);

	// Default constructor
	Result = Engine->RegisterObjectBehaviour("TSet<T>", asBEHAVE_CONSTRUCT,
		"void f(int&in)",
		asFUNCTION(TSet_Construct), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	// Explicit capacity constructor
	Result = Engine->RegisterObjectBehaviour("TSet<T>", asBEHAVE_CONSTRUCT,
		"void f(int&in, uint initialCapacity) explicit",
		asFUNCTION(TSet_ConstructWithLength), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	// Copy constructor
	Result = Engine->RegisterObjectBehaviour("TSet<T>", asBEHAVE_CONSTRUCT,
		"void f(int&in, const TSet<T>&in)",
		asFUNCTION(TSet_CopyConstruct), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	Result = Engine->RegisterObjectBehaviour("TSet<T>", asBEHAVE_DESTRUCT,
		"void f()",
		asFUNCTION(TSet_Destruct), asCALL_CDECL_OBJLAST);
	check(Result >= 0);
}

void Bind_TSet(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	Engine->SetTypeInfoUserDataCleanupCallback(CleanupTypeInfoTSetCache, TSET_CACHE);

	int Result = 0;

	// For-range iteration
	Result = Engine->RegisterObjectMethod("TSet<T>",
		"uint opForBegin() const",
		asFUNCTIONPR(TSet_opForBegin, (const FScriptTSet*), asUINT), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSet<T>",
		"bool opForEnd(uint) const",
		asFUNCTIONPR(TSet_opForEnd, (asUINT, const FScriptTSet*), bool), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSet<T>",
		"uint opForNext(uint) const",
		asFUNCTIONPR(TSet_opForNext, (asUINT, const FScriptTSet*), asUINT), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSet<T>",
		"const T &opForValue0(uint index) const",
		asMETHODPR(FScriptTSet, GetValue, (asUINT) const, const void*), asCALL_THISCALL);
	check(Result >= 0);

	// Assignment
	Result = Engine->RegisterObjectMethod("TSet<T>",
		"TSet<T> &opAssign(const TSet<T>&in)",
		asMETHOD(FScriptTSet, operator=), asCALL_THISCALL);
	check(Result >= 0);

	// Equality
	Result = Engine->RegisterObjectMethod("TSet<T>",
		"bool opEquals(const TSet<T>&in) const",
		asMETHOD(FScriptTSet, operator==), asCALL_THISCALL);
	check(Result >= 0);

	// Access elements by index
	Result = Engine->RegisterObjectMethod("TSet<T>",
		"const T &opIndex(uint index) const",
		asMETHODPR(FScriptTSet, GetValue, (asUINT) const, const void*), asCALL_THISCALL);
	check(Result >= 0);

	// Number of elements
	Result = Engine->RegisterObjectMethod("TSet<T>",
		"uint length() const",
		asMETHOD(FScriptTSet, GetSize), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSet<T>",
		"bool isEmpty() const",
		asMETHOD(FScriptTSet, IsEmpty), asCALL_THISCALL);
	check(Result >= 0);

	// Core set operations
	Result = Engine->RegisterObjectMethod("TSet<T>",
		"bool contains(const T&in if_handle_then_const value) const",
		asMETHODPR(FScriptTSet, Contains, (const void*) const, bool), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSet<T>",
		"bool add(const T&in if_handle_then_const value)",
		asMETHODPR(FScriptTSet, Add, (const void*), bool), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSet<T>",
		"bool remove(const T&in if_handle_then_const value)",
		asMETHODPR(FScriptTSet, Remove, (const void*), bool), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSet<T>",
		"void clear()",
		asMETHOD(FScriptTSet, Clear), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSet<T>",
		"void removeIndex(uint index)",
		asMETHOD(FScriptTSet, RemoveIndex), asCALL_THISCALL);
	check(Result >= 0);

	// GC behaviors
	Result = Engine->RegisterObjectBehaviour("TSet<T>", asBEHAVE_ENUMREFS,
		"void f(int&in)", asMETHOD(FScriptTSet, EnumReferences), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectBehaviour("TSet<T>", asBEHAVE_RELEASEREFS,
		"void f(int&in)", asMETHOD(FScriptTSet, ReleaseAllHandles), asCALL_THISCALL);
	check(Result >= 0);
}
