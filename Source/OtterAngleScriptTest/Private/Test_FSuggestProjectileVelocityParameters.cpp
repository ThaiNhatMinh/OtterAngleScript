// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Kismet/GameplayStatics.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFSuggestProjectileVelocityParametersTests,
	"OtterAngleScript.FSuggestProjectileVelocityParameters",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
{
	asIScriptEngine* Engine = nullptr;
	asIScriptModule* ScriptModule = nullptr;
	asIScriptContext* Context = nullptr;

	asIScriptFunction* BuildFunction(const char* SectionName, const char* Script, const char* Declaration)
	{
		if (!Assert.IsTrue(ScriptModule->AddScriptSection(SectionName, Script) >= 0))
		{
			return nullptr;
		}
		if (!Assert.IsTrue(ScriptModule->Build() >= 0))
		{
			return nullptr;
		}

		asIScriptFunction* Function = ScriptModule->GetFunctionByDecl(Declaration);
		if (!Assert.IsNotNull(Function))
		{
			return nullptr;
		}
		return Function;
	}

	int ExecuteFunction(asIScriptFunction* Function)
	{
		if (Context != nullptr)
		{
			Context->Release();
			Context = nullptr;
		}

		Context = Engine->CreateContext();
		if (!Assert.IsNotNull(Context))
		{
			return -1;
		}
		if (!Assert.IsTrue(Context->Prepare(Function) >= 0))
		{
			return -1;
		}

		return Context->Execute();
	}

	int32 ExecuteIntFunction(asIScriptFunction* Function)
	{
		const int Result = ExecuteFunction(Function);
		if (Result == asEXECUTION_EXCEPTION)
		{
			AddError(Context->GetExceptionString());
		}

		if (!Assert.IsTrue(Result == asEXECUTION_FINISHED))
		{
			return -1;
		}

		return static_cast<int32>(Context->GetReturnDWord());
	}

	BEFORE_EACH()
	{
		FOtterAngleScriptModule& Module = FModuleManager::LoadModuleChecked<FOtterAngleScriptModule>("OtterAngleScript");
		Engine = Module.GetScriptEngine();
		ASSERT_THAT(IsNotNull(Engine));

		ScriptModule = Engine->GetModule("OtterAngleScriptProjVelParamsTest", asGM_ALWAYS_CREATE);
		ASSERT_THAT(IsNotNull(ScriptModule));
	}

	AFTER_EACH()
	{
		if (Context != nullptr)
		{
			Context->Release();
			Context = nullptr;
		}

		if (ScriptModule != nullptr)
		{
			ScriptModule->Discard();
			ScriptModule = nullptr;
		}

		Engine = nullptr;
	}

	TEST_METHOD(TypeRegistered)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FSuggestProjectileVelocityParameters")));
	}

	TEST_METHOD(PropertiesReadWrite)
	{
		static const char Script[] = R"(
int RunParameterProperties()
{
    FSuggestProjectileVelocityParameters Params;
    Params.Start = FVector(0.0, 0.0, 0.0);
    Params.End = FVector(100.0, 0.0, 0.0);
    Params.TossSpeed = 500.0;
    Params.bFavorHighArc = false;
    Params.CollisionRadius = 10.0;
    Params.OverrideGravityZ = -980.0;
    Params.bDrawDebug = false;
    Params.bAcceptClosestOnNoSolutions = true;
    if (Params.Start != FVector(0.0, 0.0, 0.0))
    {
        return -1;
    }
    if (Params.End != FVector(100.0, 0.0, 0.0))
    {
        return -2;
    }
    if (Params.TossSpeed != 500.0)
    {
        return -3;
    }
    if (Params.bFavorHighArc)
    {
        return -4;
    }
    if (Params.CollisionRadius != 10.0)
    {
        return -5;
    }
    if (Params.OverrideGravityZ != -980.0)
    {
        return -6;
    }
    if (Params.bDrawDebug)
    {
        return -7;
    }
    if (!Params.bAcceptClosestOnNoSolutions)
    {
        return -8;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("ProjVelParamsProps", Script, "int RunParameterProperties()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}
};

#endif
#endif
