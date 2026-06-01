// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterAngleScript.h"
#include "Binding.h"
#include "Misc/MessageDialog.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"

#ifdef _MSC_VER
#pragma warning(disable:4191 4996)
#endif

#include "angelscript.h"

#if __has_include("OtterAngelScriptBindings.gen.h")
#include "OtterAngelScriptBindings.gen.h"
#endif
#include "AudioDevice.h"

DEFINE_LOG_CATEGORY(LogOtterAngleScript);

#define LOCTEXT_NAMESPACE "FOtterAngleScriptModule"

LLM_DEFINE_TAG(OtterAngleScript);

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

void* asCustomMalloc(size_t size)
{
	LLM_SCOPE_BYTAG(OtterAngleScript);
	void* Ptr = FMemory::Malloc(size);
	return Ptr;
}

void asCustomFree(void* ptr)
{
	LLM_SCOPE_BYTAG(OtterAngleScript);
	FMemory::Free(ptr);
}

void FOtterAngleScriptModule::StartupModule()
{
	// Create the script engine
	Engine = asCreateScriptEngine();
	Engine->SetEngineProperty(asEP_ALLOW_IMPLICIT_HANDLE_TYPES, 1);
	//asSetGlobalMemoryFunctions(asCustomMalloc, asCustomFree);

	int Result = Engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
	if (Result < 0)
	{
		UE_LOG(LogOtterAngleScript, Error, TEXT("Failed to set message callback for AngelScript engine"));
	}

	Declare_Types(Engine);
	Bind_TWeakObjectPtr(Engine);
	Bind_TSoftObjectPtr(Engine);
	Bind_TArray(Engine);
	Bind_FString(Engine);
	Bind_TSet(Engine);
	Bind_TMap(Engine);
#if __has_include("OtterAngelScriptBindings.gen.h")
	OAS_RegisterGeneratedTypes(Engine);
#endif
	Bind_FName(Engine);
	Bind_UClass(Engine);
	Bind_UObject(Engine);
	Bind_Enums(Engine);
	Bind_FVector2D(Engine);
	Bind_FRotator(Engine);
	Bind_FQuat(Engine);
	Bind_FPlane(Engine);
	Bind_FText(Engine);
	Bind_FVector(Engine);
	Bind_FTransform(Engine);
	Bind_FBox(Engine);
	Bind_FBox2D(Engine);
	Bind_FActorInstanceHandle(Engine);
	Bind_FHitResult(Engine);
	Bind_FTimerHandle(Engine);
	Bind_FLatentActionInfo(Engine);
	Bind_FFrameNumber(Engine);
	Bind_FFrameTime(Engine);
	Bind_FFrameRate(Engine);
	Bind_FNavAgentSelector(Engine);
	Bind_FLinearColor(Engine);
	Bind_FColor(Engine);
	Bind_FVector4f(Engine);
	Bind_FSoftObjectPath(Engine);
	Bind_FSoftClassPath(Engine);
	Bind_FMath(Engine);
	Bind_Logging(Engine);
	Bind_UPhysicalMaterial(Engine);
	Bind_TSubclassOf(Engine);
	Bind_FRichCurve(Engine);
	Bind_FMatrix(Engine);

	// Include and invoke the UHT-generated AngelScript bindings if they exist.
	// GeneratedAngelScriptBindings.h is produced by OtterAngleScriptUbtPlugin (one
	// header per class + a master header/source pair).  The __has_include guard
	// makes the first bootstrap build safe before any UHT output exists.
#if __has_include("OtterAngelScriptBindings.gen.h")
	Bind_Generated(Engine);
#endif


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


void SetScriptException(const char* Message)
{
	if (asIScriptContext* Context = asGetActiveContext())
	{
		Context->SetException(Message);
	}
}