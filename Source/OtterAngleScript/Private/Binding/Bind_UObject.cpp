// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Engine/World.h"
#include "Misc/AssertionMacros.h"
#include "UObject/Class.h"
#include "UObject/Object.h"
#include "UObject/Package.h"

#ifdef _MSC_VER
#pragma warning(disable:4191 4996)
#endif

#include "angelscript.h"

namespace
{
	UClass* UObject_GetClass(const UObject* Value)
	{
		return Value->GetClass();
	}

	UObject* UObject_GetOuter(const UObject* Value)
	{
		return Value->GetOuter();
	}

	UObject* UObject_GetPackage(const UObject* Value)
	{
		return Value->GetPackage();
	}

	UObject* UObject_GetOutermost(const UObject* Value)
	{
		return Value->GetOutermost();
	}

	FName UObject_GetFName(const UObject* Value)
	{
		return Value->GetFName();
	}

	FString UObject_GetName(const UObject* Value)
	{
		return Value->GetName();
	}

	FString UObject_GetPathName(const UObject* Value)
	{
		return Value->GetPathName();
	}

	FString UObject_GetPathNameFromOuter(const UObject* Value, const UObject* StopOuter)
	{
		return Value->GetPathName(StopOuter);
	}

	FString UObject_GetFullName(const UObject* Value)
	{
		return Value->GetFullName();
	}

	FString UObject_GetFullNameFromOuter(const UObject* Value, const UObject* StopOuter)
	{
		return Value->GetFullName(StopOuter);
	}

	FString UObject_GetDesc(UObject* Value)
	{
		return Value->GetDesc();
	}

	bool UObject_IsA(const UObject* Value, UClass* Class)
	{
		return Value->IsA(Class);
	}

	UObject* UObject_GetTypedOuter(const UObject* Value, UClass* Class)
	{
		return Value->GetTypedOuter(Class);
	}

	bool UObject_IsIn(const UObject* Value, const UObject* SomeOuter)
	{
		return Value->IsIn(SomeOuter);
	}

	bool UObject_IsInA(const UObject* Value, const UClass* SomeBaseClass)
	{
		return Value->IsInA(SomeBaseClass);
	}

	int32 UObject_GetUniqueID(const UObject* Value)
	{
		return Value->GetUniqueID();
	}

	bool UObject_IsValidLowLevel(const UObject* Value)
	{
		return Value->IsValidLowLevel();
	}

	bool UObject_IsValidLowLevelFast(const UObject* Value)
	{
		return Value->IsValidLowLevelFast();
	}

	bool UObject_IsRooted(const UObject* Value)
	{
		return Value->IsRooted();
	}

	bool UObject_IsNative(const UObject* Value)
	{
		return Value->IsNative();
	}

	bool UObject_IsTemplate(const UObject* Value)
	{
		return Value->IsTemplate();
	}

	bool UObject_IsDefaultSubobject(const UObject* Value)
	{
		return Value->IsDefaultSubobject();
	}

	bool UObject_IsAsset(const UObject* Value)
	{
		return Value->IsAsset();
	}

#if WITH_ENGINE
	UObject* UObject_GetWorld(const UObject* Value)
	{
		return Value->GetWorld();
	}
#endif
}

void Bind_UObject(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = Engine->RegisterObjectType("UObject", 0, asOBJ_REF | asOBJ_NOCOUNT | asOBJ_IMPLICIT_HANDLE);
	check(Result >= 0);

	REGISTER_METHOD(UObject, "UClass@ GetClass() const", asFUNCTION(UObject_GetClass), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "UObject@ GetOuter() const", asFUNCTION(UObject_GetOuter), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "UObject@ GetPackage() const", asFUNCTION(UObject_GetPackage), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "UObject@ GetOutermost() const", asFUNCTION(UObject_GetOutermost), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "FName GetFName() const", asFUNCTION(UObject_GetFName), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "FString GetName() const", asFUNCTION(UObject_GetName), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "FString GetPathName() const", asFUNCTION(UObject_GetPathName), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "FString GetPathName(UObject@ StopOuter) const", asFUNCTION(UObject_GetPathNameFromOuter), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "FString GetFullName() const", asFUNCTION(UObject_GetFullName), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "FString GetFullName(UObject@ StopOuter) const", asFUNCTION(UObject_GetFullNameFromOuter), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "FString GetDesc()", asFUNCTION(UObject_GetDesc), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "bool IsA(UClass@ Class) const", asFUNCTION(UObject_IsA), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "UObject@ GetTypedOuter(UClass@ Class) const", asFUNCTION(UObject_GetTypedOuter), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "bool IsIn(UObject@ SomeOuter) const", asFUNCTION(UObject_IsIn), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "bool IsInA(UClass@ SomeBaseClass) const", asFUNCTION(UObject_IsInA), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "int GetUniqueID() const", asFUNCTION(UObject_GetUniqueID), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "bool IsValidLowLevel() const", asFUNCTION(UObject_IsValidLowLevel), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "bool IsValidLowLevelFast() const", asFUNCTION(UObject_IsValidLowLevelFast), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "bool IsRooted() const", asFUNCTION(UObject_IsRooted), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "bool IsNative() const", asFUNCTION(UObject_IsNative), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "bool IsTemplate() const", asFUNCTION(UObject_IsTemplate), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "bool IsDefaultSubobject() const", asFUNCTION(UObject_IsDefaultSubobject), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "bool IsAsset() const", asFUNCTION(UObject_IsAsset), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "bool IsAsset() const", asMETHOD(UObject, IsAsset), asCALL_THISCALL);

#if WITH_ENGINE
	REGISTER_METHOD(UObject, "UObject@ GetWorld() const", asFUNCTION(UObject_GetWorld), asCALL_CDECL_OBJFIRST);
#endif
}
