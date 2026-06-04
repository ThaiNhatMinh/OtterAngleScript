// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Internationalization/Text.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"
#include "OtterAngleScript.h"

#include <new>

namespace
{
	static void FFormatArgumentData_DefaultConstruct(FFormatArgumentData* Memory)
	{
		new (Memory) FFormatArgumentData();
	}

	static void FFormatArgumentData_CopyConstruct(const FFormatArgumentData& Other, FFormatArgumentData* Memory)
	{
		new (Memory) FFormatArgumentData(Other);
	}

	static void FFormatArgumentData_Destruct(FFormatArgumentData* Memory)
	{
		Memory->~FFormatArgumentData();
	}

	static FFormatArgumentData& FFormatArgumentData_Assign(FFormatArgumentData& Value, const FFormatArgumentData& Other)
	{
		Value = Other;
		return Value;
	}

	static void FFormatArgumentData_ResetValue(FFormatArgumentData& Value)
	{
		Value.ResetValue();
	}
}

void Bind_FFormatArgumentData(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;

	REGISTER_BEHAVIOUR(FFormatArgumentData, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FFormatArgumentData_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FFormatArgumentData, asBEHAVE_CONSTRUCT, "void f(const FFormatArgumentData &in Other)", asFUNCTION(FFormatArgumentData_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FFormatArgumentData, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FFormatArgumentData_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FFormatArgumentData, "FFormatArgumentData &opAssign(const FFormatArgumentData &in Other)", asFUNCTION(FFormatArgumentData_Assign), asCALL_CDECL_OBJFIRST);

	// Properties
	REGISTER_PROPERTY(FFormatArgumentData, "FString ArgumentName", ArgumentName);
	REGISTER_PROPERTY(FFormatArgumentData, "int ArgumentValueType", ArgumentValueType);
	REGISTER_PROPERTY(FFormatArgumentData, "FText ArgumentValue", ArgumentValue);
	REGISTER_PROPERTY(FFormatArgumentData, "int64 ArgumentValueInt", ArgumentValueInt);
	REGISTER_PROPERTY(FFormatArgumentData, "float ArgumentValueFloat", ArgumentValueFloat);
	REGISTER_PROPERTY(FFormatArgumentData, "double ArgumentValueDouble", ArgumentValueDouble);
	REGISTER_PROPERTY(FFormatArgumentData, "ETextGender ArgumentValueGender", ArgumentValueGender);

	REGISTER_METHOD(FFormatArgumentData, "void ResetValue()", asFUNCTION(FFormatArgumentData_ResetValue), asCALL_CDECL_OBJFIRST);
}
