// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Misc/AssertionMacros.h"
#include "UObject/Object.h"
#include "UObject/WeakObjectPtr.h"
#include "angelscript.h"

#include <new>

namespace
{
	using FTWeakObjectPtrBase = TWeakObjectPtr<UObject>;

	// Template callback: only allow ref types (UObject-derived handles) as T.
	static bool TWeakObjectPtrTemplateCallback(asITypeInfo* TypeInfo, bool& bDontGarbageCollect)
	{
		int SubTypeId = TypeInfo->GetSubTypeId();

		if (SubTypeId == asTYPEID_VOID)
		{
			return false;
		}

		if (SubTypeId & asTYPEID_MASK_OBJECT)
		{
			asITypeInfo* SubType = TypeInfo->GetEngine()->GetTypeInfoById(SubTypeId);
			if (SubType && (SubType->GetFlags() & asOBJ_REF))
			{
				bDontGarbageCollect = true;
				return true;
			}
		}

		TypeInfo->GetEngine()->WriteMessage("TWeakObjectPtr", 0, 0, asMSGTYPE_ERROR,
			"TWeakObjectPtr<T> requires T to be a reference type (UObject-derived)");
		return false;
	}

	// --- Constructors / destructor ---
	// Template constructors receive asITypeInfo* as first C++ argument (from "int&in" in declaration).

	static void TWeakObjectPtr_DefaultConstruct(asITypeInfo* /*TypeInfo*/, FTWeakObjectPtrBase* Memory)
	{
		new (Memory) FTWeakObjectPtrBase();
	}

	static void TWeakObjectPtr_CopyConstruct(asITypeInfo* /*TypeInfo*/, const FTWeakObjectPtrBase& Other, FTWeakObjectPtrBase* Memory)
	{
		new (Memory) FTWeakObjectPtrBase(Other);
	}

	static void TWeakObjectPtr_ConstructFromObject(asITypeInfo* /*TypeInfo*/, UObject* Object, FTWeakObjectPtrBase* Memory)
	{
		new (Memory) FTWeakObjectPtrBase(Object);
	}

	static void TWeakObjectPtr_Destruct(FTWeakObjectPtrBase* Memory)
	{
		Memory->~FTWeakObjectPtrBase();
	}

	// --- Assignment ---

	static FTWeakObjectPtrBase& TWeakObjectPtr_CopyAssign(FTWeakObjectPtrBase& Self, const FTWeakObjectPtrBase& Other)
	{
		Self = Other;
		return Self;
	}

	static FTWeakObjectPtrBase& TWeakObjectPtr_AssignObject(FTWeakObjectPtrBase& Self, UObject* Object)
	{
		Self = Object;
		return Self;
	}

	// --- Equality ---

	static bool TWeakObjectPtr_Equals(const FTWeakObjectPtrBase& Self, const FTWeakObjectPtrBase& Other)
	{
		return Self == Other;
	}

	static bool TWeakObjectPtr_EqualsObject(const FTWeakObjectPtrBase& Self, UObject* Object)
	{
		return Self == Object;
	}

	// --- State queries ---

	static bool TWeakObjectPtr_IsStale(const FTWeakObjectPtrBase& Self)
	{
		return Self.IsStale();
	}

	// --- Object access ---

	static UObject* TWeakObjectPtr_Get(const FTWeakObjectPtrBase& Self)
	{
		return Self.Get();
	}

	static UObject* TWeakObjectPtr_GetEvenIfUnreachable(const FTWeakObjectPtrBase& Self)
	{
		return Self.GetEvenIfUnreachable();
	}

	// --- HasSameIndexAndSerialNumber ---

	static bool TWeakObjectPtr_HasSameIndexAndSerialNumber(const FTWeakObjectPtrBase& Self, const FTWeakObjectPtrBase& Other)
	{
		return Self.HasSameIndexAndSerialNumber(Other);
	}
}

void Declare_TWeakObjectPtr(asIScriptEngine* Engine)
{
	const asDWORD TypeFlags =
		asOBJ_VALUE | asOBJ_TEMPLATE |
		asOBJ_APP_CLASS | asOBJ_APP_CLASS_CONSTRUCTOR | asOBJ_APP_CLASS_DESTRUCTOR |
		asOBJ_APP_CLASS_COPY_CONSTRUCTOR | asOBJ_APP_CLASS_ASSIGNMENT |
		asOBJ_APP_CLASS_MORE_CONSTRUCTORS;

	int Result = Engine->RegisterObjectType("TWeakObjectPtr<class T>", sizeof(FTWeakObjectPtrBase), TypeFlags);
	check(Result >= 0);

	Result = Engine->RegisterObjectBehaviour("TWeakObjectPtr<T>", asBEHAVE_TEMPLATE_CALLBACK,
		"bool f(int&in, bool&out)", asFUNCTION(TWeakObjectPtrTemplateCallback), asCALL_CDECL);
	check(Result >= 0);

	// Constructors / destructor
	// Template constructors must declare "int&in" as first parameter (receives asITypeInfo*).
	Result = Engine->RegisterObjectBehaviour("TWeakObjectPtr<T>", asBEHAVE_CONSTRUCT,
		"void f(int&in)", asFUNCTION(TWeakObjectPtr_DefaultConstruct), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	Result = Engine->RegisterObjectBehaviour("TWeakObjectPtr<T>", asBEHAVE_CONSTRUCT,
		"void f(int&in, const TWeakObjectPtr<T> &in Other)", asFUNCTION(TWeakObjectPtr_CopyConstruct), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	Result = Engine->RegisterObjectBehaviour("TWeakObjectPtr<T>", asBEHAVE_CONSTRUCT,
		"void f(int&in, T@ Object)", asFUNCTION(TWeakObjectPtr_ConstructFromObject), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	Result = Engine->RegisterObjectBehaviour("TWeakObjectPtr<T>", asBEHAVE_DESTRUCT,
		"void f()", asFUNCTION(TWeakObjectPtr_Destruct), asCALL_CDECL_OBJLAST);
	check(Result >= 0);
}

void Bind_TWeakObjectPtr(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = 0;

	// Assignment
	Result = Engine->RegisterObjectMethod("TWeakObjectPtr<T>",
		"TWeakObjectPtr<T> &opAssign(const TWeakObjectPtr<T> &in Other)",
		asFUNCTION(TWeakObjectPtr_CopyAssign), asCALL_CDECL_OBJFIRST);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TWeakObjectPtr<T>",
		"TWeakObjectPtr<T> &opAssign(T@ Object)",
		asFUNCTION(TWeakObjectPtr_AssignObject), asCALL_CDECL_OBJFIRST);
	check(Result >= 0);

	// Equality
	Result = Engine->RegisterObjectMethod("TWeakObjectPtr<T>",
		"bool opEquals(const TWeakObjectPtr<T> &in Other) const",
		asFUNCTION(TWeakObjectPtr_Equals), asCALL_CDECL_OBJFIRST);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TWeakObjectPtr<T>",
		"bool opEquals(T@ Object) const",
		asFUNCTION(TWeakObjectPtr_EqualsObject), asCALL_CDECL_OBJFIRST);
	check(Result >= 0);

	// State queries
	Result = Engine->RegisterObjectMethod("TWeakObjectPtr<T>",
		"bool IsValid() const",
		asMETHODPR(FTWeakObjectPtrBase, IsValid, () const, bool), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TWeakObjectPtr<T>",
		"bool IsStale() const",
		asFUNCTION(TWeakObjectPtr_IsStale), asCALL_CDECL_OBJFIRST);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TWeakObjectPtr<T>",
		"bool IsExplicitlyNull() const",
		asMETHODPR(FTWeakObjectPtrBase, IsExplicitlyNull, () const, bool), asCALL_THISCALL);
	check(Result >= 0);

	// Reset
	Result = Engine->RegisterObjectMethod("TWeakObjectPtr<T>",
		"void Reset()",
		asMETHODPR(FTWeakObjectPtrBase, Reset, (), void), asCALL_THISCALL);
	check(Result >= 0);

	// Object access — return T@ so the handle type matches the instantiation.
	Result = Engine->RegisterObjectMethod("TWeakObjectPtr<T>",
		"T@ Get() const",
		asFUNCTION(TWeakObjectPtr_Get), asCALL_CDECL_OBJFIRST);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TWeakObjectPtr<T>",
		"T@ GetEvenIfUnreachable() const",
		asFUNCTION(TWeakObjectPtr_GetEvenIfUnreachable), asCALL_CDECL_OBJFIRST);
	check(Result >= 0);

	// Index/serial comparison
	Result = Engine->RegisterObjectMethod("TWeakObjectPtr<T>",
		"bool HasSameIndexAndSerialNumber(const TWeakObjectPtr<T> &in Other) const",
		asFUNCTION(TWeakObjectPtr_HasSameIndexAndSerialNumber), asCALL_CDECL_OBJFIRST);
	check(Result >= 0);
}
