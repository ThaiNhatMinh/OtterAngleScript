// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Engine/LatentActionManager.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "UObject/Object.h"
#include "UObject/UObjectGlobals.h"
#include "angelscript.h"

namespace
{
	UObject* GetLatentFixtureObject()
	{
		static UObject* Object = nullptr;
		if (Object == nullptr)
		{
			Object = NewObject<UObject>(GetTransientPackage(), NAME_None, RF_Transient);
			Object->AddToRoot();
		}

		return Object;
	}

	bool RegisterLatentFixtureBindings(asIScriptEngine* Engine)
	{
		static bool bRegistered = false;
		if (bRegistered)
		{
			return true;
		}

		const int Result = Engine->RegisterGlobalFunction("UObject@ GetLatentFixtureObject()", asFUNCTION(GetLatentFixtureObject), asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		bRegistered = true;
		return true;
	}
}

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFLatentActionInfoTests,
	"OtterAngleScript.FLatentActionInfo",
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
		ASSERT_THAT(IsTrue(RegisterLatentFixtureBindings(Engine)));

		ScriptModule = Engine->GetModule("OtterAngleScriptLatentActionInfoTest", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FLatentActionInfo")));
	}

	TEST_METHOD(LinkageBinding)
	{
		static const char Script[] = R"(
int RunLinkageBinding()
{
    FLatentActionInfo Info;
    Info.Linkage = 7;
    if (Info.Linkage != 7)
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("LatentActionInfoLinkageBinding", Script, "int RunLinkageBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(UUIDBinding)
	{
		static const char Script[] = R"(
int RunUUIDBinding()
{
    FLatentActionInfo Info;
    Info.UUID = 17;
    if (Info.UUID != 17)
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("LatentActionInfoUUIDBinding", Script, "int RunUUIDBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ExecutionFunctionBinding)
	{
		static const char Script[] = R"(
int RunExecutionFunctionBinding()
{
    FLatentActionInfo Info;
    Info.ExecutionFunction = FName("Run");
    if (Info.ExecutionFunction.ToString() != "Run")
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("LatentActionInfoExecutionFunctionBinding", Script, "int RunExecutionFunctionBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(CallbackTargetBinding)
	{
		static const char Script[] = R"(
int RunCallbackTargetBinding()
{
    FLatentActionInfo Info;
    Info.CallbackTarget = GetLatentFixtureObject();
    if (Info.CallbackTarget !is GetLatentFixtureObject())
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("LatentActionInfoCallbackTargetBinding", Script, "int RunCallbackTargetBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ValueConstructorBinding)
	{
		static const char Script[] = R"(
int RunValueConstructorBinding()
{
    FLatentActionInfo Info(2, 13, "Resume", GetLatentFixtureObject());
    if (Info.Linkage != 2 || Info.UUID != 13 || Info.ExecutionFunction.ToString() != "Resume" || Info.CallbackTarget !is GetLatentFixtureObject())
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("LatentActionInfoValueConstructorBinding", Script, "int RunValueConstructorBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}
};

#endif
#endif
