// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Misc/CoreMiscDefines.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"

#include <new>

namespace
{
	static void FPlatformUserId_DefaultConstruct(FPlatformUserId* Memory)
	{
		new (Memory) FPlatformUserId();
	}

	static void FPlatformUserId_CopyConstruct(const FPlatformUserId& Other, FPlatformUserId* Memory)
	{
		new (Memory) FPlatformUserId(Other);
	}

	static void FPlatformUserId_Destruct(FPlatformUserId* Memory)
	{
		Memory->~FPlatformUserId();
	}

	static FPlatformUserId& FPlatformUserId_Assign(FPlatformUserId& Value, const FPlatformUserId& Other)
	{
		Value = Other;
		return Value;
	}

	static bool FPlatformUserId_OpEquals(const FPlatformUserId& Value, const FPlatformUserId& Other)
	{
		return Value == Other;
	}

	static bool FPlatformUserId_IsValid(const FPlatformUserId& Value)
	{
		return Value.IsValid();
	}

	static int32 FPlatformUserId_GetInternalId(const FPlatformUserId& Value)
	{
		return Value.GetInternalId();
	}

	static FPlatformUserId FPlatformUserId_CreateFromInternalId(int32 InInternalId)
	{
		return FPlatformUserId::CreateFromInternalId(InInternalId);
	}
}

void Bind_FPlatformUserId(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;

	Result = Engine->RegisterObjectType("FPlatformUserId", sizeof(FPlatformUserId),
		asOBJ_VALUE | asGetTypeTraits<FPlatformUserId>());
	check(Result >= 0);

	REGISTER_BEHAVIOUR(FPlatformUserId, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FPlatformUserId_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FPlatformUserId, asBEHAVE_CONSTRUCT, "void f(const FPlatformUserId &in Other)", asFUNCTION(FPlatformUserId_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FPlatformUserId, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FPlatformUserId_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FPlatformUserId, "FPlatformUserId &opAssign(const FPlatformUserId &in Other)", asFUNCTION(FPlatformUserId_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FPlatformUserId, "bool opEquals(const FPlatformUserId &in Other) const", asFUNCTION(FPlatformUserId_OpEquals), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FPlatformUserId, "bool IsValid() const", asFUNCTION(FPlatformUserId_IsValid), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FPlatformUserId, "int GetInternalId() const", asFUNCTION(FPlatformUserId_GetInternalId), asCALL_CDECL_OBJFIRST);

	// Static factory under FPlatformUserId namespace
	Result = Engine->SetDefaultNamespace("FPlatformUserId");
	check(Result >= 0);

	Result = Engine->RegisterGlobalFunction("FPlatformUserId CreateFromInternalId(int InInternalId)", asFUNCTION(FPlatformUserId_CreateFromInternalId), asCALL_CDECL);
	check(Result >= 0);

	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
