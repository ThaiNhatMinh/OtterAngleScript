// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Math/Color.h"
#include "Math/Vector4.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"
#include "OtterAngleScript.h"

#include <new>

namespace
{
	struct FVector4f_noalign
	{
		float X;
		float Y;
		float Z;
		float W;
	};

	static float& SetVector4fIndexException(FVector4f& Value, const char* Message)
	{
		SetScriptException(Message);
		return Value.X;
	}

	// Index without using deprecated XYZW array
	static float& Vector4f_GetComponent(FVector4f& V, unsigned int Index)
	{
		switch (Index)
		{
			case 0: return V.X;
			case 1: return V.Y;
			case 2: return V.Z;
			case 3: return V.W;
			default: return SetVector4fIndexException(V, "FVector4f index out of range");
		}
	}

	static float Vector4f_GetComponentConst(const FVector4f& V, unsigned int Index)
	{
		switch (Index)
		{
			case 0: return V.X;
			case 1: return V.Y;
			case 2: return V.Z;
			case 3: return V.W;
			default:
				SetScriptException("FVector4f index out of range");
				return 0.f;
		}
	}

	// --- Constructors / destructor ---

	static void FVector4f_DefaultConstruct(FVector4f* Memory)
	{
		new (Memory) FVector4f();
	}

	static void FVector4f_CopyConstruct(const FVector4f_noalign& Other, FVector4f_noalign* Memory)
	{
		new (Memory) FVector4f_noalign(Other);
	}

	static void FVector4f_ConstructXYZW(float InX, float InY, float InZ, float InW, FVector4f* Memory)
	{
		new (Memory) FVector4f(InX, InY, InZ, InW);
	}

	static void FVector4f_ConstructFromLinearColor(const FLinearColor& InColor, FVector4f* Memory)
	{
		new (Memory) FVector4f(InColor);
	}

	static void FVector4f_Destruct(FVector4f* Memory)
	{
		Memory->~FVector4f();
	}

	// --- Assignment / comparison ---

	static FVector4f& FVector4f_Assign(FVector4f_noalign& Value, const FVector4f_noalign& Other)
	{
		Value = Other;
		return (FVector4f&)Value;
	}

	static bool FVector4f_Equals_Op(const FVector4f& Value, const FVector4f& Other)
	{
		return Value == Other;
	}

	// --- Arithmetic operators ---

	static FVector4f FVector4f_Add(const FVector4f& Value, const FVector4f& Other)
	{
		return Value + Other;
	}

	static FVector4f FVector4f_AddScalar(const FVector4f& Value, float Bias)
	{
		return Value + Bias;
	}

	static FVector4f FVector4f_Subtract(const FVector4f& Value, const FVector4f& Other)
	{
		return Value - Other;
	}

	static FVector4f FVector4f_SubtractScalar(const FVector4f& Value, float Bias)
	{
		return Value - Bias;
	}

	static FVector4f FVector4f_MultiplyScale(const FVector4f& Value, float Scale)
	{
		return Value * Scale;
	}

	static FVector4f FVector4f_MultiplyComponents(const FVector4f& Value, const FVector4f& Other)
	{
		return Value * Other;
	}

	static FVector4f FVector4f_DivideScale(const FVector4f& Value, float Scale)
	{
		return Value / Scale;
	}

	static FVector4f FVector4f_DivideComponents(const FVector4f& Value, const FVector4f& Other)
	{
		return Value / Other;
	}

	static FVector4f FVector4f_Negate(const FVector4f& Value)
	{
		return -Value;
	}

	// operator^ = 3D cross product
	static FVector4f FVector4f_CrossOperator(const FVector4f& Value, const FVector4f& Other)
	{
		return Value ^ Other;
	}

	// --- Compound assignment ---

	static void FVector4f_AddAssign(FVector4f& Value, const FVector4f& Other)
	{
		Value += Other;
	}

	static void FVector4f_SubtractAssign(FVector4f& Value, const FVector4f& Other)
	{
		Value -= Other;
	}

	static void FVector4f_MultiplyAssignScale(FVector4f& Value, float Scale)
	{
		Value *= Scale;
	}

	static void FVector4f_MultiplyAssignComponents(FVector4f& Value, const FVector4f& Other)
	{
		Value *= Other;
	}

	static void FVector4f_DivideAssignScale(FVector4f& Value, float Scale)
	{
		Value /= Scale;
	}

	static void FVector4f_DivideAssignComponents(FVector4f& Value, const FVector4f& Other)
	{
		Value /= Other;
	}

	// --- Index operators ---

	static float& FVector4f_IndexRef(FVector4f& Value, unsigned int Index)
	{
		return Vector4f_GetComponent(Value, Index);
	}

	static float FVector4f_IndexValue(const FVector4f& Value, unsigned int Index)
	{
		return Vector4f_GetComponentConst(Value, Index);
	}

	// --- Component / set ---

	static bool FVector4f_IsValidIndex(const FVector4f& Value, int32 Index)
	{
		return Value.IsValidIndex(Index);
	}

	static void FVector4f_Set(FVector4f& Value, float InX, float InY, float InZ, float InW)
	{
		Value.Set(InX, InY, InZ, InW);
	}

	// --- Component min/max ---

	static FVector4f FVector4f_ComponentMin(const FVector4f& Value, const FVector4f& Other)
	{
		return Value.ComponentMin(Other);
	}

	static FVector4f FVector4f_ComponentMax(const FVector4f& Value, const FVector4f& Other)
	{
		return Value.ComponentMax(Other);
	}

	// --- Size / length ---

	static float FVector4f_Size3(const FVector4f& Value)
	{
		return Value.Size3();
	}

	static float FVector4f_SizeSquared3(const FVector4f& Value)
	{
		return Value.SizeSquared3();
	}

	static float FVector4f_Size(const FVector4f& Value)
	{
		return Value.Size();
	}

	static float FVector4f_SizeSquared(const FVector4f& Value)
	{
		return Value.SizeSquared();
	}

	// --- Predicates ---

	static bool FVector4f_ContainsNaN(const FVector4f& Value)
	{
		return Value.ContainsNaN();
	}

	static bool FVector4f_IsNearlyZero3Default(const FVector4f& Value)
	{
		return Value.IsNearlyZero3();
	}

	static bool FVector4f_IsNearlyZero3Tolerance(const FVector4f& Value, float Tolerance)
	{
		return Value.IsNearlyZero3(Tolerance);
	}

	static bool FVector4f_IsNearlyZeroDefault(const FVector4f& Value)
	{
		return Value.IsNearlyZero();
	}

	static bool FVector4f_IsNearlyZeroTolerance(const FVector4f& Value, float Tolerance)
	{
		return Value.IsNearlyZero(Tolerance);
	}

	static bool FVector4f_IsUnit3Default(const FVector4f& Value)
	{
		return Value.IsUnit3();
	}

	static bool FVector4f_IsUnit3Tolerance(const FVector4f& Value, float Tolerance)
	{
		return Value.IsUnit3(Tolerance);
	}

	static bool FVector4f_EqualsDefault(const FVector4f& Value, const FVector4f& V)
	{
		return Value.Equals(V);
	}

	static bool FVector4f_EqualsTolerance(const FVector4f& Value, const FVector4f& V, float Tolerance)
	{
		return Value.Equals(V, Tolerance);
	}

	// --- Normalization ---

	static FVector4f FVector4f_GetSafeNormalDefault(const FVector4f& Value)
	{
		return Value.GetSafeNormal();
	}

	static FVector4f FVector4f_GetSafeNormalTolerance(const FVector4f& Value, float Tolerance)
	{
		return Value.GetSafeNormal(Tolerance);
	}

	static FVector4f FVector4f_GetUnsafeNormal3(const FVector4f& Value)
	{
		return Value.GetUnsafeNormal3();
	}

	// --- Geometry ---

	static FVector4f FVector4f_Reflect3(const FVector4f& Value, const FVector4f& Normal)
	{
		return Value.Reflect3(Normal);
	}

	static void FVector4f_FindBestAxisVectors3(const FVector4f& Value, FVector4f& Axis1, FVector4f& Axis2)
	{
		Value.FindBestAxisVectors3(Axis1, Axis2);
	}

	// --- String ---

	static FString FVector4f_ToString(const FVector4f& Value)
	{
		return Value.ToString();
	}

	static bool FVector4f_InitFromString(FVector4f& Value, const FString& Source)
	{
		return Value.InitFromString(Source);
	}

	// --- Static helpers ---

	static FVector4f FVector4f_Zero()
	{
		return FVector4f::Zero();
	}
	static FVector4f FVector4f_One()
	{
		return FVector4f::One();
	}

	static void* Vector4_Factory()
	{
		return FMemory::Malloc(sizeof(FVector4f), alignof(FVector4f));
	}

	static void Vector4_Release(FVector4f* Object)
	{
		FMemory::Free(Object);
	}
}

void Bind_FVector4f(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;

	// --- Behaviors ---
	//Engine->RegisterObjectBehaviour("FVector4f", asBEHAVE_FACTORY, "FVector4f f()", asFUNCTION(Vector4_Factory), asCALL_CDECL);
	//Engine->RegisterObjectBehaviour("FVector4f", asBEHAVE_RELEASE, "void f()", asFUNCTION(Vector4_Release), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector4f, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FVector4f_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector4f, asBEHAVE_CONSTRUCT, "void f(const FVector4f &in Other)", asFUNCTION(FVector4f_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector4f, asBEHAVE_CONSTRUCT, "void f(float InX, float InY, float InZ, float InW)", asFUNCTION(FVector4f_ConstructXYZW), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector4f, asBEHAVE_CONSTRUCT, "void f(const FLinearColor &in InColor)", asFUNCTION(FVector4f_ConstructFromLinearColor), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector4f, asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(FVector4f_Destruct), asCALL_CDECL_OBJLAST);

	// --- Properties ---

	REGISTER_PROPERTY(FVector4f, "float X", X);
	REGISTER_PROPERTY(FVector4f, "float Y", Y);
	REGISTER_PROPERTY(FVector4f, "float Z", Z);
	REGISTER_PROPERTY(FVector4f, "float W", W);

	// --- Assignment / comparison ---

	REGISTER_METHOD(FVector4f, "FVector4f &opAssign(const FVector4f &in Other)", asFUNCTION(FVector4f_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "bool opEquals(const FVector4f &in Other) const", asFUNCTION(FVector4f_Equals_Op), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "bool Equals(const FVector4f &in V) const", asFUNCTION(FVector4f_EqualsDefault), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "bool Equals(const FVector4f &in V, float Tolerance) const", asFUNCTION(FVector4f_EqualsTolerance), asCALL_CDECL_OBJFIRST);

	// --- Arithmetic operators ---

	REGISTER_METHOD(FVector4f, "FVector4f opAdd(const FVector4f &in Other) const", asFUNCTION(FVector4f_Add), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "FVector4f opAdd(float Bias) const", asFUNCTION(FVector4f_AddScalar), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "FVector4f opSub(const FVector4f &in Other) const", asFUNCTION(FVector4f_Subtract), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "FVector4f opSub(float Bias) const", asFUNCTION(FVector4f_SubtractScalar), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "FVector4f opMul(float Scale) const", asFUNCTION(FVector4f_MultiplyScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "FVector4f opMul(const FVector4f &in Other) const", asFUNCTION(FVector4f_MultiplyComponents), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "FVector4f opDiv(float Scale) const", asFUNCTION(FVector4f_DivideScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "FVector4f opDiv(const FVector4f &in Other) const", asFUNCTION(FVector4f_DivideComponents), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "FVector4f opNeg() const", asFUNCTION(FVector4f_Negate), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "FVector4f opXor(const FVector4f &in Other) const", asFUNCTION(FVector4f_CrossOperator), asCALL_CDECL_OBJFIRST);

	// --- Compound assignment ---

	REGISTER_METHOD(FVector4f, "FVector4f& opAddAssign(const FVector4f &in Other)", asFUNCTION(FVector4f_AddAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "FVector4f& opSubAssign(const FVector4f &in Other)", asFUNCTION(FVector4f_SubtractAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "FVector4f& opMulAssign(float Scale)", asFUNCTION(FVector4f_MultiplyAssignScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "FVector4f& opMulAssign(const FVector4f &in Other)", asFUNCTION(FVector4f_MultiplyAssignComponents), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "FVector4f& opDivAssign(float Scale)", asFUNCTION(FVector4f_DivideAssignScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "FVector4f& opDivAssign(const FVector4f &in Other)", asFUNCTION(FVector4f_DivideAssignComponents), asCALL_CDECL_OBJFIRST);

	// --- Index operators ---

	REGISTER_METHOD(FVector4f, "float &opIndex(uint Index)", asFUNCTION(FVector4f_IndexRef), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "float opIndex(uint Index) const", asFUNCTION(FVector4f_IndexValue), asCALL_CDECL_OBJFIRST);

	// --- Component / set ---

	REGISTER_METHOD(FVector4f, "bool IsValidIndex(int Index) const", asFUNCTION(FVector4f_IsValidIndex), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "void Set(float InX, float InY, float InZ, float InW)", asFUNCTION(FVector4f_Set), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "FVector4f ComponentMin(const FVector4f &in Other) const", asFUNCTION(FVector4f_ComponentMin), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "FVector4f ComponentMax(const FVector4f &in Other) const", asFUNCTION(FVector4f_ComponentMax), asCALL_CDECL_OBJFIRST);

	// --- Size / length ---

	REGISTER_METHOD(FVector4f, "float Size3() const", asFUNCTION(FVector4f_Size3), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "float SizeSquared3() const", asFUNCTION(FVector4f_SizeSquared3), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "float Size() const", asFUNCTION(FVector4f_Size), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "float SizeSquared() const", asFUNCTION(FVector4f_SizeSquared), asCALL_CDECL_OBJFIRST);

	// --- Predicates ---

	REGISTER_METHOD(FVector4f, "bool ContainsNaN() const", asFUNCTION(FVector4f_ContainsNaN), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "bool IsNearlyZero3() const", asFUNCTION(FVector4f_IsNearlyZero3Default), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "bool IsNearlyZero3(float Tolerance) const", asFUNCTION(FVector4f_IsNearlyZero3Tolerance), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "bool IsNearlyZero() const", asFUNCTION(FVector4f_IsNearlyZeroDefault), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "bool IsNearlyZero(float Tolerance) const", asFUNCTION(FVector4f_IsNearlyZeroTolerance), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "bool IsUnit3() const", asFUNCTION(FVector4f_IsUnit3Default), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "bool IsUnit3(float LengthSquaredTolerance) const", asFUNCTION(FVector4f_IsUnit3Tolerance), asCALL_CDECL_OBJFIRST);

	// --- Normalization ---

	REGISTER_METHOD(FVector4f, "FVector4f GetSafeNormal() const", asFUNCTION(FVector4f_GetSafeNormalDefault), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "FVector4f GetSafeNormal(float Tolerance) const", asFUNCTION(FVector4f_GetSafeNormalTolerance), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "FVector4f GetUnsafeNormal3() const", asFUNCTION(FVector4f_GetUnsafeNormal3), asCALL_CDECL_OBJFIRST);

	// --- Geometry ---

	REGISTER_METHOD(FVector4f, "FVector4f Reflect3(const FVector4f &in Normal) const", asFUNCTION(FVector4f_Reflect3), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "void FindBestAxisVectors3(FVector4f &out Axis1, FVector4f &out Axis2) const", asFUNCTION(FVector4f_FindBestAxisVectors3), asCALL_CDECL_OBJFIRST);

	// --- String ---

	REGISTER_METHOD(FVector4f, "FString ToString() const", asFUNCTION(FVector4f_ToString), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4f, "bool InitFromString(const FString &in Source)", asFUNCTION(FVector4f_InitFromString), asCALL_CDECL_OBJFIRST);

	// --- Static helpers under FVector4f namespace ---

	Result = Engine->SetDefaultNamespace("FVector4f");
	check(Result >= 0);

	Result = Engine->RegisterGlobalFunction("FVector4f Zero()", asFUNCTION(FVector4f_Zero), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector4f One()", asFUNCTION(FVector4f_One), asCALL_CDECL);
	check(Result >= 0);

	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
