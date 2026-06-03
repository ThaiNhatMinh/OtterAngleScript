// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Math/IntVector.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"
#include "OtterAngleScript.h"

#include <new>

namespace
{
	// --- Constructors / destructor ---

	static void FIntVector_DefaultConstruct(FIntVector* Memory)
	{
		new (Memory) FIntVector();
	}

	static void FIntVector_CopyConstruct(const FIntVector& Other, FIntVector* Memory)
	{
		new (Memory) FIntVector(Other);
	}

	static void FIntVector_ConstructXYZ(int32 X, int32 Y, int32 Z, FIntVector* Memory)
	{
		new (Memory) FIntVector(X, Y, Z);
	}

	static void FIntVector_ConstructSingle(int32 InValue, FIntVector* Memory)
	{
		new (Memory) FIntVector(InValue);
	}

	static void FIntVector_Destruct(FIntVector* Memory)
	{
		Memory->~FIntVector();
	}

	// --- Assignment / comparison ---

	static FIntVector& FIntVector_Assign(FIntVector& Value, const FIntVector& Other)
	{
		Value = Other;
		return Value;
	}

	static bool FIntVector_OpEquals(const FIntVector& Value, const FIntVector& Other)
	{
		return Value == Other;
	}

	// --- Arithmetic operators (compound) ---

	static FIntVector& FIntVector_AddAssign(FIntVector& Value, const FIntVector& Other)
	{
		Value += Other;
		return Value;
	}

	static FIntVector& FIntVector_SubAssign(FIntVector& Value, const FIntVector& Other)
	{
		Value -= Other;
		return Value;
	}

	static FIntVector& FIntVector_MulAssignVector(FIntVector& Value, const FIntVector& Other)
	{
		Value *= Other;
		return Value;
	}

	static FIntVector& FIntVector_MulAssignScalar(FIntVector& Value, int32 Scale)
	{
		Value *= Scale;
		return Value;
	}

	static FIntVector& FIntVector_DivAssign(FIntVector& Value, int32 Divisor)
	{
		Value /= Divisor;
		return Value;
	}

	static FIntVector& FIntVector_ModAssign(FIntVector& Value, int32 Divisor)
	{
		Value %= Divisor;
		return Value;
	}

	// --- Arithmetic operators (binary) ---

	static FIntVector FIntVector_Add(const FIntVector& Value, const FIntVector& Other)
	{
		return Value + Other;
	}

	static FIntVector FIntVector_Sub(const FIntVector& Value, const FIntVector& Other)
	{
		return Value - Other;
	}

	static FIntVector FIntVector_MulVector(const FIntVector& Value, const FIntVector& Other)
	{
		return Value * Other;
	}

	static FIntVector FIntVector_MulScalar(const FIntVector& Value, int32 Scale)
	{
		return Value * Scale;
	}

	static FIntVector FIntVector_Div(const FIntVector& Value, int32 Divisor)
	{
		return Value / Divisor;
	}

	static FIntVector FIntVector_Mod(const FIntVector& Value, int32 Divisor)
	{
		return Value % Divisor;
	}

	// --- Bitwise operators ---

	static FIntVector FIntVector_ShiftRight(const FIntVector& Value, int32 Shift)
	{
		return Value >> Shift;
	}

	static FIntVector FIntVector_ShiftLeft(const FIntVector& Value, int32 Shift)
	{
		return Value << Shift;
	}

	static FIntVector FIntVector_BitAnd(const FIntVector& Value, int32 Other)
	{
		return Value & Other;
	}

	static FIntVector FIntVector_BitOr(const FIntVector& Value, int32 Other)
	{
		return Value | Other;
	}

	static FIntVector FIntVector_BitXor(const FIntVector& Value, int32 Other)
	{
		return Value ^ Other;
	}

	// --- Methods ---

	static bool FIntVector_IsZero(const FIntVector& Value)
	{
		return Value.IsZero();
	}

	static int32 FIntVector_GetMax(const FIntVector& Value)
	{
		return Value.GetMax();
	}

	static int32 FIntVector_GetAbsMax(const FIntVector& Value)
	{
		return Value.GetAbsMax();
	}

	static int32 FIntVector_GetMin(const FIntVector& Value)
	{
		return Value.GetMin();
	}

	static int32 FIntVector_GetAbsMin(const FIntVector& Value)
	{
		return Value.GetAbsMin();
	}

	static int32 FIntVector_Size(const FIntVector& Value)
	{
		return Value.Size();
	}

	static FIntVector FIntVector_ComponentMax(const FIntVector& Value, const FIntVector& Other)
	{
		return Value.ComponentMax(Other);
	}

	static FIntVector FIntVector_ComponentMin(const FIntVector& Value, const FIntVector& Other)
	{
		return Value.ComponentMin(Other);
	}

	static FString FIntVector_ToString(const FIntVector& Value)
	{
		return Value.ToString();
	}

	static bool FIntVector_InitFromString(FIntVector& Value, const FString& InSourceString)
	{
		return Value.InitFromString(InSourceString);
	}

	// --- Static methods ---

	static int32 FIntVector_Num()
	{
		return FIntVector::Num();
	}
}

void Bind_FIntVector(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;

	Result = Engine->RegisterObjectType("FIntVector", sizeof(FIntVector),
		asOBJ_VALUE | asGetTypeTraits<FIntVector>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	// Properties
	REGISTER_PROPERTY(FIntVector, "int X", X);
	REGISTER_PROPERTY(FIntVector, "int Y", Y);
	REGISTER_PROPERTY(FIntVector, "int Z", Z);

	// Behaviours (constructors / destructor)
	REGISTER_BEHAVIOUR(FIntVector, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FIntVector_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FIntVector, asBEHAVE_CONSTRUCT, "void f(const FIntVector &in Other)", asFUNCTION(FIntVector_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FIntVector, asBEHAVE_CONSTRUCT, "void f(int X, int Y, int Z)", asFUNCTION(FIntVector_ConstructXYZ), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FIntVector, asBEHAVE_CONSTRUCT, "void f(int InValue)", asFUNCTION(FIntVector_ConstructSingle), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FIntVector, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FIntVector_Destruct), asCALL_CDECL_OBJLAST);

	// Operators
	REGISTER_METHOD(FIntVector, "FIntVector &opAssign(const FIntVector &in Other)", asFUNCTION(FIntVector_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector, "bool opEquals(const FIntVector &in Other) const", asFUNCTION(FIntVector_OpEquals), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FIntVector, "FIntVector &opAddAssign(const FIntVector &in Other)", asFUNCTION(FIntVector_AddAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector, "FIntVector &opSubAssign(const FIntVector &in Other)", asFUNCTION(FIntVector_SubAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector, "FIntVector &opMulAssign(const FIntVector &in Other)", asFUNCTION(FIntVector_MulAssignVector), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector, "FIntVector &opMulAssign(int Scale)", asFUNCTION(FIntVector_MulAssignScalar), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector, "FIntVector &opDivAssign(int Divisor)", asFUNCTION(FIntVector_DivAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector, "FIntVector &opModAssign(int Divisor)", asFUNCTION(FIntVector_ModAssign), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FIntVector, "FIntVector opAdd(const FIntVector &in Other) const", asFUNCTION(FIntVector_Add), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector, "FIntVector opSub(const FIntVector &in Other) const", asFUNCTION(FIntVector_Sub), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector, "FIntVector opMul(const FIntVector &in Other) const", asFUNCTION(FIntVector_MulVector), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector, "FIntVector opMul(int Scale) const", asFUNCTION(FIntVector_MulScalar), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector, "FIntVector opDiv(int Divisor) const", asFUNCTION(FIntVector_Div), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector, "FIntVector opMod(int Divisor) const", asFUNCTION(FIntVector_Mod), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FIntVector, "FIntVector opShr(int Shift) const", asFUNCTION(FIntVector_ShiftRight), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector, "FIntVector opShl(int Shift) const", asFUNCTION(FIntVector_ShiftLeft), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector, "FIntVector opAnd(int Other) const", asFUNCTION(FIntVector_BitAnd), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector, "FIntVector opOr(int Other) const", asFUNCTION(FIntVector_BitOr), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector, "FIntVector opXor(int Other) const", asFUNCTION(FIntVector_BitXor), asCALL_CDECL_OBJFIRST);

	// Methods
	REGISTER_METHOD(FIntVector, "bool IsZero() const", asFUNCTION(FIntVector_IsZero), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector, "int GetMax() const", asFUNCTION(FIntVector_GetMax), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector, "int GetAbsMax() const", asFUNCTION(FIntVector_GetAbsMax), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector, "int GetMin() const", asFUNCTION(FIntVector_GetMin), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector, "int GetAbsMin() const", asFUNCTION(FIntVector_GetAbsMin), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector, "int Size() const", asFUNCTION(FIntVector_Size), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector, "FIntVector ComponentMax(const FIntVector &in Other) const", asFUNCTION(FIntVector_ComponentMax), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector, "FIntVector ComponentMin(const FIntVector &in Other) const", asFUNCTION(FIntVector_ComponentMin), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector, "FString ToString() const", asFUNCTION(FIntVector_ToString), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntVector, "bool InitFromString(const FString &in InSourceString)", asFUNCTION(FIntVector_InitFromString), asCALL_CDECL_OBJFIRST);
}
