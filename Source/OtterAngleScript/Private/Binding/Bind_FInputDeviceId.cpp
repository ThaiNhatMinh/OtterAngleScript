// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Misc/CoreMiscDefines.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"

#include <new>

namespace
{
	static void FInputDeviceId_DefaultConstruct(FInputDeviceId* Memory)
	{
		new (Memory) FInputDeviceId();
	}

	static void FInputDeviceId_CopyConstruct(const FInputDeviceId& Other, FInputDeviceId* Memory)
	{
		new (Memory) FInputDeviceId(Other);
	}

	static void FInputDeviceId_Destruct(FInputDeviceId* Memory)
	{
		Memory->~FInputDeviceId();
	}

	static FInputDeviceId& FInputDeviceId_Assign(FInputDeviceId& Value, const FInputDeviceId& Other)
	{
		Value = Other;
		return Value;
	}

	static bool FInputDeviceId_OpEquals(const FInputDeviceId& Value, const FInputDeviceId& Other)
	{
		return Value == Other;
	}

	static int FInputDeviceId_OpCmp(const FInputDeviceId& Value, const FInputDeviceId& Other)
	{
		if (Value < Other) return -1;
		if (Other < Value) return 1;
		return 0;
	}

	static bool FInputDeviceId_IsValid(const FInputDeviceId& Value)
	{
		return Value.IsValid();
	}

	static int32 FInputDeviceId_GetId(const FInputDeviceId& Value)
	{
		return Value.GetId();
	}

	static FInputDeviceId FInputDeviceId_CreateFromInternalId(int32 InInternalId)
	{
		return FInputDeviceId::CreateFromInternalId(InInternalId);
	}
}

void Bind_FInputDeviceId(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;

	Result = Engine->RegisterObjectType("FInputDeviceId", sizeof(FInputDeviceId),
		asOBJ_VALUE | asGetTypeTraits<FInputDeviceId>());
	check(Result >= 0);

	REGISTER_BEHAVIOUR(FInputDeviceId, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FInputDeviceId_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FInputDeviceId, asBEHAVE_CONSTRUCT, "void f(const FInputDeviceId &in Other)", asFUNCTION(FInputDeviceId_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FInputDeviceId, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FInputDeviceId_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FInputDeviceId, "FInputDeviceId &opAssign(const FInputDeviceId &in Other)", asFUNCTION(FInputDeviceId_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputDeviceId, "bool opEquals(const FInputDeviceId &in Other) const", asFUNCTION(FInputDeviceId_OpEquals), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputDeviceId, "int opCmp(const FInputDeviceId &in Other) const", asFUNCTION(FInputDeviceId_OpCmp), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FInputDeviceId, "bool IsValid() const", asFUNCTION(FInputDeviceId_IsValid), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputDeviceId, "int GetId() const", asFUNCTION(FInputDeviceId_GetId), asCALL_CDECL_OBJFIRST);

	// Static factory under FInputDeviceId namespace
	Result = Engine->SetDefaultNamespace("FInputDeviceId");
	check(Result >= 0);

	Result = Engine->RegisterGlobalFunction("FInputDeviceId CreateFromInternalId(int InInternalId)", asFUNCTION(FInputDeviceId_CreateFromInternalId), asCALL_CDECL);
	check(Result >= 0);

	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
