// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Engine/LatentActionManager.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"

#include <new>

namespace
{
	void FLatentActionInfo_DefaultConstruct(FLatentActionInfo* Memory)
	{
		new (Memory) FLatentActionInfo();
	}

	void FLatentActionInfo_CopyConstruct(const FLatentActionInfo& Other, FLatentActionInfo* Memory)
	{
		new (Memory) FLatentActionInfo(Other);
	}

	void FLatentActionInfo_ConstructValues(int32 Linkage, int32 UUID, const FString& ExecutionFunction, UObject* CallbackTarget, FLatentActionInfo* Memory)
	{
		new (Memory) FLatentActionInfo(Linkage, UUID, *ExecutionFunction, CallbackTarget);
	}

	void FLatentActionInfo_Destruct(FLatentActionInfo* Memory)
	{
		Memory->~FLatentActionInfo();
	}

	FLatentActionInfo& FLatentActionInfo_Assign(FLatentActionInfo& Value, const FLatentActionInfo& Other)
	{
		Value = Other;
		return Value;
	}

	int32 FLatentActionInfo_GetLinkage(const FLatentActionInfo& Value)
	{
		return Value.Linkage;
	}

	void FLatentActionInfo_SetLinkage(FLatentActionInfo& Value, int32 Linkage)
	{
		Value.Linkage = Linkage;
	}

	int32 FLatentActionInfo_GetUUID(const FLatentActionInfo& Value)
	{
		return Value.UUID;
	}

	void FLatentActionInfo_SetUUID(FLatentActionInfo& Value, int32 UUID)
	{
		Value.UUID = UUID;
	}

	FName FLatentActionInfo_GetExecutionFunction(const FLatentActionInfo& Value)
	{
		return Value.ExecutionFunction;
	}

	void FLatentActionInfo_SetExecutionFunction(FLatentActionInfo& Value, const FName& ExecutionFunction)
	{
		Value.ExecutionFunction = ExecutionFunction;
	}

	UObject* FLatentActionInfo_GetCallbackTarget(const FLatentActionInfo& Value)
	{
		return Value.CallbackTarget.Get();
	}

	void FLatentActionInfo_SetCallbackTarget(FLatentActionInfo& Value, UObject* CallbackTarget)
	{
		Value.CallbackTarget = CallbackTarget;
	}
}

void Bind_FLatentActionInfo(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = Engine->RegisterObjectType(
		"FLatentActionInfo",
		sizeof(FLatentActionInfo),
		asOBJ_VALUE | asGetTypeTraits<FLatentActionInfo>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	REGISTER_BEHAVIOUR(FLatentActionInfo, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FLatentActionInfo_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FLatentActionInfo, asBEHAVE_CONSTRUCT, "void f(const FLatentActionInfo &in Other)", asFUNCTION(FLatentActionInfo_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FLatentActionInfo, asBEHAVE_CONSTRUCT, "void f(int Linkage, int UUID, const FString &in ExecutionFunction, UObject@ CallbackTarget)", asFUNCTION(FLatentActionInfo_ConstructValues), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FLatentActionInfo, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FLatentActionInfo_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FLatentActionInfo, "FLatentActionInfo &opAssign(const FLatentActionInfo &in Other)", asFUNCTION(FLatentActionInfo_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLatentActionInfo, "int get_Linkage() const property", asFUNCTION(FLatentActionInfo_GetLinkage), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLatentActionInfo, "void set_Linkage(int Value) property", asFUNCTION(FLatentActionInfo_SetLinkage), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLatentActionInfo, "int get_UUID() const property", asFUNCTION(FLatentActionInfo_GetUUID), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLatentActionInfo, "void set_UUID(int Value) property", asFUNCTION(FLatentActionInfo_SetUUID), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLatentActionInfo, "FName get_ExecutionFunction() const property", asFUNCTION(FLatentActionInfo_GetExecutionFunction), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLatentActionInfo, "void set_ExecutionFunction(const FName &in Value) property", asFUNCTION(FLatentActionInfo_SetExecutionFunction), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLatentActionInfo, "UObject@ get_CallbackTarget() const property", asFUNCTION(FLatentActionInfo_GetCallbackTarget), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLatentActionInfo, "void set_CallbackTarget(UObject@ Value) property", asFUNCTION(FLatentActionInfo_SetCallbackTarget), asCALL_CDECL_OBJFIRST);
}
