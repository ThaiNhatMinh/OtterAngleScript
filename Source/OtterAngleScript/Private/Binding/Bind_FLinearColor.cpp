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

	// opMul_r: free function operator*(float, FLinearColor) — cannot use asMETHODPR
	FLinearColor FLinearColor_MultiplyScalarReverse(const FLinearColor& Value, float Scalar)
	{
		return Scalar * Value;
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

	REGISTER_METHOD(FLinearColor, "FLinearColor &opAssign(const FLinearColor &in Other)", asMETHODPR(FLinearColor, operator=, (const FLinearColor&), FLinearColor&), asCALL_THISCALL);
	REGISTER_METHOD(FLinearColor, "bool opEquals(const FLinearColor &in Other) const", asMETHODPR(FLinearColor, operator==, (const FLinearColor&) const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FLinearColor, "FLinearColor opAdd(const FLinearColor &in Other) const", asMETHODPR(FLinearColor, operator+, (const FLinearColor&) const, FLinearColor), asCALL_THISCALL);
	REGISTER_METHOD(FLinearColor, "FLinearColor opSub(const FLinearColor &in Other) const", asMETHODPR(FLinearColor, operator-, (const FLinearColor&) const, FLinearColor), asCALL_THISCALL);
	REGISTER_METHOD(FLinearColor, "FLinearColor opMul(const FLinearColor &in Other) const", asMETHODPR(FLinearColor, operator*, (const FLinearColor&) const, FLinearColor), asCALL_THISCALL);
	REGISTER_METHOD(FLinearColor, "FLinearColor opMul(float Scalar) const", asMETHODPR(FLinearColor, operator*, (float) const, FLinearColor), asCALL_THISCALL);
	REGISTER_METHOD(FLinearColor, "FLinearColor opMul_r(float Scalar) const", asFUNCTION(FLinearColor_MultiplyScalarReverse), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FLinearColor, "FLinearColor opDiv(const FLinearColor &in Other) const", asMETHODPR(FLinearColor, operator/, (const FLinearColor&) const, FLinearColor), asCALL_THISCALL);
	REGISTER_METHOD(FLinearColor, "FLinearColor opDiv(float Scalar) const", asMETHODPR(FLinearColor, operator/, (float) const, FLinearColor), asCALL_THISCALL);
	REGISTER_METHOD(FLinearColor, "FLinearColor opAddAssign(const FLinearColor &in Other)", asMETHODPR(FLinearColor, operator+=, (const FLinearColor&), FLinearColor&), asCALL_THISCALL);
	REGISTER_METHOD(FLinearColor, "FLinearColor opSubAssign(const FLinearColor &in Other)", asMETHODPR(FLinearColor, operator-=, (const FLinearColor&), FLinearColor&), asCALL_THISCALL);
	REGISTER_METHOD(FLinearColor, "FLinearColor opMulAssign(const FLinearColor &in Other)", asMETHODPR(FLinearColor, operator*=, (const FLinearColor&), FLinearColor&), asCALL_THISCALL);
	REGISTER_METHOD(FLinearColor, "FLinearColor opMulAssign(float Scalar)", asMETHODPR(FLinearColor, operator*=, (float), FLinearColor&), asCALL_THISCALL);
	REGISTER_METHOD(FLinearColor, "FLinearColor opDivAssign(const FLinearColor &in Other)", asMETHODPR(FLinearColor, operator/=, (const FLinearColor&), FLinearColor&), asCALL_THISCALL);
	REGISTER_METHOD(FLinearColor, "FLinearColor opDivAssign(float Scalar)", asMETHODPR(FLinearColor, operator/=, (float), FLinearColor&), asCALL_THISCALL);

	REGISTER_METHOD(FLinearColor, "bool Equals(const FLinearColor &in Other, float Tolerance = 0.0001) const", asMETHODPR(FLinearColor, Equals, (const FLinearColor&, float) const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FLinearColor, "FLinearColor CopyWithNewOpacity(float NewOpacity) const", asMETHODPR(FLinearColor, CopyWithNewOpacity, (float) const, FLinearColor), asCALL_THISCALL);
	REGISTER_METHOD(FLinearColor, "FLinearColor GetClamped(float InMin = 0.0f, float InMax = 1.0f) const", asMETHODPR(FLinearColor, GetClamped, (float, float) const, FLinearColor), asCALL_THISCALL);
	REGISTER_METHOD(FLinearColor, "FLinearColor LinearRGBToHSV() const", asMETHODPR(FLinearColor, LinearRGBToHSV, () const, FLinearColor), asCALL_THISCALL);
	REGISTER_METHOD(FLinearColor, "FLinearColor HSVToLinearRGB() const", asMETHODPR(FLinearColor, HSVToLinearRGB, () const, FLinearColor), asCALL_THISCALL);
	REGISTER_METHOD(FLinearColor, "FLinearColor Desaturate(float Desaturation) const", asMETHODPR(FLinearColor, Desaturate, (float) const, FLinearColor), asCALL_THISCALL);
	REGISTER_METHOD(FLinearColor, "float GetLuminance() const", asMETHODPR(FLinearColor, GetLuminance, () const, float), asCALL_THISCALL);
	REGISTER_METHOD(FLinearColor, "float GetMax() const", asMETHODPR(FLinearColor, GetMax, () const, float), asCALL_THISCALL);
	REGISTER_METHOD(FLinearColor, "float GetMin() const", asMETHODPR(FLinearColor, GetMin, () const, float), asCALL_THISCALL);
	REGISTER_METHOD(FLinearColor, "bool IsAlmostBlack() const", asMETHODPR(FLinearColor, IsAlmostBlack, () const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FLinearColor, "FString ToString() const", asMETHODPR(FLinearColor, ToString, () const, FString), asCALL_THISCALL);
	REGISTER_METHOD(FLinearColor, "bool InitFromString(const FString &in Source)", asMETHODPR(FLinearColor, InitFromString, (const FString&), bool), asCALL_THISCALL);

	REGISTER_PROPERTY(FLinearColor, "float R", R);
	REGISTER_PROPERTY(FLinearColor, "float G", G);
	REGISTER_PROPERTY(FLinearColor, "float B", B);
	REGISTER_PROPERTY(FLinearColor, "float A", A);

	Result = Engine->SetDefaultNamespace("FLinearColor");
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FLinearColor MakeFromHSV8(uint8 H, uint8 S, uint8 V)", asFUNCTION(FLinearColor::MakeFromHSV8), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FLinearColor MakeRandomColor()", asFUNCTION(FLinearColor::MakeRandomColor), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FLinearColor MakeFromColorTemperature(float Temp)", asFUNCTION(FLinearColor::MakeFromColorTemperature), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FLinearColor MakeRandomSeededColor(int Seed)", asFUNCTION(FLinearColor::MakeRandomSeededColor), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FLinearColor LerpUsingHSV(const FLinearColor &in From, const FLinearColor &in To, float Progress)", asFUNCTION(FLinearColor::LerpUsingHSV), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("float Dist(const FLinearColor &in V1, const FLinearColor &in V2)", asFUNCTION(FLinearColor::Dist), asCALL_CDECL);
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
