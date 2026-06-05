// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#ifdef _MSC_VER
#pragma warning(disable:4191 4996)
#endif
#include "angelscript.h"

void Declare_Types(asIScriptEngine* Engine);

#define REGISTER_BEHAVIOUR(ClassType, Behaviour, Declaration, Function, CallConv) \
	Result = Engine->RegisterObjectBehaviour(#ClassType, Behaviour, Declaration, Function, CallConv); \
	check(Result >= 0)
#define REGISTER_METHOD(ClassType, Declaration, Function, CallConv) \
	Result = Engine->RegisterObjectMethod(#ClassType, Declaration, Function, CallConv); \
	check(Result >= 0)

#define REGISTER_METHOD_NAMED(ClassType, Declaration, Function, CallConv) \
	Result = Engine->RegisterObjectMethod(ClassType, Declaration, Function, CallConv); \
	check(Result >= 0)

#define REGISTER_PROPERTY(ClassType, Declaration, Member) \
	Result = Engine->RegisterObjectProperty(#ClassType, Declaration, asOFFSET(ClassType, Member)); \
	check(Result >= 0)

#define REGISTER_PROPERTY_NAMED(ClassType, Declaration, Member) \
	Result = Engine->RegisterObjectProperty(ClassType, Declaration, asOFFSET(ClassType, Member)); \
	check(Result >= 0)

/**
 * Registers FName as a value type in the AngelScript engine.
 * The type is exposed as "FName" in scripts.
 */
void Declare_FName(asIScriptEngine* Engine);
void Bind_FName(asIScriptEngine* Engine);

/**
 * Registers UObject as a reference type in the AngelScript engine.
 */
void Bind_UObject(asIScriptEngine* Engine);
void OAS_RegisterMethods_UObject(asIScriptEngine* Engine, const FString& ChildName);

/**
 * Registers UClass as a reference type in the AngelScript engine.
 */
void Bind_UClass(asIScriptEngine* Engine);
void OAS_RegisterMethods_UClass(asIScriptEngine* Engine, const FString& ChildName);

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
 * Registers UWorld as a reference type in the AngelScript engine.
 * The type is exposed as "UWorld" in scripts.
 */
void Bind_UWorld(asIScriptEngine* Engine);

/**
 * Registers FVector4f (TVector4<float>) as a value type in the AngelScript engine.
 * Exposes X/Y/Z/W float components, arithmetic operators, index operators,
 * geometric methods, and FVector4f::Zero/One static helpers.
 */
void Bind_FVector4f(asIScriptEngine* Engine);

/**
 * Registers FVector3f (TVector<float>) as a value type in the AngelScript engine.
 * Exposes X/Y/Z float components, arithmetic/compound operators, index operators,
 * normalization, geometric methods, and FVector3f::Zero/One/UnitX/UnitY/UnitZ
 * static helpers.
 */
void Bind_FVector3f(asIScriptEngine* Engine);

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

/**
 * Registers FMatrix (FMatrix44d) as a value type in the AngelScript engine.
 * Exposes constructors, arithmetic/compound operators, transform methods,
 * axis/column accessors, rotation conversion, scaling helpers, and the
 * Identity static property.
 */
void Bind_FMatrix(asIScriptEngine* Engine);

/**
 * Registers FKey as a value type in the AngelScript engine.
 * The type is exposed as "FKey" in scripts.
 * Exposes constructors (default, copy, from FName, from FString),
 * assignment, equality/comparison, type queries (IsValid, IsModifierKey,
 * IsGamepadKey, IsTouch, IsMouseButton, IsDigital, IsAnalog, etc.),
 * and helpers (GetDisplayName, ToString, GetFName, GetMenuCategory,
 * GetPairedAxis, GetPairedAxisKey).
 */
void Bind_FKey(asIScriptEngine* Engine);

/**
 * Registers FDateTime as a value type in the AngelScript engine.
 * The type is exposed as "FDateTime" in scripts.
 * Also registers EDayOfWeek and EMonthOfYear enums required by its API.
 * Exposes constructors, date/time component accessors, string conversion
 * (ToString, ToIso8601, ToHttpDate, ToFormattedString), Unix timestamp
 * helpers, and static methods (Now, UtcNow, Today, Parse, etc.).
 */
void Bind_FDateTime(asIScriptEngine* Engine);

/**
 * Registers FIntVector (TIntVector3<int32>) as a value type in the AngelScript engine.
 * Exposes X/Y/Z properties, arithmetic/bitwise operators, and utility methods
 * (IsZero, GetMax, GetMin, Size, ComponentMin/Max, ToString, etc.).
 */
void Bind_FIntVector(asIScriptEngine* Engine);

/**
 * Registers FPlatformUserId as a value type in the AngelScript engine.
 * Exposes IsValid, GetInternalId, and the CreateFromInternalId static factory.
 */
void Bind_FPlatformUserId(asIScriptEngine* Engine);

/**
 * Registers FGuid as a value type in the AngelScript engine.
 * Also registers the EGuidFormats enum. Exposes A/B/C/D components,
 * constructors, IsValid, Invalidate, ToString, NewGuid, Parse, etc.
 */
void Bind_FGuid(asIScriptEngine* Engine);

/**
 * Registers FInputDeviceId as a value type in the AngelScript engine.
 * Exposes IsValid, GetId, and the CreateFromInternalId static factory.
 */
void Bind_FInputDeviceId(asIScriptEngine* Engine);

/**
 * Registers FInputChord as a value type in the AngelScript engine.
 * Also registers the nested ERelationshipType enum. Exposes Key, modifier
 * properties (bShift/bCtrl/bAlt/bCmd), GetRelationship, Needs* methods,
 * GetInputText, IsValidChord, etc.
 */
void Bind_FInputChord(asIScriptEngine* Engine);

/**
 * Registers FPrimaryAssetType and FPrimaryAssetId as value types in the
 * AngelScript engine. Exposes constructors, IsValid, ToString, property
 * accessors, and static helpers (FromString, ParseTypeAndName).
 */
void Bind_FPrimaryAssetId(asIScriptEngine* Engine);

/**
 * Registers TSoftClassPtr<T> as a value-type AngelScript template.
 * Declare_TSoftClassPtr registers the template type declaration; Bind_TSoftClassPtr registers
 * the template callback, constructors, and all methods.
 * TSoftClassPtr inherits from TSoftObjectPtr and exposes UClass@ Get() / LoadSynchronous().
 */
void Declare_TSoftClassPtr(asIScriptEngine* Engine);
void Bind_TSoftClassPtr(asIScriptEngine* Engine);

/**
 * Registers FSuggestProjectileVelocityParameters as a value type and the
 * ESuggestProjVelocityTraceOption enum in the AngelScript engine.
 * The type is exposed as "FSuggestProjectileVelocityParameters" in scripts.
 */
void Bind_FSuggestProjectileVelocityParameters(asIScriptEngine* Engine);

/**
 * Registers FIntPoint (FInt32Point) as a value type in the AngelScript engine.
 * Exposes X/Y int32 components, arithmetic operators, index operator,
 * utility methods (ComponentMin/Max, GetMin/Max, Size, SizeSquared, ToString,
 * InitFromString), and static helpers (Num, DivideAndRoundUp/Down).
 */
void Bind_FIntPoint(asIScriptEngine* Engine);

/**
 * Registers FRandomStream as a value type in the AngelScript engine.
 * Exposes seed management, random number generation (FRand, RandRange,
 * FRandRange, VRand, RandPointInBox, VRandCone), and serialization.
 */
void Bind_FRandomStream(asIScriptEngine* Engine);

/**
 * Registers FVector4 (FVector4d, TVector4<double>) as a value type in the
 * AngelScript engine.  Exposes X/Y/Z/W double components, arithmetic
 * operators, index operators, geometric methods, and FVector4::Zero/One
 * static helpers.
 */
void Bind_FVector4(asIScriptEngine* Engine);

/**
 * Registers FInputEvent as a value type in the AngelScript engine.
 * Exposes modifier key queries (shift/control/alt/command/caps), repeat
 * state, user/device identifiers, timestamp helpers, and event type checks.
 */
void Bind_FInputEvent(asIScriptEngine* Engine);

/**
 * Registers FKeyEvent as a value type in the AngelScript engine.
 * Exposes the pressed key, character code, hardware key code, and text
 * representation.
 */
void Bind_FKeyEvent(asIScriptEngine* Engine);

/**
 * Registers FPointerEvent as a value type in the AngelScript engine.
 * Exposes cursor position/delta, mouse button state, touch parameters,
 * gesture type/delta, and wheel delta.
 */
void Bind_FPointerEvent(asIScriptEngine* Engine);

/**
 * Registers FIntVector2 (TIntVector2<int32>) as a value type in the
 * AngelScript engine. Exposes X/Y int32 components, arithmetic/bitwise
 * operators, index operator, utility methods, and static helpers.
 */
void Bind_FIntVector2(asIScriptEngine* Engine);

/**
 * Registers FTimespan as a value type in the AngelScript engine.
 * Exposes constructors, arithmetic/comparison operators, component/total
 * accessors, ToString, and static helpers (FromDays/Hours/Minutes/Seconds,
 * FromMilliseconds/Microseconds, MaxValue/MinValue, Parse).
 */
void Bind_FTimespan(asIScriptEngine* Engine);

/**
 * Registers FTopLevelAssetPath as a value type in the AngelScript engine.
 * Exposes constructors, assignment, equality, PackageName/AssetName
 * property accessors, IsValid, IsNull, Reset, ToString, and Compare.
 */
void Bind_FTopLevelAssetPath(asIScriptEngine* Engine);

/**
 * Registers FFormatArgumentData as a value type, plus the EFormatArgumentType
 * and ETextGender enums, in the AngelScript engine.
 * Exposes properties (ArgumentName, ArgumentValueType, ArgumentValue,
 * ArgumentValueInt/Float/Double, ArgumentValueGender) and ResetValue.
 */
void Bind_FFormatArgumentData(asIScriptEngine* Engine);

