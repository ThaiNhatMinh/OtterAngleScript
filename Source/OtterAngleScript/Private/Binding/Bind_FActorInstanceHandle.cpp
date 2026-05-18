// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/ActorInstanceHandle.h"
#include "GameFramework/Actor.h"
#include "Misc/AssertionMacros.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "angelscript.h"

#include <new>

namespace
{
	static void RegisterReferenceTypeIfNeeded(asIScriptEngine* Engine, const char* Declaration)
	{
		if (Engine->GetTypeInfoByDecl(Declaration) != nullptr)
		{
			return;
		}

		const int Result = Engine->RegisterObjectType(Declaration, 0, asOBJ_REF | asOBJ_NOCOUNT);
		check(Result >= 0);
	}

	static void FActorInstanceHandle_DefaultConstruct(FActorInstanceHandle* Memory)
	{
		new (Memory) FActorInstanceHandle();
	}

	static void FActorInstanceHandle_CopyConstruct(const FActorInstanceHandle& Other, FActorInstanceHandle* Memory)
	{
		new (Memory) FActorInstanceHandle(Other);
	}

	static void FActorInstanceHandle_ConstructActor(AActor* Actor, FActorInstanceHandle* Memory)
	{
		new (Memory) FActorInstanceHandle(Actor);
	}

	static void FActorInstanceHandle_Destruct(FActorInstanceHandle* Memory)
	{
		Memory->~FActorInstanceHandle();
	}

	static FActorInstanceHandle& FActorInstanceHandle_Assign(FActorInstanceHandle& Value, const FActorInstanceHandle& Other)
	{
		Value = Other;
		return Value;
	}

	static bool FActorInstanceHandle_OpEquals(const FActorInstanceHandle& Value, const FActorInstanceHandle& Other)
	{
		return Value == Other;
	}

	static bool FActorInstanceHandle_IsValid(const FActorInstanceHandle& Value)
	{
		return Value.IsValid();
	}

	static AActor* FActorInstanceHandle_FetchActor(const FActorInstanceHandle& Value)
	{
		return Value.FetchActor();
	}
}

void Bind_FActorInstanceHandle(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	RegisterReferenceTypeIfNeeded(Engine, "AActor");
	RegisterReferenceTypeIfNeeded(Engine, "UPrimitiveComponent");
	RegisterReferenceTypeIfNeeded(Engine, "UPhysicalMaterial");

	int Result = Engine->RegisterObjectType(
		"FActorInstanceHandle",
		sizeof(FActorInstanceHandle),
		asOBJ_VALUE | asGetTypeTraits<FActorInstanceHandle>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	REGISTER_BEHAVIOUR(FActorInstanceHandle, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FActorInstanceHandle_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FActorInstanceHandle, asBEHAVE_CONSTRUCT, "void f(const FActorInstanceHandle &in Other)", asFUNCTION(FActorInstanceHandle_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FActorInstanceHandle, asBEHAVE_CONSTRUCT, "void f(AActor@ Actor)", asFUNCTION(FActorInstanceHandle_ConstructActor), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FActorInstanceHandle, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FActorInstanceHandle_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FActorInstanceHandle, "FActorInstanceHandle &opAssign(const FActorInstanceHandle &in Other)", asFUNCTION(FActorInstanceHandle_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FActorInstanceHandle, "bool opEquals(const FActorInstanceHandle &in Other) const", asFUNCTION(FActorInstanceHandle_OpEquals), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FActorInstanceHandle, "bool IsValid() const", asFUNCTION(FActorInstanceHandle_IsValid), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FActorInstanceHandle, "AActor@ FetchActor() const", asFUNCTION(FActorInstanceHandle_FetchActor), asCALL_CDECL_OBJFIRST);
}
