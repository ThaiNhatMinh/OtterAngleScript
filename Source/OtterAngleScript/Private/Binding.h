// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

class asIScriptEngine;

void Declare_Types(asIScriptEngine* Engine);

#define REGISTER_BEHAVIOUR(ClassType, Behaviour, Declaration, Function, CallConv) \
	Result = Engine->RegisterObjectBehaviour(#ClassType, Behaviour, Declaration, Function, CallConv); \
	check(Result >= 0)
#define REGISTER_METHOD(ClassType, Declaration, Function, CallConv) \
	Result = Engine->RegisterObjectMethod(#ClassType, Declaration, Function, CallConv); \
	check(Result >= 0)

#define REGISTER_PROPERTY(ClassType, Declaration, Member) \
	Result = Engine->RegisterObjectProperty(#ClassType, Declaration, asOFFSET(ClassType, Member)); \
	check(Result >= 0)

/**
 * Registers FName as a value type in the AngelScript engine.
 * The type is exposed as "FName" in scripts.
 */
void Bind_FName(asIScriptEngine* Engine);

/**
 * Registers UObject as a reference type in the AngelScript engine.
 */
void Bind_UObject(asIScriptEngine* Engine);

/**
 * Registers UClass as a reference type in the AngelScript engine.
 */
void Bind_UClass(asIScriptEngine* Engine);

/**
 * Registers FActorInstanceHandle as a value type in the AngelScript engine.
 * The type is exposed as "FActorInstanceHandle" in scripts.
 */
void Bind_FActorInstanceHandle(asIScriptEngine* Engine);

/**
 * Registers TWeakObjectPtr<T> as a value-type AngelScript template.
 * Declare_TWeakObjectPtr registers the template type declaration; Bind_TWeakObjectPtr registers
 * the template callback, constructors, and all methods.
 */
void Declare_TWeakObjectPtr(asIScriptEngine* Engine);
void Bind_TWeakObjectPtr(asIScriptEngine* Engine);

/**
 * Registers FString as a value type in the AngelScript engine.
 * Supports both native and generic AngelScript bindings.
 * The type is exposed as "FString" in scripts.
 */
void Bind_FString(asIScriptEngine* Engine);

/**
 * Registers all script-exposed enums: EAxisType, EForceInitType, EObjectTypeQuery, ETraceTypeQuery.
 */
void Bind_Enums(asIScriptEngine* Engine);

/**
 * Registers FVector2D as a value type in the AngelScript engine.
 */
void Bind_FVector2D(asIScriptEngine* Engine);

/**
 * Registers FRotator as a value type in the AngelScript engine.
 */
void Bind_FRotator(asIScriptEngine* Engine);

/**
 * Registers FQuat as a value type in the AngelScript engine.
 */
void Bind_FQuat(asIScriptEngine* Engine);

/**
 * Registers FTransform as a value type in the AngelScript engine.
 */
void Bind_FTransform(asIScriptEngine* Engine);

/**
 * Registers FBox as a value type in the AngelScript engine.
 */
void Bind_FBox(asIScriptEngine* Engine);

/**
 * Registers FPlane as a value type in the AngelScript engine.
 */
void Bind_FPlane(asIScriptEngine* Engine);

/**
 * Registers FText as a value type in the AngelScript engine.
 * Exposes constructors, assignment, comparison (CompareTo, EqualTo, IdenticalTo, case-ignored variants),
 * conversion helpers (ToString, BuildSourceString, ToLower, ToUpper), state queries
 * (IsEmpty, IsEmptyOrWhitespace, IsNumeric, IsTransient, IsCultureInvariant,
 * IsInitializedFromString, IsFromStringTable), and static factory/formatting methods
 * (GetEmpty, FromString, FromName, AsCultureInvariant, TrimPreceding/Trailing, AsNumber,
 * AsPercent, Format with 1-3 ordered FText args or a single int/float arg) under the FText namespace.
 */
void Bind_FText(asIScriptEngine* Engine);

/**
 * Registers FVector as a value type in the AngelScript engine.
 * The type is exposed as "FVector" in scripts.
 */
void Bind_FVector(asIScriptEngine* Engine);

/**
 * Registers FHitResult as a value type in the AngelScript engine.
 * The type is exposed as "FHitResult" in scripts.
 */
void Bind_FHitResult(asIScriptEngine* Engine);

/**
 * Registers FTimerHandle as a value type in the AngelScript engine.
 */
void Bind_FTimerHandle(asIScriptEngine* Engine);

/**
 * Registers FLatentActionInfo as a value type in the AngelScript engine.
 */
void Bind_FLatentActionInfo(asIScriptEngine* Engine);

/**
 * Registers global FMath helpers under the FMath script namespace.
 */
void Bind_FMath(asIScriptEngine* Engine);

/**
 * Registers FLinearColor as a value type in the AngelScript engine.
 * The type is exposed as "FLinearColor" in scripts.
 */
void Bind_FLinearColor(asIScriptEngine* Engine);

/**
 * Registers FColor as a value type in the AngelScript engine.
 * The type is exposed as "FColor" in scripts.
 */
void Bind_FColor(asIScriptEngine* Engine);

/**
 * Registers FSoftObjectPath as a value type in the AngelScript engine.
 * The type is exposed as "FSoftObjectPath" in scripts.
 */
void Bind_FSoftObjectPath(asIScriptEngine* Engine);

/**
 * Registers FSoftClassPath as a value type in the AngelScript engine.
 * The type is exposed as "FSoftClassPath" in scripts.
 */
void Bind_FSoftClassPath(asIScriptEngine* Engine);

/**
 * Registers TSoftObjectPtr concrete specializations in the AngelScript engine.
 * The type is exposed as "TSoftObjectPtr_UObject" in scripts.
 */
void Bind_TSoftObjectPtr(asIScriptEngine* Engine);

/**
 * Registers global logging helpers for scripts.
 * Messages are routed through LogOtterAngleScript.
 */
void Bind_Logging(asIScriptEngine* Engine);

/**
 * Registers FFrameNumber as a value type in the AngelScript engine.
 * The type is exposed as "FFrameNumber" in scripts.
 */
void Bind_FFrameNumber(asIScriptEngine* Engine);

/**
 * Registers FFrameTime as a value type in the AngelScript engine.
 * The type is exposed as "FFrameTime" in scripts.
 */
void Bind_FFrameTime(asIScriptEngine* Engine);

/**
 * Registers FFrameRate as a value type in the AngelScript engine.
 * The type is exposed as "FFrameRate" in scripts.
 */
void Bind_FFrameRate(asIScriptEngine* Engine);

/**
 * Registers FNavAgentSelector as a value type in the AngelScript engine.
 * The type is exposed as "FNavAgentSelector" in scripts.
 */
void Bind_FNavAgentSelector(asIScriptEngine* Engine);

/**
 * Registers TArray<T> as a GC-managed template reference type in the AngelScript engine.
 * The type is exposed as "TArray<T>" in scripts.
 */
void Declare_TArray(asIScriptEngine* Engine);
void Declare_TSoftObjectPtr(asIScriptEngine* Engine);
void Bind_TArray(asIScriptEngine* Engine);

/**
 * Registers TSet<T> as a GC-managed template value type in the AngelScript engine.
 * The type is exposed as "TSet<T>" in scripts. Elements are stored unordered and
 * uniqueness is enforced using the subtype's opEquals method.
 */
void Declare_TSet(asIScriptEngine* Engine);
void Bind_TSet(asIScriptEngine* Engine);

/**
 * Registers TMap<K,V> as a GC-managed template value type in the AngelScript engine.
 * The type is exposed as "TMap<K,V>" in scripts. Key uniqueness is enforced using the
 * key type's opEquals method.
 */
void Declare_TMap(asIScriptEngine* Engine);
void Bind_TMap(asIScriptEngine* Engine);

/**
 * Registers TSubclassOf<T> as a value-type AngelScript template.
 * Declare_TSubclassOf registers the template type declaration; Bind_TSubclassOf registers
 * the template callback, constructors, and all methods.
 */
void Declare_TSubclassOf(asIScriptEngine* Engine);
void Bind_TSubclassOf(asIScriptEngine* Engine);

/**
 * Registers UPhysicalMaterial as a reference type, FPhysicalMaterialStrength and
 * FPhysicalMaterialDamageModifier as value types, and the EFrictionCombineMode,
 * EPhysicalSurface, and EPhysicalMaterialSoftCollisionMode enums.
 */
void Bind_UPhysicalMaterial(asIScriptEngine* Engine);

/**
 * Registers EAudioParameterType enum and FAudioParameter value type, including
 * constructors, scalar properties, virtual properties for ParamType and ObjectParam,
 * the Merge instance method, and FAudioParameter::CreateDefaultArray static factory.
 */
void Bind_FAudioParameter(asIScriptEngine* Engine);

/**
 * Registers FVector4f (TVector4<float>) as a value type in the AngelScript engine.
 * Exposes X/Y/Z/W float components, arithmetic operators, index operators,
 * geometric methods, and FVector4f::Zero/One static helpers.
 */
void Bind_FVector4f(asIScriptEngine* Engine);

/**
 * Registers FBox2D (TBox2<double>) as a value type in the AngelScript engine.
 * Exposes Min/Max/bIsValid properties, extend operators (+ and +=), index operator,
 * geometric query methods, and a FBox2D::BuildAABB static helper.
 */
void Bind_FBox2D(asIScriptEngine* Engine);

/**
 * Registers ERichCurveInterpMode, ERichCurveTangentMode, ERichCurveTangentWeightMode, and
 * ERichCurveExtrapolation enums, plus FKeyHandle, FRichCurveKey, and FRichCurve value types.
 * Exposes all key management, evaluation, tangent, extrapolation, and curve-transform methods.
 */
void Bind_FRichCurve(asIScriptEngine* Engine);
