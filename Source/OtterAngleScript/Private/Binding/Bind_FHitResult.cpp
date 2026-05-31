// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Engine/EngineTypes.h"
#include "Math/Vector.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"

#include <new>

namespace
{
	static void FHitResult_DefaultConstruct(FHitResult* Memory)
	{
		new (Memory) FHitResult();
	}

	static void FHitResult_CopyConstruct(const FHitResult& Other, FHitResult* Memory)
	{
		new (Memory) FHitResult(Other);
	}

	static void FHitResult_Destruct(FHitResult* Memory)
	{
		Memory->~FHitResult();
	}

	static FHitResult& FHitResult_Assign(FHitResult& Value, const FHitResult& Other)
	{
		Value = Other;
		return Value;
	}

	static void FHitResult_Init(FHitResult& Value, const FVector& Start, const FVector& End)
	{
		Value.Init(Start, End);
	}

	static void FHitResult_Reset(FHitResult& Value, float InTime, bool bPreserveTraceData)
	{
		Value.Reset(InTime, bPreserveTraceData);
	}

	static bool FHitResult_IsValidBlockingHit(const FHitResult& Value)
	{
		return Value.IsValidBlockingHit();
	}

	static FString FHitResult_ToString(const FHitResult& Value)
	{
		return Value.ToString();
	}

	static bool FHitResult_GetBlockingHit(const FHitResult& Value)
	{
		return Value.bBlockingHit;
	}

	static void FHitResult_SetBlockingHit(FHitResult& Value, bool bBlockingHit)
	{
		Value.bBlockingHit = bBlockingHit;
	}

	static bool FHitResult_GetStartPenetrating(const FHitResult& Value)
	{
		return Value.bStartPenetrating;
	}

	static void FHitResult_SetStartPenetrating(FHitResult& Value, bool bStartPenetrating)
	{
		Value.bStartPenetrating = bStartPenetrating;
	}

}


void Bind_FHitResult(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = Engine->RegisterObjectType(
		"FHitResult",
		sizeof(FHitResult),
		asOBJ_VALUE | asGetTypeTraits<FHitResult>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	REGISTER_BEHAVIOUR(FHitResult, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FHitResult_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FHitResult, asBEHAVE_CONSTRUCT, "void f(const FHitResult &in Other)", asFUNCTION(FHitResult_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FHitResult, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FHitResult_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FHitResult, "FHitResult &opAssign(const FHitResult &in Other)", asFUNCTION(FHitResult_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FHitResult, "void Init(const FVector &in Start, const FVector &in End)", asFUNCTION(FHitResult_Init), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FHitResult, "void Reset(float InTime = 1.0f, bool bPreserveTraceData = false)", asFUNCTION(FHitResult_Reset), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FHitResult, "bool IsValidBlockingHit() const", asFUNCTION(FHitResult_IsValidBlockingHit), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FHitResult, "FString ToString() const", asFUNCTION(FHitResult_ToString), asCALL_CDECL_OBJFIRST);

	REGISTER_PROPERTY(FHitResult, "int FaceIndex", FaceIndex);
	REGISTER_PROPERTY(FHitResult, "float Time", Time);
	REGISTER_PROPERTY(FHitResult, "float Distance", Distance);
	REGISTER_PROPERTY(FHitResult, "FVector Location", Location);
	REGISTER_PROPERTY(FHitResult, "FVector ImpactPoint", ImpactPoint);
	REGISTER_PROPERTY(FHitResult, "FVector Normal", Normal);
	REGISTER_PROPERTY(FHitResult, "FVector ImpactNormal", ImpactNormal);
	REGISTER_PROPERTY(FHitResult, "FVector TraceStart", TraceStart);
	REGISTER_PROPERTY(FHitResult, "FVector TraceEnd", TraceEnd);
	REGISTER_PROPERTY(FHitResult, "float PenetrationDepth", PenetrationDepth);
	REGISTER_PROPERTY(FHitResult, "int MyItem", MyItem);
	REGISTER_PROPERTY(FHitResult, "int Item", Item);
	REGISTER_PROPERTY(FHitResult, "uint8 ElementIndex", ElementIndex);
	REGISTER_PROPERTY(FHitResult, "TWeakObjectPtr<UPhysicalMaterial> PhysMaterial", PhysMaterial);
	REGISTER_PROPERTY(FHitResult, "FActorInstanceHandle HitObjectHandle", HitObjectHandle);
	REGISTER_PROPERTY(FHitResult, "TWeakObjectPtr<UPrimitiveComponent> Component", Component);
	REGISTER_PROPERTY(FHitResult, "FName BoneName", BoneName);
	REGISTER_PROPERTY(FHitResult, "FName MyBoneName", MyBoneName);
}
