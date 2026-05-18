// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Math/Vector2D.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"

#include <new>

namespace
{
	void FVector2D_DefaultConstruct(FVector2D* Memory)
	{
		new (Memory) FVector2D();
	}

	void FVector2D_CopyConstruct(const FVector2D& Other, FVector2D* Memory)
	{
		new (Memory) FVector2D(Other);
	}

	void FVector2D_ConstructXY(double X, double Y, FVector2D* Memory)
	{
		new (Memory) FVector2D(X, Y);
	}

	void FVector2D_Destruct(FVector2D* Memory)
	{
		Memory->~FVector2D();
	}

	FVector2D& FVector2D_Assign(FVector2D& Value, const FVector2D& Other)
	{
		Value = Other;
		return Value;
	}

	bool FVector2D_Equals(const FVector2D& Value, const FVector2D& Other)
	{
		return Value == Other;
	}

	bool FVector2D_EqualsTolerance(const FVector2D& Value, const FVector2D& Other, double Tolerance)
	{
		return Value.Equals(Other, Tolerance);
	}

	double FVector2D_GetX(const FVector2D& Value)
	{
		return Value.X;
	}

	double FVector2D_GetY(const FVector2D& Value)
	{
		return Value.Y;
	}

	void FVector2D_SetX(FVector2D& Value, double X)
	{
		Value.X = X;
	}

	void FVector2D_SetY(FVector2D& Value, double Y)
	{
		Value.Y = Y;
	}
}

void Bind_FVector2D(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = Engine->RegisterObjectType(
		"FVector2D",
		sizeof(FVector2D),
		asOBJ_VALUE | asGetTypeTraits<FVector2D>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	REGISTER_BEHAVIOUR(FVector2D, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FVector2D_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector2D, asBEHAVE_CONSTRUCT, "void f(const FVector2D &in Other)", asFUNCTION(FVector2D_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector2D, asBEHAVE_CONSTRUCT, "void f(double X, double Y)", asFUNCTION(FVector2D_ConstructXY), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FVector2D, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FVector2D_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FVector2D, "FVector2D &opAssign(const FVector2D &in Other)", asFUNCTION(FVector2D_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "bool opEquals(const FVector2D &in Other) const", asFUNCTION(FVector2D_Equals), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "bool Equals(const FVector2D &in Other, double Tolerance) const", asFUNCTION(FVector2D_EqualsTolerance), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "double get_X() const property", asFUNCTION(FVector2D_GetX), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "double get_Y() const property", asFUNCTION(FVector2D_GetY), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "void set_X(double Value) property", asFUNCTION(FVector2D_SetX), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FVector2D, "void set_Y(double Value) property", asFUNCTION(FVector2D_SetY), asCALL_CDECL_OBJFIRST);
}
