// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Math/Vector.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"

#include <new>

namespace
{
	static void FVector_DefaultConstruct(FVector* Memory)
	{
		new (Memory) FVector();
	}

	static void FVector_CopyConstruct(const FVector& Other, FVector* Memory)
	{
		new (Memory) FVector(Other);
	}

	static void FVector_ConstructXYZ(double X, double Y, double Z, FVector* Memory)
	{
		new (Memory) FVector(X, Y, Z);
	}

	static void FVector_Destruct(FVector* Memory)
	{
		Memory->~FVector();
	}

	static FVector& FVector_Assign(FVector& Value, const FVector& Other)
	{
		Value = Other;
		return Value;
	}

	static bool FVector_Equals(const FVector& Value, const FVector& Other)
	{
		return Value == Other;
	}

	static double FVector_GetX(const FVector& Value)
	{
		return Value.X;
	}

	static double FVector_GetY(const FVector& Value)
	{
		return Value.Y;
	}

	static double FVector_GetZ(const FVector& Value)
	{
		return Value.Z;
	}

	static void FVector_SetX(FVector& Value, double X)
	{
		Value.X = X;
	}

	static void FVector_SetY(FVector& Value, double Y)
	{
		Value.Y = Y;
	}

	static void FVector_SetZ(FVector& Value, double Z)
	{
		Value.Z = Z;
	}
}

#define REGISTER_FVECTOR_BEHAVIOUR(Behaviour, Declaration, Function, CallConv) \
	Result = Engine->RegisterObjectBehaviour("FVector", Behaviour, Declaration, Function, CallConv); \
	check(Result >= 0)

#define REGISTER_FVECTOR_METHOD(Declaration, Function, CallConv) \
	Result = Engine->RegisterObjectMethod("FVector", Declaration, Function, CallConv); \
	check(Result >= 0)

void Bind_FVector(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = Engine->RegisterObjectType(
		"FVector",
		sizeof(FVector),
		asOBJ_VALUE | asGetTypeTraits<FVector>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	REGISTER_BEHAVIOUR(FVector, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FVector_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector, asBEHAVE_CONSTRUCT, "void f(const FVector &in Other)", asFUNCTION(FVector_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector, asBEHAVE_CONSTRUCT, "void f(double X, double Y, double Z)", asFUNCTION(FVector_ConstructXYZ), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FVector_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FVector, "FVector &opAssign(const FVector &in Other)", asFUNCTION(FVector_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "bool opEquals(const FVector &in Other) const", asFUNCTION(FVector_Equals), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "double get_X() const property", asFUNCTION(FVector_GetX), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "double get_Y() const property", asFUNCTION(FVector_GetY), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "double get_Z() const property", asFUNCTION(FVector_GetZ), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "void set_X(double Value) property", asFUNCTION(FVector_SetX), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "void set_Y(double Value) property", asFUNCTION(FVector_SetY), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector, "void set_Z(double Value) property", asFUNCTION(FVector_SetZ), asCALL_CDECL_OBJFIRST);
}

#undef REGISTER_FVECTOR_METHOD
#undef REGISTER_FVECTOR_BEHAVIOUR
