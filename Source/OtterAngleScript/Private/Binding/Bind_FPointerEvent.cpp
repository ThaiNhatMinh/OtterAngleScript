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

	static void FPointerEvent_DefaultConstruct(FPointerEvent* Memory)
	{
		new (Memory) FPointerEvent();
	}

	static void FPointerEvent_CopyConstruct(const FPointerEvent& Other, FPointerEvent* Memory)
	{
		new (Memory) FPointerEvent(Other);
	}

	static void FPointerEvent_Destruct(FPointerEvent* Memory)
	{
		Memory->~FPointerEvent();
	}

	// --- Assignment ---

	static FPointerEvent& FPointerEvent_Assign(FPointerEvent& Value, const FPointerEvent& Other)
	{
		Value = Other;
		return Value;
	}

	// --- Methods ---

	static FVector2D FPointerEvent_GetScreenSpacePosition(const FPointerEvent& Value)
	{
		return FVector2D(Value.GetScreenSpacePosition());
	}

	static FVector2D FPointerEvent_GetLastScreenSpacePosition(const FPointerEvent& Value)
	{
		return FVector2D(Value.GetLastScreenSpacePosition());
	}

	static FVector2D FPointerEvent_GetCursorDelta(const FPointerEvent& Value)
	{
		return FVector2D(Value.GetCursorDelta());
	}

	static bool FPointerEvent_IsMouseButtonDown(const FPointerEvent& Value, FKey MouseButton)
	{
		return Value.IsMouseButtonDown(MouseButton);
	}

	static FKey FPointerEvent_GetEffectingButton(const FPointerEvent& Value)
	{
		return Value.GetEffectingButton();
	}

	static float FPointerEvent_GetWheelDelta(const FPointerEvent& Value)
	{
		return Value.GetWheelDelta();
	}

	static uint32 FPointerEvent_GetPointerIndex(const FPointerEvent& Value)
	{
		return Value.GetPointerIndex();
	}

	static uint32 FPointerEvent_GetTouchpadIndex(const FPointerEvent& Value)
	{
		return Value.GetTouchpadIndex();
	}

	static float FPointerEvent_GetTouchForce(const FPointerEvent& Value)
	{
		return Value.GetTouchForce();
	}

	static bool FPointerEvent_IsTouchEvent(const FPointerEvent& Value)
	{
		return Value.IsTouchEvent();
	}

	static bool FPointerEvent_IsTouchForceChangedEvent(const FPointerEvent& Value)
	{
		return Value.IsTouchForceChangedEvent();
	}

	static bool FPointerEvent_IsTouchFirstMoveEvent(const FPointerEvent& Value)
	{
		return Value.IsTouchFirstMoveEvent();
	}

	static int FPointerEvent_GetGestureType(const FPointerEvent& Value)
	{
		return static_cast<int>(Value.GetGestureType());
	}

	static FVector2D FPointerEvent_GetGestureDelta(const FPointerEvent& Value)
	{
		return FVector2D(Value.GetGestureDelta());
	}

	static bool FPointerEvent_IsDirectionInvertedFromDevice(const FPointerEvent& Value)
	{
		return Value.IsDirectionInvertedFromDevice();
	}
}

void Bind_FPointerEvent(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result;

	REGISTER_BEHAVIOUR(FPointerEvent, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(FPointerEvent_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FPointerEvent, asBEHAVE_CONSTRUCT, "void f(const FPointerEvent &in Other)", asFUNCTION(FPointerEvent_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FPointerEvent, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(FPointerEvent_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FPointerEvent, "FPointerEvent &opAssign(const FPointerEvent &in Other)", asFUNCTION(FPointerEvent_Assign), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FPointerEvent, "FVector2D GetScreenSpacePosition() const", asFUNCTION(FPointerEvent_GetScreenSpacePosition), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FPointerEvent, "FVector2D GetLastScreenSpacePosition() const", asFUNCTION(FPointerEvent_GetLastScreenSpacePosition), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FPointerEvent, "FVector2D GetCursorDelta() const", asFUNCTION(FPointerEvent_GetCursorDelta), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FPointerEvent, "bool IsMouseButtonDown(FKey MouseButton) const", asFUNCTION(FPointerEvent_IsMouseButtonDown), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FPointerEvent, "FKey GetEffectingButton() const", asFUNCTION(FPointerEvent_GetEffectingButton), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FPointerEvent, "float GetWheelDelta() const", asFUNCTION(FPointerEvent_GetWheelDelta), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FPointerEvent, "uint GetPointerIndex() const", asFUNCTION(FPointerEvent_GetPointerIndex), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FPointerEvent, "uint GetTouchpadIndex() const", asFUNCTION(FPointerEvent_GetTouchpadIndex), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FPointerEvent, "float GetTouchForce() const", asFUNCTION(FPointerEvent_GetTouchForce), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FPointerEvent, "bool IsTouchEvent() const", asFUNCTION(FPointerEvent_IsTouchEvent), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FPointerEvent, "bool IsTouchForceChangedEvent() const", asFUNCTION(FPointerEvent_IsTouchForceChangedEvent), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FPointerEvent, "bool IsTouchFirstMoveEvent() const", asFUNCTION(FPointerEvent_IsTouchFirstMoveEvent), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FPointerEvent, "int GetGestureType() const", asFUNCTION(FPointerEvent_GetGestureType), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FPointerEvent, "FVector2D GetGestureDelta() const", asFUNCTION(FPointerEvent_GetGestureDelta), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FPointerEvent, "bool IsDirectionInvertedFromDevice() const", asFUNCTION(FPointerEvent_IsDirectionInvertedFromDevice), asCALL_CDECL_OBJFIRST);
}
