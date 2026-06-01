// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Curves/RichCurve.h"
#include "Misc/FrameRate.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"

#include <new>

namespace
{
	// =========================================================================
	// FKeyHandle wrappers
	// =========================================================================

	static void FKeyHandle_DefaultConstruct(FKeyHandle* Memory)
	{
		new (Memory) FKeyHandle();
	}

	static void FKeyHandle_CopyConstruct(const FKeyHandle& Other, FKeyHandle* Memory)
	{
		new (Memory) FKeyHandle(Other);
	}

	static void FKeyHandle_Destruct(FKeyHandle* Memory)
	{
		Memory->~FKeyHandle();
	}

	static FKeyHandle& FKeyHandle_Assign(FKeyHandle& Value, const FKeyHandle& Other)
	{
		Value = Other;
		return Value;
	}

	static bool FKeyHandle_Equals(const FKeyHandle& Value, const FKeyHandle& Other)
	{
		return Value == Other;
	}

	static bool FKeyHandle_IsValid(const FKeyHandle& Value)
	{
		return Value.IsValid();
	}

	static FKeyHandle FKeyHandle_Invalid()
	{
		return FKeyHandle::Invalid();
	}

	// =========================================================================
	// FRichCurveKey wrappers
	// =========================================================================

	static void FRichCurveKey_DefaultConstruct(FRichCurveKey* Memory)
	{
		new (Memory) FRichCurveKey();
	}

	static void FRichCurveKey_CopyConstruct(const FRichCurveKey& Other, FRichCurveKey* Memory)
	{
		new (Memory) FRichCurveKey(Other);
	}

	static void FRichCurveKey_ConstructTimeValue(float InTime, float InValue, FRichCurveKey* Memory)
	{
		new (Memory) FRichCurveKey(InTime, InValue);
	}

	static void FRichCurveKey_ConstructFull(float InTime, float InValue, float InArriveTangent, float InLeaveTangent, ERichCurveInterpMode InInterpMode, FRichCurveKey* Memory)
	{
		new (Memory) FRichCurveKey(InTime, InValue, InArriveTangent, InLeaveTangent, InInterpMode);
	}

	static void FRichCurveKey_Destruct(FRichCurveKey* Memory)
	{
		Memory->~FRichCurveKey();
	}

	static FRichCurveKey& FRichCurveKey_Assign(FRichCurveKey& Value, const FRichCurveKey& Other)
	{
		Value = Other;
		return Value;
	}

	static bool FRichCurveKey_Equals(const FRichCurveKey& Value, const FRichCurveKey& Other)
	{
		return Value == Other;
	}

	// TEnumAsByte<ERichCurveInterpMode> is uint8; expose as ERichCurveInterpMode (int32-backed enum in AS).
	static ERichCurveInterpMode FRichCurveKey_GetInterpMode(const FRichCurveKey& Key)
	{
		return Key.InterpMode.GetValue();
	}

	static void FRichCurveKey_SetInterpMode(FRichCurveKey& Key, ERichCurveInterpMode Mode)
	{
		Key.InterpMode = Mode;
	}

	static ERichCurveTangentMode FRichCurveKey_GetTangentMode(const FRichCurveKey& Key)
	{
		return Key.TangentMode.GetValue();
	}

	static void FRichCurveKey_SetTangentMode(FRichCurveKey& Key, ERichCurveTangentMode Mode)
	{
		Key.TangentMode = Mode;
	}

	static ERichCurveTangentWeightMode FRichCurveKey_GetTangentWeightMode(const FRichCurveKey& Key)
	{
		return Key.TangentWeightMode.GetValue();
	}

	static void FRichCurveKey_SetTangentWeightMode(FRichCurveKey& Key, ERichCurveTangentWeightMode Mode)
	{
		Key.TangentWeightMode = Mode;
	}

	// =========================================================================
	// FRichCurve wrappers
	// =========================================================================

	static void FRichCurve_DefaultConstruct(FRichCurve* Memory)
	{
		new (Memory) FRichCurve();
	}

	static void FRichCurve_CopyConstruct(const FRichCurve& Other, FRichCurve* Memory)
	{
		new (Memory) FRichCurve(Other);
	}

	static void FRichCurve_Destruct(FRichCurve* Memory)
	{
		Memory->~FRichCurve();
	}

	static FRichCurve& FRichCurve_Assign(FRichCurve& Value, const FRichCurve& Other)
	{
		Value = Other;
		return Value;
	}

	static bool FRichCurve_Equals(const FRichCurve& Value, const FRichCurve& Other)
	{
		return Value == Other;
	}

	// --- Key management ---

	static FKeyHandle FRichCurve_AddKey(FRichCurve& Curve, float InTime, float InValue)
	{
		return Curve.AddKey(InTime, InValue);
	}

	static FKeyHandle FRichCurve_AddKeyUnwind(FRichCurve& Curve, float InTime, float InValue, bool bUnwindRotation)
	{
		return Curve.AddKey(InTime, InValue, bUnwindRotation);
	}

	static FKeyHandle FRichCurve_AddKeyFull(FRichCurve& Curve, float InTime, float InValue, bool bUnwindRotation, FKeyHandle KeyHandle)
	{
		return Curve.AddKey(InTime, InValue, bUnwindRotation, KeyHandle);
	}

	static void FRichCurve_DeleteKey(FRichCurve& Curve, FKeyHandle KeyHandle)
	{
		Curve.DeleteKey(KeyHandle);
	}

	static void FRichCurve_ReserveKeys(FRichCurve& Curve, int32 Number)
	{
		Curve.ReserveKeys(Number);
	}

	static FKeyHandle FRichCurve_UpdateOrAddKey(FRichCurve& Curve, float InTime, float InValue)
	{
		return Curve.UpdateOrAddKey(InTime, InValue);
	}

	static FKeyHandle FRichCurve_UpdateOrAddKeyFull(FRichCurve& Curve, float InTime, float InValue, bool bUnwindRotation, float KeyTimeTolerance)
	{
		return Curve.UpdateOrAddKey(InTime, InValue, bUnwindRotation, KeyTimeTolerance);
	}

	static void FRichCurve_SetKeyTime(FRichCurve& Curve, FKeyHandle KeyHandle, float NewTime)
	{
		Curve.SetKeyTime(KeyHandle, NewTime);
	}

	static float FRichCurve_GetKeyTime(const FRichCurve& Curve, FKeyHandle KeyHandle)
	{
		return Curve.GetKeyTime(KeyHandle);
	}

	static void FRichCurve_SetKeyValue(FRichCurve& Curve, FKeyHandle KeyHandle, float NewValue, bool bAutoSetTangents)
	{
		Curve.SetKeyValue(KeyHandle, NewValue, bAutoSetTangents);
	}

	static float FRichCurve_GetKeyValue(const FRichCurve& Curve, FKeyHandle KeyHandle)
	{
		return Curve.GetKeyValue(KeyHandle);
	}

	// --- Key info ---

	static FRichCurveKey FRichCurve_GetFirstKey(const FRichCurve& Curve)
	{
		return Curve.GetFirstKey();
	}

	static FRichCurveKey FRichCurve_GetLastKey(const FRichCurve& Curve)
	{
		return Curve.GetLastKey();
	}

	static FRichCurveKey FRichCurve_GetKey(const FRichCurve& Curve, FKeyHandle KeyHandle)
	{
		return Curve.GetKey(KeyHandle);
	}

	static int32 FRichCurve_GetNumKeys(const FRichCurve& Curve)
	{
		return Curve.GetNumKeys();
	}

	static TArray<FRichCurveKey> FRichCurve_GetCopyOfKeys(const FRichCurve& Curve)
	{
		return Curve.GetCopyOfKeys();
	}

	static void FRichCurve_SetKeys(FRichCurve& Curve, const TArray<FRichCurveKey>& InKeys)
	{
		Curve.SetKeys(InKeys);
	}

	// --- Interp/tangent mode ---

	static void FRichCurve_SetKeyInterpMode(FRichCurve& Curve, FKeyHandle KeyHandle, ERichCurveInterpMode NewInterpMode)
	{
		Curve.SetKeyInterpMode(KeyHandle, NewInterpMode);
	}

	static void FRichCurve_SetKeyInterpModeAutoTangents(FRichCurve& Curve, FKeyHandle KeyHandle, ERichCurveInterpMode NewInterpMode, bool bAutoSetTangents)
	{
		Curve.SetKeyInterpMode(KeyHandle, NewInterpMode, bAutoSetTangents);
	}

	static ERichCurveInterpMode FRichCurve_GetKeyInterpMode(const FRichCurve& Curve, FKeyHandle KeyHandle)
	{
		return Curve.GetKeyInterpMode(KeyHandle);
	}

	static void FRichCurve_SetKeyTangentMode(FRichCurve& Curve, FKeyHandle KeyHandle, ERichCurveTangentMode NewTangentMode, bool bAutoSetTangents)
	{
		Curve.SetKeyTangentMode(KeyHandle, NewTangentMode, bAutoSetTangents);
	}

	static ERichCurveTangentMode FRichCurve_GetKeyTangentMode(const FRichCurve& Curve, FKeyHandle KeyHandle)
	{
		return Curve.GetKeyTangentMode(KeyHandle);
	}

	static void FRichCurve_SetKeyTangentWeightMode(FRichCurve& Curve, FKeyHandle KeyHandle, ERichCurveTangentWeightMode NewTangentWeightMode, bool bAutoSetTangents)
	{
		Curve.SetKeyTangentWeightMode(KeyHandle, NewTangentWeightMode, bAutoSetTangents);
	}

	// --- Range queries ---

	static void FRichCurve_GetTimeRange(const FRichCurve& Curve, float& MinTime, float& MaxTime)
	{
		Curve.GetTimeRange(MinTime, MaxTime);
	}

	static void FRichCurve_GetValueRange(const FRichCurve& Curve, float& MinValue, float& MaxValue)
	{
		Curve.GetValueRange(MinValue, MaxValue);
	}

	// --- Queries ---

	static bool FRichCurve_IsConstant(const FRichCurve& Curve, float Tolerance)
	{
		return Curve.IsConstant(Tolerance);
	}

	static bool FRichCurve_IsEmpty(const FRichCurve& Curve)
	{
		return Curve.IsEmpty();
	}

	static bool FRichCurve_HasAnyData(const FRichCurve& Curve)
	{
		return Curve.HasAnyData();
	}

	static FKeyHandle FRichCurve_FindKey(const FRichCurve& Curve, float KeyTime, float KeyTimeTolerance)
	{
		return Curve.FindKey(KeyTime, KeyTimeTolerance);
	}

	static bool FRichCurve_KeyExistsAtTime(const FRichCurve& Curve, float KeyTime, float KeyTimeTolerance)
	{
		return Curve.KeyExistsAtTime(KeyTime, KeyTimeTolerance);
	}

	// --- Evaluation ---

	static float FRichCurve_Eval(const FRichCurve& Curve, float InTime)
	{
		return Curve.Eval(InTime);
	}

	static float FRichCurve_EvalWithDefault(const FRichCurve& Curve, float InTime, float InDefaultValue)
	{
		return Curve.Eval(InTime, InDefaultValue);
	}

	// --- Mutation ---

	static void FRichCurve_Reset(FRichCurve& Curve)
	{
		Curve.Reset();
	}

	static void FRichCurve_AutoSetTangents(FRichCurve& Curve)
	{
		Curve.AutoSetTangents();
	}

	static void FRichCurve_AutoSetTangentsTension(FRichCurve& Curve, float Tension)
	{
		Curve.AutoSetTangents(Tension);
	}

	static void FRichCurve_BakeCurve(FRichCurve& Curve, float SampleRate)
	{
		Curve.BakeCurve(SampleRate);
	}

	static void FRichCurve_BakeCurveRange(FRichCurve& Curve, float SampleRate, float FirstKeyTime, float LastKeyTime)
	{
		Curve.BakeCurve(SampleRate, FirstKeyTime, LastKeyTime);
	}

	static void FRichCurve_RemoveRedundantAutoTangentKeys(FRichCurve& Curve, float Tolerance)
	{
		Curve.RemoveRedundantAutoTangentKeys(Tolerance);
	}

	static void FRichCurve_RemoveRedundantKeys(FRichCurve& Curve, float Tolerance)
	{
		// Pass FFrameRate(0,0) explicitly since the override has no default parameter.
		Curve.RemoveRedundantKeys(Tolerance, FFrameRate(0, 0));
	}

	static void FRichCurve_ReadjustTimeRange(FRichCurve& Curve, float NewMinTimeRange, float NewMaxTimeRange, bool bInsert, float OldStartTime, float OldEndTime)
	{
		Curve.ReadjustTimeRange(NewMinTimeRange, NewMaxTimeRange, bInsert, OldStartTime, OldEndTime);
	}

	static void FRichCurve_RemapTimeValue(const FRichCurve& Curve, float& InTime, float& CycleValueOffset)
	{
		Curve.RemapTimeValue(InTime, CycleValueOffset);
	}

	// --- Handle navigation ---

	static bool FRichCurve_IsKeyHandleValid(const FRichCurve& Curve, FKeyHandle KeyHandle)
	{
		return Curve.IsKeyHandleValid(KeyHandle);
	}

	static FKeyHandle FRichCurve_GetFirstKeyHandle(const FRichCurve& Curve)
	{
		return Curve.GetFirstKeyHandle();
	}

	static FKeyHandle FRichCurve_GetLastKeyHandle(const FRichCurve& Curve)
	{
		return Curve.GetLastKeyHandle();
	}

	static FKeyHandle FRichCurve_GetNextKey(const FRichCurve& Curve, FKeyHandle KeyHandle)
	{
		return Curve.GetNextKey(KeyHandle);
	}

	static FKeyHandle FRichCurve_GetPreviousKey(const FRichCurve& Curve, FKeyHandle KeyHandle)
	{
		return Curve.GetPreviousKey(KeyHandle);
	}

	static int32 FRichCurve_GetIndexSafe(const FRichCurve& Curve, FKeyHandle KeyHandle)
	{
		return Curve.GetIndexSafe(KeyHandle);
	}

	// --- Curve transforms ---

	static void FRichCurve_ShiftCurve(FRichCurve& Curve, float DeltaTime)
	{
		Curve.ShiftCurve(DeltaTime);
	}

	static void FRichCurve_ScaleCurve(FRichCurve& Curve, float ScaleOrigin, float ScaleFactor)
	{
		Curve.ScaleCurve(ScaleOrigin, ScaleFactor);
	}

	// --- Default value ---

	static void FRichCurve_SetDefaultValue(FRichCurve& Curve, float InDefaultValue)
	{
		Curve.SetDefaultValue(InDefaultValue);
	}

	static float FRichCurve_GetDefaultValue(const FRichCurve& Curve)
	{
		return Curve.GetDefaultValue();
	}

	static void FRichCurve_ClearDefaultValue(FRichCurve& Curve)
	{
		Curve.ClearDefaultValue();
	}

	// --- TEnumAsByte extrapolation accessors ---
	// FRealCurve::PreInfinityExtrap / PostInfinityExtrap are TEnumAsByte<ERichCurveExtrapolation> (uint8).

	static ERichCurveExtrapolation FRichCurve_GetPreInfinityExtrap(const FRichCurve& Curve)
	{
		return Curve.PreInfinityExtrap.GetValue();
	}

	static void FRichCurve_SetPreInfinityExtrap(FRichCurve& Curve, ERichCurveExtrapolation Mode)
	{
		Curve.PreInfinityExtrap = Mode;
	}

	static ERichCurveExtrapolation FRichCurve_GetPostInfinityExtrap(const FRichCurve& Curve)
	{
		return Curve.PostInfinityExtrap.GetValue();
	}

	static void FRichCurve_SetPostInfinityExtrap(FRichCurve& Curve, ERichCurveExtrapolation Mode)
	{
		Curve.PostInfinityExtrap = Mode;
	}
}

void Bind_FRichCurve(asIScriptEngine* Engine)
{
	check(Engine != nullptr);
	int Result;

	// =========================================================================
	// Enums
	// =========================================================================

	Result = Engine->RegisterEnum("ERichCurveInterpMode");
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("ERichCurveInterpMode", "RCIM_Linear",   static_cast<int>(RCIM_Linear));   check(Result >= 0);
	Result = Engine->RegisterEnumValue("ERichCurveInterpMode", "RCIM_Constant", static_cast<int>(RCIM_Constant)); check(Result >= 0);
	Result = Engine->RegisterEnumValue("ERichCurveInterpMode", "RCIM_Cubic",    static_cast<int>(RCIM_Cubic));    check(Result >= 0);
	Result = Engine->RegisterEnumValue("ERichCurveInterpMode", "RCIM_None",     static_cast<int>(RCIM_None));     check(Result >= 0);

	Result = Engine->RegisterEnum("ERichCurveTangentMode");
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("ERichCurveTangentMode", "RCTM_Auto",      static_cast<int>(RCTM_Auto));      check(Result >= 0);
	Result = Engine->RegisterEnumValue("ERichCurveTangentMode", "RCTM_User",      static_cast<int>(RCTM_User));      check(Result >= 0);
	Result = Engine->RegisterEnumValue("ERichCurveTangentMode", "RCTM_Break",     static_cast<int>(RCTM_Break));     check(Result >= 0);
	Result = Engine->RegisterEnumValue("ERichCurveTangentMode", "RCTM_None",      static_cast<int>(RCTM_None));      check(Result >= 0);
	Result = Engine->RegisterEnumValue("ERichCurveTangentMode", "RCTM_SmartAuto", static_cast<int>(RCTM_SmartAuto)); check(Result >= 0);

	Result = Engine->RegisterEnum("ERichCurveTangentWeightMode");
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("ERichCurveTangentWeightMode", "RCTWM_WeightedNone",   static_cast<int>(RCTWM_WeightedNone));   check(Result >= 0);
	Result = Engine->RegisterEnumValue("ERichCurveTangentWeightMode", "RCTWM_WeightedArrive", static_cast<int>(RCTWM_WeightedArrive)); check(Result >= 0);
	Result = Engine->RegisterEnumValue("ERichCurveTangentWeightMode", "RCTWM_WeightedLeave",  static_cast<int>(RCTWM_WeightedLeave));  check(Result >= 0);
	Result = Engine->RegisterEnumValue("ERichCurveTangentWeightMode", "RCTWM_WeightedBoth",   static_cast<int>(RCTWM_WeightedBoth));   check(Result >= 0);

	Result = Engine->RegisterEnum("ERichCurveExtrapolation");
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("ERichCurveExtrapolation", "RCCE_Cycle",           static_cast<int>(RCCE_Cycle));           check(Result >= 0);
	Result = Engine->RegisterEnumValue("ERichCurveExtrapolation", "RCCE_CycleWithOffset", static_cast<int>(RCCE_CycleWithOffset)); check(Result >= 0);
	Result = Engine->RegisterEnumValue("ERichCurveExtrapolation", "RCCE_Oscillate",       static_cast<int>(RCCE_Oscillate));       check(Result >= 0);
	Result = Engine->RegisterEnumValue("ERichCurveExtrapolation", "RCCE_Linear",          static_cast<int>(RCCE_Linear));          check(Result >= 0);
	Result = Engine->RegisterEnumValue("ERichCurveExtrapolation", "RCCE_Constant",        static_cast<int>(RCCE_Constant));        check(Result >= 0);
	Result = Engine->RegisterEnumValue("ERichCurveExtrapolation", "RCCE_None",            static_cast<int>(RCCE_None));            check(Result >= 0);

	// =========================================================================
	// FKeyHandle
	// =========================================================================

	REGISTER_BEHAVIOUR(FKeyHandle, asBEHAVE_CONSTRUCT, "void f()",
		asFUNCTION(FKeyHandle_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FKeyHandle, asBEHAVE_CONSTRUCT, "void f(const FKeyHandle &in Other)",
		asFUNCTION(FKeyHandle_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FKeyHandle, asBEHAVE_DESTRUCT, "void f()",
		asFUNCTION(FKeyHandle_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FKeyHandle, "FKeyHandle &opAssign(const FKeyHandle &in Other)",
		asFUNCTION(FKeyHandle_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FKeyHandle, "bool opEquals(const FKeyHandle &in Other) const",
		asFUNCTION(FKeyHandle_Equals), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FKeyHandle, "bool IsValid() const",
		asFUNCTION(FKeyHandle_IsValid), asCALL_CDECL_OBJFIRST);

	Result = Engine->SetDefaultNamespace("FKeyHandle");
	check(Result >= 0);
	Result = Engine->RegisterGlobalFunction("FKeyHandle Invalid()", asFUNCTION(FKeyHandle_Invalid), asCALL_CDECL);
	check(Result >= 0);
	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);

	// =========================================================================
	// FRichCurveKey
	// =========================================================================

	REGISTER_BEHAVIOUR(FRichCurveKey, asBEHAVE_CONSTRUCT, "void f()",
		asFUNCTION(FRichCurveKey_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FRichCurveKey, asBEHAVE_CONSTRUCT, "void f(const FRichCurveKey &in Other)",
		asFUNCTION(FRichCurveKey_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FRichCurveKey, asBEHAVE_CONSTRUCT, "void f(float InTime, float InValue)",
		asFUNCTION(FRichCurveKey_ConstructTimeValue), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FRichCurveKey, asBEHAVE_CONSTRUCT,
		"void f(float InTime, float InValue, float InArriveTangent, float InLeaveTangent, ERichCurveInterpMode InInterpMode)",
		asFUNCTION(FRichCurveKey_ConstructFull), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FRichCurveKey, asBEHAVE_DESTRUCT, "void f()",
		asFUNCTION(FRichCurveKey_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FRichCurveKey, "FRichCurveKey &opAssign(const FRichCurveKey &in Other)",
		asFUNCTION(FRichCurveKey_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurveKey, "bool opEquals(const FRichCurveKey &in Other) const",
		asFUNCTION(FRichCurveKey_Equals), asCALL_CDECL_OBJFIRST);

	REGISTER_PROPERTY(FRichCurveKey, "float Time",                Time);
	REGISTER_PROPERTY(FRichCurveKey, "float Value",               Value);
	REGISTER_PROPERTY(FRichCurveKey, "float ArriveTangent",       ArriveTangent);
	REGISTER_PROPERTY(FRichCurveKey, "float ArriveTangentWeight", ArriveTangentWeight);
	REGISTER_PROPERTY(FRichCurveKey, "float LeaveTangent",        LeaveTangent);
	REGISTER_PROPERTY(FRichCurveKey, "float LeaveTangentWeight",  LeaveTangentWeight);

	// TEnumAsByte fields are uint8 and cannot be bound directly as int32-backed AS enums.
	REGISTER_METHOD(FRichCurveKey, "ERichCurveInterpMode get_InterpMode() const",
		asFUNCTION(FRichCurveKey_GetInterpMode), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurveKey, "void set_InterpMode(ERichCurveInterpMode Mode)",
		asFUNCTION(FRichCurveKey_SetInterpMode), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FRichCurveKey, "ERichCurveTangentMode get_TangentMode() const",
		asFUNCTION(FRichCurveKey_GetTangentMode), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurveKey, "void set_TangentMode(ERichCurveTangentMode Mode)",
		asFUNCTION(FRichCurveKey_SetTangentMode), asCALL_CDECL_OBJFIRST);

	REGISTER_METHOD(FRichCurveKey, "ERichCurveTangentWeightMode get_TangentWeightMode() const",
		asFUNCTION(FRichCurveKey_GetTangentWeightMode), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurveKey, "void set_TangentWeightMode(ERichCurveTangentWeightMode Mode)",
		asFUNCTION(FRichCurveKey_SetTangentWeightMode), asCALL_CDECL_OBJFIRST);

	// =========================================================================
	// FRichCurve
	// =========================================================================

	REGISTER_BEHAVIOUR(FRichCurve, asBEHAVE_CONSTRUCT, "void f()",
		asFUNCTION(FRichCurve_DefaultConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FRichCurve, asBEHAVE_CONSTRUCT, "void f(const FRichCurve &in Other)",
		asFUNCTION(FRichCurve_CopyConstruct), asCALL_CDECL_OBJLAST);
	REGISTER_BEHAVIOUR(FRichCurve, asBEHAVE_DESTRUCT, "void f()",
		asFUNCTION(FRichCurve_Destruct), asCALL_CDECL_OBJLAST);

	REGISTER_METHOD(FRichCurve, "FRichCurve &opAssign(const FRichCurve &in Other)",
		asFUNCTION(FRichCurve_Assign), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "bool opEquals(const FRichCurve &in Other) const",
		asFUNCTION(FRichCurve_Equals), asCALL_CDECL_OBJFIRST);

	// --- Key management ---
	REGISTER_METHOD(FRichCurve, "FKeyHandle AddKey(float InTime, float InValue)",
		asFUNCTION(FRichCurve_AddKey), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "FKeyHandle AddKey(float InTime, float InValue, bool bUnwindRotation)",
		asFUNCTION(FRichCurve_AddKeyUnwind), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "FKeyHandle AddKey(float InTime, float InValue, bool bUnwindRotation, FKeyHandle KeyHandle)",
		asFUNCTION(FRichCurve_AddKeyFull), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "void DeleteKey(FKeyHandle KeyHandle)",
		asFUNCTION(FRichCurve_DeleteKey), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "void ReserveKeys(int Number)",
		asFUNCTION(FRichCurve_ReserveKeys), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "FKeyHandle UpdateOrAddKey(float InTime, float InValue)",
		asFUNCTION(FRichCurve_UpdateOrAddKey), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "FKeyHandle UpdateOrAddKey(float InTime, float InValue, bool bUnwindRotation, float KeyTimeTolerance)",
		asFUNCTION(FRichCurve_UpdateOrAddKeyFull), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "void SetKeyTime(FKeyHandle KeyHandle, float NewTime)",
		asFUNCTION(FRichCurve_SetKeyTime), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "float GetKeyTime(FKeyHandle KeyHandle) const",
		asFUNCTION(FRichCurve_GetKeyTime), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "void SetKeyValue(FKeyHandle KeyHandle, float NewValue, bool bAutoSetTangents)",
		asFUNCTION(FRichCurve_SetKeyValue), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "float GetKeyValue(FKeyHandle KeyHandle) const",
		asFUNCTION(FRichCurve_GetKeyValue), asCALL_CDECL_OBJFIRST);

	// --- Key info ---
	REGISTER_METHOD(FRichCurve, "FRichCurveKey GetFirstKey() const",
		asFUNCTION(FRichCurve_GetFirstKey), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "FRichCurveKey GetLastKey() const",
		asFUNCTION(FRichCurve_GetLastKey), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "FRichCurveKey GetKey(FKeyHandle KeyHandle) const",
		asFUNCTION(FRichCurve_GetKey), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "int GetNumKeys() const",
		asFUNCTION(FRichCurve_GetNumKeys), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "TArray<FRichCurveKey> GetCopyOfKeys() const",
		asFUNCTION(FRichCurve_GetCopyOfKeys), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "void SetKeys(const TArray<FRichCurveKey> &in InKeys)",
		asFUNCTION(FRichCurve_SetKeys), asCALL_CDECL_OBJFIRST);

	// --- Interp/tangent mode ---
	REGISTER_METHOD(FRichCurve, "void SetKeyInterpMode(FKeyHandle KeyHandle, ERichCurveInterpMode NewInterpMode)",
		asFUNCTION(FRichCurve_SetKeyInterpMode), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "void SetKeyInterpMode(FKeyHandle KeyHandle, ERichCurveInterpMode NewInterpMode, bool bAutoSetTangents)",
		asFUNCTION(FRichCurve_SetKeyInterpModeAutoTangents), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "ERichCurveInterpMode GetKeyInterpMode(FKeyHandle KeyHandle) const",
		asFUNCTION(FRichCurve_GetKeyInterpMode), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "void SetKeyTangentMode(FKeyHandle KeyHandle, ERichCurveTangentMode NewTangentMode, bool bAutoSetTangents)",
		asFUNCTION(FRichCurve_SetKeyTangentMode), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "ERichCurveTangentMode GetKeyTangentMode(FKeyHandle KeyHandle) const",
		asFUNCTION(FRichCurve_GetKeyTangentMode), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "void SetKeyTangentWeightMode(FKeyHandle KeyHandle, ERichCurveTangentWeightMode NewTangentWeightMode, bool bAutoSetTangents)",
		asFUNCTION(FRichCurve_SetKeyTangentWeightMode), asCALL_CDECL_OBJFIRST);

	// --- Range / queries ---
	REGISTER_METHOD(FRichCurve, "void GetTimeRange(float &out MinTime, float &out MaxTime) const",
		asFUNCTION(FRichCurve_GetTimeRange), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "void GetValueRange(float &out MinValue, float &out MaxValue) const",
		asFUNCTION(FRichCurve_GetValueRange), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "bool IsConstant(float Tolerance) const",
		asFUNCTION(FRichCurve_IsConstant), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "bool IsEmpty() const",
		asFUNCTION(FRichCurve_IsEmpty), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "bool HasAnyData() const",
		asFUNCTION(FRichCurve_HasAnyData), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "FKeyHandle FindKey(float KeyTime, float KeyTimeTolerance) const",
		asFUNCTION(FRichCurve_FindKey), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "bool KeyExistsAtTime(float KeyTime, float KeyTimeTolerance) const",
		asFUNCTION(FRichCurve_KeyExistsAtTime), asCALL_CDECL_OBJFIRST);

	// --- Evaluation ---
	REGISTER_METHOD(FRichCurve, "float Eval(float InTime) const",
		asFUNCTION(FRichCurve_Eval), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "float Eval(float InTime, float InDefaultValue) const",
		asFUNCTION(FRichCurve_EvalWithDefault), asCALL_CDECL_OBJFIRST);

	// --- Mutation ---
	REGISTER_METHOD(FRichCurve, "void Reset()",
		asFUNCTION(FRichCurve_Reset), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "void AutoSetTangents()",
		asFUNCTION(FRichCurve_AutoSetTangents), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "void AutoSetTangents(float Tension)",
		asFUNCTION(FRichCurve_AutoSetTangentsTension), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "void BakeCurve(float SampleRate)",
		asFUNCTION(FRichCurve_BakeCurve), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "void BakeCurve(float SampleRate, float FirstKeyTime, float LastKeyTime)",
		asFUNCTION(FRichCurve_BakeCurveRange), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "void RemoveRedundantAutoTangentKeys(float Tolerance)",
		asFUNCTION(FRichCurve_RemoveRedundantAutoTangentKeys), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "void RemoveRedundantKeys(float Tolerance)",
		asFUNCTION(FRichCurve_RemoveRedundantKeys), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "void ReadjustTimeRange(float NewMinTimeRange, float NewMaxTimeRange, bool bInsert, float OldStartTime, float OldEndTime)",
		asFUNCTION(FRichCurve_ReadjustTimeRange), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "void RemapTimeValue(float &inout InTime, float &inout CycleValueOffset) const",
		asFUNCTION(FRichCurve_RemapTimeValue), asCALL_CDECL_OBJFIRST);

	// --- Handle navigation ---
	REGISTER_METHOD(FRichCurve, "bool IsKeyHandleValid(FKeyHandle KeyHandle) const",
		asFUNCTION(FRichCurve_IsKeyHandleValid), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "FKeyHandle GetFirstKeyHandle() const",
		asFUNCTION(FRichCurve_GetFirstKeyHandle), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "FKeyHandle GetLastKeyHandle() const",
		asFUNCTION(FRichCurve_GetLastKeyHandle), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "FKeyHandle GetNextKey(FKeyHandle KeyHandle) const",
		asFUNCTION(FRichCurve_GetNextKey), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "FKeyHandle GetPreviousKey(FKeyHandle KeyHandle) const",
		asFUNCTION(FRichCurve_GetPreviousKey), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "int GetIndexSafe(FKeyHandle KeyHandle) const",
		asFUNCTION(FRichCurve_GetIndexSafe), asCALL_CDECL_OBJFIRST);

	// --- Curve transforms ---
	REGISTER_METHOD(FRichCurve, "void ShiftCurve(float DeltaTime)",
		asFUNCTION(FRichCurve_ShiftCurve), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "void ScaleCurve(float ScaleOrigin, float ScaleFactor)",
		asFUNCTION(FRichCurve_ScaleCurve), asCALL_CDECL_OBJFIRST);

	// --- Default value ---
	REGISTER_METHOD(FRichCurve, "void SetDefaultValue(float InDefaultValue)",
		asFUNCTION(FRichCurve_SetDefaultValue), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "float GetDefaultValue() const",
		asFUNCTION(FRichCurve_GetDefaultValue), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "void ClearDefaultValue()",
		asFUNCTION(FRichCurve_ClearDefaultValue), asCALL_CDECL_OBJFIRST);

	// --- Extrapolation (TEnumAsByte fields in FRealCurve, exposed via virtual properties) ---
	REGISTER_METHOD(FRichCurve, "ERichCurveExtrapolation get_PreInfinityExtrap() const",
		asFUNCTION(FRichCurve_GetPreInfinityExtrap), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "void set_PreInfinityExtrap(ERichCurveExtrapolation Mode)",
		asFUNCTION(FRichCurve_SetPreInfinityExtrap), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "ERichCurveExtrapolation get_PostInfinityExtrap() const",
		asFUNCTION(FRichCurve_GetPostInfinityExtrap), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(FRichCurve, "void set_PostInfinityExtrap(ERichCurveExtrapolation Mode)",
		asFUNCTION(FRichCurve_SetPostInfinityExtrap), asCALL_CDECL_OBJFIRST);
}
