// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterAngleScript.h"
#include "Binding.h"
#include "Misc/MessageDialog.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "angelscript.h"

DEFINE_LOG_CATEGORY(LogOtterAngleScript);

#define LOCTEXT_NAMESPACE "FOtterAngleScriptModule"

void MessageCallback(const asSMessageInfo* msg, void* param)
{
	if (msg->type == asMSGTYPE_ERROR)
	{
		UE_LOG(LogOtterAngleScript, Error, TEXT("%s (%d, %d) : %s"), UTF8_TO_TCHAR(msg->section), msg->row, msg->col, UTF8_TO_TCHAR(msg->message));
	}
	if (msg->type == asMSGTYPE_WARNING)
	{
		UE_LOG(LogOtterAngleScript, Warning, TEXT("%s (%d, %d) : %s"), UTF8_TO_TCHAR(msg->section), msg->row, msg->col, UTF8_TO_TCHAR(msg->message));
	}
	else if (msg->type == asMSGTYPE_INFORMATION)
	{
		UE_LOG(LogOtterAngleScript, Log, TEXT("%s (%d, %d) : %s"), UTF8_TO_TCHAR(msg->section), msg->row, msg->col, UTF8_TO_TCHAR(msg->message));
	}
}

void FOtterAngleScriptModule::StartupModule()
{
	// Create the script engine
	Engine = asCreateScriptEngine();
	int Result = Engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
	if (Result < 0)
	{
		UE_LOG(LogOtterAngleScript, Error, TEXT("Failed to set message callback for AngelScript engine"));
	}

	Bind_FString(Engine);
	Bind_Logging(Engine);

	// TODO: Multiple modules or single module?
	ScriptModule = Engine->GetModule("OtterAngleScript", asGM_ALWAYS_CREATE);
}

void FOtterAngleScriptModule::ShutdownModule()
{
	Engine->ShutDownAndRelease();
	Engine = nullptr;
	ScriptModule = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOtterAngleScriptModule, OtterAngleScript)

asIScriptEngine* GetScriptEngine()
{
	auto MyModule = FModuleManager::Get().GetModulePtr<FOtterAngleScriptModule>("OtterAngleScript");
	if (MyModule)
	{
		return MyModule->GetScriptEngine();
	}
	else
	{
		UE_LOG(LogOtterAngleScript, Error, TEXT("Failed to get OtterAngleScript module"));
	}
	return nullptr;
}
