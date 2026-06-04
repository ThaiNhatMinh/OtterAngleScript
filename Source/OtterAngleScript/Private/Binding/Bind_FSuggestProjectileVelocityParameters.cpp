// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/AssertionMacros.h"

#include <new>

namespace
{
	// --- Constructors / destructor ---
	using FSuggestProjectileVelocityParameters = UGameplayStatics::FSuggestProjectileVelocityParameters;

	static void FSuggestProjectileVelocityParameters_CopyConstruct(const FSuggestProjectileVelocityParameters& Other, FSuggestProjectileVelocityParameters* Memory)
	{
		new (Memory) FSuggestProjectileVelocityParameters(Other);
	}

	static void FSuggestProjectileVelocityParameters_Construct(const UObject* World, FVector StartLocation, FVector EndLocation, float Speed, FSuggestProjectileVelocityParameters* Memory)
	{
		new (Memory) FSuggestProjectileVelocityParameters(World, StartLocation, EndLocation, Speed);
	}

	static void FSuggestProjectileVelocityParameters_Destruct(FSuggestProjectileVelocityParameters* Memory)
	{
		Memory->~FSuggestProjectileVelocityParameters();
	}

	// --- Operators ---

	static FSuggestProjectileVelocityParameters& FSuggestProjectileVelocityParameters_Assign(
		FSuggestProjectileVelocityParameters& Value, const FSuggestProjectileVelocityParameters& Other)
	{
		Value = Other;
		return Value;
	}

	// --- WorldContextObject needs get/set because UObject* requires UObject@ handle syntax ---

	static UObject* FSuggestProjectileVelocityParameters_GetWorldContextObject(const FSuggestProjectileVelocityParameters& Value)
	{
		return const_cast<UObject*>(Value.WorldContextObject);
	}

	static void FSuggestProjectileVelocityParameters_SetWorldContextObject(FSuggestProjectileVelocityParameters& Value, UObject* WorldContextObject)
	{
		Value.WorldContextObject = WorldContextObject;
	}
}

void Bind_FSuggestProjectileVelocityParameters(asIScriptEngine* Engine)
{
	check(Engine != nullptr);
	int Result;

	REGISTER_BEHAVIOUR(FSuggestProjectileVelocityParameters, asBEHAVE_CONSTRUCT, "void f(const FSuggestProjectileVelocityParameters &in Other)", asFUNCTION(FSuggestProjectileVelocityParameters_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FSuggestProjectileVelocityParameters, asBEHAVE_CONSTRUCT, "void f(const UObject World, const FVector &in StartLocation, const FVector &in EndLocation, float Speed)", asFUNCTION(FSuggestProjectileVelocityParameters_Construct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FSuggestProjectileVelocityParameters, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FSuggestProjectileVelocityParameters_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FSuggestProjectileVelocityParameters, "FSuggestProjectileVelocityParameters &opAssign(const FSuggestProjectileVelocityParameters &in Other)", asFUNCTION(FSuggestProjectileVelocityParameters_Assign), asCALL_CDECL_OBJFIRST);

	// Properties
	REGISTER_PROPERTY(FSuggestProjectileVelocityParameters, "FVector Start", Start);
	REGISTER_PROPERTY(FSuggestProjectileVelocityParameters, "FVector End", End);
	REGISTER_PROPERTY(FSuggestProjectileVelocityParameters, "float TossSpeed", TossSpeed);
	REGISTER_PROPERTY(FSuggestProjectileVelocityParameters, "bool bFavorHighArc", bFavorHighArc);
	REGISTER_PROPERTY(FSuggestProjectileVelocityParameters, "float CollisionRadius", CollisionRadius);
	REGISTER_PROPERTY(FSuggestProjectileVelocityParameters, "float OverrideGravityZ", OverrideGravityZ);
	REGISTER_PROPERTY(FSuggestProjectileVelocityParameters, "int TraceOption", TraceOption);
	REGISTER_PROPERTY(FSuggestProjectileVelocityParameters, "bool bDrawDebug", bDrawDebug);
	REGISTER_PROPERTY(FSuggestProjectileVelocityParameters, "bool bAcceptClosestOnNoSolutions", bAcceptClosestOnNoSolutions);
	REGISTER_PROPERTY(FSuggestProjectileVelocityParameters, "const UObject WorldContextObject", WorldContextObject);
}
