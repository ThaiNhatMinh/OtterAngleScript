// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Misc/AssertionMacros.h"
#include "Misc/FrameNumber.h"
#include "Misc/FrameTime.h"
#include "angelscript.h"

#include <new>

namespace
{
	void FFrameTime_DefaultConstruct(FFrameTime* Memory)
	{
		new (Memory) FFrameTime();
	}

	void FFrameTime_CopyConstruct(const FFrameTime& Other, FFrameTime* Memory)
	{
		new (Memory) FFrameTime(Other);
	}

	void FFrameTime_ConstructFromInt(int32 InFrameNumber, FFrameTime* Memory)
	{
		new (Memory) FFrameTime(InFrameNumber);
	}

	void FFrameTime_ConstructFromFrameNumber(const FFrameNumber& InFrameNumber, FFrameTime* Memory)
	{
		new (Memory) FFrameTime(InFrameNumber);
	}

	void FFrameTime_ConstructFromFrameNumberAndSubFrame(const FFrameNumber& InFrameNumber, float InSubFrame, FFrameTime* Memory)
	{
		new (Memory) FFrameTime(InFrameNumber, InSubFrame);
	}

	void FFrameTime_Destruct(FFrameTime* Memory)
	{
		Memory->~FFrameTime();
	}

	FFrameTime& FFrameTime_Assign(FFrameTime& Self, const FFrameTime& Other)
	{
		Self = Other;
		return Self;
	}

	bool FFrameTime_Equals(const FFrameTime& Self, FFrameTime Other)
	{
		return Self == Other;
	}

	int FFrameTime_Cmp(const FFrameTime& Self, FFrameTime Other)
	{
		if (Self < Other) return -1;
		if (Self > Other) return  1;
		return 0;
	}

	FFrameTime FFrameTime_Add(const FFrameTime& Self, FFrameTime Other)
	{
		return Self + Other;
	}

	FFrameTime FFrameTime_Sub(const FFrameTime& Self, FFrameTime Other)
	{
		return Self - Other;
	}

	FFrameTime FFrameTime_Mod(const FFrameTime& Self, FFrameTime Other)
	{
		return Self % Other;
	}

	FFrameTime FFrameTime_MulDouble(const FFrameTime& Self, double Scalar)
	{
		return Self * Scalar;
	}

	FFrameTime FFrameTime_DivDouble(const FFrameTime& Self, double Scalar)
	{
		return Self / Scalar;
	}

	FFrameTime FFrameTime_Neg(const FFrameTime& Self)
	{
		return -Self;
	}

	FFrameTime& FFrameTime_AddAssign(FFrameTime& Self, FFrameTime Other)
	{
		Self += Other;
		return Self;
	}

	FFrameTime& FFrameTime_SubAssign(FFrameTime& Self, FFrameTime Other)
	{
		Self -= Other;
		return Self;
	}

	FFrameNumber FFrameTime_GetFrame(const FFrameTime& Self)
	{
		return Self.GetFrame();
	}

	float FFrameTime_GetSubFrame(const FFrameTime& Self)
	{
		return Self.GetSubFrame();
	}

	FFrameNumber FFrameTime_FloorToFrame(const FFrameTime& Self)
	{
		return Self.FloorToFrame();
	}

	FFrameNumber FFrameTime_CeilToFrame(const FFrameTime& Self)
	{
		return Self.CeilToFrame();
	}

	FFrameNumber FFrameTime_RoundToFrame(const FFrameTime& Self)
	{
		return Self.RoundToFrame();
	}

	double FFrameTime_AsDecimal(const FFrameTime& Self)
	{
		return Self.AsDecimal();
	}

	FFrameTime FFrameTime_FromDecimal(double InDecimalFrame)
	{
		return FFrameTime::FromDecimal(InDecimalFrame);
	}
}

void Bind_FFrameTime(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = 0;

	REGISTER_BEHAVIOUR(FFrameTime, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FFrameTime_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FFrameTime, asBEHAVE_CONSTRUCT, "void f(const FFrameTime &in Other)", asFUNCTION(FFrameTime_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FFrameTime, asBEHAVE_CONSTRUCT, "void f(int InFrameNumber)", asFUNCTION(FFrameTime_ConstructFromInt), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FFrameTime, asBEHAVE_CONSTRUCT, "void f(const FFrameNumber &in InFrameNumber)", asFUNCTION(FFrameTime_ConstructFromFrameNumber), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FFrameTime, asBEHAVE_CONSTRUCT, "void f(const FFrameNumber &in InFrameNumber, float InSubFrame)", asFUNCTION(FFrameTime_ConstructFromFrameNumberAndSubFrame), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FFrameTime, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FFrameTime_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FFrameTime, "FFrameTime &opAssign(const FFrameTime &in Other)", asFUNCTION(FFrameTime_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameTime, "bool opEquals(FFrameTime Other) const", asFUNCTION(FFrameTime_Equals), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameTime, "int opCmp(FFrameTime Other) const", asFUNCTION(FFrameTime_Cmp), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FFrameTime, "FFrameTime opAdd(FFrameTime Other) const", asFUNCTION(FFrameTime_Add), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameTime, "FFrameTime opSub(FFrameTime Other) const", asFUNCTION(FFrameTime_Sub), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameTime, "FFrameTime opMod(FFrameTime Other) const", asFUNCTION(FFrameTime_Mod), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameTime, "FFrameTime opMul(double Scalar) const", asFUNCTION(FFrameTime_MulDouble), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameTime, "FFrameTime opDiv(double Scalar) const", asFUNCTION(FFrameTime_DivDouble), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameTime, "FFrameTime opNeg() const", asFUNCTION(FFrameTime_Neg), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameTime, "FFrameTime &opAddAssign(FFrameTime Other)", asFUNCTION(FFrameTime_AddAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameTime, "FFrameTime &opSubAssign(FFrameTime Other)", asFUNCTION(FFrameTime_SubAssign), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FFrameTime, "FFrameNumber GetFrame() const", asFUNCTION(FFrameTime_GetFrame), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameTime, "float GetSubFrame() const", asFUNCTION(FFrameTime_GetSubFrame), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameTime, "FFrameNumber FloorToFrame() const", asFUNCTION(FFrameTime_FloorToFrame), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameTime, "FFrameNumber CeilToFrame() const", asFUNCTION(FFrameTime_CeilToFrame), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameTime, "FFrameNumber RoundToFrame() const", asFUNCTION(FFrameTime_RoundToFrame), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FFrameTime, "double AsDecimal() const", asFUNCTION(FFrameTime_AsDecimal), asCALL_CDECL_OBJFIRST);

	REGISTER_PROPERTY(FFrameTime, "FFrameNumber FrameNumber", FrameNumber);

	Result = Engine->SetDefaultNamespace("FFrameTime");
	check(Result >= 0);

	Result = Engine->RegisterGlobalFunction("FFrameTime FromDecimal(double InDecimalFrame)", asFUNCTION(FFrameTime_FromDecimal), asCALL_CDECL);
	check(Result >= 0);

	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
