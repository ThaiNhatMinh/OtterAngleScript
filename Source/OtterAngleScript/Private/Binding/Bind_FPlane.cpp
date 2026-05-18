// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Math/Plane.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"

#include <new>

namespace
{
	void FPlane_DefaultConstruct(FPlane* Memory)
	{
		new (Memory) FPlane();
	}

	void FPlane_CopyConstruct(const FPlane& Other, FPlane* Memory)
	{
		new (Memory) FPlane(Other);
	}

	void FPlane_ConstructXYZW(double X, double Y, double Z, double W, FPlane* Memory)
	{
		new (Memory) FPlane(X, Y, Z, W);
	}

	void FPlane_Destruct(FPlane* Memory)
	{
		Memory->~FPlane();
	}

	FPlane& FPlane_Assign(FPlane& Value, const FPlane& Other)
	{
		Value = Other;
		return Value;
	}

	bool FPlane_Equals(const FPlane& Value, const FPlane& Other)
	{
		return Value == Other;
	}
}

void Bind_FPlane(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = Engine->RegisterObjectType(
		"FPlane",
		sizeof(FPlane),
		asOBJ_VALUE | asGetTypeTraits<FPlane>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	REGISTER_BEHAVIOUR(FPlane, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FPlane_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FPlane, asBEHAVE_CONSTRUCT, "void f(const FPlane &in Other)", asFUNCTION(FPlane_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FPlane, asBEHAVE_CONSTRUCT, "void f(double X, double Y, double Z, double W)", asFUNCTION(FPlane_ConstructXYZW), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FPlane, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FPlane_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FPlane, "FPlane &opAssign(const FPlane &in Other)", asFUNCTION(FPlane_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FPlane, "bool opEquals(const FPlane &in Other) const", asFUNCTION(FPlane_Equals), asCALL_CDECL_OBJFIRST);
	REGISTER_PROPERTY(FPlane, "double X", X);
	REGISTER_PROPERTY(FPlane, "double Y", Y);
	REGISTER_PROPERTY(FPlane, "double Z", Z);
	REGISTER_PROPERTY(FPlane, "double W", W);
}
