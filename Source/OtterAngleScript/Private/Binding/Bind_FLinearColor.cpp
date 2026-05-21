// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Math/Color.h"
#include "Misc/CoreMiscDefines.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"

#include <new>

namespace
{
	void FLinearColor_DefaultConstruct(FLinearColor* Memory)
	{
		new (Memory) FLinearColor();
	}

	void FLinearColor_CopyConstruct(const FLinearColor& Other, FLinearColor* Memory)
	{
		new (Memory) FLinearColor(Other);
	}

	void FLinearColor_ConstructForceInit(EForceInit ForceInitValue, FLinearColor* Memory)
	{
		new (Memory) FLinearColor(ForceInitValue);
	}

	void FLinearColor_ConstructRGB(float InR, float InG, float InB, FLinearColor* Memory)
	{
		new (Memory) FLinearColor(InR, InG, InB);
	}

	void FLinearColor_ConstructRGBA(float InR, float InG, float InB, float InA, FLinearColor* Memory)
	{
		new (Memory) FLinearColor(InR, InG, InB, InA);
	}

	void FLinearColor_Destruct(FLinearColor* Memory)
	{
		Memory->~FLinearColor();
	}

	FLinearColor& FLinearColor_Assign(FLinearColor& Value, const FLinearColor& Other)
	{
		Value = Other;
		return Value;
	}

	bool FLinearColor_EqualsExact(const FLinearColor& Value, const FLinearColor& Other)
	{
		return Value == Other;
	}

	bool FLinearColor_Equals(const FLinearColor& Value, const FLinearColor& Other, float Tolerance)
	{
		return Value.Equals(Other, Tolerance);
	}

	FLinearColor FLinearColor_Add(const FLinearColor& Value, const FLinearColor& Other)
	{
		return Value + Other;
	}

	FLinearColor FLinearColor_Subtract(const FLinearColor& Value, const FLinearColor& Other)
	{
		return Value - Other;
	}

	FLinearColor FLinearColor_MultiplyColor(const FLinearColor& Value, const FLinearColor& Other)
	{
		return Value * Other;
	}

	FLinearColor FLinearColor_MultiplyScalar(const FLinearColor& Value, float Scalar)
	{
		return Value * Scalar;
	}

	FLinearColor FLinearColor_MultiplyScalarReverse(const FLinearColor& Value, float Scalar)
	{
		return Scalar * Value;
	}

	FLinearColor FLinearColor_DivideColor(const FLinearColor& Value, const FLinearColor& Other)
	{
		return Value / Other;
	}

	FLinearColor FLinearColor_DivideScalar(const FLinearColor& Value, float Scalar)
	{
		return Value / Scalar;
	}

	FLinearColor FLinearColor_AddAssign(FLinearColor& Value, const FLinearColor& Other)
	{
		return Value += Other;
	}

	FLinearColor FLinearColor_SubtractAssign(FLinearColor& Value, const FLinearColor& Other)
	{
		return Value -= Other;
	}

	FLinearColor FLinearColor_MultiplyAssignColor(FLinearColor& Value, const FLinearColor& Other)
	{
		return Value *= Other;
	}

	FLinearColor FLinearColor_MultiplyAssignScalar(FLinearColor& Value, float Scalar)
	{
		return Value *= Scalar;
	}

	FLinearColor FLinearColor_DivideAssignColor(FLinearColor& Value, const FLinearColor& Other)
	{
		return Value /= Other;
	}

	FLinearColor FLinearColor_DivideAssignScalar(FLinearColor& Value, float Scalar)
	{
		return Value /= Scalar;
	}

	FLinearColor FLinearColor_CopyWithNewOpacity(const FLinearColor& Value, float NewOpacity)
	{
		return Value.CopyWithNewOpacity(NewOpacity);
	}

	FLinearColor FLinearColor_GetClamped(const FLinearColor& Value, float InMin, float InMax)
	{
		return Value.GetClamped(InMin, InMax);
	}

	FLinearColor FLinearColor_LinearRGBToHSV(const FLinearColor& Value)
	{
		return Value.LinearRGBToHSV();
	}

	FLinearColor FLinearColor_HSVToLinearRGB(const FLinearColor& Value)
	{
		return Value.HSVToLinearRGB();
	}

	FLinearColor FLinearColor_Desaturate(const FLinearColor& Value, float Desaturation)
	{
		return Value.Desaturate(Desaturation);
	}

	float FLinearColor_GetLuminance(const FLinearColor& Value)
	{
		return Value.GetLuminance();
	}

	float FLinearColor_GetMax(const FLinearColor& Value)
	{
		return Value.GetMax();
	}

	float FLinearColor_GetMin(const FLinearColor& Value)
	{
		return Value.GetMin();
	}

	bool FLinearColor_IsAlmostBlack(const FLinearColor& Value)
	{
		return Value.IsAlmostBlack();
	}

	FString FLinearColor_ToString(const FLinearColor& Value)
	{
		return Value.ToString();
	}

	bool FLinearColor_InitFromString(FLinearColor& Value, const FString& Source)
	{
		return Value.InitFromString(Source);
	}

	FLinearColor FLinearColor_MakeFromHSV8(uint8 H, uint8 S, uint8 V)
	{
		return FLinearColor::MakeFromHSV8(H, S, V);
	}

	FLinearColor FLinearColor_MakeRandomColor()
	{
		return FLinearColor::MakeRandomColor();
	}

	FLinearColor FLinearColor_MakeFromColorTemperature(float Temp)
	{
		return FLinearColor::MakeFromColorTemperature(Temp);
	}

	FLinearColor FLinearColor_MakeRandomSeededColor(int32 Seed)
	{
		return FLinearColor::MakeRandomSeededColor(Seed);
	}

	FLinearColor FLinearColor_LerpUsingHSV(const FLinearColor& From, const FLinearColor& To, float Progress)
	{
		return FLinearColor::LerpUsingHSV(From, To, Progress);
	}

	float FLinearColor_Dist(const FLinearColor& V1, const FLinearColor& V2)
	{
		return FLinearColor::Dist(V1, V2);
	}
}

void Bind_FLinearColor(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = Engine->RegisterObjectType(
		"FLinearColor",
		sizeof(FLinearColor),
		asOBJ_VALUE | asGetTypeTraits<FLinearColor>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	REGISTER_BEHAVIOUR(FLinearColor, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FLinearColor_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FLinearColor, asBEHAVE_CONSTRUCT, "void f(const FLinearColor &in Other)", asFUNCTION(FLinearColor_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FLinearColor, asBEHAVE_CONSTRUCT, "void f(EForceInitType ForceInitValue)", asFUNCTION(FLinearColor_ConstructForceInit), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FLinearColor, asBEHAVE_CONSTRUCT, "void f(float R, float G, float B)", asFUNCTION(FLinearColor_ConstructRGB), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FLinearColor, asBEHAVE_CONSTRUCT, "void f(float R, float G, float B, float A)", asFUNCTION(FLinearColor_ConstructRGBA), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FLinearColor, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FLinearColor_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FLinearColor, "FLinearColor &opAssign(const FLinearColor &in Other)", asFUNCTION(FLinearColor_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLinearColor, "bool opEquals(const FLinearColor &in Other) const", asFUNCTION(FLinearColor_EqualsExact), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLinearColor, "FLinearColor opAdd(const FLinearColor &in Other) const", asFUNCTION(FLinearColor_Add), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLinearColor, "FLinearColor opSub(const FLinearColor &in Other) const", asFUNCTION(FLinearColor_Subtract), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLinearColor, "FLinearColor opMul(const FLinearColor &in Other) const", asFUNCTION(FLinearColor_MultiplyColor), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLinearColor, "FLinearColor opMul(float Scalar) const", asFUNCTION(FLinearColor_MultiplyScalar), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLinearColor, "FLinearColor opMul_r(float Scalar) const", asFUNCTION(FLinearColor_MultiplyScalarReverse), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLinearColor, "FLinearColor opDiv(const FLinearColor &in Other) const", asFUNCTION(FLinearColor_DivideColor), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLinearColor, "FLinearColor opDiv(float Scalar) const", asFUNCTION(FLinearColor_DivideScalar), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLinearColor, "FLinearColor opAddAssign(const FLinearColor &in Other)", asFUNCTION(FLinearColor_AddAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLinearColor, "FLinearColor opSubAssign(const FLinearColor &in Other)", asFUNCTION(FLinearColor_SubtractAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLinearColor, "FLinearColor opMulAssign(const FLinearColor &in Other)", asFUNCTION(FLinearColor_MultiplyAssignColor), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLinearColor, "FLinearColor opMulAssign(float Scalar)", asFUNCTION(FLinearColor_MultiplyAssignScalar), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLinearColor, "FLinearColor opDivAssign(const FLinearColor &in Other)", asFUNCTION(FLinearColor_DivideAssignColor), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLinearColor, "FLinearColor opDivAssign(float Scalar)", asFUNCTION(FLinearColor_DivideAssignScalar), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FLinearColor, "bool Equals(const FLinearColor &in Other, float Tolerance = 0.0001) const", asFUNCTION(FLinearColor_Equals), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLinearColor, "FLinearColor CopyWithNewOpacity(float NewOpacity) const", asFUNCTION(FLinearColor_CopyWithNewOpacity), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLinearColor, "FLinearColor GetClamped(float InMin = 0.0f, float InMax = 1.0f) const", asFUNCTION(FLinearColor_GetClamped), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLinearColor, "FLinearColor LinearRGBToHSV() const", asFUNCTION(FLinearColor_LinearRGBToHSV), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLinearColor, "FLinearColor HSVToLinearRGB() const", asFUNCTION(FLinearColor_HSVToLinearRGB), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLinearColor, "FLinearColor Desaturate(float Desaturation) const", asFUNCTION(FLinearColor_Desaturate), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLinearColor, "float GetLuminance() const", asFUNCTION(FLinearColor_GetLuminance), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLinearColor, "float GetMax() const", asFUNCTION(FLinearColor_GetMax), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLinearColor, "float GetMin() const", asFUNCTION(FLinearColor_GetMin), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLinearColor, "bool IsAlmostBlack() const", asFUNCTION(FLinearColor_IsAlmostBlack), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLinearColor, "FString ToString() const", asFUNCTION(FLinearColor_ToString), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLinearColor, "bool InitFromString(const FString &in Source)", asFUNCTION(FLinearColor_InitFromString), asCALL_CDECL_OBJFIRST);

	REGISTER_PROPERTY(FLinearColor, "float R", R);
	REGISTER_PROPERTY(FLinearColor, "float G", G);
	REGISTER_PROPERTY(FLinearColor, "float B", B);
	REGISTER_PROPERTY(FLinearColor, "float A", A);

	Result = Engine->SetDefaultNamespace("FLinearColor");
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FLinearColor MakeFromHSV8(uint8 H, uint8 S, uint8 V)", asFUNCTION(FLinearColor_MakeFromHSV8), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FLinearColor MakeRandomColor()", asFUNCTION(FLinearColor_MakeRandomColor), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FLinearColor MakeFromColorTemperature(float Temp)", asFUNCTION(FLinearColor_MakeFromColorTemperature), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FLinearColor MakeRandomSeededColor(int Seed)", asFUNCTION(FLinearColor_MakeRandomSeededColor), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FLinearColor LerpUsingHSV(const FLinearColor &in From, const FLinearColor &in To, float Progress)", asFUNCTION(FLinearColor_LerpUsingHSV), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("float Dist(const FLinearColor &in V1, const FLinearColor &in V2)", asFUNCTION(FLinearColor_Dist), asCALL_CDECL);
	check(Result >= 0);

	Result = Engine->RegisterGlobalProperty("const FLinearColor White", const_cast<FLinearColor*>(&FLinearColor::White));
	check(Result >= 0);
	Result = Engine->RegisterGlobalProperty("const FLinearColor Gray", const_cast<FLinearColor*>(&FLinearColor::Gray));
	check(Result >= 0);
	Result = Engine->RegisterGlobalProperty("const FLinearColor Black", const_cast<FLinearColor*>(&FLinearColor::Black));
	check(Result >= 0);
	Result = Engine->RegisterGlobalProperty("const FLinearColor Transparent", const_cast<FLinearColor*>(&FLinearColor::Transparent));
	check(Result >= 0);
	Result = Engine->RegisterGlobalProperty("const FLinearColor Red", const_cast<FLinearColor*>(&FLinearColor::Red));
	check(Result >= 0);
	Result = Engine->RegisterGlobalProperty("const FLinearColor Green", const_cast<FLinearColor*>(&FLinearColor::Green));
	check(Result >= 0);
	Result = Engine->RegisterGlobalProperty("const FLinearColor Blue", const_cast<FLinearColor*>(&FLinearColor::Blue));
	check(Result >= 0);
	Result = Engine->RegisterGlobalProperty("const FLinearColor Yellow", const_cast<FLinearColor*>(&FLinearColor::Yellow));
	check(Result >= 0);

	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
