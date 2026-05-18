// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Internationalization/Text.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"

#include <new>

namespace
{
	void FText_DefaultConstruct(FText* Memory)
	{
		new (Memory) FText();
	}

	void FText_CopyConstruct(const FText& Other, FText* Memory)
	{
		new (Memory) FText(Other);
	}

	void FText_Destruct(FText* Memory)
	{
		Memory->~FText();
	}

	FText& FText_Assign(FText& Value, const FText& Other)
	{
		Value = Other;
		return Value;
	}

	FString FText_ToString(const FText& Value)
	{
		return Value.ToString();
	}
}

void Bind_FText(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = Engine->RegisterObjectType(
		"FText",
		sizeof(FText),
		asOBJ_VALUE | asGetTypeTraits<FText>() | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);
	check(Result >= 0);

	REGISTER_BEHAVIOUR(FText, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FText_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FText, asBEHAVE_CONSTRUCT, "void f(const FText &in Other)", asFUNCTION(FText_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FText, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FText_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FText, "FText &opAssign(const FText &in Other)", asFUNCTION(FText_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FText, "FString ToString() const", asFUNCTION(FText_ToString), asCALL_CDECL_OBJFIRST);
}
