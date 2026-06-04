// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Math/RandomStream.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"
#include "OtterAngleScript.h"

#include <new>

namespace
{
	// --- Constructors / destructor ---

	static void FRandomStream_DefaultConstruct(FRandomStream* Memory)
	{
		new (Memory) FRandomStream();
	}

	static void FRandomStream_CopyConstruct(const FRandomStream& Other, FRandomStream* Memory)
	{
		new (Memory) FRandomStream(Other);
	}

	static void FRandomStream_ConstructSeed(int32 InSeed, FRandomStream* Memory)
	{
		new (Memory) FRandomStream(InSeed);
	}

	static void FRandomStream_ConstructName(const FName& InName, FRandomStream* Memory)
	{
		new (Memory) FRandomStream(InName);
	}

	static void FRandomStream_Destruct(FRandomStream* Memory)
	{
		Memory->~FRandomStream();
	}

	// --- Assignment ---

	static FRandomStream& FRandomStream_Assign(FRandomStream& Value, const FRandomStream& Other)
	{
		Value = Other;
		return Value;
	}

	// --- Methods ---

	static void FRandomStream_Initialize_Seed(FRandomStream& Value, int32 InSeed)
	{
		Value.Initialize(InSeed);
	}

	static void FRandomStream_Initialize_Name(FRandomStream& Value, const FName& InName)
	{
		Value.Initialize(InName);
	}

	static void FRandomStream_Reset(const FRandomStream& Value)
	{
		Value.Reset();
	}

	static int32 FRandomStream_GetInitialSeed(const FRandomStream& Value)
	{
		return Value.GetInitialSeed();
	}

	static void FRandomStream_GenerateNewSeed(FRandomStream& Value)
	{
		Value.GenerateNewSeed();
	}

	static int32 FRandomStream_GetCurrentSeed(const FRandomStream& Value)
	{
		return Value.GetCurrentSeed();
	}

	static float FRandomStream_GetFraction(const FRandomStream& Value)
	{
		return Value.GetFraction();
	}

	static uint32 FRandomStream_GetUnsignedInt(const FRandomStream& Value)
	{
		return Value.GetUnsignedInt();
	}

	static float FRandomStream_FRand(const FRandomStream& Value)
	{
		return Value.FRand();
	}

	static int32 FRandomStream_RandHelper(const FRandomStream& Value, int32 A)
	{
		return Value.RandHelper(A);
	}

	static int32 FRandomStream_RandRange(const FRandomStream& Value, int32 Min, int32 Max)
	{
		return Value.RandRange(Min, Max);
	}

	static double FRandomStream_FRandRange(const FRandomStream& Value, double InMin, double InMax)
	{
		return Value.FRandRange(InMin, InMax);
	}

	static FVector FRandomStream_VRand(const FRandomStream& Value)
	{
		return Value.VRand();
	}

	static FVector FRandomStream_RandPointInBox(const FRandomStream& Value, const FBox& Box)
	{
		return Value.RandPointInBox(Box);
	}

	static FVector FRandomStream_VRandCone_HalfAngle(const FRandomStream& Value, const FVector& Dir, float ConeHalfAngleRad)
	{
		return Value.VRandCone(Dir, ConeHalfAngleRad);
	}

	static FVector FRandomStream_VRandCone_DualHalfAngle(const FRandomStream& Value, const FVector& Dir,
		float HorizontalConeHalfAngleRad, float VerticalConeHalfAngleRad)
	{
		return Value.VRandCone(Dir, HorizontalConeHalfAngleRad, VerticalConeHalfAngleRad);
	}

	static FString FRandomStream_ToString(const FRandomStream& Value)
	{
		return Value.ToString();
	}
}

void Bind_FRandomStream(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;

	REGISTER_BEHAVIOUR(FRandomStream, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FRandomStream_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FRandomStream, asBEHAVE_CONSTRUCT, "void f(const FRandomStream &in Other)", asFUNCTION(FRandomStream_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FRandomStream, asBEHAVE_CONSTRUCT, "void f(int Seed)", asFUNCTION(FRandomStream_ConstructSeed), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FRandomStream, asBEHAVE_CONSTRUCT, "void f(const FName &in Name)", asFUNCTION(FRandomStream_ConstructName), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FRandomStream, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FRandomStream_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FRandomStream, "FRandomStream &opAssign(const FRandomStream &in Other)", asFUNCTION(FRandomStream_Assign), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FRandomStream, "void Initialize(int InSeed)", asFUNCTION(FRandomStream_Initialize_Seed), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRandomStream, "void Initialize(const FName &in Name)", asFUNCTION(FRandomStream_Initialize_Name), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRandomStream, "void Reset() const", asFUNCTION(FRandomStream_Reset), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRandomStream, "int GetInitialSeed() const", asFUNCTION(FRandomStream_GetInitialSeed), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRandomStream, "void GenerateNewSeed()", asFUNCTION(FRandomStream_GenerateNewSeed), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRandomStream, "int GetCurrentSeed() const", asFUNCTION(FRandomStream_GetCurrentSeed), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRandomStream, "float GetFraction() const", asFUNCTION(FRandomStream_GetFraction), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRandomStream, "uint GetUnsignedInt() const", asFUNCTION(FRandomStream_GetUnsignedInt), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRandomStream, "float FRand() const", asFUNCTION(FRandomStream_FRand), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRandomStream, "int RandHelper(int A) const", asFUNCTION(FRandomStream_RandHelper), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRandomStream, "int RandRange(int Min, int Max) const", asFUNCTION(FRandomStream_RandRange), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRandomStream, "double FRandRange(double InMin, double InMax) const", asFUNCTION(FRandomStream_FRandRange), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRandomStream, "FVector VRand() const", asFUNCTION(FRandomStream_VRand), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRandomStream, "FVector RandPointInBox(const FBox &in Box) const", asFUNCTION(FRandomStream_RandPointInBox), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRandomStream, "FVector VRandCone(const FVector &in Dir, float ConeHalfAngleRad) const", asFUNCTION(FRandomStream_VRandCone_HalfAngle), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRandomStream, "FVector VRandCone(const FVector &in Dir, float HorizontalConeHalfAngleRad, float VerticalConeHalfAngleRad) const", asFUNCTION(FRandomStream_VRandCone_DualHalfAngle), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRandomStream, "FString ToString() const", asFUNCTION(FRandomStream_ToString), asCALL_CDECL_OBJFIRST);
}
