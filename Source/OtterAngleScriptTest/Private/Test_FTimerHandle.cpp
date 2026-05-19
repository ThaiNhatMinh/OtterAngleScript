// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Engine/TimerHandle.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFTimerHandleTests,
	"OtterAngleScript.FTimerHandle",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptTimerHandleTest", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FTimerHandle")));
	}

	TEST_METHOD(IsValidBinding)
	{
		static const char Script[] = R"(
int RunIsValidBinding()
{
    FTimerHandle Handle;
    if (Handle.IsValid())
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("TimerHandleIsValidBinding", Script, "int RunIsValidBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(InvalidateBinding)
	{
		static const char Script[] = R"(
int RunInvalidateBinding()
{
    FTimerHandle Handle;
    Handle.Invalidate();
    if (Handle.IsValid())
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("TimerHandleInvalidateBinding", Script, "int RunInvalidateBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(EqualsBinding)
	{
		static const char Script[] = R"(
int RunEqualsBinding()
{
    FTimerHandle A;
    FTimerHandle B(A);
    if (!A.opEquals(B))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("TimerHandleEqualsBinding", Script, "int RunEqualsBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ToStringBinding)
	{
		static const char Script[] = R"(
int RunToStringBinding()
{
    FTimerHandle Handle;
    if (Handle.ToString() != "0")
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("TimerHandleToStringBinding", Script, "int RunToStringBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}
};

#endif
#endif
