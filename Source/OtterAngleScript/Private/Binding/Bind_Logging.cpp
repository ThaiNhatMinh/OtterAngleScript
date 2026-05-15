// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterAngleScript.h"
#include <asbind20/asbind.hpp>

static void ScriptLogInfo(const FString& Message)
{
    UE_LOG(LogOtterAngleScript, Log, TEXT("%s"), *Message);
}

static void ScriptLogWarning(const FString& Message)
{
    UE_LOG(LogOtterAngleScript, Warning, TEXT("%s"), *Message);
}

static void ScriptLogError(const FString& Message)
{
    UE_LOG(LogOtterAngleScript, Error, TEXT("%s"), *Message);
}

void Bind_Logging(asIScriptEngine* Engine)
{
    check(Engine != nullptr);

    asbind20::global(Engine)
        .function("void Log(const FString &in Message)", asbind20::fp<&ScriptLogInfo>)
        .function("void LogWarning(const FString &in Message)", asbind20::fp<&ScriptLogWarning>)
        .function("void LogError(const FString &in Message)", asbind20::fp<&ScriptLogError>);
}
