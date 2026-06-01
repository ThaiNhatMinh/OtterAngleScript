// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Misc/AssertionMacros.h"
#include "Templates/SubclassOf.h"
#include "UObject/Class.h"
#include "UObject/Object.h"
#include "angelscript.h"

#include <new>

namespace
{
	using FTSubclassOfBase = TSubclassOf<UObject>;

	// Template callback: only allow ref types (UObject-derived) as T.
	static bool TSubclassOfTemplateCallback(asITypeInfo* TypeInfo, bool& bDontGarbageCollect)
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

		TypeInfo->GetEngine()->WriteMessage("TSubclassOf", 0, 0, asMSGTYPE_ERROR,
			"TSubclassOf<T> requires T to be a reference type (UObject-derived)");
		return false;
	}

	// --- Constructors / destructor ---
	// Template constructors receive asITypeInfo* as first C++ argument (from "int&in" in declaration).

	static void TSubclassOf_DefaultConstruct(asITypeInfo* /*TypeInfo*/, FTSubclassOfBase* Memory)
	{
		new (Memory) FTSubclassOfBase();
	}

	static void TSubclassOf_CopyConstruct(asITypeInfo* /*TypeInfo*/, const FTSubclassOfBase& Other, FTSubclassOfBase* Memory)
	{
		new (Memory) FTSubclassOfBase(Other);
	}

	static void TSubclassOf_ConstructFromClass(asITypeInfo* /*TypeInfo*/, UClass* Class, FTSubclassOfBase* Memory)
	{
		new (Memory) FTSubclassOfBase(Class);
	}

	static void TSubclassOf_Destruct(FTSubclassOfBase* Memory)
	{
		Memory->~FTSubclassOfBase();
	}

	// --- Assignment ---

	static FTSubclassOfBase& TSubclassOf_CopyAssign(FTSubclassOfBase& Self, const FTSubclassOfBase& Other)
	{
		Self = Other;
		return Self;
	}

	static FTSubclassOfBase& TSubclassOf_AssignFromClass(FTSubclassOfBase& Self, UClass* Class)
	{
		Self = Class;
		return Self;
	}

	// --- Equality ---

	static bool TSubclassOf_Equals(const FTSubclassOfBase& Self, const FTSubclassOfBase& Other)
	{
		return *Self == *Other;
	}

	// --- Class access ---

	// Get() returns the held UClass* with runtime type checking against T.
	static UClass* TSubclassOf_Get(const FTSubclassOfBase& Self)
	{
		return Self.Get();
	}

	// GetDefaultObject() returns the CDO; declared as T@ in script so AS uses the subtype.
	static UObject* TSubclassOf_GetDefaultObject(const FTSubclassOfBase& Self)
	{
		return Self.GetDefaultObject();
	}
}

void Declare_TSubclassOf(asIScriptEngine* Engine)
{
	const asDWORD TypeFlags =
		asOBJ_VALUE | asOBJ_TEMPLATE |
		asOBJ_APP_CLASS | asOBJ_APP_CLASS_CONSTRUCTOR | asOBJ_APP_CLASS_DESTRUCTOR |
		asOBJ_APP_CLASS_COPY_CONSTRUCTOR | asOBJ_APP_CLASS_ASSIGNMENT |
		asOBJ_APP_CLASS_MORE_CONSTRUCTORS;

	int Result = Engine->RegisterObjectType("TSubclassOf<class T>", sizeof(FTSubclassOfBase), TypeFlags);
	check(Result >= 0);
}

void Bind_TSubclassOf(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = 0;

	Result = Engine->RegisterObjectBehaviour("TSubclassOf<T>", asBEHAVE_TEMPLATE_CALLBACK,
		"bool f(int&in, bool&out)", asFUNCTION(TSubclassOfTemplateCallback), asCALL_CDECL);
	check(Result >= 0);

	// Constructors / destructor
	// Template constructors must declare "int&in" as first parameter (receives asITypeInfo*).
	Result = Engine->RegisterObjectBehaviour("TSubclassOf<T>", asBEHAVE_CONSTRUCT,
		"void f(int&in)", asFUNCTION(TSubclassOf_DefaultConstruct), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	Result = Engine->RegisterObjectBehaviour("TSubclassOf<T>", asBEHAVE_CONSTRUCT,
		"void f(int&in, const TSubclassOf<T> &in Other)", asFUNCTION(TSubclassOf_CopyConstruct), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	Result = Engine->RegisterObjectBehaviour("TSubclassOf<T>", asBEHAVE_CONSTRUCT,
		"void f(int&in, UClass@ Class)", asFUNCTION(TSubclassOf_ConstructFromClass), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	Result = Engine->RegisterObjectBehaviour("TSubclassOf<T>", asBEHAVE_DESTRUCT,
		"void f()", asFUNCTION(TSubclassOf_Destruct), asCALL_CDECL_OBJLAST);
	check(Result >= 0);

	// Assignment
	Result = Engine->RegisterObjectMethod("TSubclassOf<T>",
		"TSubclassOf<T> &opAssign(const TSubclassOf<T> &in Other)",
		asFUNCTION(TSubclassOf_CopyAssign), asCALL_CDECL_OBJFIRST);
	check(Result >= 0);

	Result = Engine->RegisterObjectMethod("TSubclassOf<T>",
		"TSubclassOf<T> &opAssign(UClass@ Class)",
		asFUNCTION(TSubclassOf_AssignFromClass), asCALL_CDECL_OBJFIRST);
	check(Result >= 0);

	// Equality
	Result = Engine->RegisterObjectMethod("TSubclassOf<T>",
		"bool opEquals(const TSubclassOf<T> &in Other) const",
		asFUNCTION(TSubclassOf_Equals), asCALL_CDECL_OBJFIRST);
	check(Result >= 0);

	// Class access
	Result = Engine->RegisterObjectMethod("TSubclassOf<T>",
		"UClass@ Get() const",
		asFUNCTION(TSubclassOf_Get), asCALL_CDECL_OBJFIRST);
	check(Result >= 0);

	// GetDefaultObject — declared as T@ so the returned UObject* is treated as the subtype in scripts.
	Result = Engine->RegisterObjectMethod("TSubclassOf<T>",
		"T@ GetDefaultObject() const",
		asFUNCTION(TSubclassOf_GetDefaultObject), asCALL_CDECL_OBJFIRST);
	check(Result >= 0);
}
