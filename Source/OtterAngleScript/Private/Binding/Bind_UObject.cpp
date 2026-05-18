// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Misc/AssertionMacros.h"
#include "UObject/Class.h"
#include "UObject/Object.h"
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

	FString UObject_GetFullName(const UObject* Value)
	{
		return Value->GetFullName();
	}

	bool UObject_IsA(const UObject* Value, UClass* Class)
	{
		return Value->IsA(Class);
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

	bool UObject_IsTemplate(const UObject* Value)
	{
		return Value->IsTemplate();
	}
}

void Bind_UObject(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = Engine->RegisterObjectType("UObject", 0, asOBJ_REF | asOBJ_NOCOUNT);
	check(Result >= 0);

	REGISTER_METHOD(UObject, "UClass@ GetClass() const", asFUNCTION(UObject_GetClass), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "UObject@ GetOuter() const", asFUNCTION(UObject_GetOuter), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "FName GetFName() const", asFUNCTION(UObject_GetFName), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "FString GetName() const", asFUNCTION(UObject_GetName), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "FString GetPathName() const", asFUNCTION(UObject_GetPathName), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "FString GetFullName() const", asFUNCTION(UObject_GetFullName), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "bool IsA(UClass@ Class) const", asFUNCTION(UObject_IsA), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "int GetUniqueID() const", asFUNCTION(UObject_GetUniqueID), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "bool IsValidLowLevel() const", asFUNCTION(UObject_IsValidLowLevel), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "bool IsValidLowLevelFast() const", asFUNCTION(UObject_IsValidLowLevelFast), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UObject, "bool IsTemplate() const", asFUNCTION(UObject_IsTemplate), asCALL_CDECL_OBJFIRST);
}
