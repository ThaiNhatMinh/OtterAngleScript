// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Misc/Timespan.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"
#include "OtterAngleScript.h"

#include <new>

namespace
{
	// --- Constructors / destructor ---

	static void FTimespan_DefaultConstruct(FTimespan* Memory)
	{
		new (Memory) FTimespan();
	}

	static void FTimespan_CopyConstruct(const FTimespan& Other, FTimespan* Memory)
	{
		new (Memory) FTimespan(Other);
	}

	static void FTimespan_ConstructTicks(int64 InTicks, FTimespan* Memory)
	{
		new (Memory) FTimespan(InTicks);
	}

	static void FTimespan_ConstructHMS(int32 Hours, int32 Minutes, int32 Seconds, FTimespan* Memory)
	{
		new (Memory) FTimespan(Hours, Minutes, Seconds);
	}

	static void FTimespan_ConstructDHMS(int32 Days, int32 Hours, int32 Minutes, int32 Seconds, FTimespan* Memory)
	{
		new (Memory) FTimespan(Days, Hours, Minutes, Seconds);
	}

	static void FTimespan_ConstructDHMSNano(int32 Days, int32 Hours, int32 Minutes, int32 Seconds, int32 FractionNano, FTimespan* Memory)
	{
		new (Memory) FTimespan(Days, Hours, Minutes, Seconds, FractionNano);
	}

	static void FTimespan_Destruct(FTimespan* Memory)
	{
		Memory->~FTimespan();
	}

	// --- Operators ---

	static FTimespan& FTimespan_Assign(FTimespan& Value, const FTimespan& Other)
	{
		Value = Other;
		return Value;
	}

	static bool FTimespan_OpEquals(const FTimespan& Value, const FTimespan& Other)
	{
		return Value == Other;
	}

	static int FTimespan_OpCmp(const FTimespan& Value, const FTimespan& Other)
	{
		if (Value < Other) return -1;
		if (Other < Value) return 1;
		return 0;
	}

	static FTimespan FTimespan_Add(const FTimespan& Value, const FTimespan& Other)
	{
		return Value + Other;
	}

	static FTimespan& FTimespan_AddAssign(FTimespan& Value, const FTimespan& Other)
	{
		Value += Other;
		return Value;
	}

	static FTimespan FTimespan_Negate(const FTimespan& Value)
	{
		return -Value;
	}

	static FTimespan FTimespan_Sub(const FTimespan& Value, const FTimespan& Other)
	{
		return Value - Other;
	}

	static FTimespan& FTimespan_SubAssign(FTimespan& Value, const FTimespan& Other)
	{
		Value -= Other;
		return Value;
	}

	static FTimespan FTimespan_Mul(const FTimespan& Value, double Scalar)
	{
		return Value * Scalar;
	}

	static FTimespan& FTimespan_MulAssign(FTimespan& Value, double Scalar)
	{
		Value *= Scalar;
		return Value;
	}

	static FTimespan FTimespan_Div(const FTimespan& Value, double Scalar)
	{
		return Value / Scalar;
	}

	static FTimespan& FTimespan_DivAssign(FTimespan& Value, double Scalar)
	{
		Value /= Scalar;
		return Value;
	}

	static FTimespan FTimespan_Mod(const FTimespan& Value, const FTimespan& Other)
	{
		return Value % Other;
	}

	static FTimespan& FTimespan_ModAssign(FTimespan& Value, const FTimespan& Other)
	{
		Value %= Other;
		return Value;
	}

	// --- Accessors ---

	static int32 FTimespan_GetDays(const FTimespan& Value)
	{
		return Value.GetDays();
	}

	static FTimespan FTimespan_GetDuration(FTimespan& Value)
	{
		return Value.GetDuration();
	}

	static int32 FTimespan_GetFractionMicro(const FTimespan& Value)
	{
		return Value.GetFractionMicro();
	}

	static int32 FTimespan_GetFractionMilli(const FTimespan& Value)
	{
		return Value.GetFractionMilli();
	}

	static int32 FTimespan_GetFractionNano(const FTimespan& Value)
	{
		return Value.GetFractionNano();
	}

	static int32 FTimespan_GetHours(const FTimespan& Value)
	{
		return Value.GetHours();
	}

	static int32 FTimespan_GetMinutes(const FTimespan& Value)
	{
		return Value.GetMinutes();
	}

	static int32 FTimespan_GetSeconds(const FTimespan& Value)
	{
		return Value.GetSeconds();
	}

	static int64 FTimespan_GetTicks(const FTimespan& Value)
	{
		return Value.GetTicks();
	}

	static double FTimespan_GetTotalDays(const FTimespan& Value)
	{
		return Value.GetTotalDays();
	}

	static double FTimespan_GetTotalHours(const FTimespan& Value)
	{
		return Value.GetTotalHours();
	}

	static double FTimespan_GetTotalMicroseconds(const FTimespan& Value)
	{
		return Value.GetTotalMicroseconds();
	}

	static double FTimespan_GetTotalMilliseconds(const FTimespan& Value)
	{
		return Value.GetTotalMilliseconds();
	}

	static double FTimespan_GetTotalMinutes(const FTimespan& Value)
	{
		return Value.GetTotalMinutes();
	}

	static double FTimespan_GetTotalSeconds(const FTimespan& Value)
	{
		return Value.GetTotalSeconds();
	}

	static bool FTimespan_IsZero(const FTimespan& Value)
	{
		return Value.IsZero();
	}

	static FString FTimespan_ToStringDefault(const FTimespan& Value)
	{
		return Value.ToString();
	}

	static FString FTimespan_ToStringFormat(const FTimespan& Value, const FString& Format)
	{
		return Value.ToString(*Format);
	}

	// --- Static methods ---

	static FTimespan FTimespan_FromDays(double Days)
	{
		return FTimespan::FromDays(Days);
	}

	static FTimespan FTimespan_FromHours(double Hours)
	{
		return FTimespan::FromHours(Hours);
	}

	static FTimespan FTimespan_FromMicroseconds(double Microseconds)
	{
		return FTimespan::FromMicroseconds(Microseconds);
	}

	static FTimespan FTimespan_FromMilliseconds(double Milliseconds)
	{
		return FTimespan::FromMilliseconds(Milliseconds);
	}

	static FTimespan FTimespan_FromMinutes(double Minutes)
	{
		return FTimespan::FromMinutes(Minutes);
	}

	static FTimespan FTimespan_FromSeconds(double Seconds)
	{
		return FTimespan::FromSeconds(Seconds);
	}

	static FTimespan FTimespan_MaxValue()
	{
		return FTimespan::MaxValue();
	}

	static FTimespan FTimespan_MinValue()
	{
		return FTimespan::MinValue();
	}

	static bool FTimespan_Parse(const FString& TimespanString, FTimespan& OutTimespan)
	{
		return FTimespan::Parse(TimespanString, OutTimespan);
	}
}

void Bind_FTimespan(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;

	REGISTER_BEHAVIOUR(FTimespan, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FTimespan_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FTimespan, asBEHAVE_CONSTRUCT, "void f(const FTimespan &in Other)", asFUNCTION(FTimespan_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FTimespan, asBEHAVE_CONSTRUCT, "void f(int64 Ticks)", asFUNCTION(FTimespan_ConstructTicks), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FTimespan, asBEHAVE_CONSTRUCT, "void f(int Hours, int Minutes, int Seconds)", asFUNCTION(FTimespan_ConstructHMS), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FTimespan, asBEHAVE_CONSTRUCT, "void f(int Days, int Hours, int Minutes, int Seconds)", asFUNCTION(FTimespan_ConstructDHMS), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FTimespan, asBEHAVE_CONSTRUCT, "void f(int Days, int Hours, int Minutes, int Seconds, int FractionNano)", asFUNCTION(FTimespan_ConstructDHMSNano), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FTimespan, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FTimespan_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FTimespan, "FTimespan &opAssign(const FTimespan &in Other)", asFUNCTION(FTimespan_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimespan, "bool opEquals(const FTimespan &in Other) const", asFUNCTION(FTimespan_OpEquals), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimespan, "int opCmp(const FTimespan &in Other) const", asFUNCTION(FTimespan_OpCmp), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FTimespan, "FTimespan opAdd(const FTimespan &in Other) const", asFUNCTION(FTimespan_Add), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimespan, "FTimespan &opAddAssign(const FTimespan &in Other)", asFUNCTION(FTimespan_AddAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimespan, "FTimespan opNeg() const", asFUNCTION(FTimespan_Negate), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimespan, "FTimespan opSub(const FTimespan &in Other) const", asFUNCTION(FTimespan_Sub), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimespan, "FTimespan &opSubAssign(const FTimespan &in Other)", asFUNCTION(FTimespan_SubAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimespan, "FTimespan opMul(double Scalar) const", asFUNCTION(FTimespan_Mul), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimespan, "FTimespan &opMulAssign(double Scalar)", asFUNCTION(FTimespan_MulAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimespan, "FTimespan opDiv(double Scalar) const", asFUNCTION(FTimespan_Div), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimespan, "FTimespan &opDivAssign(double Scalar)", asFUNCTION(FTimespan_DivAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimespan, "FTimespan opMod(const FTimespan &in Other) const", asFUNCTION(FTimespan_Mod), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimespan, "FTimespan &opModAssign(const FTimespan &in Other)", asFUNCTION(FTimespan_ModAssign), asCALL_CDECL_OBJFIRST);

	// Accessors
	REGISTER_METHOD(FTimespan, "int GetDays() const", asFUNCTION(FTimespan_GetDays), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimespan, "FTimespan GetDuration() const", asFUNCTION(FTimespan_GetDuration), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimespan, "int GetFractionMicro() const", asFUNCTION(FTimespan_GetFractionMicro), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimespan, "int GetFractionMilli() const", asFUNCTION(FTimespan_GetFractionMilli), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimespan, "int GetFractionNano() const", asFUNCTION(FTimespan_GetFractionNano), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimespan, "int GetHours() const", asFUNCTION(FTimespan_GetHours), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimespan, "int GetMinutes() const", asFUNCTION(FTimespan_GetMinutes), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimespan, "int GetSeconds() const", asFUNCTION(FTimespan_GetSeconds), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimespan, "int64 GetTicks() const", asFUNCTION(FTimespan_GetTicks), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimespan, "double GetTotalDays() const", asFUNCTION(FTimespan_GetTotalDays), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimespan, "double GetTotalHours() const", asFUNCTION(FTimespan_GetTotalHours), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimespan, "double GetTotalMicroseconds() const", asFUNCTION(FTimespan_GetTotalMicroseconds), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimespan, "double GetTotalMilliseconds() const", asFUNCTION(FTimespan_GetTotalMilliseconds), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimespan, "double GetTotalMinutes() const", asFUNCTION(FTimespan_GetTotalMinutes), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimespan, "double GetTotalSeconds() const", asFUNCTION(FTimespan_GetTotalSeconds), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimespan, "bool IsZero() const", asFUNCTION(FTimespan_IsZero), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimespan, "FString ToString() const", asFUNCTION(FTimespan_ToStringDefault), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FTimespan, "FString ToString(const FString &in Format) const", asFUNCTION(FTimespan_ToStringFormat), asCALL_CDECL_OBJFIRST);

	// Static methods under FTimespan namespace
	Result = Engine->SetDefaultNamespace("FTimespan");
	check(Result >= 0);

	Result = Engine->RegisterGlobalFunction("FTimespan FromDays(double Days)", asFUNCTION(FTimespan_FromDays), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FTimespan FromHours(double Hours)", asFUNCTION(FTimespan_FromHours), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FTimespan FromMicroseconds(double Microseconds)", asFUNCTION(FTimespan_FromMicroseconds), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FTimespan FromMilliseconds(double Milliseconds)", asFUNCTION(FTimespan_FromMilliseconds), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FTimespan FromMinutes(double Minutes)", asFUNCTION(FTimespan_FromMinutes), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FTimespan FromSeconds(double Seconds)", asFUNCTION(FTimespan_FromSeconds), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FTimespan MaxValue()", asFUNCTION(FTimespan_MaxValue), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FTimespan MinValue()", asFUNCTION(FTimespan_MinValue), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("bool Parse(const FString &in TimespanString, FTimespan &out OutTimespan)", asFUNCTION(FTimespan_Parse), asCALL_CDECL);
	check(Result >= 0);

	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
