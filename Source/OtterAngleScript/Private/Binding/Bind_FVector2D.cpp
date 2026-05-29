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

	REGISTER_METHOD(FVector2D, "FVector2D &opAssign(const FVector2D &in Other)", asMETHODPR(FVector2D, operator=, (const FVector2D&), FVector2D&), asCALL_THISCALL);
	REGISTER_METHOD(FVector2D, "bool opEquals(const FVector2D &in Other) const", asMETHODPR(FVector2D, operator==, (const FVector2D&) const, bool), asCALL_THISCALL);
	REGISTER_METHOD(FVector2D, "bool Equals(const FVector2D &in Other, double Tolerance) const", asMETHODPR(FVector2D, Equals, (const FVector2D&, double) const, bool), asCALL_THISCALL);
	REGISTER_PROPERTY(FVector2D, "double X", X);
	REGISTER_PROPERTY(FVector2D, "double Y", Y);
}
