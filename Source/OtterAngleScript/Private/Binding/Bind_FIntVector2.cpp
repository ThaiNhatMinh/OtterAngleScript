// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Math/IntVector.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"
#include "OtterAngleScript.h"

#include <new>

namespace
{
	using FIntVector2 = UE::Math::TIntVector2<int32>;

	// --- Constructors / destructor ---

	static void FIntVector2_DefaultConstruct(FIntVector2* Memory)
	{
		new (Memory) FIntVector2();
	}

	static void FIntVector2_CopyConstruct(const FIntVector2& Other, FIntVector2* Memory)
	{
		new (Memory) FIntVector2(Other);
	}

	static void FIntVector2_ConstructXY(int32 X, int32 Y, FIntVector2* Memory)
	{
		new (Memory) FIntVector2(X, Y);
	}

	static void FIntVector2_ConstructSingle(int32 InValue, FIntVector2* Memory)
	{
		new (Memory) FIntVector2(InValue);
	}

	static void FIntVector2_Destruct(FIntVector2* Memory)
	{
		Memory->~FIntVector2();
	}

	// --- Assignment / comparison ---

	static FIntVector2& FIntVector2_Assign(FIntVector2& Value, const FIntVector2& Other)
	{
		Value = Other;
		return Value;
	}

	static bool FIntVector2_OpEquals(const FIntVector2& Value, const FIntVector2& Other)
	{
		return Value == Other;
	}

	// --- Arithmetic operators (compound) ---

	static FIntVector2& FIntVector2_AddAssign(FIntVector2& Value, const FIntVector2& Other)
	{
		Value += Other;
		return Value;
	}

	static FIntVector2& FIntVector2_SubAssign(FIntVector2& Value, const FIntVector2& Other)
	{
		Value -= Other;
		return Value;
	}

	static FIntVector2& FIntVector2_MulAssignVector(FIntVector2& Value, const FIntVector2& Other)
	{
		Value *= Other;
		return Value;
	}

	static FIntVector2& FIntVector2_MulAssignScalar(FIntVector2& Value, int32 Scale)
	{
		Value *= Scale;
		return Value;
	}

	static FIntVector2& FIntVector2_DivAssign(FIntVector2& Value, int32 Divisor)
	{
		Value /= Divisor;
		return Value;
	}

	static FIntVector2& FIntVector2_ModAssign(FIntVector2& Value, int32 Divisor)
	{
		Value %= Divisor;
		return Value;
	}

	// --- Arithmetic operators (binary) ---

	static FIntVector2 FIntVector2_Add(const FIntVector2& Value, const FIntVector2& Other)
	{
		return Value + Other;
	}

	static FIntVector2 FIntVector2_Sub(const FIntVector2& Value, const FIntVector2& Other)
	{
		return Value - Other;
	}

	static FIntVector2 FIntVector2_MulVector(const FIntVector2& Value, const FIntVector2& Other)
	{
		return Value * Other;
	}

	static FIntVector2 FIntVector2_MulScalar(const FIntVector2& Value, int32 Scale)
	{
		return Value * Scale;
	}

	static FIntVector2 FIntVector2_Div(const FIntVector2& Value, int32 Divisor)
	{
		return Value / Divisor;
	}

	static FIntVector2 FIntVector2_Mod(const FIntVector2& Value, int32 Divisor)
	{
		return Value % Divisor;
	}

	// --- Bitwise operators ---

	static FIntVector2 FIntVector2_ShiftRight(const FIntVector2& Value, int32 Shift)
	{
		return Value >> Shift;
	}

	static FIntVector2 FIntVector2_ShiftLeft(const FIntVector2& Value, int32 Shift)
	{
		return Value << Shift;
	}

	static FIntVector2 FIntVector2_BitAnd(const FIntVector2& Value, int32 Other)
	{
		return Value & Other;
	}

	static FIntVector2 FIntVector2_BitOr(const FIntVector2& Value, int32 Other)
	{
		return Value | Other;
	}

	static FIntVector2 FIntVector2_BitXor(const FIntVector2& Value, int32 Other)
	{
		return Value ^ Other;
	}

	// --- Index operator ---

	static int32 FIntVector2_GetIndex(const FIntVector2& Value, int32 Index)
	{
		return Value[Index];
	}

	static void FIntVector2_SetIndex(FIntVector2& Value, int32 Index, int32 InValue)
	{
		Value[Index] = InValue;
	}

	// --- Methods ---

	static bool FIntVector2_IsZero(const FIntVector2& Value)
	{
		return Value.IsZero();
	}

	static int32 FIntVector2_GetMax(const FIntVector2& Value)
	{
		return Value.GetMax();
	}

	static int32 FIntVector2_GetAbsMax(const FIntVector2& Value)
	{
		return Value.GetAbsMax();
	}

	static int32 FIntVector2_GetMin(const FIntVector2& Value)
	{
		return Value.GetMin();
	}

	static int32 FIntVector2_GetAbsMin(const FIntVector2& Value)
	{
		return Value.GetAbsMin();
	}

	static FIntVector2 FIntVector2_ComponentMax(const FIntVector2& Value, const FIntVector2& Other)
	{
		return Value.ComponentMax(Other);
	}

	static FIntVector2 FIntVector2_ComponentMin(const FIntVector2& Value, const FIntVector2& Other)
	{
		return Value.ComponentMin(Other);
	}

	static FString FIntVector2_ToString(const FIntVector2& Value)
	{
		return Value.ToString();
	}

	static bool FIntVector2_InitFromString(FIntVector2& Value, const FString& InSourceString)
	{
		return Value.InitFromString(InSourceString);
	}

	// --- Static methods ---

	static int32 FIntVector2_Num()
	{
		return FIntVector2::Num();
	}

	static FIntVector2 FIntVector2_DivideAndRoundUpScalar(FIntVector2 Lhs, int32 Divisor)
	{
		return FIntVector2::DivideAndRoundUp(Lhs, Divisor);
	}

	static FIntVector2 FIntVector2_DivideAndRoundUpVector(FIntVector2 Lhs, FIntVector2 Divisor)
	{
		return FIntVector2::DivideAndRoundUp(Lhs, Divisor);
	}
}

void Bind_FIntVector2(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;

	// Properties
	REGISTER_PROPERTY(FIntVector2, "int X", X);
	REGISTER_PROPERTY(FIntVector2, "int Y", Y);

	// Behaviours (constructors / destructor)
	REGISTER_BEHAVIOUR(FIntVector2, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FIntVector2_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FIntVector2, asBEHAVE_CONSTRUCT, "void f(const FIntVector2 &in Other)", asFUNCTION(FIntVector2_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FIntVector2, asBEHAVE_CONSTRUCT, "void f(int X, int Y)", asFUNCTION(FIntVector2_ConstructXY), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FIntVector2, asBEHAVE_CONSTRUCT, "void f(int InValue)", asFUNCTION(FIntVector2_ConstructSingle), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FIntVector2, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FIntVector2_Destruct), asCALL_CDECL_OBJLAST);

	// Operators
	REGISTER_METHOD(FIntVector2, "FIntVector2 &opAssign(const FIntVector2 &in Other)", asFUNCTION(FIntVector2_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector2, "bool opEquals(const FIntVector2 &in Other) const", asFUNCTION(FIntVector2_OpEquals), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FIntVector2, "FIntVector2 &opAddAssign(const FIntVector2 &in Other)", asFUNCTION(FIntVector2_AddAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector2, "FIntVector2 &opSubAssign(const FIntVector2 &in Other)", asFUNCTION(FIntVector2_SubAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector2, "FIntVector2 &opMulAssign(const FIntVector2 &in Other)", asFUNCTION(FIntVector2_MulAssignVector), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector2, "FIntVector2 &opMulAssign(int Scale)", asFUNCTION(FIntVector2_MulAssignScalar), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector2, "FIntVector2 &opDivAssign(int Divisor)", asFUNCTION(FIntVector2_DivAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector2, "FIntVector2 &opModAssign(int Divisor)", asFUNCTION(FIntVector2_ModAssign), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FIntVector2, "FIntVector2 opAdd(const FIntVector2 &in Other) const", asFUNCTION(FIntVector2_Add), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector2, "FIntVector2 opSub(const FIntVector2 &in Other) const", asFUNCTION(FIntVector2_Sub), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector2, "FIntVector2 opMul(const FIntVector2 &in Other) const", asFUNCTION(FIntVector2_MulVector), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector2, "FIntVector2 opMul(int Scale) const", asFUNCTION(FIntVector2_MulScalar), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector2, "FIntVector2 opDiv(int Divisor) const", asFUNCTION(FIntVector2_Div), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector2, "FIntVector2 opMod(int Divisor) const", asFUNCTION(FIntVector2_Mod), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FIntVector2, "FIntVector2 opShr(int Shift) const", asFUNCTION(FIntVector2_ShiftRight), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector2, "FIntVector2 opShl(int Shift) const", asFUNCTION(FIntVector2_ShiftLeft), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector2, "FIntVector2 opAnd(int Other) const", asFUNCTION(FIntVector2_BitAnd), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector2, "FIntVector2 opOr(int Other) const", asFUNCTION(FIntVector2_BitOr), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector2, "FIntVector2 opXor(int Other) const", asFUNCTION(FIntVector2_BitXor), asCALL_CDECL_OBJFIRST);

	// Index operator
	REGISTER_METHOD(FIntVector2, "int opIndex(int Index) const", asFUNCTION(FIntVector2_GetIndex), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector2, "void opIndexAssign(int Index, int Value)", asFUNCTION(FIntVector2_SetIndex), asCALL_CDECL_OBJFIRST);

	// Methods
	REGISTER_METHOD(FIntVector2, "bool IsZero() const", asFUNCTION(FIntVector2_IsZero), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector2, "int GetMax() const", asFUNCTION(FIntVector2_GetMax), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector2, "int GetAbsMax() const", asFUNCTION(FIntVector2_GetAbsMax), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector2, "int GetMin() const", asFUNCTION(FIntVector2_GetMin), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector2, "int GetAbsMin() const", asFUNCTION(FIntVector2_GetAbsMin), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector2, "FIntVector2 ComponentMax(const FIntVector2 &in Other) const", asFUNCTION(FIntVector2_ComponentMax), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector2, "FIntVector2 ComponentMin(const FIntVector2 &in Other) const", asFUNCTION(FIntVector2_ComponentMin), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector2, "FString ToString() const", asFUNCTION(FIntVector2_ToString), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector2, "bool InitFromString(const FString &in InSourceString)", asFUNCTION(FIntVector2_InitFromString), asCALL_CDECL_OBJFIRST);

	// Static methods
	Result = Engine->SetDefaultNamespace("FIntVector2");
	check(Result >= 0);

	Result = Engine->RegisterGlobalFunction("int Num()", asFUNCTION(FIntVector2_Num), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FIntVector2 DivideAndRoundUp(FIntVector2 Lhs, int Divisor)", asFUNCTION(FIntVector2_DivideAndRoundUpScalar), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FIntVector2 DivideAndRoundUp(FIntVector2 Lhs, FIntVector2 Divisor)", asFUNCTION(FIntVector2_DivideAndRoundUpVector), asCALL_CDECL);
	check(Result >= 0);

	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
