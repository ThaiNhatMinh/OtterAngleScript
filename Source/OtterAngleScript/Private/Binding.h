// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

class asIScriptEngine;


#define REGISTER_BEHAVIOUR(ClassType, Behaviour, Declaration, Function, CallConv) \
	Result = Engine->RegisterObjectBehaviour(#ClassType, Behaviour, Declaration, Function, CallConv); \
	check(Result >= 0)
#define REGISTER_METHOD(ClassType, Declaration, Function, CallConv) \
	Result = Engine->RegisterObjectMethod(#ClassType, Declaration, Function, CallConv); \
	check(Result >= 0)

/**
 * Registers FString as a value type in the AngelScript engine.
 * Supports both native and generic AngelScript bindings.
 * The type is exposed as "FString" in scripts.
 */
void Bind_FString(asIScriptEngine* Engine);

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
 * Registers global logging helpers for scripts.
 * Messages are routed through LogOtterAngleScript.
 */
void Bind_Logging(asIScriptEngine* Engine);
