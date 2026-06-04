// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Math/IntPoint.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"
#include "OtterAngleScript.h"

#include <new>

namespace
{
	using FIntPoint = UE::Math::TIntPoint<int32>;

	// --- Constructors / destructor ---

	static void FIntPoint_DefaultConstruct(FIntPoint* Memory)
	{
		new (Memory) FIntPoint();
	}

	static void FIntPoint_CopyConstruct(const FIntPoint& Other, FIntPoint* Memory)
	{
		new (Memory) FIntPoint(Other);
	}

	static void FIntPoint_ConstructXY(int32 X, int32 Y, FIntPoint* Memory)
	{
		new (Memory) FIntPoint(X, Y);
	}

	static void FIntPoint_ConstructSingle(int32 InValue, FIntPoint* Memory)
	{
		new (Memory) FIntPoint(InValue);
	}

	static void FIntPoint_Destruct(FIntPoint* Memory)
	{
		Memory->~FIntPoint();
	}

	// --- Assignment / comparison ---

	static FIntPoint& FIntPoint_Assign(FIntPoint& Value, const FIntPoint& Other)
	{
		Value = Other;
		return Value;
	}

	static bool FIntPoint_OpEquals(const FIntPoint& Value, const FIntPoint& Other)
	{
		return Value == Other;
	}

	// --- Arithmetic operators (compound) ---

	static FIntPoint& FIntPoint_AddAssign(FIntPoint& Value, const FIntPoint& Other)
	{
		Value += Other;
		return Value;
	}

	static FIntPoint& FIntPoint_SubAssign(FIntPoint& Value, const FIntPoint& Other)
	{
		Value -= Other;
		return Value;
	}

	static FIntPoint& FIntPoint_MulAssignPoint(FIntPoint& Value, const FIntPoint& Other)
	{
		Value *= Other;
		return Value;
	}

	static FIntPoint& FIntPoint_MulAssignScalar(FIntPoint& Value, int32 Scale)
	{
		Value *= Scale;
		return Value;
	}

	static FIntPoint& FIntPoint_DivAssignPoint(FIntPoint& Value, const FIntPoint& Other)
	{
		Value /= Other;
		return Value;
	}

	static FIntPoint& FIntPoint_DivAssignScalar(FIntPoint& Value, int32 Divisor)
	{
		Value /= Divisor;
		return Value;
	}

	// --- Arithmetic operators (binary) ---

	static FIntPoint FIntPoint_Add(const FIntPoint& Value, const FIntPoint& Other)
	{
		return Value + Other;
	}

	static FIntPoint FIntPoint_Sub(const FIntPoint& Value, const FIntPoint& Other)
	{
		return Value - Other;
	}

	static FIntPoint FIntPoint_MulPoint(const FIntPoint& Value, const FIntPoint& Other)
	{
		return Value * Other;
	}

	static FIntPoint FIntPoint_MulScalar(const FIntPoint& Value, int32 Scale)
	{
		return Value * Scale;
	}

	static FIntPoint FIntPoint_DivPoint(const FIntPoint& Value, const FIntPoint& Other)
	{
		return Value / Other;
	}

	static FIntPoint FIntPoint_DivScalar(const FIntPoint& Value, int32 Divisor)
	{
		return Value / Divisor;
	}

	// --- Index operator ---

	static int32 FIntPoint_GetIndex(const FIntPoint& Value, int32 Index)
	{
		return Value[Index];
	}

	static void FIntPoint_SetIndex(FIntPoint& Value, int32 Index, int32 InValue)
	{
		Value[Index] = InValue;
	}

	// --- Methods ---

	static FIntPoint FIntPoint_ComponentMin(const FIntPoint& Value, const FIntPoint& Other)
	{
		return Value.ComponentMin(Other);
	}

	static FIntPoint FIntPoint_ComponentMax(const FIntPoint& Value, const FIntPoint& Other)
	{
		return Value.ComponentMax(Other);
	}

	static int32 FIntPoint_GetMax(const FIntPoint& Value)
	{
		return Value.GetMax();
	}

	static int32 FIntPoint_GetMin(const FIntPoint& Value)
	{
		return Value.GetMin();
	}

	static int32 FIntPoint_Size(const FIntPoint& Value)
	{
		return Value.Size();
	}

	static int32 FIntPoint_SizeSquared(const FIntPoint& Value)
	{
		return Value.SizeSquared();
	}

	static FString FIntPoint_ToString(const FIntPoint& Value)
	{
		return Value.ToString();
	}

	static bool FIntPoint_InitFromString(FIntPoint& Value, const FString& InSourceString)
	{
		return Value.InitFromString(InSourceString);
	}

	// --- Static methods ---

	static int32 FIntPoint_Num()
	{
		return FIntPoint::Num();
	}

	static FIntPoint FIntPoint_DivideAndRoundUpScalar(FIntPoint Lhs, int32 Divisor)
	{
		return FIntPoint::DivideAndRoundUp(Lhs, Divisor);
	}

	static FIntPoint FIntPoint_DivideAndRoundUpPoint(FIntPoint Lhs, FIntPoint Divisor)
	{
		return FIntPoint::DivideAndRoundUp(Lhs, Divisor);
	}

	static FIntPoint FIntPoint_DivideAndRoundDownScalar(FIntPoint Lhs, int32 Divisor)
	{
		return FIntPoint::DivideAndRoundDown(Lhs, Divisor);
	}

	static FIntPoint FIntPoint_DivideAndRoundDownPoint(FIntPoint Lhs, FIntPoint Divisor)
	{
		return FIntPoint::DivideAndRoundDown(Lhs, Divisor);
	}
}

void Bind_FIntPoint(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;

	// Properties
	REGISTER_PROPERTY(FIntPoint, "int X", X);
	REGISTER_PROPERTY(FIntPoint, "int Y", Y);

	// Behaviours (constructors / destructor)
	REGISTER_BEHAVIOUR(FIntPoint, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FIntPoint_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FIntPoint, asBEHAVE_CONSTRUCT, "void f(const FIntPoint &in Other)", asFUNCTION(FIntPoint_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FIntPoint, asBEHAVE_CONSTRUCT, "void f(int X, int Y)", asFUNCTION(FIntPoint_ConstructXY), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FIntPoint, asBEHAVE_CONSTRUCT, "void f(int InValue)", asFUNCTION(FIntPoint_ConstructSingle), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FIntPoint, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FIntPoint_Destruct), asCALL_CDECL_OBJLAST);

	// Operators
	REGISTER_METHOD(FIntPoint, "FIntPoint &opAssign(const FIntPoint &in Other)", asFUNCTION(FIntPoint_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntPoint, "bool opEquals(const FIntPoint &in Other) const", asFUNCTION(FIntPoint_OpEquals), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FIntPoint, "FIntPoint &opAddAssign(const FIntPoint &in Other)", asFUNCTION(FIntPoint_AddAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntPoint, "FIntPoint &opSubAssign(const FIntPoint &in Other)", asFUNCTION(FIntPoint_SubAssign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntPoint, "FIntPoint &opMulAssign(const FIntPoint &in Other)", asFUNCTION(FIntPoint_MulAssignPoint), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntPoint, "FIntPoint &opMulAssign(int Scale)", asFUNCTION(FIntPoint_MulAssignScalar), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntPoint, "FIntPoint &opDivAssign(const FIntPoint &in Other)", asFUNCTION(FIntPoint_DivAssignPoint), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntPoint, "FIntPoint &opDivAssign(int Divisor)", asFUNCTION(FIntPoint_DivAssignScalar), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FIntPoint, "FIntPoint opAdd(const FIntPoint &in Other) const", asFUNCTION(FIntPoint_Add), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntPoint, "FIntPoint opSub(const FIntPoint &in Other) const", asFUNCTION(FIntPoint_Sub), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntPoint, "FIntPoint opMul(const FIntPoint &in Other) const", asFUNCTION(FIntPoint_MulPoint), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntPoint, "FIntPoint opMul(int Scale) const", asFUNCTION(FIntPoint_MulScalar), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntPoint, "FIntPoint opDiv(const FIntPoint &in Other) const", asFUNCTION(FIntPoint_DivPoint), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntPoint, "FIntPoint opDiv(int Divisor) const", asFUNCTION(FIntPoint_DivScalar), asCALL_CDECL_OBJFIRST);

	// Index operator
	REGISTER_METHOD(FIntPoint, "int opIndex(int Index) const", asFUNCTION(FIntPoint_GetIndex), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntPoint, "void opIndexAssign(int Index, int Value)", asFUNCTION(FIntPoint_SetIndex), asCALL_CDECL_OBJFIRST);

	// Methods
	REGISTER_METHOD(FIntPoint, "FIntPoint ComponentMin(const FIntPoint &in Other) const", asFUNCTION(FIntPoint_ComponentMin), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntPoint, "FIntPoint ComponentMax(const FIntPoint &in Other) const", asFUNCTION(FIntPoint_ComponentMax), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntPoint, "int GetMax() const", asFUNCTION(FIntPoint_GetMax), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntPoint, "int GetMin() const", asFUNCTION(FIntPoint_GetMin), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntPoint, "int Size() const", asFUNCTION(FIntPoint_Size), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntPoint, "int SizeSquared() const", asFUNCTION(FIntPoint_SizeSquared), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntPoint, "FString ToString() const", asFUNCTION(FIntPoint_ToString), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FIntPoint, "bool InitFromString(const FString &in InSourceString)", asFUNCTION(FIntPoint_InitFromString), asCALL_CDECL_OBJFIRST);

	// Static methods
	Result = Engine->SetDefaultNamespace("FIntPoint");
	check(Result >= 0);

	Result = Engine->RegisterGlobalFunction("int Num()", asFUNCTION(FIntPoint_Num), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FIntPoint DivideAndRoundUp(FIntPoint Lhs, int Divisor)", asFUNCTION(FIntPoint_DivideAndRoundUpScalar), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FIntPoint DivideAndRoundUp(FIntPoint Lhs, FIntPoint Divisor)", asFUNCTION(FIntPoint_DivideAndRoundUpPoint), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FIntPoint DivideAndRoundDown(FIntPoint Lhs, int Divisor)", asFUNCTION(FIntPoint_DivideAndRoundDownScalar), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FIntPoint DivideAndRoundDown(FIntPoint Lhs, FIntPoint Divisor)", asFUNCTION(FIntPoint_DivideAndRoundDownPoint), asCALL_CDECL);
	check(Result >= 0);

	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
