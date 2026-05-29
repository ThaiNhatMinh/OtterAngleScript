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

	bool UClass_IsNative(const UClass* Value)
	{
		return Value->IsNative();
	}
}

void Bind_UClass(asIScriptEngine* Engine)
{
	check(Engine != nullptr);
	int Result;
	REGISTER_METHOD(UClass, "FName GetFName() const", asFUNCTION(UClass_GetFName), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UClass, "FString GetName() const", asFUNCTION(UClass_GetName), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UClass, "FString GetPathName() const", asFUNCTION(UClass_GetPathName), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UClass, "FString GetFullName() const", asFUNCTION(UClass_GetFullName), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UClass, "UObject GetOuter() const", asFUNCTION(UClass_GetOuter), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UClass, "UClass GetSuperClass() const", asFUNCTION(UClass_GetSuperClass), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UClass, "bool IsChildOf(UClass SomeBase) const", asFUNCTION(UClass_IsChildOf), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UClass, "UObject GetDefaultObject() const", asFUNCTION(UClass_GetDefaultObject), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UClass, "int GetPropertiesSize() const", asFUNCTION(UClass_GetPropertiesSize), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UClass, "bool IsNative() const", asFUNCTION(UClass_IsNative), asCALL_CDECL_OBJFIRST);
}
