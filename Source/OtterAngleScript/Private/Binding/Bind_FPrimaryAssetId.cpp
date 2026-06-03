// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "UObject/PrimaryAssetId.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"
#include "OtterAngleScript.h"

#include <new>

namespace
{
	// --- FPrimaryAssetType helpers ---

	static void FPrimaryAssetType_DefaultConstruct(FPrimaryAssetType* Memory)
	{
		new (Memory) FPrimaryAssetType();
	}

	static void FPrimaryAssetType_CopyConstruct(const FPrimaryAssetType& Other, FPrimaryAssetType* Memory)
	{
		new (Memory) FPrimaryAssetType(Other);
	}

	static void FPrimaryAssetType_ConstructFromFName(const FName& InName, FPrimaryAssetType* Memory)
	{
		new (Memory) FPrimaryAssetType(InName);
	}

	static void FPrimaryAssetType_ConstructFromString(const FString& InName, FPrimaryAssetType* Memory)
	{
		new (Memory) FPrimaryAssetType(*InName);
	}

	static void FPrimaryAssetType_Destruct(FPrimaryAssetType* Memory)
	{
		Memory->~FPrimaryAssetType();
	}

	static FPrimaryAssetType& FPrimaryAssetType_Assign(FPrimaryAssetType& Value, const FPrimaryAssetType& Other)
	{
		Value = Other;
		return Value;
	}

	static bool FPrimaryAssetType_OpEquals(const FPrimaryAssetType& Value, const FPrimaryAssetType& Other)
	{
		return Value == Other;
	}

	static FName FPrimaryAssetType_GetName(const FPrimaryAssetType& Value)
	{
		return Value.GetName();
	}

	static bool FPrimaryAssetType_IsValid(const FPrimaryAssetType& Value)
	{
		return Value.IsValid();
	}

	static FString FPrimaryAssetType_ToString(const FPrimaryAssetType& Value)
	{
		return Value.ToString();
	}

	// --- FPrimaryAssetId helpers ---

	static void FPrimaryAssetId_DefaultConstruct(FPrimaryAssetId* Memory)
	{
		new (Memory) FPrimaryAssetId();
	}

	static void FPrimaryAssetId_CopyConstruct(const FPrimaryAssetId& Other, FPrimaryAssetId* Memory)
	{
		new (Memory) FPrimaryAssetId(Other);
	}

	static void FPrimaryAssetId_ConstructTypeAndName(const FPrimaryAssetType& InAssetType, const FName& InAssetName, FPrimaryAssetId* Memory)
	{
		new (Memory) FPrimaryAssetId(InAssetType, InAssetName);
	}

	static void FPrimaryAssetId_ConstructFromString(const FString& TypeAndName, FPrimaryAssetId* Memory)
	{
		new (Memory) FPrimaryAssetId(TypeAndName);
	}

	static void FPrimaryAssetId_Destruct(FPrimaryAssetId* Memory)
	{
		Memory->~FPrimaryAssetId();
	}

	static FPrimaryAssetId& FPrimaryAssetId_Assign(FPrimaryAssetId& Value, const FPrimaryAssetId& Other)
	{
		Value = Other;
		return Value;
	}

	static bool FPrimaryAssetId_OpEquals(const FPrimaryAssetId& Value, const FPrimaryAssetId& Other)
	{
		return Value == Other;
	}

	static FPrimaryAssetType FPrimaryAssetId_GetPrimaryAssetType(const FPrimaryAssetId& Value)
	{
		return Value.PrimaryAssetType;
	}

	static void FPrimaryAssetId_SetPrimaryAssetType(FPrimaryAssetId& Value, const FPrimaryAssetType& InType)
	{
		Value.PrimaryAssetType = InType;
	}

	static FName FPrimaryAssetId_GetPrimaryAssetName(const FPrimaryAssetId& Value)
	{
		return Value.PrimaryAssetName;
	}

	static void FPrimaryAssetId_SetPrimaryAssetName(FPrimaryAssetId& Value, const FName& InName)
	{
		Value.PrimaryAssetName = InName;
	}

	static bool FPrimaryAssetId_IsValid(const FPrimaryAssetId& Value)
	{
		return Value.IsValid();
	}

	static FString FPrimaryAssetId_ToString(const FPrimaryAssetId& Value)
	{
		return Value.ToString();
	}

	static FPrimaryAssetId FPrimaryAssetId_FromString(const FString& String)
	{
		return FPrimaryAssetId::FromString(String);
	}

	static FPrimaryAssetId FPrimaryAssetId_ParseTypeAndName(const FString& TypeAndName)
	{
		return FPrimaryAssetId::ParseTypeAndName(TypeAndName);
	}
}

void Bind_FPrimaryAssetId(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;

	// Register FPrimaryAssetType
	Result = Engine->RegisterObjectType("FPrimaryAssetType", sizeof(FPrimaryAssetType),
		asOBJ_VALUE | asGetTypeTraits<FPrimaryAssetType>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	REGISTER_BEHAVIOUR(FPrimaryAssetType, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FPrimaryAssetType_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FPrimaryAssetType, asBEHAVE_CONSTRUCT, "void f(const FPrimaryAssetType &in Other)", asFUNCTION(FPrimaryAssetType_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FPrimaryAssetType, asBEHAVE_CONSTRUCT, "void f(const FName &in Name)", asFUNCTION(FPrimaryAssetType_ConstructFromFName), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FPrimaryAssetType, asBEHAVE_CONSTRUCT, "void f(const FString &in Name)", asFUNCTION(FPrimaryAssetType_ConstructFromString), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FPrimaryAssetType, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FPrimaryAssetType_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FPrimaryAssetType, "FPrimaryAssetType &opAssign(const FPrimaryAssetType &in Other)", asFUNCTION(FPrimaryAssetType_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FPrimaryAssetType, "bool opEquals(const FPrimaryAssetType &in Other) const", asFUNCTION(FPrimaryAssetType_OpEquals), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FPrimaryAssetType, "FName GetName() const", asFUNCTION(FPrimaryAssetType_GetName), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FPrimaryAssetType, "bool IsValid() const", asFUNCTION(FPrimaryAssetType_IsValid), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FPrimaryAssetType, "FString ToString() const", asFUNCTION(FPrimaryAssetType_ToString), asCALL_CDECL_OBJFIRST);

	// Register FPrimaryAssetId
	Result = Engine->RegisterObjectType("FPrimaryAssetId", sizeof(FPrimaryAssetId),
		asOBJ_VALUE | asGetTypeTraits<FPrimaryAssetId>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	REGISTER_BEHAVIOUR(FPrimaryAssetId, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FPrimaryAssetId_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FPrimaryAssetId, asBEHAVE_CONSTRUCT, "void f(const FPrimaryAssetId &in Other)", asFUNCTION(FPrimaryAssetId_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FPrimaryAssetId, asBEHAVE_CONSTRUCT, "void f(const FPrimaryAssetType &in AssetType, const FName &in AssetName)", asFUNCTION(FPrimaryAssetId_ConstructTypeAndName), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FPrimaryAssetId, asBEHAVE_CONSTRUCT, "void f(const FString &in TypeAndName)", asFUNCTION(FPrimaryAssetId_ConstructFromString), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FPrimaryAssetId, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FPrimaryAssetId_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FPrimaryAssetId, "FPrimaryAssetId &opAssign(const FPrimaryAssetId &in Other)", asFUNCTION(FPrimaryAssetId_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FPrimaryAssetId, "bool opEquals(const FPrimaryAssetId &in Other) const", asFUNCTION(FPrimaryAssetId_OpEquals), asCALL_CDECL_OBJFIRST);

	// Property accessors
	REGISTER_METHOD(FPrimaryAssetId, "FPrimaryAssetType get_PrimaryAssetType() const", asFUNCTION(FPrimaryAssetId_GetPrimaryAssetType), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FPrimaryAssetId, "void set_PrimaryAssetType(const FPrimaryAssetType &in InType)", asFUNCTION(FPrimaryAssetId_SetPrimaryAssetType), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FPrimaryAssetId, "FName get_PrimaryAssetName() const", asFUNCTION(FPrimaryAssetId_GetPrimaryAssetName), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FPrimaryAssetId, "void set_PrimaryAssetName(const FName &in InName)", asFUNCTION(FPrimaryAssetId_SetPrimaryAssetName), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FPrimaryAssetId, "bool IsValid() const", asFUNCTION(FPrimaryAssetId_IsValid), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FPrimaryAssetId, "FString ToString() const", asFUNCTION(FPrimaryAssetId_ToString), asCALL_CDECL_OBJFIRST);

	// Static methods under the FPrimaryAssetId namespace
	Result = Engine->SetDefaultNamespace("FPrimaryAssetId");
	check(Result >= 0);

	Result = Engine->RegisterGlobalFunction("FPrimaryAssetId FromString(const FString &in String)", asFUNCTION(FPrimaryAssetId_FromString), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FPrimaryAssetId ParseTypeAndName(const FString &in TypeAndName)", asFUNCTION(FPrimaryAssetId_ParseTypeAndName), asCALL_CDECL);
	check(Result >= 0);

	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
