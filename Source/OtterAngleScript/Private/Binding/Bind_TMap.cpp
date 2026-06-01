// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Misc/AssertionMacros.h"
#include "HAL/UnrealMemory.h"
#include "angelscript.h"
#include "OtterAngleScript.h"

#include <new>

namespace
{
	static const asPWORD TMAP_CACHE = 1003;

	struct FScriptTMapBuffer
	{
		uint32 MaxElements;
		uint32 NumElements;
		uint8  Data[1];
	};

	struct FScriptTMapCache
	{
		asIScriptFunction* KeyEqFunc;
		int                KeyEqFuncReturnCode;
	};

	static void CleanupTypeInfoTMapCache(asITypeInfo* TypeInfo)
	{
		FScriptTMapCache* Cache = reinterpret_cast<FScriptTMapCache*>(TypeInfo->GetUserData(TMAP_CACHE));
		if (Cache)
		{
			Cache->~FScriptTMapCache();
			FMemory::Free(Cache);
		}
	}

	static bool TMapTemplateCallback(asITypeInfo* TypeInfo, bool& bDontGarbageCollect)
	{
		if (TypeInfo->GetSubTypeCount() < 2)
			return false;

		// Validate both key and value subtypes
		bool bCheckGC = false;
		for (asUINT idx = 0; idx < 2; idx++)
		{
			int TypeId = TypeInfo->GetSubTypeId(idx);
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
						TypeInfo->GetEngine()->WriteMessage("TMap", 0, 0, asMSGTYPE_ERROR, "The subtype has no default constructor");
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
						TypeInfo->GetEngine()->WriteMessage("TMap", 0, 0, asMSGTYPE_ERROR, "The subtype has no default factory");
						return false;
					}
				}
				if (!(Flags & asOBJ_GC))
					bCheckGC = true;
			}
			else if (!(TypeId & asTYPEID_OBJHANDLE))
			{
				bCheckGC = true;
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
							bCheckGC = true;
					}
					else
					{
						bCheckGC = true;
					}
				}
			}
		}

		if (bCheckGC)
			bDontGarbageCollect = true;
		return true;
	}

	static int GetElementSize(asITypeInfo* TypeInfo, asUINT SubTypeIndex)
	{
		int TypeId = TypeInfo->GetSubTypeId(SubTypeIndex);
		if (TypeId & asTYPEID_MASK_OBJECT)
			return (int)sizeof(asPWORD);
		return TypeInfo->GetEngine()->GetSizeOfPrimitiveType(TypeId);
	}

	class FScriptTMap
	{
	public:
		FScriptTMap()
			: ObjType(nullptr), Buffer(nullptr), KeyElementSize(0), ValueElementSize(0),
			  PairSize(0), KeySubTypeId(0), ValueSubTypeId(0)
		{
		}

		FScriptTMap(asUINT InitialCapacity, asITypeInfo* TypeInfo)
		{
			ObjType = TypeInfo;
			ObjType->AddRef();
			Buffer = nullptr;
			Precache();

			if (!CheckMaxSize(InitialCapacity))
				return;
			CreateBuffer(&Buffer, InitialCapacity);
		}

		FScriptTMap(const FScriptTMap& Other)
		{
			ObjType = Other.ObjType;
			if (ObjType) ObjType->AddRef();
			Buffer = nullptr;
			KeyElementSize = Other.KeyElementSize;
			ValueElementSize = Other.ValueElementSize;
			PairSize = Other.PairSize;
			KeySubTypeId = Other.KeySubTypeId;
			ValueSubTypeId = Other.ValueSubTypeId;
			if (ObjType)
			{
				CreateBuffer(&Buffer, 0);
				*this = Other;
			}
		}

		~FScriptTMap()
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

		bool Contains(const void* Key) const
		{
			return FindIndex(Key) != -1;
		}

		void Set(const void* Key, const void* Value)
		{
			int Idx = FindIndex(Key);
			if (Idx >= 0)
			{
				// Update existing entry
				SetValueAt((asUINT)Idx, Value);
				return;
			}

			asUINT OldSize = GetSize();
			Resize(1, OldSize);
			SetKeyAt(OldSize, Key);
			SetValueAt(OldSize, Value);
		}

		bool Remove(const void* Key)
		{
			int Idx = FindIndex(Key);
			if (Idx < 0)
				return false;

			RemoveIndex((asUINT)Idx);
			return true;
		}

		void RemoveIndex(asUINT Index)
		{
			if (!Buffer || Index >= Buffer->NumElements)
			{
				if (asIScriptContext* Ctx = asGetActiveContext())
					Ctx->SetException("TMap index out of bounds");
				return;
			}

			// Swap-and-pop: move the last entry to fill the hole
			asUINT Last = Buffer->NumElements - 1;
			if (Index < Last)
			{
				DestructPair(Buffer, Index);
				CopyPair(Buffer, Index, Buffer, Last);
				FMemory::Memset(Buffer->Data + Last * PairSize, 0, PairSize);
			}
			Resize(-1, Last);
		}

		void Clear()
		{
			if (Buffer)
				Resize(-(int)Buffer->NumElements, 0);
		}

		const void* GetValueByKey(const void* Key) const
		{
			int Idx = FindIndex(Key);
			if (Idx < 0)
			{
				if (asIScriptContext* Ctx = asGetActiveContext())
					Ctx->SetException("TMap key not found");
				return nullptr;
			}
			return GetValuePtr((asUINT)Idx);
		}

		const void* GetKeyAt(asUINT Index) const
		{
			if (!Buffer || Index >= Buffer->NumElements)
			{
				if (asIScriptContext* Ctx = asGetActiveContext())
					Ctx->SetException("TMap index out of bounds");
				return nullptr;
			}
			return GetKeyPtr(Index);
		}

		const void* GetValueAt(asUINT Index) const
		{
			if (!Buffer || Index >= Buffer->NumElements)
			{
				if (asIScriptContext* Ctx = asGetActiveContext())
					Ctx->SetException("TMap index out of bounds");
				return nullptr;
			}
			return GetValuePtr(Index);
		}

		asITypeInfo* GetMapObjectType() const { return ObjType; }

		FScriptTMap& operator=(const FScriptTMap& Other)
		{
			if (&Other != this && Other.ObjType == ObjType)
			{
				Clear();
				asUINT OtherSize = Other.Buffer ? Other.Buffer->NumElements : 0;
				if (OtherSize > 0)
				{
					Resize((int)OtherSize, 0);
					for (asUINT i = 0; i < OtherSize; i++)
						CopyPair(Buffer, i, Other.Buffer, i);
				}
			}
			return *this;
		}

		// GC behaviors
		void EnumReferences(asIScriptEngine* Engine)
		{
			if (!Buffer) return;

			asITypeInfo* KeyType = nullptr;
			asITypeInfo* ValueType = nullptr;
			if (KeySubTypeId & asTYPEID_MASK_OBJECT)
				KeyType = Engine->GetTypeInfoById(KeySubTypeId);
			if (ValueSubTypeId & asTYPEID_MASK_OBJECT)
				ValueType = Engine->GetTypeInfoById(ValueSubTypeId);

			for (asUINT n = 0; n < Buffer->NumElements; n++)
			{
				void* KeyData = (void*)(GetKeyPtr(n));
				void* ValueData = (void*)(GetValuePtr(n));

				// Enumerate key
				if (KeyType)
				{
					if (KeyType->GetFlags() & asOBJ_REF)
					{
						void* Obj = *(void**)KeyData;
						if (Obj) Engine->GCEnumCallback(Obj);
					}
					else if ((KeyType->GetFlags() & asOBJ_VALUE) && (KeyType->GetFlags() & asOBJ_GC))
					{
						void* Obj = *(void**)KeyData;
						if (Obj) Engine->ForwardGCEnumReferences(Obj, KeyType);
					}
				}

				// Enumerate value
				if (ValueType)
				{
					if (ValueType->GetFlags() & asOBJ_REF)
					{
						void* Obj = *(void**)ValueData;
						if (Obj) Engine->GCEnumCallback(Obj);
					}
					else if ((ValueType->GetFlags() & asOBJ_VALUE) && (ValueType->GetFlags() & asOBJ_GC))
					{
						void* Obj = *(void**)ValueData;
						if (Obj) Engine->ForwardGCEnumReferences(Obj, ValueType);
					}
				}
			}
		}

		void ReleaseAllHandles(asIScriptEngine*)
		{
			Clear();
		}

	private:
		asITypeInfo*         ObjType;
		FScriptTMapBuffer*   Buffer;
		int                  KeyElementSize;
		int                  ValueElementSize;
		int                  PairSize;
		int                  KeySubTypeId;
		int                  ValueSubTypeId;

		uint8* GetKeyPtr(asUINT Index) const
		{
			return Buffer->Data + Index * PairSize;
		}

		uint8* GetValuePtr(asUINT Index) const
		{
			return Buffer->Data + Index * PairSize + KeyElementSize;
		}

		void Precache()
		{
			KeySubTypeId = ObjType->GetSubTypeId(0);
			ValueSubTypeId = ObjType->GetSubTypeId(1);

			KeyElementSize = GetElementSize(ObjType, 0);
			ValueElementSize = GetElementSize(ObjType, 1);
			PairSize = KeyElementSize + ValueElementSize;

			if (!(KeySubTypeId & ~asTYPEID_MASK_SEQNBR)) return;

			FScriptTMapCache* Cache = reinterpret_cast<FScriptTMapCache*>(ObjType->GetUserData(TMAP_CACHE));
			if (Cache) return;

			asAcquireExclusiveLock();

			Cache = reinterpret_cast<FScriptTMapCache*>(ObjType->GetUserData(TMAP_CACHE));
			if (!Cache)
			{
				Cache = reinterpret_cast<FScriptTMapCache*>(FMemory::Malloc(sizeof(FScriptTMapCache)));
				if (!Cache)
				{
					asReleaseExclusiveLock();
					return;
				}
				FMemory::Memset(Cache, 0, sizeof(FScriptTMapCache));

				bool bMustBeConst = (KeySubTypeId & asTYPEID_HANDLETOCONST) ? true : false;
				asITypeInfo* KeyType = ObjType->GetEngine()->GetTypeInfoById(KeySubTypeId);
				if (KeyType)
				{
					for (asUINT i = 0; i < KeyType->GetMethodCount(); i++)
					{
						asIScriptFunction* Func = KeyType->GetMethodByIndex(i);
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
						    (KeySubTypeId & ~(asTYPEID_OBJHANDLE | asTYPEID_HANDLETOCONST)))
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

						if (Cache->KeyEqFunc || Cache->KeyEqFuncReturnCode)
						{
							Cache->KeyEqFunc = nullptr;
							Cache->KeyEqFuncReturnCode = asMULTIPLE_FUNCTIONS;
						}
						else
							Cache->KeyEqFunc = Func;
					}
				}

				if (!Cache->KeyEqFunc && Cache->KeyEqFuncReturnCode == 0)
					Cache->KeyEqFuncReturnCode = asNO_FUNCTION;

				ObjType->SetUserData(Cache, TMAP_CACHE);
			}
			asReleaseExclusiveLock();
		}

		int FindIndex(const void* Key) const
		{
			FScriptTMapCache* Cache = nullptr;
			if (KeySubTypeId & ~asTYPEID_MASK_SEQNBR)
			{
				Cache = reinterpret_cast<FScriptTMapCache*>(ObjType->GetUserData(TMAP_CACHE));
				if (!Cache || !Cache->KeyEqFunc)
				{
					if (asIScriptContext* Ctx = asGetActiveContext())
						Ctx->SetException("Key type does not support map operations - no opEquals method");
					return -1;
				}
			}

			asIScriptContext* CmpCtx = nullptr;
			bool bIsNested = false;
			if (KeySubTypeId & ~asTYPEID_MASK_SEQNBR)
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
				if (KeysEqual(GetKeyPtr(i), Key, CmpCtx, Cache))
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

		bool KeysEqual(const void* A, const void* B, asIScriptContext* Ctx, FScriptTMapCache* Cache) const
		{
			if (!(KeySubTypeId & ~asTYPEID_MASK_SEQNBR))
			{
				switch (KeySubTypeId)
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
				if ((KeySubTypeId & asTYPEID_OBJHANDLE) &&
				    *(const void* const*)A == *(const void* const*)B)
					return true;

				if (Cache && Cache->KeyEqFunc && Ctx)
				{
					if (Ctx->Prepare(Cache->KeyEqFunc) < 0) return false;
					if (KeySubTypeId & asTYPEID_OBJHANDLE)
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

		void SetKeyAt(asUINT Index, const void* Key)
		{
			void* Dst = (void*)(GetKeyPtr(Index));
			if (!Dst)
				return;

			if ((KeySubTypeId & ~asTYPEID_MASK_SEQNBR) && !(KeySubTypeId & asTYPEID_OBJHANDLE))
			{
				ObjType->GetEngine()->AssignScriptObject(Dst, const_cast<void*>(Key), ObjType->GetSubType(0));
			}
			else if (KeySubTypeId & asTYPEID_OBJHANDLE)
			{
				void* Tmp = *(void**)Dst;
				*(void**)Dst = *(void**)Key;
				ObjType->GetEngine()->AddRefScriptObject(*(void**)Key, ObjType->GetSubType(0));
				if (Tmp)
					ObjType->GetEngine()->ReleaseScriptObject(Tmp, ObjType->GetSubType(0));
			}
			else if (KeySubTypeId == asTYPEID_BOOL || KeySubTypeId == asTYPEID_INT8 || KeySubTypeId == asTYPEID_UINT8)
				*(char*)Dst = *(char*)Key;
			else if (KeySubTypeId == asTYPEID_INT16 || KeySubTypeId == asTYPEID_UINT16)
				*(short*)Dst = *(short*)Key;
			else if (KeySubTypeId == asTYPEID_INT32 || KeySubTypeId == asTYPEID_UINT32 ||
			         KeySubTypeId == asTYPEID_FLOAT || KeySubTypeId > asTYPEID_DOUBLE)
				*(int*)Dst = *(int*)Key;
			else if (KeySubTypeId == asTYPEID_INT64 || KeySubTypeId == asTYPEID_UINT64 || KeySubTypeId == asTYPEID_DOUBLE)
				*(double*)Dst = *(double*)Key;
		}

		void SetValueAt(asUINT Index, const void* Value)
		{
			void* Dst = (void*)(GetValuePtr(Index));
			if (!Dst) return;

			if ((ValueSubTypeId & ~asTYPEID_MASK_SEQNBR) && !(ValueSubTypeId & asTYPEID_OBJHANDLE))
			{
				ObjType->GetEngine()->AssignScriptObject(Dst, const_cast<void*>(Value), ObjType->GetSubType(1));
			}
			else if (ValueSubTypeId & asTYPEID_OBJHANDLE)
			{
				void* Tmp = *(void**)Dst;
				*(void**)Dst = *(void**)Value;
				ObjType->GetEngine()->AddRefScriptObject(*(void**)Value, ObjType->GetSubType(1));
				if (Tmp)
					ObjType->GetEngine()->ReleaseScriptObject(Tmp, ObjType->GetSubType(1));
			}
			else if (ValueSubTypeId == asTYPEID_BOOL || ValueSubTypeId == asTYPEID_INT8 || ValueSubTypeId == asTYPEID_UINT8)
				*(char*)Dst = *(char*)Value;
			else if (ValueSubTypeId == asTYPEID_INT16 || ValueSubTypeId == asTYPEID_UINT16)
				*(short*)Dst = *(short*)Value;
			else if (ValueSubTypeId == asTYPEID_INT32 || ValueSubTypeId == asTYPEID_UINT32 ||
			         ValueSubTypeId == asTYPEID_FLOAT || ValueSubTypeId > asTYPEID_DOUBLE)
				*(int*)Dst = *(int*)Value;
			else if (ValueSubTypeId == asTYPEID_INT64 || ValueSubTypeId == asTYPEID_UINT64 || ValueSubTypeId == asTYPEID_DOUBLE)
				*(double*)Dst = *(double*)Value;
		}

		bool CheckMaxSize(asUINT NumElems)
		{
			asUINT MaxSize = 0xFFFFFFFFul - (asUINT)sizeof(FScriptTMapBuffer) + 1;
			if (PairSize > 0)
				MaxSize /= (asUINT)PairSize;
			if (NumElems > MaxSize)
			{
				if (asIScriptContext* Ctx = asGetActiveContext())
					Ctx->SetException("TMap size too large");
				return false;
			}
			return true;
		}

		void CreateBuffer(FScriptTMapBuffer** Buf, asUINT NumElems)
		{
			*Buf = reinterpret_cast<FScriptTMapBuffer*>(
				FMemory::Malloc(sizeof(FScriptTMapBuffer) - 1 + PairSize * NumElems));
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

		void DeleteBuffer(FScriptTMapBuffer* Buf)
		{
			DestructRange(Buf, 0, Buf->NumElements);
			FMemory::Free(Buf);
		}

		void ConstructRange(FScriptTMapBuffer* Buf, asUINT Start, asUINT End)
		{
			asIScriptEngine* Engine = ObjType->GetEngine();
			for (asUINT i = Start; i < End; i++)
			{
				uint8* KeyPtr = Buf->Data + i * PairSize;
				uint8* ValuePtr = KeyPtr + KeyElementSize;

				// Construct key
				if ((KeySubTypeId & asTYPEID_MASK_OBJECT) && !(KeySubTypeId & asTYPEID_OBJHANDLE))
				{
					void** KeySlot = (void**)KeyPtr;
					*KeySlot = Engine->CreateScriptObject(ObjType->GetSubType(0));
					if (!*KeySlot)
					{
						FMemory::Memset(KeyPtr, 0, PairSize);
						return;
					}
				}
				else
				{
					FMemory::Memset(KeyPtr, 0, KeyElementSize);
				}

				// Construct value
				if ((ValueSubTypeId & asTYPEID_MASK_OBJECT) && !(ValueSubTypeId & asTYPEID_OBJHANDLE))
				{
					void** ValueSlot = (void**)ValuePtr;
					*ValueSlot = Engine->CreateScriptObject(ObjType->GetSubType(1));
					if (!*ValueSlot)
					{
						FMemory::Memset(ValuePtr, 0, ValueElementSize);
						return;
					}
				}
				else
				{
					FMemory::Memset(ValuePtr, 0, ValueElementSize);
				}
			}
		}

		void DestructRange(FScriptTMapBuffer* Buf, asUINT Start, asUINT End)
		{
			asIScriptEngine* Engine = ObjType->GetEngine();
			for (asUINT i = Start; i < End; i++)
			{
				uint8* KeyPtr = Buf->Data + i * PairSize;
				uint8* ValuePtr = KeyPtr + KeyElementSize;

				if (KeySubTypeId & asTYPEID_MASK_OBJECT)
				{
					void* Obj = *(void**)KeyPtr;
					if (Obj) Engine->ReleaseScriptObject(Obj, ObjType->GetSubType(0));
				}
				if (ValueSubTypeId & asTYPEID_MASK_OBJECT)
				{
					void* Obj = *(void**)ValuePtr;
					if (Obj) Engine->ReleaseScriptObject(Obj, ObjType->GetSubType(1));
				}
			}
		}

		void CopyPair(FScriptTMapBuffer* Dst, asUINT DstIndex, FScriptTMapBuffer* Src, asUINT SrcIndex)
		{
			uint8* DstKey = Dst->Data + DstIndex * PairSize;
			uint8* DstValue = DstKey + KeyElementSize;
			uint8* SrcKey = Src->Data + SrcIndex * PairSize;
			uint8* SrcValue = SrcKey + KeyElementSize;

			// Copy key
			if (KeySubTypeId & asTYPEID_OBJHANDLE)
			{
				*(void**)DstKey = *(void**)SrcKey;
				if (*(void**)DstKey)
					ObjType->GetEngine()->AddRefScriptObject(*(void**)DstKey, ObjType->GetSubType(0));
			}
			else if (KeySubTypeId & asTYPEID_MASK_OBJECT)
			{
				ObjType->GetEngine()->AssignScriptObject(*(void**)DstKey, *(void**)SrcKey, ObjType->GetSubType(0));
			}
			else
			{
				FMemory::Memcpy(DstKey, SrcKey, KeyElementSize);
			}

			// Copy value
			if (ValueSubTypeId & asTYPEID_OBJHANDLE)
			{
				*(void**)DstValue = *(void**)SrcValue;
				if (*(void**)DstValue)
					ObjType->GetEngine()->AddRefScriptObject(*(void**)DstValue, ObjType->GetSubType(1));
			}
			else if (ValueSubTypeId & asTYPEID_MASK_OBJECT)
			{
				ObjType->GetEngine()->AssignScriptObject(*(void**)DstValue, *(void**)SrcValue, ObjType->GetSubType(1));
			}
			else
			{
				FMemory::Memcpy(DstValue, SrcValue, ValueElementSize);
			}
		}

		void DestructPair(FScriptTMapBuffer* Buf, asUINT Index)
		{
			uint8* KeyPtr = Buf->Data + Index * PairSize;
			uint8* ValuePtr = KeyPtr + KeyElementSize;

			if (KeySubTypeId & asTYPEID_MASK_OBJECT)
			{
				void* Obj = *(void**)KeyPtr;
				if (Obj) ObjType->GetEngine()->ReleaseScriptObject(Obj, ObjType->GetSubType(0));
			}
			if (ValueSubTypeId & asTYPEID_MASK_OBJECT)
			{
				void* Obj = *(void**)ValuePtr;
				if (Obj) ObjType->GetEngine()->ReleaseScriptObject(Obj, ObjType->GetSubType(1));
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
				FScriptTMapBuffer* NewBuffer = reinterpret_cast<FScriptTMapBuffer*>(
					FMemory::Malloc(sizeof(FScriptTMapBuffer) - 1 +
					                (size_t)PairSize * (Buffer->NumElements + Delta)));
				if (!NewBuffer)
				{
					if (asIScriptContext* Ctx = asGetActiveContext())
						Ctx->SetException("Out of memory");
					return;
				}
				NewBuffer->NumElements = Buffer->NumElements + Delta;
				NewBuffer->MaxElements = NewBuffer->NumElements;

				// Copy elements before At
				for (asUINT i = 0; i < At; i++)
					CopyPair(NewBuffer, i, Buffer, i);

				// Copy elements after At
				for (asUINT i = At; i < Buffer->NumElements; i++)
					CopyPair(NewBuffer, i + Delta, Buffer, i);

				// Construct new elements
				if (Delta > 0)
				{
					for (asUINT i = At; i < At + (asUINT)Delta; i++)
					{
						FMemory::Memset(NewBuffer->Data + i * PairSize, 0, PairSize);
					}
				}

				FMemory::Free(Buffer);
				Buffer = NewBuffer;
			}
			else if (Delta < 0)
			{
				DestructRange(Buffer, At, At - Delta);
				FMemory::Memmove(
					Buffer->Data + At * PairSize,
					Buffer->Data + (At - Delta) * PairSize,
					(Buffer->NumElements - (At - Delta)) * PairSize);
				Buffer->NumElements += Delta;
			}
			else
			{
				FMemory::Memmove(
					Buffer->Data + (At + Delta) * PairSize,
					Buffer->Data + At * PairSize,
					(Buffer->NumElements - At) * PairSize);
				FMemory::Memset(Buffer->Data + At * PairSize, 0, (size_t)Delta * PairSize);
				Buffer->NumElements += Delta;
			}
		}
	};

	// ---------------------------------------------------------------------------
	// Value-type CONSTRUCT / DESTRUCT helpers
	// ---------------------------------------------------------------------------

	static void TMap_Construct(asITypeInfo* TypeInfo, FScriptTMap* Self)
	{
		new (Self) FScriptTMap(asUINT(0), TypeInfo);
	}

	static void TMap_ConstructWithCapacity(asITypeInfo* TypeInfo, asUINT InitialCapacity, FScriptTMap* Self)
	{
		new (Self) FScriptTMap(InitialCapacity, TypeInfo);
	}

	static void TMap_CopyConstruct(asITypeInfo* TypeInfo, const FScriptTMap& Other, FScriptTMap* Self)
	{
		(void)TypeInfo;
		new (Self) FScriptTMap(Other);
	}

	static void TMap_Destruct(FScriptTMap* Self)
	{
		Self->~FScriptTMap();
	}

	// ---------------------------------------------------------------------------
	// For-loop iteration helpers (iterate over keys)
	// ---------------------------------------------------------------------------

	static asUINT TMap_opForBegin(const FScriptTMap*) { return 0; }
	static bool   TMap_opForEnd  (asUINT Iter, const FScriptTMap* Map) { return !Map || Map->GetSize() <= Iter; }
	static asUINT TMap_opForNext (asUINT Iter, const FScriptTMap*) { return Iter + 1; }
}

void Declare_TMap(asIScriptEngine* Engine)
{
	const asDWORD TypeFlags =
		asOBJ_VALUE | asOBJ_GC | asOBJ_TEMPLATE |
		asOBJ_APP_CLASS | asOBJ_APP_CLASS_CONSTRUCTOR | asOBJ_APP_CLASS_DESTRUCTOR |
		asOBJ_APP_CLASS_COPY_CONSTRUCTOR | asOBJ_APP_CLASS_ASSIGNMENT |
		asOBJ_APP_CLASS_MORE_CONSTRUCTORS;

	int Result = Engine->RegisterObjectType("TMap<class K, class V>", sizeof(FScriptTMap), TypeFlags);
	check(Result >= 0);
}

void Bind_TMap(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	Engine->SetTypeInfoUserDataCleanupCallback(CleanupTypeInfoTMapCache, TMAP_CACHE);

	int Result = 0;

	Result = Engine->RegisterObjectBehaviour("TMap<K,V>", asBEHAVE_TEMPLATE_CALLBACK,
		"bool f(int&in, bool&out)", asFUNCTION(TMapTemplateCallback), asCALL_CDECL);
	check(Result >= 0);

	// Default constructor
	Result = Engine->RegisterObjectBehaviour("TMap<K,V>", asBEHAVE_CONSTRUCT,
		"void f(int&in)",
		asFUNCTION(TMap_Construct), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	// Capacity constructor
	Result = Engine->RegisterObjectBehaviour("TMap<K,V>", asBEHAVE_CONSTRUCT,
		"void f(int&in, uint initialCapacity) explicit",
		asFUNCTION(TMap_ConstructWithCapacity), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	// Copy constructor
	Result = Engine->RegisterObjectBehaviour("TMap<K,V>", asBEHAVE_CONSTRUCT,
		"void f(int&in, const TMap<K,V>&in)",
		asFUNCTION(TMap_CopyConstruct), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	Result = Engine->RegisterObjectBehaviour("TMap<K,V>", asBEHAVE_DESTRUCT,
		"void f()",
		asFUNCTION(TMap_Destruct), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	// For-range iteration (over keys)
	Result = Engine->RegisterObjectMethod("TMap<K,V>",
		"uint opForBegin() const",
		asFUNCTIONPR(TMap_opForBegin, (const FScriptTMap*), asUINT), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TMap<K,V>",
		"bool opForEnd(uint) const",
		asFUNCTIONPR(TMap_opForEnd, (asUINT, const FScriptTMap*), bool), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TMap<K,V>",
		"uint opForNext(uint) const",
		asFUNCTIONPR(TMap_opForNext, (asUINT, const FScriptTMap*), asUINT), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TMap<K,V>",
		"const K &opForValue0(uint index) const",
		asMETHODPR(FScriptTMap, GetKeyAt, (asUINT) const, const void*), asCALL_THISCALL);
	check(Result >= 0);

	// Assignment
	Result = Engine->RegisterObjectMethod("TMap<K,V>",
		"TMap<K,V> &opAssign(const TMap<K,V>&in)",
		asMETHOD(FScriptTMap, operator=), asCALL_THISCALL);
	check(Result >= 0);

	// Number of entries
	Result = Engine->RegisterObjectMethod("TMap<K,V>",
		"uint length() const",
		asMETHOD(FScriptTMap, GetSize), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TMap<K,V>",
		"bool isEmpty() const",
		asMETHOD(FScriptTMap, IsEmpty), asCALL_THISCALL);
	check(Result >= 0);

	// Core map operations
	Result = Engine->RegisterObjectMethod("TMap<K,V>",
		"void set(const K &in key, const V &in value)",
		asMETHODPR(FScriptTMap, Set, (const void*, const void*), void), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TMap<K,V>",
		"bool contains(const K &in key) const",
		asMETHODPR(FScriptTMap, Contains, (const void*) const, bool), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TMap<K,V>",
		"bool remove(const K &in key)",
		asMETHODPR(FScriptTMap, Remove, (const void*), bool), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TMap<K,V>",
		"void clear()",
		asMETHOD(FScriptTMap, Clear), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TMap<K,V>",
		"void removeIndex(uint index)",
		asMETHOD(FScriptTMap, RemoveIndex), asCALL_THISCALL);
	check(Result >= 0);

	// Key/value access by position
	Result = Engine->RegisterObjectMethod("TMap<K,V>",
		"const K &getKey(uint index) const",
		asMETHODPR(FScriptTMap, GetKeyAt, (asUINT) const, const void*), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TMap<K,V>",
		"const V &getValue(uint index) const",
		asMETHODPR(FScriptTMap, GetValueAt, (asUINT) const, const void*), asCALL_THISCALL);
	check(Result >= 0);

	// Key-based value access
	Result = Engine->RegisterObjectMethod("TMap<K,V>",
		"const V &opIndex(const K &in key) const",
		asMETHODPR(FScriptTMap, GetValueByKey, (const void*) const, const void*), asCALL_THISCALL);
	check(Result >= 0);

	// GC behaviors
	Result = Engine->RegisterObjectBehaviour("TMap<K,V>", asBEHAVE_ENUMREFS,
		"void f(int&in)", asMETHOD(FScriptTMap, EnumReferences), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectBehaviour("TMap<K,V>", asBEHAVE_RELEASEREFS,
		"void f(int&in)", asMETHOD(FScriptTMap, ReleaseAllHandles), asCALL_THISCALL);
	check(Result >= 0);
}
