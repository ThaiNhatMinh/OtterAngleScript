// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Internationalization/Text.h"
#include "Misc/AssertionMacros.h"
#include "Misc/FrameNumber.h"
#include "Misc/FrameRate.h"
#include "Misc/FrameTime.h"
#include "angelscript.h"

#include <new>

namespace
{
	void FFrameRate_DefaultConstruct(FFrameRate* Memory)
	{
		new (Memory) FFrameRate();
	}

	void FFrameRate_CopyConstruct(const FFrameRate& Other, FFrameRate* Memory)
	{
		new (Memory) FFrameRate(Other);
	}

	void FFrameRate_ConstructNumeratorDenominator(int32 InNumerator, int32 InDenominator, FFrameRate* Memory)
	{
		new (Memory) FFrameRate(static_cast<uint32>(InNumerator), static_cast<uint32>(InDenominator));
	}

	void FFrameRate_Destruct(FFrameRate* Memory)
	{
		Memory->~FFrameRate();
	}

	FFrameRate& FFrameRate_Assign(FFrameRate& Self, const FFrameRate& Other)
	{
		Self = Other;
		return Self;
	}

	bool FFrameRate_Equals(const FFrameRate& Self, const FFrameRate& Other)
	{
		return Self == Other;
	}

	FFrameRate FFrameRate_Mul(const FFrameRate& Self, FFrameRate Other)
	{
		return Self * Other;
	}

	FFrameRate FFrameRate_Div(const FFrameRate& Self, FFrameRate Other)
	{
		return Self / Other;
	}

	bool FFrameRate_IsValid(const FFrameRate& Self)
	{
		return Self.IsValid();
	}

	double FFrameRate_AsInterval(const FFrameRate& Self)
	{
		return Self.AsInterval();
	}

	double FFrameRate_AsDecimal(const FFrameRate& Self)
	{
		return Self.AsDecimal();
	}

	double FFrameRate_AsSeconds(const FFrameRate& Self, FFrameTime FrameTime)
	{
		return Self.AsSeconds(FrameTime);
	}

	FFrameTime FFrameRate_AsFrameTime(const FFrameRate& Self, double InTimeSeconds)
	{
		return Self.AsFrameTime(InTimeSeconds);
	}

	FFrameNumber FFrameRate_AsFrameNumber(const FFrameRate& Self, double InTimeSeconds)
	{
		return Self.AsFrameNumber(InTimeSeconds);
	}

	bool FFrameRate_IsMultipleOf(const FFrameRate& Self, FFrameRate Other)
	{
		return Self.IsMultipleOf(Other);
	}

	bool FFrameRate_IsFactorOf(const FFrameRate& Self, FFrameRate Other)
	{
		return Self.IsFactorOf(Other);
	}

	FFrameRate FFrameRate_Reciprocal(const FFrameRate& Self)
	{
		return Self.Reciprocal();
	}

	FText FFrameRate_ToPrettyText(const FFrameRate& Self)
	{
		return Self.ToPrettyText();
	}

	double FFrameRate_MaxSeconds(const FFrameRate& Self)
	{
		return Self.MaxSeconds();
	}

	FFrameTime FFrameRate_TransformTime(FFrameTime SourceTime, FFrameRate SourceRate, FFrameRate DestinationRate)
	{
		return FFrameRate::TransformTime(SourceTime, SourceRate, DestinationRate);
	}

	FFrameTime FFrameRate_Snap(FFrameTime SourceTime, FFrameRate SourceRate, FFrameRate SnapToRate)
	{
		return FFrameRate::Snap(SourceTime, SourceRate, SnapToRate);
	}
}

void Bind_FFrameRate(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = 0;

	REGISTER_BEHAVIOUR(FFrameRate, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FFrameRate_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FFrameRate, asBEHAVE_CONSTRUCT, "void f(const FFrameRate &in Other)", asFUNCTION(FFrameRate_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FFrameRate, asBEHAVE_CONSTRUCT, "void f(int InNumerator, int InDenominator)", asFUNCTION(FFrameRate_ConstructNumeratorDenominator), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FFrameRate, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FFrameRate_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FFrameRate, "FFrameRate &opAssign(const FFrameRate &in Other)", asFUNCTION(FFrameRate_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameRate, "bool opEquals(const FFrameRate &in Other) const", asFUNCTION(FFrameRate_Equals), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameRate, "FFrameRate opMul(FFrameRate Other) const", asFUNCTION(FFrameRate_Mul), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameRate, "FFrameRate opDiv(FFrameRate Other) const", asFUNCTION(FFrameRate_Div), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FFrameRate, "bool IsValid() const", asFUNCTION(FFrameRate_IsValid), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameRate, "double AsInterval() const", asFUNCTION(FFrameRate_AsInterval), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameRate, "double AsDecimal() const", asFUNCTION(FFrameRate_AsDecimal), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameRate, "double AsSeconds(FFrameTime FrameTime) const", asFUNCTION(FFrameRate_AsSeconds), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameRate, "FFrameTime AsFrameTime(double InTimeSeconds) const", asFUNCTION(FFrameRate_AsFrameTime), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameRate, "FFrameNumber AsFrameNumber(double InTimeSeconds) const", asFUNCTION(FFrameRate_AsFrameNumber), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameRate, "bool IsMultipleOf(FFrameRate Other) const", asFUNCTION(FFrameRate_IsMultipleOf), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameRate, "bool IsFactorOf(FFrameRate Other) const", asFUNCTION(FFrameRate_IsFactorOf), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameRate, "FFrameRate Reciprocal() const", asFUNCTION(FFrameRate_Reciprocal), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameRate, "FText ToPrettyText() const", asFUNCTION(FFrameRate_ToPrettyText), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameRate, "double MaxSeconds() const", asFUNCTION(FFrameRate_MaxSeconds), asCALL_CDECL_OBJFIRST);

	REGISTER_PROPERTY(FFrameRate, "int Numerator", Numerator);
	REGISTER_PROPERTY(FFrameRate, "int Denominator", Denominator);

	Result = Engine->SetDefaultNamespace("FFrameRate");
	check(Result >= 0);

	Result = Engine->RegisterGlobalFunction(
		"FFrameTime TransformTime(FFrameTime SourceTime, FFrameRate SourceRate, FFrameRate DestinationRate)",
		asFUNCTION(FFrameRate_TransformTime), asCALL_CDECL);
	check(Result >= 0);

	Result = Engine->RegisterGlobalFunction(
		"FFrameTime Snap(FFrameTime SourceTime, FFrameRate SourceRate, FFrameRate SnapToRate)",
		asFUNCTION(FFrameRate_Snap), asCALL_CDECL);
	check(Result >= 0);

	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
