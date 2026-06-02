// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Misc/AssertionMacros.h"
#include "UObject/Class.h"
#include "UObject/Object.h"
#include "UObject/SoftObjectPtr.h"
#include "angelscript.h"

#include <new>

namespace
{
	using FSoftClassPtrBase = TSoftClassPtr<UObject>;

	// Template callback: only allow ref types (UObject-derived handles) as T.
	static bool TSoftClassPtrTemplateCallback(asITypeInfo* TypeInfo, bool& bDontGarbageCollect)
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

		TypeInfo->GetEngine()->WriteMessage("TSoftClassPtr", 0, 0, asMSGTYPE_ERROR,
			"TSoftClassPtr<T> requires T to be a reference type (UObject-derived)");
		return false;
	}

	// --- Constructors / destructor ---
	// Template constructors receive asITypeInfo* as first C++ argument (from "int&in" in declaration).

	static void TSoftClassPtr_DefaultConstruct(asITypeInfo* /*TypeInfo*/, FSoftClassPtrBase* Memory)
	{
		new (Memory) FSoftClassPtrBase();
	}

	static void TSoftClassPtr_CopyConstruct(asITypeInfo* /*TypeInfo*/, const FSoftClassPtrBase& Other, FSoftClassPtrBase* Memory)
	{
		new (Memory) FSoftClassPtrBase(Other);
	}

	static void TSoftClassPtr_ConstructFromClass(asITypeInfo* /*TypeInfo*/, UClass* Class, FSoftClassPtrBase* Memory)
	{
		new (Memory) FSoftClassPtrBase(Class);
	}

	static void TSoftClassPtr_ConstructFromPath(asITypeInfo* /*TypeInfo*/, const FSoftObjectPath& Path, FSoftClassPtrBase* Memory)
	{
		new (Memory) FSoftClassPtrBase(Path);
	}

	static void TSoftClassPtr_Destruct(FSoftClassPtrBase* Memory)
	{
		Memory->~FSoftClassPtrBase();
	}

	// --- Assignment ---

	static FSoftClassPtrBase& TSoftClassPtr_AssignClass(FSoftClassPtrBase& Self, UClass* Class)
	{
		Self = Class;
		return Self;
	}

	static FSoftClassPtrBase& TSoftClassPtr_AssignPath(FSoftClassPtrBase& Self, const FSoftObjectPath& Path)
	{
		Self = Path;
		return Self;
	}

	// --- Equality with raw class pointer ---

	static bool TSoftClassPtr_EqualsClass(const FSoftClassPtrBase& Self, UClass* Class)
	{
		return Self.Get() == Class;
	}

	// --- Get / load helpers ---
	// TSoftClassPtr::Get() returns UClass* rather than UObject*, but still needs wrapping.

	static UClass* TSoftClassPtr_Get(const FSoftClassPtrBase& Self)
	{
		return Self.Get();
	}

	static UClass* TSoftClassPtr_LoadSynchronous(const FSoftClassPtrBase& Self)
	{
		return Self.LoadSynchronous();
	}

	// ToSoftObjectPath returns const FSoftObjectPath& -- wrap to return by value for script safety.
	static FSoftObjectPath TSoftClassPtr_ToSoftObjectPath(const FSoftClassPtrBase& Self)
	{
		return Self.ToSoftObjectPath();
	}
}

void Declare_TSoftClassPtr(asIScriptEngine* Engine)
{
	const asDWORD TypeFlags =
		asOBJ_VALUE | asOBJ_TEMPLATE |
		asOBJ_APP_CLASS | asOBJ_APP_CLASS_CONSTRUCTOR | asOBJ_APP_CLASS_DESTRUCTOR |
		asOBJ_APP_CLASS_COPY_CONSTRUCTOR | asOBJ_APP_CLASS_ASSIGNMENT |
		asOBJ_APP_CLASS_MORE_CONSTRUCTORS;

	int Result = Engine->RegisterObjectType("TSoftClassPtr<class T>", sizeof(FSoftClassPtrBase), TypeFlags);
	check(Result >= 0);

	// Constructors / destructor
	// Template constructors must declare "int&in" as first parameter (receives asITypeInfo*).
	Result = Engine->RegisterObjectBehaviour("TSoftClassPtr<T>", asBEHAVE_CONSTRUCT,
		"void f(int&in)", asFUNCTION(TSoftClassPtr_DefaultConstruct), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	Result = Engine->RegisterObjectBehaviour("TSoftClassPtr<T>", asBEHAVE_CONSTRUCT,
		"void f(int&in, const TSoftClassPtr<T> &in Other)", asFUNCTION(TSoftClassPtr_CopyConstruct), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	Result = Engine->RegisterObjectBehaviour("TSoftClassPtr<T>", asBEHAVE_CONSTRUCT,
		"void f(int&in, UClass@ Class)", asFUNCTION(TSoftClassPtr_ConstructFromClass), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	Result = Engine->RegisterObjectBehaviour("TSoftClassPtr<T>", asBEHAVE_CONSTRUCT,
		"void f(int&in, const FSoftObjectPath &in Path)", asFUNCTION(TSoftClassPtr_ConstructFromPath), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	Result = Engine->RegisterObjectBehaviour("TSoftClassPtr<T>", asBEHAVE_DESTRUCT,
		"void f()", asFUNCTION(TSoftClassPtr_Destruct), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	Result = Engine->RegisterObjectBehaviour("TSoftClassPtr<T>", asBEHAVE_TEMPLATE_CALLBACK,
		"bool f(int&in, bool&out)", asFUNCTION(TSoftClassPtrTemplateCallback), asCALL_CDECL);
	check(Result >= 0);
}

void Bind_TSoftClassPtr(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = 0;

	// Assignment
	Result = Engine->RegisterObjectMethod("TSoftClassPtr<T>",
		"TSoftClassPtr<T> &opAssign(const TSoftClassPtr<T> &in Other)",
		asMETHODPR(FSoftClassPtrBase, operator=, (const FSoftClassPtrBase&), FSoftClassPtrBase&), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSoftClassPtr<T>",
		"TSoftClassPtr<T> &opAssign(UClass@ Class)",
		asFUNCTION(TSoftClassPtr_AssignClass), asCALL_CDECL_OBJFIRST);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSoftClassPtr<T>",
		"TSoftClassPtr<T> &opAssign(const FSoftObjectPath &in Path)",
		asFUNCTION(TSoftClassPtr_AssignPath), asCALL_CDECL_OBJFIRST);
	check(Result >= 0);

	// Equality
	Result = Engine->RegisterObjectMethod("TSoftClassPtr<T>",
		"bool opEquals(const TSoftClassPtr<T> &in Other) const",
		asMETHODPR(FSoftClassPtrBase, operator==, (const FSoftClassPtrBase&) const, bool), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSoftClassPtr<T>",
		"bool opEquals(UClass@ Class) const",
		asFUNCTION(TSoftClassPtr_EqualsClass), asCALL_CDECL_OBJFIRST);
	check(Result >= 0);

	// State queries
	Result = Engine->RegisterObjectMethod("TSoftClassPtr<T>",
		"bool IsValid() const",
		asMETHODPR(FSoftClassPtrBase, IsValid, () const, bool), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSoftClassPtr<T>",
		"bool IsPending() const",
		asMETHODPR(FSoftClassPtrBase, IsPending, () const, bool), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSoftClassPtr<T>",
		"bool IsNull() const",
		asMETHODPR(FSoftClassPtrBase, IsNull, () const, bool), asCALL_THISCALL);
	check(Result >= 0);

	// Reset
	Result = Engine->RegisterObjectMethod("TSoftClassPtr<T>",
		"void Reset()",
		asMETHODPR(FSoftClassPtrBase, Reset, (), void), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSoftClassPtr<T>",
		"void ResetWeakPtr()",
		asMETHODPR(FSoftClassPtrBase, ResetWeakPtr, (), void), asCALL_THISCALL);
	check(Result >= 0);

	// Class access -- return UClass@ rather than T@ so the script sees it as a class handle
	Result = Engine->RegisterObjectMethod("TSoftClassPtr<T>",
		"UClass@ Get() const",
		asFUNCTION(TSoftClassPtr_Get), asCALL_CDECL_OBJFIRST);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSoftClassPtr<T>",
		"UClass@ LoadSynchronous() const",
		asFUNCTION(TSoftClassPtr_LoadSynchronous), asCALL_CDECL_OBJFIRST);
	check(Result >= 0);

	// Path accessors
	Result = Engine->RegisterObjectMethod("TSoftClassPtr<T>",
		"FSoftObjectPath ToSoftObjectPath() const",
		asFUNCTION(TSoftClassPtr_ToSoftObjectPath), asCALL_CDECL_OBJFIRST);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSoftClassPtr<T>",
		"FString ToString() const",
		asMETHODPR(FSoftClassPtrBase, ToString, () const, FString), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSoftClassPtr<T>",
		"FString GetLongPackageName() const",
		asMETHODPR(FSoftClassPtrBase, GetLongPackageName, () const, FString), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSoftClassPtr<T>",
		"FName GetLongPackageFName() const",
		asMETHODPR(FSoftClassPtrBase, GetLongPackageFName, () const, FName), asCALL_THISCALL);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSoftClassPtr<T>",
		"FString GetAssetName() const",
		asMETHODPR(FSoftClassPtrBase, GetAssetName, () const, FString), asCALL_THISCALL);
	check(Result >= 0);
}
