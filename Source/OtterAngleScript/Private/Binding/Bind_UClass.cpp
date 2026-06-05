// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Misc/AssertionMacros.h"
#include "UObject/Class.h"
#include "angelscript.h"

namespace
{
	FName UClass_GetFName(const UClass* Value)
	{
		return Value->GetFName();
	}

	FString UClass_GetName(const UClass* Value)
	{
		return Value->GetName();
	}

	FString UClass_GetPathName(const UClass* Value)
	{
		return Value->GetPathName();
	}

	FString UClass_GetFullName(const UClass* Value)
	{
		return Value->GetFullName();
	}

	UObject* UClass_GetOuter(const UClass* Value)
	{
		return Value->GetOuter();
	}

	UClass* UClass_GetSuperClass(const UClass* Value)
	{
		return Value->GetSuperClass();
	}

	bool UClass_IsChildOf(const UClass* Value, UClass* SomeBase)
	{
		return Value->IsChildOf(SomeBase);
	}

	UObject* UClass_GetDefaultObject(const UClass* Value)
	{
		return Value->GetDefaultObject();
	}

	int32 UClass_GetPropertiesSize(const UClass* Value)
	{
		return Value->GetPropertiesSize();
	}
}

void Bind_UClass(asIScriptEngine* Engine)
{
	check(Engine != nullptr);
	OAS_RegisterMethods_UObject(Engine, "UClass");
	OAS_RegisterMethods_UClass(Engine, "UClass");
}

void OAS_RegisterMethods_UClass(asIScriptEngine* Engine, const FString& ChildName)
{
	check(Engine != nullptr);
	int Result;

	REGISTER_METHOD_NAMED(TCHAR_TO_ANSI(*ChildName), "UClass GetSuperClass() const", asFUNCTION(UClass_GetSuperClass), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD_NAMED(TCHAR_TO_ANSI(*ChildName), "bool IsChildOf(UClass SomeBase) const", asFUNCTION(UClass_IsChildOf), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD_NAMED(TCHAR_TO_ANSI(*ChildName), "UObject GetDefaultObject() const", asFUNCTION(UClass_GetDefaultObject), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD_NAMED(TCHAR_TO_ANSI(*ChildName), "int GetPropertiesSize() const", asFUNCTION(UClass_GetPropertiesSize), asCALL_CDECL_OBJFIRST);
}