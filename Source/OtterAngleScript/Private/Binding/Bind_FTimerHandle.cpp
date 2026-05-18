// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Engine/TimerHandle.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"

#include <new>

namespace
{
	void FTimerHandle_DefaultConstruct(FTimerHandle* Memory)
	{
		new (Memory) FTimerHandle();
	}

	void FTimerHandle_CopyConstruct(const FTimerHandle& Other, FTimerHandle* Memory)
	{
		new (Memory) FTimerHandle(Other);
	}

	void FTimerHandle_Destruct(FTimerHandle* Memory)
	{
		Memory->~FTimerHandle();
	}

	FTimerHandle& FTimerHandle_Assign(FTimerHandle& Value, const FTimerHandle& Other)
	{
		Value = Other;
		return Value;
	}

	bool FTimerHandle_Equals(const FTimerHandle& Value, const FTimerHandle& Other)
	{
		return Value == Other;
	}

	bool FTimerHandle_IsValid(const FTimerHandle& Value)
	{
		return Value.IsValid();
	}

	void FTimerHandle_Invalidate(FTimerHandle& Value)
	{
		Value.Invalidate();
	}

	FString FTimerHandle_ToString(const FTimerHandle& Value)
	{
		return Value.ToString();
	}
}

void Bind_FTimerHandle(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = Engine->RegisterObjectType(
		"FTimerHandle",
		sizeof(FTimerHandle),
		asOBJ_VALUE | asGetTypeTraits<FTimerHandle>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	REGISTER_BEHAVIOUR(FTimerHandle, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FTimerHandle_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FTimerHandle, asBEHAVE_CONSTRUCT, "void f(const FTimerHandle &in Other)", asFUNCTION(FTimerHandle_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FTimerHandle, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FTimerHandle_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FTimerHandle, "FTimerHandle &opAssign(const FTimerHandle &in Other)", asFUNCTION(FTimerHandle_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimerHandle, "bool opEquals(const FTimerHandle &in Other) const", asFUNCTION(FTimerHandle_Equals), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimerHandle, "bool IsValid() const", asFUNCTION(FTimerHandle_IsValid), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimerHandle, "void Invalidate()", asFUNCTION(FTimerHandle_Invalidate), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimerHandle, "FString ToString() const", asFUNCTION(FTimerHandle_ToString), asCALL_CDECL_OBJFIRST);
}
