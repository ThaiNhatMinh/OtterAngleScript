// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

class asIScriptEngine;

/**
 * Registers FString as a value type in the AngelScript engine using asbind20.
 * The type is exposed as "FString" in scripts.
 */
void Bind_FString(asIScriptEngine* Engine);

/**
 * Registers global logging helpers for scripts.
 * Messages are routed through LogOtterAngleScript.
 */
void Bind_Logging(asIScriptEngine* Engine);
