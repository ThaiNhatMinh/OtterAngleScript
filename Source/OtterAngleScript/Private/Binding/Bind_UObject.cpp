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
#include <OtterAngleScript.h>

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

	UObject* UObject_GetWorld(const UObject* Value)
	{
		return Value->GetWorld();
	}

	void Internal_NewObject_FromTemplate(asIScriptGeneric* Gen)
	{
		UObject* Outer = (UObject*)Gen->GetArgAddress(0);
		FName* Name = (FName*)Gen->GetArgAddress(1);
		uint32 Flags = Gen->GetArgDWord(2);
		UObject* Template = (UObject*)Gen->GetArgAddress(3);
		auto TypeId = Gen->GetReturnTypeId();
		auto TypeInfo = Gen->GetEngine()->GetTypeInfoById(TypeId);
		UClass* Class = (UClass*)TypeInfo->GetUserData(USERDATA_UNREAL_TYPE);
		if (!Class)
		{
			SetScriptException(TCHAR_TO_ANSI(*FString::Printf(TEXT("Type '%s' is not a UObject class"), ANSI_TO_TCHAR(TypeInfo->GetName()))));
			Gen->SetReturnAddress(nullptr);
			return;
		}
		auto Result = NewObject<UObject>(Outer != nullptr ? Outer : GetTransientPackage(), Class, Name != nullptr ? *Name : NAME_None, (EObjectFlags)Flags, Template);
		Gen->SetReturnAddress(Result);
	}

	void Internal_NewObject(asIScriptGeneric* Gen)
	{
		UObject* Outer = (UObject*)Gen->GetArgAddress(0);
		UClass* Class = (UClass*)Gen->GetArgAddress(1);
		FName* Name = (FName*)Gen->GetArgAddress(2);
		uint32 Flags = Gen->GetArgDWord(3);

		if (!Class)
		{
			SetScriptException("NewObject with null class");
			Gen->SetReturnAddress(nullptr);
			return;
		}
		auto Result = NewObject<UObject>(Outer != nullptr ? Outer : GetTransientPackage(), Class, Name != nullptr ? *Name : NAME_None, (EObjectFlags)Flags);
		Gen->SetReturnAddress(Result);
	}

	void Internal_DuplicateObject_Template(asIScriptGeneric* Gen)
	{
		UObject* SourceObject = (UObject*)Gen->GetArgAddress(0);
		if (!IsValid(SourceObject))
		{
			SetScriptException("DuplicateObject with invalid source object");
			Gen->SetReturnAddress(nullptr);
			return;
		}

		UObject* Outer = (UObject*)Gen->GetArgAddress(1);
		FName* Name = (FName*)Gen->GetArgAddress(2);
		auto Result = DuplicateObject<UObject>(SourceObject, Outer != nullptr ? Outer : GetTransientPackage(), Name != nullptr ? *Name : NAME_None);
		Gen->SetReturnAddress(Result);
	}

	UObject* Ref_Factory()
	{
		SetScriptException("UObject cannot be created directly. Use NewObject or DuplicateObject instead.");
		return nullptr;
	}
}

void Bind_UObject(asIScriptEngine* Engine)
{
	check(Engine != nullptr);
	int Result;

	OAS_RegisterMethods_UObject(Engine, "UObject");

	Result = Engine->SetDefaultNamespace("UObject");
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("UClass StaticClass()", asFUNCTION(UObject::StaticClass), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);

	Result = Engine->RegisterGlobalFunction("UObject NewObject(UObject Outer, UClass Class, const FName&in Name = \"\", uint Flags = 0)", asFUNCTION(Internal_NewObject), asCALL_GENERIC);
	check(Result >= 0);

	Result = Engine->RegisterGlobalFunction("T NewObject<T>(UObject Outer, const FName&in Name = \"\", uint Flags = 0, UObject Template = null)", asFUNCTION(Internal_NewObject_FromTemplate), asCALL_GENERIC);
	check(Result >= 0);

	Result = Engine->RegisterGlobalFunction("T DuplicateObject<T>(T SourceObject, UObject Outer, const FName&in Name = \"\")", asFUNCTION(Internal_DuplicateObject_Template), asCALL_GENERIC);
	check(Result >= 0);
}

void OAS_RegisterMethods_UObject(asIScriptEngine* Engine, const FString& ChildName)
{
	check(Engine != nullptr);
	int Result;
	const char* TypeName = TCHAR_TO_ANSI(*ChildName);
	REGISTER_METHOD_NAMED(TypeName, "UClass GetClass() const", asFUNCTION(UObject_GetClass), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD_NAMED(TypeName, "UObject GetOuter() const", asFUNCTION(UObject_GetOuter), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD_NAMED(TypeName, "UObject GetPackage() const", asFUNCTION(UObject_GetPackage), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD_NAMED(TypeName, "UObject GetOutermost() const", asFUNCTION(UObject_GetOutermost), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD_NAMED(TypeName, "FName GetFName() const", asFUNCTION(UObject_GetFName), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD_NAMED(TypeName, "FString GetName() const", asFUNCTION(UObject_GetName), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD_NAMED(TypeName, "FString GetPathName() const", asFUNCTION(UObject_GetPathName), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD_NAMED(TypeName, "FString GetPathName(UObject StopOuter) const", asFUNCTION(UObject_GetPathNameFromOuter), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD_NAMED(TypeName, "FString GetFullName() const", asFUNCTION(UObject_GetFullName), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD_NAMED(TypeName, "FString GetFullName(UObject StopOuter) const", asFUNCTION(UObject_GetFullNameFromOuter), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD_NAMED(TypeName, "FString GetDesc()", asFUNCTION(UObject_GetDesc), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD_NAMED(TypeName, "bool IsA(UClass Class) const", asFUNCTION(UObject_IsA), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD_NAMED(TypeName, "UObject GetTypedOuter(UClass Class) const", asFUNCTION(UObject_GetTypedOuter), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD_NAMED(TypeName, "bool IsIn(UObject SomeOuter) const", asFUNCTION(UObject_IsIn), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD_NAMED(TypeName, "bool IsInA(UClass SomeBaseClass) const", asFUNCTION(UObject_IsInA), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD_NAMED(TypeName, "int GetUniqueID() const", asFUNCTION(UObject_GetUniqueID), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD_NAMED(TypeName, "bool IsValidLowLevel() const", asFUNCTION(UObject_IsValidLowLevel), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD_NAMED(TypeName, "bool IsValidLowLevelFast() const", asFUNCTION(UObject_IsValidLowLevelFast), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD_NAMED(TypeName, "bool IsRooted() const", asFUNCTION(UObject_IsRooted), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD_NAMED(TypeName, "bool IsNative() const", asFUNCTION(UObject_IsNative), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD_NAMED(TypeName, "bool IsTemplate() const", asFUNCTION(UObject_IsTemplate), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD_NAMED(TypeName, "bool IsDefaultSubobject() const", asFUNCTION(UObject_IsDefaultSubobject), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD_NAMED(TypeName, "bool IsAsset() const", asMETHOD(UObject, IsAsset), asCALL_THISCALL);
	REGISTER_METHOD_NAMED(TypeName, "UObject GetWorld() const", asFUNCTION(UObject_GetWorld), asCALL_CDECL_OBJFIRST);

	// Registering the factory behaviour
	//Result = Engine->RegisterObjectBehaviour(TypeName, asBEHAVE_FACTORY, TCHAR_TO_ANSI(*FString::Printf(TEXT("%s f()"), *ChildName)), asFUNCTION(Ref_Factory), asCALL_CDECL); check(Result >= 0);
}
