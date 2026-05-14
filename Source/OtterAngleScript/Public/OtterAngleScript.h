// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogOtterAngleScript, Log, All);

class asIScriptEngine;
class asIScriptModule;

asIScriptEngine* GetScriptEngine();

class FOtterAngleScriptModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	asIScriptEngine* GetScriptEngine() const { return Engine; }
	asIScriptModule* GetScriptModule() const { return ScriptModule; }

private:
	asIScriptEngine* Engine = nullptr;
	asIScriptModule* ScriptModule = nullptr;
};
