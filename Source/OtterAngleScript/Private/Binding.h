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
 * Registers the concrete TWeakObjectPtr specializations used by script-exposed engine types.
 */
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
 * Registers global logging helpers for scripts.
 * Messages are routed through LogOtterAngleScript.
 */
void Bind_Logging(asIScriptEngine* Engine);

/**
 * Registers TArray<T> as a GC-managed template reference type in the AngelScript engine.
 * The type is exposed as "TArray<T>" in scripts.
 */
void Bind_TArray(asIScriptEngine* Engine);
