// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Math/Color.h"
#include "Misc/CoreMiscDefines.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"

#include <new>

namespace
{
	void FColor_DefaultConstruct(FColor* Memory)
	{
		new (Memory) FColor();
	}

	void FColor_CopyConstruct(const FColor& Other, FColor* Memory)
	{
		new (Memory) FColor(Other);
	}

	void FColor_ConstructForceInit(EForceInit, FColor* Memory)
	{
		new (Memory) FColor(ForceInit);
	}

	void FColor_ConstructRGB(uint8 InR, uint8 InG, uint8 InB, FColor* Memory)
	{
		new (Memory) FColor(InR, InG, InB);
	}

	void FColor_ConstructRGBA(uint8 InR, uint8 InG, uint8 InB, uint8 InA, FColor* Memory)
	{
		new (Memory) FColor(InR, InG, InB, InA);
	}

	void FColor_ConstructFromBits(uint32 InBits, FColor* Memory)
	{
		new (Memory) FColor(InBits);
	}

	void FColor_Destruct(FColor* Memory)
	{
		Memory->~FColor();
	}
}

void Bind_FColor(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;
	REGISTER_BEHAVIOUR(FColor, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FColor_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FColor, asBEHAVE_CONSTRUCT, "void f(const FColor &in Other)", asFUNCTION(FColor_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FColor, asBEHAVE_CONSTRUCT, "void f(EForceInitType)", asFUNCTION(FColor_ConstructForceInit), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FColor, asBEHAVE_CONSTRUCT, "void f(uint8 R, uint8 G, uint8 B)", asFUNCTION(FColor_ConstructRGB), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FColor, asBEHAVE_CONSTRUCT, "void f(uint8 R, uint8 G, uint8 B, uint8 A)", asFUNCTION(FColor_ConstructRGBA), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FColor, asBEHAVE_CONSTRUCT, "void f(uint InBits)", asFUNCTION(FColor_ConstructFromBits), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FColor, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FColor_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FColor, "FColor &opAssign(const FColor &in Other)", asMETHODPR(FColor, operator=, (const FColor&), FColor&), asCALL_THISCALL);
	REGISTER_METHOD(FColor, "bool opEquals(const FColor &in C) const", asMETHODPR(FColor, operator==, (const FColor&) const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FColor, "void opAddAssign(const FColor &in C)", asMETHODPR(FColor, operator+=, (const FColor&), void), asCALL_THISCALL);

	REGISTER_METHOD(FColor, "FLinearColor FromRGBE() const", asMETHODPR(FColor, FromRGBE, () const, FLinearColor), asCALL_THISCALL);
	REGISTER_METHOD(FColor, "FColor WithAlpha(uint8 Alpha) const", asMETHODPR(FColor, WithAlpha, (uint8) const, FColor), asCALL_THISCALL);
	REGISTER_METHOD(FColor, "FLinearColor ReinterpretAsLinear() const", asMETHODPR(FColor, ReinterpretAsLinear, () const, FLinearColor), asCALL_THISCALL);
	REGISTER_METHOD(FColor, "FString ToHex() const", asMETHODPR(FColor, ToHex, () const, FString), asCALL_THISCALL);
	REGISTER_METHOD(FColor, "FString ToString() const", asMETHODPR(FColor, ToString, () const, FString), asCALL_THISCALL);
	REGISTER_METHOD(FColor, "bool InitFromString(const FString &in InSourceString)", asMETHODPR(FColor, InitFromString, (const FString&), bool), asCALL_THISCALL);
	REGISTER_METHOD(FColor, "uint ToPackedARGB() const", asMETHODPR(FColor, ToPackedARGB, () const, uint32), asCALL_THISCALL);
	REGISTER_METHOD(FColor, "uint ToPackedABGR() const", asMETHODPR(FColor, ToPackedABGR, () const, uint32), asCALL_THISCALL);
	REGISTER_METHOD(FColor, "uint ToPackedRGBA() const", asMETHODPR(FColor, ToPackedRGBA, () const, uint32), asCALL_THISCALL);
	REGISTER_METHOD(FColor, "uint ToPackedBGRA() const", asMETHODPR(FColor, ToPackedBGRA, () const, uint32), asCALL_THISCALL);

	REGISTER_PROPERTY(FColor, "uint8 R", R);
	REGISTER_PROPERTY(FColor, "uint8 G", G);
	REGISTER_PROPERTY(FColor, "uint8 B", B);
	REGISTER_PROPERTY(FColor, "uint8 A", A);
	REGISTER_PROPERTY(FColor, "uint Bits", Bits);

	Result = Engine->SetDefaultNamespace("FColor");
	check(Result >= 0);

	Result = Engine->RegisterGlobalFunction("FColor FromHex(const FString &in HexString)", asFUNCTION(FColor::FromHex), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FColor MakeRandomColor()", asFUNCTION(FColor::MakeRandomColor), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FColor MakeRedToGreenColorFromScalar(float Scalar)", asFUNCTION(FColor::MakeRedToGreenColorFromScalar), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FColor MakeFromColorTemperature(float Temp)", asFUNCTION(FColor::MakeFromColorTemperature), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FColor MakeRandomSeededColor(int Seed)", asFUNCTION(FColor::MakeRandomSeededColor), asCALL_CDECL);
	check(Result >= 0);

	Result = Engine->RegisterGlobalProperty("const FColor White", const_cast<FColor*>(&FColor::White));
	check(Result >= 0);
	Result = Engine->RegisterGlobalProperty("const FColor Black", const_cast<FColor*>(&FColor::Black));
	check(Result >= 0);
	Result = Engine->RegisterGlobalProperty("const FColor Transparent", const_cast<FColor*>(&FColor::Transparent));
	check(Result >= 0);
	Result = Engine->RegisterGlobalProperty("const FColor Red", const_cast<FColor*>(&FColor::Red));
	check(Result >= 0);
	Result = Engine->RegisterGlobalProperty("const FColor Green", const_cast<FColor*>(&FColor::Green));
	check(Result >= 0);
	Result = Engine->RegisterGlobalProperty("const FColor Blue", const_cast<FColor*>(&FColor::Blue));
	check(Result >= 0);
	Result = Engine->RegisterGlobalProperty("const FColor Yellow", const_cast<FColor*>(&FColor::Yellow));
	check(Result >= 0);
	Result = Engine->RegisterGlobalProperty("const FColor Cyan", const_cast<FColor*>(&FColor::Cyan));
	check(Result >= 0);
	Result = Engine->RegisterGlobalProperty("const FColor Magenta", const_cast<FColor*>(&FColor::Magenta));
	check(Result >= 0);
	Result = Engine->RegisterGlobalProperty("const FColor Orange", const_cast<FColor*>(&FColor::Orange));
	check(Result >= 0);
	Result = Engine->RegisterGlobalProperty("const FColor Purple", const_cast<FColor*>(&FColor::Purple));
	check(Result >= 0);
	Result = Engine->RegisterGlobalProperty("const FColor Turquoise", const_cast<FColor*>(&FColor::Turquoise));
	check(Result >= 0);
	Result = Engine->RegisterGlobalProperty("const FColor Silver", const_cast<FColor*>(&FColor::Silver));
	check(Result >= 0);
	Result = Engine->RegisterGlobalProperty("const FColor Emerald", const_cast<FColor*>(&FColor::Emerald));
	check(Result >= 0);

	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
