// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Misc/AssertionMacros.h"
#include "Misc/FrameNumber.h"
#include "angelscript.h"

#include <new>

namespace
{
	void FFrameNumber_DefaultConstruct(FFrameNumber* Memory)
	{
		new (Memory) FFrameNumber();
	}

	void FFrameNumber_CopyConstruct(const FFrameNumber& Other, FFrameNumber* Memory)
	{
		new (Memory) FFrameNumber(Other);
	}

	void FFrameNumber_ConstructValue(int32 InValue, FFrameNumber* Memory)
	{
		new (Memory) FFrameNumber(InValue);
	}

	void FFrameNumber_Destruct(FFrameNumber* Memory)
	{
		Memory->~FFrameNumber();
	}

	FFrameNumber& FFrameNumber_Assign(FFrameNumber& Self, const FFrameNumber& Other)
	{
		Self = Other;
		return Self;
	}

	bool FFrameNumber_Equals(const FFrameNumber& Self, const FFrameNumber& Other)
	{
		return Self == Other;
	}

	int FFrameNumber_Cmp(const FFrameNumber& Self, const FFrameNumber& Other)
	{
		if (Self < Other) return -1;
		if (Self > Other) return 1;
		return 0;
	}

	FFrameNumber FFrameNumber_Add(const FFrameNumber& Self, const FFrameNumber& Other)
	{
		return Self + Other;
	}

	FFrameNumber FFrameNumber_Sub(const FFrameNumber& Self, const FFrameNumber& Other)
	{
		return Self - Other;
	}

	FFrameNumber FFrameNumber_Mod(const FFrameNumber& Self, const FFrameNumber& Other)
	{
		return Self % Other;
	}

	FFrameNumber FFrameNumber_MulFloat(const FFrameNumber& Self, float Scalar)
	{
		return Self * Scalar;
	}

	FFrameNumber FFrameNumber_DivFloat(const FFrameNumber& Self, float Scalar)
	{
		return Self / Scalar;
	}

	FFrameNumber FFrameNumber_Neg(const FFrameNumber& Self)
	{
		return -Self;
	}

	FFrameNumber& FFrameNumber_AddAssign(FFrameNumber& Self, const FFrameNumber& Other)
	{
		Self += Other;
		return Self;
	}

	FFrameNumber& FFrameNumber_SubAssign(FFrameNumber& Self, const FFrameNumber& Other)
	{
		Self -= Other;
		return Self;
	}

	FFrameNumber& FFrameNumber_ModAssign(FFrameNumber& Self, const FFrameNumber& Other)
	{
		Self %= Other;
		return Self;
	}

	FFrameNumber& FFrameNumber_PreInc(FFrameNumber& Self)
	{
		++Self;
		return Self;
	}

	FFrameNumber& FFrameNumber_PreDec(FFrameNumber& Self)
	{
		--Self;
		return Self;
	}

	FFrameNumber FFrameNumber_PostInc(FFrameNumber& Self)
	{
		return Self++;
	}

	FFrameNumber FFrameNumber_PostDec(FFrameNumber& Self)
	{
		return Self--;
	}
}

void Bind_FFrameNumber(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = 0;

	REGISTER_BEHAVIOUR(FFrameNumber, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FFrameNumber_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FFrameNumber, asBEHAVE_CONSTRUCT, "void f(const FFrameNumber &in Other)", asFUNCTION(FFrameNumber_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FFrameNumber, asBEHAVE_CONSTRUCT, "void f(int InValue)", asFUNCTION(FFrameNumber_ConstructValue), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FFrameNumber, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FFrameNumber_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FFrameNumber, "FFrameNumber &opAssign(const FFrameNumber &in Other)", asFUNCTION(FFrameNumber_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameNumber, "bool opEquals(const FFrameNumber &in Other) const", asFUNCTION(FFrameNumber_Equals), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameNumber, "int opCmp(const FFrameNumber &in Other) const", asFUNCTION(FFrameNumber_Cmp), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FFrameNumber, "FFrameNumber opAdd(const FFrameNumber &in Other) const", asFUNCTION(FFrameNumber_Add), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameNumber, "FFrameNumber opSub(const FFrameNumber &in Other) const", asFUNCTION(FFrameNumber_Sub), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameNumber, "FFrameNumber opMod(const FFrameNumber &in Other) const", asFUNCTION(FFrameNumber_Mod), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameNumber, "FFrameNumber opMul(float Scalar) const", asFUNCTION(FFrameNumber_MulFloat), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameNumber, "FFrameNumber opDiv(float Scalar) const", asFUNCTION(FFrameNumber_DivFloat), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameNumber, "FFrameNumber opNeg() const", asFUNCTION(FFrameNumber_Neg), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FFrameNumber, "FFrameNumber &opAddAssign(const FFrameNumber &in Other)", asFUNCTION(FFrameNumber_AddAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameNumber, "FFrameNumber &opSubAssign(const FFrameNumber &in Other)", asFUNCTION(FFrameNumber_SubAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameNumber, "FFrameNumber &opModAssign(const FFrameNumber &in Other)", asFUNCTION(FFrameNumber_ModAssign), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FFrameNumber, "FFrameNumber &opPreInc()", asFUNCTION(FFrameNumber_PreInc), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameNumber, "FFrameNumber &opPreDec()", asFUNCTION(FFrameNumber_PreDec), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameNumber, "FFrameNumber opPostInc()", asFUNCTION(FFrameNumber_PostInc), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameNumber, "FFrameNumber opPostDec()", asFUNCTION(FFrameNumber_PostDec), asCALL_CDECL_OBJFIRST);

	REGISTER_PROPERTY(FFrameNumber, "int Value", Value);
}
