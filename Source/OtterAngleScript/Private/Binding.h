// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

class asIScriptEngine;


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
 * Registers EAxisType for axis-based FVector helpers.
 */
void Bind_EAxisType(asIScriptEngine* Engine);

/**
 * Registers EForceInit for constructors that support explicit zero-initialization.
 */
void Bind_EForceInit(asIScriptEngine* Engine);

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
 * Registers global FMath helpers under the FMath script namespace.
 */
void Bind_FMath(asIScriptEngine* Engine);

/**
 * Registers global logging helpers for scripts.
 * Messages are routed through LogOtterAngleScript.
 */
void Bind_Logging(asIScriptEngine* Engine);
