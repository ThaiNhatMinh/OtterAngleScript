// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Misc/AssertionMacros.h"
#include "UObject/Object.h"
#include "UObject/SoftObjectPtr.h"
#include "angelscript.h"

#include <new>

namespace
{
	using FSoftObjectPtrBase = TSoftObjectPtr<UObject>;

	// Template callback: only allow ref types (UObject-derived handles) as T.
	static bool TSoftObjectPtrTemplateCallback(asITypeInfo* TypeInfo, bool& bDontGarbageCollect)
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

		TypeInfo->GetEngine()->WriteMessage("TSoftObjectPtr", 0, 0, asMSGTYPE_ERROR,
			"TSoftObjectPtr<T> requires T to be a reference type (UObject-derived)");
		return false;
	}

	// --- Constructors / destructor ---
	// Template constructors receive asITypeInfo* as first C++ argument (from "int&in" in declaration).

	static void TSoftObjectPtr_DefaultConstruct(asITypeInfo* /*TypeInfo*/, FSoftObjectPtrBase* Memory)
	{
		new (Memory) FSoftObjectPtrBase();
	}

	static void TSoftObjectPtr_CopyConstruct(asITypeInfo* /*TypeInfo*/, const FSoftObjectPtrBase& Other, FSoftObjectPtrBase* Memory)
	{
		new (Memory) FSoftObjectPtrBase(Other);
	}

	static void TSoftObjectPtr_ConstructFromObject(asITypeInfo* /*TypeInfo*/, UObject* Object, FSoftObjectPtrBase* Memory)
	{
		new (Memory) FSoftObjectPtrBase(Object);
	}

	static void TSoftObjectPtr_ConstructFromPath(asITypeInfo* /*TypeInfo*/, const FSoftObjectPath& Path, FSoftObjectPtrBase* Memory)
	{
		new (Memory) FSoftObjectPtrBase(Path);
	}

	static void TSoftObjectPtr_Destruct(FSoftObjectPtrBase* Memory)
	{
		Memory->~FSoftObjectPtrBase();
	}

	// --- Assignment ---

	static FSoftObjectPtrBase& TSoftObjectPtr_AssignObject(FSoftObjectPtrBase& Self, UObject* Object)
	{
		Self = Object;
		return Self;
	}

	static FSoftObjectPtrBase& TSoftObjectPtr_AssignPath(FSoftObjectPtrBase& Self, const FSoftObjectPath& Path)
	{
		Self = Path;
		return Self;
	}

	// --- Equality with raw pointer ---

	static bool TSoftObjectPtr_EqualsObject(const FSoftObjectPtrBase& Self, UObject* Object)
	{
		return Self == Object;
	}

	// --- Get / load helpers ---
	// Wrapped because Get() and LoadSynchronous() are out-of-class template definitions.

	static UObject* TSoftObjectPtr_Get(const FSoftObjectPtrBase& Self)
	{
		return Self.Get();
	}

	static UObject* TSoftObjectPtr_LoadSynchronous(const FSoftObjectPtrBase& Self)
	{
		return Self.LoadSynchronous();
	}

	// ToSoftObjectPath returns const FSoftObjectPath& — wrap to return by value for script safety.
	static FSoftObjectPath TSoftObjectPtr_ToSoftObjectPath(const FSoftObjectPtrBase& Self)
	{
		return Self.ToSoftObjectPath();
	}
}

void Declare_TSoftObjectPtr(asIScriptEngine* Engine)
{
	const asDWORD TypeFlags =
		asOBJ_VALUE | asOBJ_TEMPLATE |
		asOBJ_APP_CLASS | asOBJ_APP_CLASS_CONSTRUCTOR | asOBJ_APP_CLASS_DESTRUCTOR |
		asOBJ_APP_CLASS_COPY_CONSTRUCTOR | asOBJ_APP_CLASS_ASSIGNMENT |
		asOBJ_APP_CLASS_MORE_CONSTRUCTORS;

	int Result = Engine->RegisterObjectType("TSoftObjectPtr<class T>", sizeof(FSoftObjectPtrBase), TypeFlags);
	check(Result >= 0);
}

void Bind_TSoftObjectPtr(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = 0;

	Result = Engine->RegisterObjectBehaviour("TSoftObjectPtr<T>", asBEHAVE_TEMPLATE_CALLBACK,
		"bool f(int&in, bool&out)", asFUNCTION(TSoftObjectPtrTemplateCallback), asCALL_CDECL);
	check(Result >= 0);

	// Constructors / destructor
	// Template constructors must declare "int&in" as first parameter (receives asITypeInfo*).
	Result = Engine->RegisterObjectBehaviour("TSoftObjectPtr<T>", asBEHAVE_CONSTRUCT,
		"void f(int&in)", asFUNCTION(TSoftObjectPtr_DefaultConstruct), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	Result = Engine->RegisterObjectBehaviour("TSoftObjectPtr<T>", asBEHAVE_CONSTRUCT,
		"void f(int&in, const TSoftObjectPtr<T> &in Other)", asFUNCTION(TSoftObjectPtr_CopyConstruct), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	Result = Engine->RegisterObjectBehaviour("TSoftObjectPtr<T>", asBEHAVE_CONSTRUCT,
		"void f(int&in, T@ Object)", asFUNCTION(TSoftObjectPtr_ConstructFromObject), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	Result = Engine->RegisterObjectBehaviour("TSoftObjectPtr<T>", asBEHAVE_CONSTRUCT,
		"void f(int&in, const FSoftObjectPath &in Path)", asFUNCTION(TSoftObjectPtr_ConstructFromPath), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	Result = Engine->RegisterObjectBehaviour("TSoftObjectPtr<T>", asBEHAVE_DESTRUCT,
		"void f()", asFUNCTION(TSoftObjectPtr_Destruct), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	// Assignment
	Result = Engine->RegisterObjectMethod("TSoftObjectPtr<T>",
		"TSoftObjectPtr<T> &opAssign(const TSoftObjectPtr<T> &in Other)",
		asMETHODPR(FSoftObjectPtrBase, operator=, (const FSoftObjectPtrBase&), FSoftObjectPtrBase&), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSoftObjectPtr<T>",
		"TSoftObjectPtr<T> &opAssign(T@ Object)",
		asFUNCTION(TSoftObjectPtr_AssignObject), asCALL_CDECL_OBJFIRST);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSoftObjectPtr<T>",
		"TSoftObjectPtr<T> &opAssign(const FSoftObjectPath &in Path)",
		asFUNCTION(TSoftObjectPtr_AssignPath), asCALL_CDECL_OBJFIRST);
	check(Result >= 0);

	// Equality
	Result = Engine->RegisterObjectMethod("TSoftObjectPtr<T>",
		"bool opEquals(const TSoftObjectPtr<T> &in Other) const",
		asMETHODPR(FSoftObjectPtrBase, operator==, (const FSoftObjectPtrBase&) const, bool), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSoftObjectPtr<T>",
		"bool opEquals(T@ Object) const",
		asFUNCTION(TSoftObjectPtr_EqualsObject), asCALL_CDECL_OBJFIRST);
	check(Result >= 0);

	// State queries
	Result = Engine->RegisterObjectMethod("TSoftObjectPtr<T>",
		"bool IsValid() const",
		asMETHODPR(FSoftObjectPtrBase, IsValid, () const, bool), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSoftObjectPtr<T>",
		"bool IsPending() const",
		asMETHODPR(FSoftObjectPtrBase, IsPending, () const, bool), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSoftObjectPtr<T>",
		"bool IsNull() const",
		asMETHODPR(FSoftObjectPtrBase, IsNull, () const, bool), asCALL_THISCALL);
	check(Result >= 0);

	// Reset
	Result = Engine->RegisterObjectMethod("TSoftObjectPtr<T>",
		"void Reset()",
		asMETHODPR(FSoftObjectPtrBase, Reset, (), void), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSoftObjectPtr<T>",
		"void ResetWeakPtr()",
		asMETHODPR(FSoftObjectPtrBase, ResetWeakPtr, (), void), asCALL_THISCALL);
	check(Result >= 0);

	// Object access — return T@ so the handle type matches the instantiation.
	Result = Engine->RegisterObjectMethod("TSoftObjectPtr<T>",
		"T@ Get() const",
		asFUNCTION(TSoftObjectPtr_Get), asCALL_CDECL_OBJFIRST);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSoftObjectPtr<T>",
		"T@ LoadSynchronous() const",
		asFUNCTION(TSoftObjectPtr_LoadSynchronous), asCALL_CDECL_OBJFIRST);
	check(Result >= 0);

	// Path accessors
	Result = Engine->RegisterObjectMethod("TSoftObjectPtr<T>",
		"FSoftObjectPath ToSoftObjectPath() const",
		asFUNCTION(TSoftObjectPtr_ToSoftObjectPath), asCALL_CDECL_OBJFIRST);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSoftObjectPtr<T>",
		"FString ToString() const",
		asMETHODPR(FSoftObjectPtrBase, ToString, () const, FString), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSoftObjectPtr<T>",
		"FString GetLongPackageName() const",
		asMETHODPR(FSoftObjectPtrBase, GetLongPackageName, () const, FString), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSoftObjectPtr<T>",
		"FName GetLongPackageFName() const",
		asMETHODPR(FSoftObjectPtrBase, GetLongPackageFName, () const, FName), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSoftObjectPtr<T>",
		"FString GetAssetName() const",
		asMETHODPR(FSoftObjectPtrBase, GetAssetName, () const, FString), asCALL_THISCALL);
	check(Result >= 0);
}
