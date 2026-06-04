// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Input/Events.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"
#include "OtterAngleScript.h"

#include <new>

namespace
{
	// --- Constructors / destructor ---

	static void FKeyEvent_DefaultConstruct(FKeyEvent* Memory)
	{
		new (Memory) FKeyEvent();
	}

	static void FKeyEvent_CopyConstruct(const FKeyEvent& Other, FKeyEvent* Memory)
	{
		new (Memory) FKeyEvent(Other);
	}

	static void FKeyEvent_Destruct(FKeyEvent* Memory)
	{
		Memory->~FKeyEvent();
	}

	// --- Assignment ---

	static FKeyEvent& FKeyEvent_Assign(FKeyEvent& Value, const FKeyEvent& Other)
	{
		Value = Other;
		return Value;
	}

	// --- Methods ---

	static FKey FKeyEvent_GetKey(const FKeyEvent& Value)
	{
		return Value.GetKey();
	}

	static uint32 FKeyEvent_GetCharacter(const FKeyEvent& Value)
	{
		return Value.GetCharacter();
	}

	static uint32 FKeyEvent_GetKeyCode(const FKeyEvent& Value)
	{
		return Value.GetKeyCode();
	}

	static FText FKeyEvent_ToText(const FKeyEvent& Value)
	{
		return Value.ToText();
	}
}

void Bind_FKeyEvent(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;

	REGISTER_BEHAVIOUR(FKeyEvent, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FKeyEvent_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FKeyEvent, asBEHAVE_CONSTRUCT, "void f(const FKeyEvent &in Other)", asFUNCTION(FKeyEvent_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FKeyEvent, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FKeyEvent_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FKeyEvent, "FKeyEvent &opAssign(const FKeyEvent &in Other)", asFUNCTION(FKeyEvent_Assign), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FKeyEvent, "FKey GetKey() const", asFUNCTION(FKeyEvent_GetKey), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FKeyEvent, "uint GetCharacter() const", asFUNCTION(FKeyEvent_GetCharacter), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FKeyEvent, "uint GetKeyCode() const", asFUNCTION(FKeyEvent_GetKeyCode), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FKeyEvent, "FText ToText() const", asFUNCTION(FKeyEvent_ToText), asCALL_CDECL_OBJFIRST);
}
