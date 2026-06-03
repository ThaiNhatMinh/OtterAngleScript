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
	// FVector4 is 16-byte aligned. Use a plain struct for copy/assign wrappers
	// so that AngelScript's default calling convention sees a natural alignment.
	struct FVector4_noalign
	{
		double X;
		double Y;
		double Z;
		double W;
	};

	static double& SetVector4IndexException(FVector4& Value, const char* Message)
	{
		SetScriptException(Message);
		return Value.X;
	}

	// Index without using deprecated XYZW array
	static double& Vector4_GetComponent(FVector4& V, unsigned int Index)
	{
		switch (Index)
		{
			case 0: return V.X;
			case 1: return V.Y;
			case 2: return V.Z;
			case 3: return V.W;
			default: return SetVector4IndexException(V, "FVector4 index out of range");
		}
	}

	static double Vector4_GetComponentConst(const FVector4& V, unsigned int Index)
	{
		switch (Index)
		{
			case 0: return V.X;
			case 1: return V.Y;
			case 2: return V.Z;
			case 3: return V.W;
			default:
				SetScriptException("FVector4 index out of range");
				return 0.0;
		}
	}

	// --- Constructors / destructor ---

	static void FVector4_DefaultConstruct(FVector4* Memory)
	{
		new (Memory) FVector4();
	}

	static void FVector4_CopyConstruct(const FVector4_noalign& Other, FVector4_noalign* Memory)
	{
		new (Memory) FVector4_noalign(Other);
	}

	static void FVector4_ConstructXYZW(double InX, double InY, double InZ, double InW, FVector4* Memory)
	{
		new (Memory) FVector4(InX, InY, InZ, InW);
	}

	static void FVector4_ConstructFromLinearColor(const FLinearColor& InColor, FVector4* Memory)
	{
		new (Memory) FVector4(InColor);
	}

	static void FVector4_Destruct(FVector4* Memory)
	{
		Memory->~FVector4();
	}

	// --- Assignment / comparison ---

	static FVector4& FVector4_Assign(FVector4_noalign& Value, const FVector4_noalign& Other)
	{
		Value = Other;
		return (FVector4&)Value;
	}

	static bool FVector4_Equals_Op(const FVector4& Value, const FVector4& Other)
	{
		return Value == Other;
	}

	// --- Arithmetic operators ---

	static FVector4 FVector4_Add(const FVector4& Value, const FVector4& Other)
	{
		return Value + Other;
	}

	static FVector4 FVector4_AddScalar(const FVector4& Value, double Bias)
	{
		return Value + Bias;
	}

	static FVector4 FVector4_Subtract(const FVector4& Value, const FVector4& Other)
	{
		return Value - Other;
	}

	static FVector4 FVector4_SubtractScalar(const FVector4& Value, double Bias)
	{
		return Value - Bias;
	}

	static FVector4 FVector4_MultiplyScale(const FVector4& Value, double Scale)
	{
		return Value * Scale;
	}

	static FVector4 FVector4_MultiplyComponents(const FVector4& Value, const FVector4& Other)
	{
		return Value * Other;
	}

	static FVector4 FVector4_DivideScale(const FVector4& Value, double Scale)
	{
		return Value / Scale;
	}

	static FVector4 FVector4_DivideComponents(const FVector4& Value, const FVector4& Other)
	{
		return Value / Other;
	}

	static FVector4 FVector4_Negate(const FVector4& Value)
	{
		return -Value;
	}

	// operator^ = 3D cross product
	static FVector4 FVector4_CrossOperator(const FVector4& Value, const FVector4& Other)
	{
		return Value ^ Other;
	}

	// --- Compound assignment ---

	static void FVector4_AddAssign(FVector4& Value, const FVector4& Other)
	{
		Value += Other;
	}

	static void FVector4_SubtractAssign(FVector4& Value, const FVector4& Other)
	{
		Value -= Other;
	}

	static void FVector4_MultiplyAssignScale(FVector4& Value, double Scale)
	{
		Value *= Scale;
	}

	static void FVector4_MultiplyAssignComponents(FVector4& Value, const FVector4& Other)
	{
		Value *= Other;
	}

	static void FVector4_DivideAssignScale(FVector4& Value, double Scale)
	{
		Value /= Scale;
	}

	static void FVector4_DivideAssignComponents(FVector4& Value, const FVector4& Other)
	{
		Value /= Other;
	}

	// --- Index operators ---

	static double& FVector4_IndexRef(FVector4& Value, unsigned int Index)
	{
		return Vector4_GetComponent(Value, Index);
	}

	static double FVector4_IndexValue(const FVector4& Value, unsigned int Index)
	{
		return Vector4_GetComponentConst(Value, Index);
	}

	// --- Component / set ---

	static bool FVector4_IsValidIndex(const FVector4& Value, int32 Index)
	{
		return Value.IsValidIndex(Index);
	}

	static void FVector4_Set(FVector4& Value, double InX, double InY, double InZ, double InW)
	{
		Value.Set(InX, InY, InZ, InW);
	}

	// --- Component min/max ---

	static FVector4 FVector4_ComponentMin(const FVector4& Value, const FVector4& Other)
	{
		return Value.ComponentMin(Other);
	}

	static FVector4 FVector4_ComponentMax(const FVector4& Value, const FVector4& Other)
	{
		return Value.ComponentMax(Other);
	}

	// --- Size / length ---

	static double FVector4_Size3(const FVector4& Value)
	{
		return Value.Size3();
	}

	static double FVector4_SizeSquared3(const FVector4& Value)
	{
		return Value.SizeSquared3();
	}

	static double FVector4_Size(const FVector4& Value)
	{
		return Value.Size();
	}

	static double FVector4_SizeSquared(const FVector4& Value)
	{
		return Value.SizeSquared();
	}

	// --- Predicates ---

	static bool FVector4_ContainsNaN(const FVector4& Value)
	{
		return Value.ContainsNaN();
	}

	static bool FVector4_IsNearlyZero3Default(const FVector4& Value)
	{
		return Value.IsNearlyZero3();
	}

	static bool FVector4_IsNearlyZero3Tolerance(const FVector4& Value, double Tolerance)
	{
		return Value.IsNearlyZero3(Tolerance);
	}

	static bool FVector4_IsNearlyZeroDefault(const FVector4& Value)
	{
		return Value.IsNearlyZero();
	}

	static bool FVector4_IsNearlyZeroTolerance(const FVector4& Value, double Tolerance)
	{
		return Value.IsNearlyZero(Tolerance);
	}

	static bool FVector4_IsUnit3Default(const FVector4& Value)
	{
		return Value.IsUnit3();
	}

	static bool FVector4_IsUnit3Tolerance(const FVector4& Value, double Tolerance)
	{
		return Value.IsUnit3(Tolerance);
	}

	static bool FVector4_EqualsDefault(const FVector4& Value, const FVector4& V)
	{
		return Value.Equals(V);
	}

	static bool FVector4_EqualsTolerance(const FVector4& Value, const FVector4& V, double Tolerance)
	{
		return Value.Equals(V, Tolerance);
	}

	// --- Normalization ---

	static FVector4 FVector4_GetSafeNormalDefault(const FVector4& Value)
	{
		return Value.GetSafeNormal();
	}

	static FVector4 FVector4_GetSafeNormalTolerance(const FVector4& Value, double Tolerance)
	{
		return Value.GetSafeNormal(Tolerance);
	}

	static FVector4 FVector4_GetUnsafeNormal3(const FVector4& Value)
	{
		return Value.GetUnsafeNormal3();
	}

	// --- Geometry ---

	static FVector4 FVector4_Reflect3(const FVector4& Value, const FVector4& Normal)
	{
		return Value.Reflect3(Normal);
	}

	static void FVector4_FindBestAxisVectors3(const FVector4& Value, FVector4& Axis1, FVector4& Axis2)
	{
		Value.FindBestAxisVectors3(Axis1, Axis2);
	}

	// --- String ---

	static FString FVector4_ToString(const FVector4& Value)
	{
		return Value.ToString();
	}

	static bool FVector4_InitFromString(FVector4& Value, const FString& Source)
	{
		return Value.InitFromString(Source);
	}

	// --- Static helpers ---

	static FVector4 FVector4_Zero()
	{
		return FVector4::Zero();
	}

	static FVector4 FVector4_One()
	{
		return FVector4::One();
	}
}

void Bind_FVector4(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;

	REGISTER_BEHAVIOUR(FVector4, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FVector4_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector4, asBEHAVE_CONSTRUCT, "void f(const FVector4 &in Other)", asFUNCTION(FVector4_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector4, asBEHAVE_CONSTRUCT, "void f(double InX, double InY, double InZ, double InW)", asFUNCTION(FVector4_ConstructXYZW), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector4, asBEHAVE_CONSTRUCT, "void f(const FLinearColor &in InColor)", asFUNCTION(FVector4_ConstructFromLinearColor), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector4, asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(FVector4_Destruct), asCALL_CDECL_OBJLAST);

	// --- Properties ---

	REGISTER_PROPERTY(FVector4, "double X", X);
	REGISTER_PROPERTY(FVector4, "double Y", Y);
	REGISTER_PROPERTY(FVector4, "double Z", Z);
	REGISTER_PROPERTY(FVector4, "double W", W);

	// --- Assignment / comparison ---

	REGISTER_METHOD(FVector4, "FVector4 &opAssign(const FVector4 &in Other)", asFUNCTION(FVector4_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "bool opEquals(const FVector4 &in Other) const", asFUNCTION(FVector4_Equals_Op), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "bool Equals(const FVector4 &in V) const", asFUNCTION(FVector4_EqualsDefault), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "bool Equals(const FVector4 &in V, double Tolerance) const", asFUNCTION(FVector4_EqualsTolerance), asCALL_CDECL_OBJFIRST);

	// --- Arithmetic operators ---

	REGISTER_METHOD(FVector4, "FVector4 opAdd(const FVector4 &in Other) const", asFUNCTION(FVector4_Add), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "FVector4 opAdd(double Bias) const", asFUNCTION(FVector4_AddScalar), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "FVector4 opSub(const FVector4 &in Other) const", asFUNCTION(FVector4_Subtract), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "FVector4 opSub(double Bias) const", asFUNCTION(FVector4_SubtractScalar), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "FVector4 opMul(double Scale) const", asFUNCTION(FVector4_MultiplyScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "FVector4 opMul(const FVector4 &in Other) const", asFUNCTION(FVector4_MultiplyComponents), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "FVector4 opDiv(double Scale) const", asFUNCTION(FVector4_DivideScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "FVector4 opDiv(const FVector4 &in Other) const", asFUNCTION(FVector4_DivideComponents), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "FVector4 opNeg() const", asFUNCTION(FVector4_Negate), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "FVector4 opXor(const FVector4 &in Other) const", asFUNCTION(FVector4_CrossOperator), asCALL_CDECL_OBJFIRST);

	// --- Compound assignment ---

	REGISTER_METHOD(FVector4, "FVector4& opAddAssign(const FVector4 &in Other)", asFUNCTION(FVector4_AddAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "FVector4& opSubAssign(const FVector4 &in Other)", asFUNCTION(FVector4_SubtractAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "FVector4& opMulAssign(double Scale)", asFUNCTION(FVector4_MultiplyAssignScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "FVector4& opMulAssign(const FVector4 &in Other)", asFUNCTION(FVector4_MultiplyAssignComponents), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "FVector4& opDivAssign(double Scale)", asFUNCTION(FVector4_DivideAssignScale), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "FVector4& opDivAssign(const FVector4 &in Other)", asFUNCTION(FVector4_DivideAssignComponents), asCALL_CDECL_OBJFIRST);

	// --- Index operators ---

	REGISTER_METHOD(FVector4, "double &opIndex(uint Index)", asFUNCTION(FVector4_IndexRef), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "double opIndex(uint Index) const", asFUNCTION(FVector4_IndexValue), asCALL_CDECL_OBJFIRST);

	// --- Component / set ---

	REGISTER_METHOD(FVector4, "bool IsValidIndex(int Index) const", asFUNCTION(FVector4_IsValidIndex), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "void Set(double InX, double InY, double InZ, double InW)", asFUNCTION(FVector4_Set), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "FVector4 ComponentMin(const FVector4 &in Other) const", asFUNCTION(FVector4_ComponentMin), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "FVector4 ComponentMax(const FVector4 &in Other) const", asFUNCTION(FVector4_ComponentMax), asCALL_CDECL_OBJFIRST);

	// --- Size / length ---

	REGISTER_METHOD(FVector4, "double Size3() const", asFUNCTION(FVector4_Size3), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "double SizeSquared3() const", asFUNCTION(FVector4_SizeSquared3), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "double Size() const", asFUNCTION(FVector4_Size), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "double SizeSquared() const", asFUNCTION(FVector4_SizeSquared), asCALL_CDECL_OBJFIRST);

	// --- Predicates ---

	REGISTER_METHOD(FVector4, "bool ContainsNaN() const", asFUNCTION(FVector4_ContainsNaN), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "bool IsNearlyZero3() const", asFUNCTION(FVector4_IsNearlyZero3Default), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "bool IsNearlyZero3(double Tolerance) const", asFUNCTION(FVector4_IsNearlyZero3Tolerance), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "bool IsNearlyZero() const", asFUNCTION(FVector4_IsNearlyZeroDefault), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "bool IsNearlyZero(double Tolerance) const", asFUNCTION(FVector4_IsNearlyZeroTolerance), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "bool IsUnit3() const", asFUNCTION(FVector4_IsUnit3Default), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "bool IsUnit3(double LengthSquaredTolerance) const", asFUNCTION(FVector4_IsUnit3Tolerance), asCALL_CDECL_OBJFIRST);

	// --- Normalization ---

	REGISTER_METHOD(FVector4, "FVector4 GetSafeNormal() const", asFUNCTION(FVector4_GetSafeNormalDefault), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "FVector4 GetSafeNormal(double Tolerance) const", asFUNCTION(FVector4_GetSafeNormalTolerance), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "FVector4 GetUnsafeNormal3() const", asFUNCTION(FVector4_GetUnsafeNormal3), asCALL_CDECL_OBJFIRST);

	// --- Geometry ---

	REGISTER_METHOD(FVector4, "FVector4 Reflect3(const FVector4 &in Normal) const", asFUNCTION(FVector4_Reflect3), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "void FindBestAxisVectors3(FVector4 &out Axis1, FVector4 &out Axis2) const", asFUNCTION(FVector4_FindBestAxisVectors3), asCALL_CDECL_OBJFIRST);

	// --- String ---

	REGISTER_METHOD(FVector4, "FString ToString() const", asFUNCTION(FVector4_ToString), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector4, "bool InitFromString(const FString &in Source)", asFUNCTION(FVector4_InitFromString), asCALL_CDECL_OBJFIRST);

	// --- Static helpers under FVector4 namespace ---

	Result = Engine->SetDefaultNamespace("FVector4");
	check(Result >= 0);

	Result = Engine->RegisterGlobalFunction("FVector4 Zero()", asFUNCTION(FVector4_Zero), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FVector4 One()", asFUNCTION(FVector4_One), asCALL_CDECL);
	check(Result >= 0);

	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
