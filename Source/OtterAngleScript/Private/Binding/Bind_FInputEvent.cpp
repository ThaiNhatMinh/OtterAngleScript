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

	static void FInputEvent_DefaultConstruct(FInputEvent* Memory)
	{
		new (Memory) FInputEvent();
	}

	static void FInputEvent_CopyConstruct(const FInputEvent& Other, FInputEvent* Memory)
	{
		new (Memory) FInputEvent(Other);
	}

	static void FInputEvent_Destruct(FInputEvent* Memory)
	{
		Memory->~FInputEvent();
	}

	// --- Assignment ---

	static FInputEvent& FInputEvent_Assign(FInputEvent& Value, const FInputEvent& Other)
	{
		Value = Other;
		return Value;
	}

	// --- Methods ---

	static bool FInputEvent_IsRepeat(const FInputEvent& Value)
	{
		return Value.IsRepeat();
	}

	static bool FInputEvent_IsShiftDown(const FInputEvent& Value)
	{
		return Value.IsShiftDown();
	}

	static bool FInputEvent_IsLeftShiftDown(const FInputEvent& Value)
	{
		return Value.IsLeftShiftDown();
	}

	static bool FInputEvent_IsRightShiftDown(const FInputEvent& Value)
	{
		return Value.IsRightShiftDown();
	}

	static bool FInputEvent_IsControlDown(const FInputEvent& Value)
	{
		return Value.IsControlDown();
	}

	static bool FInputEvent_IsLeftControlDown(const FInputEvent& Value)
	{
		return Value.IsLeftControlDown();
	}

	static bool FInputEvent_IsRightControlDown(const FInputEvent& Value)
	{
		return Value.IsRightControlDown();
	}

	static bool FInputEvent_IsAltDown(const FInputEvent& Value)
	{
		return Value.IsAltDown();
	}

	static bool FInputEvent_IsLeftAltDown(const FInputEvent& Value)
	{
		return Value.IsLeftAltDown();
	}

	static bool FInputEvent_IsRightAltDown(const FInputEvent& Value)
	{
		return Value.IsRightAltDown();
	}

	static bool FInputEvent_IsCommandDown(const FInputEvent& Value)
	{
		return Value.IsCommandDown();
	}

	static bool FInputEvent_IsLeftCommandDown(const FInputEvent& Value)
	{
		return Value.IsLeftCommandDown();
	}

	static bool FInputEvent_IsRightCommandDown(const FInputEvent& Value)
	{
		return Value.IsRightCommandDown();
	}

	static bool FInputEvent_AreCapsLocked(const FInputEvent& Value)
	{
		return Value.AreCapsLocked();
	}

	static uint32 FInputEvent_GetUserIndex(const FInputEvent& Value)
	{
		return Value.GetUserIndex();
	}

	static FInputDeviceId FInputEvent_GetInputDeviceId(const FInputEvent& Value)
	{
		return Value.GetInputDeviceId();
	}

	static FPlatformUserId FInputEvent_GetPlatformUserId(const FInputEvent& Value)
	{
		return Value.GetPlatformUserId();
	}

	static uint64 FInputEvent_GetEventTimestamp(const FInputEvent& Value)
	{
		return Value.GetEventTimestamp();
	}

	static double FInputEvent_GetMillisecondsSinceEvent(const FInputEvent& Value)
	{
		return Value.GetMillisecondsSinceEvent();
	}

	static FText FInputEvent_ToText(const FInputEvent& Value)
	{
		return Value.ToText();
	}

	static bool FInputEvent_IsPointerEvent(const FInputEvent& Value)
	{
		return Value.IsPointerEvent();
	}

	static bool FInputEvent_IsKeyEvent(const FInputEvent& Value)
	{
		return Value.IsKeyEvent();
	}
}

void Bind_FInputEvent(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;

	REGISTER_BEHAVIOUR(FInputEvent, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FInputEvent_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FInputEvent, asBEHAVE_CONSTRUCT, "void f(const FInputEvent &in Other)", asFUNCTION(FInputEvent_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FInputEvent, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FInputEvent_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FInputEvent, "FInputEvent &opAssign(const FInputEvent &in Other)", asFUNCTION(FInputEvent_Assign), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FInputEvent, "bool IsRepeat() const", asFUNCTION(FInputEvent_IsRepeat), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FInputEvent, "bool IsShiftDown() const", asFUNCTION(FInputEvent_IsShiftDown), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputEvent, "bool IsLeftShiftDown() const", asFUNCTION(FInputEvent_IsLeftShiftDown), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputEvent, "bool IsRightShiftDown() const", asFUNCTION(FInputEvent_IsRightShiftDown), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FInputEvent, "bool IsControlDown() const", asFUNCTION(FInputEvent_IsControlDown), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputEvent, "bool IsLeftControlDown() const", asFUNCTION(FInputEvent_IsLeftControlDown), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputEvent, "bool IsRightControlDown() const", asFUNCTION(FInputEvent_IsRightControlDown), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FInputEvent, "bool IsAltDown() const", asFUNCTION(FInputEvent_IsAltDown), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputEvent, "bool IsLeftAltDown() const", asFUNCTION(FInputEvent_IsLeftAltDown), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputEvent, "bool IsRightAltDown() const", asFUNCTION(FInputEvent_IsRightAltDown), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FInputEvent, "bool IsCommandDown() const", asFUNCTION(FInputEvent_IsCommandDown), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputEvent, "bool IsLeftCommandDown() const", asFUNCTION(FInputEvent_IsLeftCommandDown), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputEvent, "bool IsRightCommandDown() const", asFUNCTION(FInputEvent_IsRightCommandDown), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FInputEvent, "bool AreCapsLocked() const", asFUNCTION(FInputEvent_AreCapsLocked), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FInputEvent, "uint GetUserIndex() const", asFUNCTION(FInputEvent_GetUserIndex), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputEvent, "FInputDeviceId GetInputDeviceId() const", asFUNCTION(FInputEvent_GetInputDeviceId), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputEvent, "FPlatformUserId GetPlatformUserId() const", asFUNCTION(FInputEvent_GetPlatformUserId), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputEvent, "uint64 GetEventTimestamp() const", asFUNCTION(FInputEvent_GetEventTimestamp), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputEvent, "double GetMillisecondsSinceEvent() const", asFUNCTION(FInputEvent_GetMillisecondsSinceEvent), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FInputEvent, "FText ToText() const", asFUNCTION(FInputEvent_ToText), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputEvent, "bool IsPointerEvent() const", asFUNCTION(FInputEvent_IsPointerEvent), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FInputEvent, "bool IsKeyEvent() const", asFUNCTION(FInputEvent_IsKeyEvent), asCALL_CDECL_OBJFIRST);
}
