// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptKismetEnumTests,
	"OtterAngleScript.KismetEnums",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptKismetEnumTest", asGM_ALWAYS_CREATE);
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

	TEST_METHOD(EObjectTypeQueryRegistered)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("EObjectTypeQuery")));
	}

	TEST_METHOD(ETraceTypeQueryRegistered)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("ETraceTypeQuery")));
	}

	TEST_METHOD(EObjectTypeQueryValuesBinding)
	{
		static const char Script[] = R"(
int RunEObjectTypeQueryValuesBinding()
{
    if (ObjectTypeQuery1 == ObjectTypeQuery32)
    {
        return -1;
    }

    EObjectTypeQuery Value = ObjectTypeQuery32;
    if (Value != ObjectTypeQuery32)
    {
        return -2;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("EObjectTypeQueryValuesBinding", Script, "int RunEObjectTypeQueryValuesBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ETraceTypeQueryValuesBinding)
	{
		static const char Script[] = R"(
int RunETraceTypeQueryValuesBinding()
{
    if (TraceTypeQuery1 == TraceTypeQuery32)
    {
        return -1;
    }

    ETraceTypeQuery Value = TraceTypeQuery32;
    if (Value != TraceTypeQuery32)
    {
        return -2;
    }

    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("ETraceTypeQueryValuesBinding", Script, "int RunETraceTypeQueryValuesBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}
};

#endif
#endif
