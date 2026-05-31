// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "AlphaBlend.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"

#include <new>

namespace
{
	static void FAlphaBlendArgs_DefaultConstruct(FAlphaBlendArgs* Memory)
	{
		new (Memory) FAlphaBlendArgs();
	}

	static void FAlphaBlendArgs_ConstructFromBlendTime(float InBlendTime, FAlphaBlendArgs* Memory)
	{
		new (Memory) FAlphaBlendArgs(InBlendTime);
	}

	static void FAlphaBlendArgs_ConstructFromFAlphaBlend(const FAlphaBlend& InAlphaBlend, FAlphaBlendArgs* Memory)
	{
		new (Memory) FAlphaBlendArgs(InAlphaBlend);
	}

	static void FAlphaBlendArgs_CopyConstruct(const FAlphaBlendArgs& Other, FAlphaBlendArgs* Memory)
	{
		new (Memory) FAlphaBlendArgs(Other);
	}

	static void FAlphaBlendArgs_Destruct(FAlphaBlendArgs* Memory)
	{
		Memory->~FAlphaBlendArgs();
	}

	static FAlphaBlendArgs& FAlphaBlendArgs_Assign(FAlphaBlendArgs& Value, const FAlphaBlendArgs& Other)
	{
		Value = Other;
		return Value;
	}

	static void FAlphaBlend_DefaultConstruct(FAlphaBlend* Memory)
	{
		new (Memory) FAlphaBlend();
	}

	static void FAlphaBlend_ConstructFromBlendTime(float NewBlendTime, FAlphaBlend* Memory)
	{
		new (Memory) FAlphaBlend(NewBlendTime);
	}

	static void FAlphaBlend_ConstructFromOtherAndTime(const FAlphaBlend& Other, float NewBlendTime, FAlphaBlend* Memory)
	{
		new (Memory) FAlphaBlend(Other, NewBlendTime);
	}

	static void FAlphaBlend_ConstructFromArgs(const FAlphaBlendArgs& InArgs, FAlphaBlend* Memory)
	{
		new (Memory) FAlphaBlend(InArgs);
	}

	static void FAlphaBlend_CopyConstruct(const FAlphaBlend& Other, FAlphaBlend* Memory)
	{
		new (Memory) FAlphaBlend(Other);
	}

	static void FAlphaBlend_Destruct(FAlphaBlend* Memory)
	{
		Memory->~FAlphaBlend();
	}

	static FAlphaBlend& FAlphaBlend_Assign(FAlphaBlend& Value, const FAlphaBlend& Other)
	{
		Value = Other;
		return Value;
	}

	static void FAlphaBlend_SetBlendOption(FAlphaBlend& Value, EAlphaBlendOption InBlendOption)
	{
		Value.SetBlendOption(InBlendOption);
	}

	static void FAlphaBlend_SetBlendTime(FAlphaBlend& Value, float InBlendTime)
	{
		Value.SetBlendTime(InBlendTime);
	}

	static void FAlphaBlend_SetValueRange(FAlphaBlend& Value, float Begin, float Desired)
	{
		Value.SetValueRange(Begin, Desired);
	}

	static void FAlphaBlend_SetDesiredValue(FAlphaBlend& Value, float InDesired)
	{
		Value.SetDesiredValue(InDesired);
	}

	static void FAlphaBlend_SetAlpha(FAlphaBlend& Value, float InAlpha)
	{
		Value.SetAlpha(InAlpha);
	}

	static float FAlphaBlend_Update(FAlphaBlend& Value, float InDeltaTime)
	{
		return Value.Update(InDeltaTime);
	}

	static bool FAlphaBlend_IsComplete(const FAlphaBlend& Value)
	{
		return Value.IsComplete();
	}

	static float FAlphaBlend_GetAlpha(const FAlphaBlend& Value)
	{
		return Value.GetAlpha();
	}

	static float FAlphaBlend_GetBlendedValue(const FAlphaBlend& Value)
	{
		return Value.GetBlendedValue();
	}

	static float FAlphaBlend_GetBlendTime(const FAlphaBlend& Value)
	{
		return Value.GetBlendTime();
	}

	static float FAlphaBlend_GetBlendTimeRemaining(const FAlphaBlend& Value)
	{
		return Value.GetBlendTimeRemaining();
	}

	static EAlphaBlendOption FAlphaBlend_GetBlendOption(const FAlphaBlend& Value)
	{
		return Value.GetBlendOption();
	}

	static float FAlphaBlend_GetBeginValue(const FAlphaBlend& Value)
	{
		return Value.GetBeginValue();
	}

	static float FAlphaBlend_GetDesiredValue(const FAlphaBlend& Value)
	{
		return Value.GetDesiredValue();
	}

	static void FAlphaBlend_Reset(FAlphaBlend& Value)
	{
		Value.Reset();
	}

	static void FAlphaBlend_ResetAlpha(FAlphaBlend& Value)
	{
		Value.ResetAlpha();
	}

	static float AlphaToBlendOption_Global(float InAlpha, EAlphaBlendOption InBlendOption)
	{
		return FAlphaBlend::AlphaToBlendOption(InAlpha, InBlendOption, nullptr);
	}
}

void Bind_FAlphaBlend(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = Engine->RegisterEnum("EAlphaBlendOption");
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAlphaBlendOption", "Linear",       static_cast<int>(EAlphaBlendOption::Linear));       check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAlphaBlendOption", "Cubic",        static_cast<int>(EAlphaBlendOption::Cubic));        check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAlphaBlendOption", "HermiteCubic", static_cast<int>(EAlphaBlendOption::HermiteCubic)); check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAlphaBlendOption", "Sinusoidal",   static_cast<int>(EAlphaBlendOption::Sinusoidal));   check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAlphaBlendOption", "QuadraticInOut", static_cast<int>(EAlphaBlendOption::QuadraticInOut)); check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAlphaBlendOption", "CubicInOut",   static_cast<int>(EAlphaBlendOption::CubicInOut));   check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAlphaBlendOption", "QuarticInOut", static_cast<int>(EAlphaBlendOption::QuarticInOut)); check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAlphaBlendOption", "QuinticInOut", static_cast<int>(EAlphaBlendOption::QuinticInOut)); check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAlphaBlendOption", "CircularIn",   static_cast<int>(EAlphaBlendOption::CircularIn));   check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAlphaBlendOption", "CircularOut",  static_cast<int>(EAlphaBlendOption::CircularOut));  check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAlphaBlendOption", "CircularInOut",static_cast<int>(EAlphaBlendOption::CircularInOut));check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAlphaBlendOption", "ExpIn",        static_cast<int>(EAlphaBlendOption::ExpIn));        check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAlphaBlendOption", "ExpOut",       static_cast<int>(EAlphaBlendOption::ExpOut));       check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAlphaBlendOption", "ExpInOut",     static_cast<int>(EAlphaBlendOption::ExpInOut));     check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAlphaBlendOption", "Custom",       static_cast<int>(EAlphaBlendOption::Custom));       check(Result >= 0);

	// FAlphaBlendArgs behaviors
	REGISTER_BEHAVIOUR(FAlphaBlendArgs, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FAlphaBlendArgs_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FAlphaBlendArgs, asBEHAVE_CONSTRUCT, "void f(float InBlendTime)", asFUNCTION(FAlphaBlendArgs_ConstructFromBlendTime), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FAlphaBlendArgs, asBEHAVE_CONSTRUCT, "void f(const FAlphaBlend &in InAlphaBlend)", asFUNCTION(FAlphaBlendArgs_ConstructFromFAlphaBlend), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FAlphaBlendArgs, asBEHAVE_CONSTRUCT, "void f(const FAlphaBlendArgs &in Other)", asFUNCTION(FAlphaBlendArgs_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FAlphaBlendArgs, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FAlphaBlendArgs_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FAlphaBlendArgs, "FAlphaBlendArgs &opAssign(const FAlphaBlendArgs &in Other)", asFUNCTION(FAlphaBlendArgs_Assign), asCALL_CDECL_OBJFIRST);

	REGISTER_PROPERTY(FAlphaBlendArgs, "float BlendTime", BlendTime);
	REGISTER_PROPERTY(FAlphaBlendArgs, "EAlphaBlendOption BlendOption", BlendOption);

	// FAlphaBlend behaviors
	REGISTER_BEHAVIOUR(FAlphaBlend, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FAlphaBlend_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FAlphaBlend, asBEHAVE_CONSTRUCT, "void f(float NewBlendTime)", asFUNCTION(FAlphaBlend_ConstructFromBlendTime), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FAlphaBlend, asBEHAVE_CONSTRUCT, "void f(const FAlphaBlend &in Other, float NewBlendTime)", asFUNCTION(FAlphaBlend_ConstructFromOtherAndTime), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FAlphaBlend, asBEHAVE_CONSTRUCT, "void f(const FAlphaBlendArgs &in InArgs)", asFUNCTION(FAlphaBlend_ConstructFromArgs), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FAlphaBlend, asBEHAVE_CONSTRUCT, "void f(const FAlphaBlend &in Other)", asFUNCTION(FAlphaBlend_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FAlphaBlend, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FAlphaBlend_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FAlphaBlend, "FAlphaBlend &opAssign(const FAlphaBlend &in Other)", asFUNCTION(FAlphaBlend_Assign), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FAlphaBlend, "void SetBlendOption(EAlphaBlendOption InBlendOption)", asFUNCTION(FAlphaBlend_SetBlendOption), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FAlphaBlend, "void SetBlendTime(float InBlendTime)", asFUNCTION(FAlphaBlend_SetBlendTime), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FAlphaBlend, "void SetValueRange(float Begin, float Desired)", asFUNCTION(FAlphaBlend_SetValueRange), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FAlphaBlend, "void SetDesiredValue(float InDesired)", asFUNCTION(FAlphaBlend_SetDesiredValue), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FAlphaBlend, "void SetAlpha(float InAlpha)", asFUNCTION(FAlphaBlend_SetAlpha), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FAlphaBlend, "float Update(float InDeltaTime)", asFUNCTION(FAlphaBlend_Update), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FAlphaBlend, "bool IsComplete() const", asFUNCTION(FAlphaBlend_IsComplete), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FAlphaBlend, "float GetAlpha() const", asFUNCTION(FAlphaBlend_GetAlpha), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FAlphaBlend, "float GetBlendedValue() const", asFUNCTION(FAlphaBlend_GetBlendedValue), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FAlphaBlend, "float GetBlendTime() const", asFUNCTION(FAlphaBlend_GetBlendTime), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FAlphaBlend, "float GetBlendTimeRemaining() const", asFUNCTION(FAlphaBlend_GetBlendTimeRemaining), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FAlphaBlend, "EAlphaBlendOption GetBlendOption() const", asFUNCTION(FAlphaBlend_GetBlendOption), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FAlphaBlend, "float GetBeginValue() const", asFUNCTION(FAlphaBlend_GetBeginValue), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FAlphaBlend, "float GetDesiredValue() const", asFUNCTION(FAlphaBlend_GetDesiredValue), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FAlphaBlend, "void Reset()", asFUNCTION(FAlphaBlend_Reset), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FAlphaBlend, "void ResetAlpha()", asFUNCTION(FAlphaBlend_ResetAlpha), asCALL_CDECL_OBJFIRST);

	Result = Engine->RegisterGlobalFunction(
		"float FAlphaBlend_AlphaToBlendOption(float InAlpha, EAlphaBlendOption InBlendOption)",
		asFUNCTION(AlphaToBlendOption_Global),
		asCALL_CDECL);
	check(Result >= 0);
}
