// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFStringTests,
	"OtterAngleScript.FString",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
{
	asIScriptEngine* Engine = nullptr;
	asIScriptModule* ScriptModule = nullptr;
	asIScriptContext* Context = nullptr;

	BEFORE_EACH()
	{
		FOtterAngleScriptModule& Module = FModuleManager::LoadModuleChecked<FOtterAngleScriptModule>("OtterAngleScript");
		Engine = Module.GetScriptEngine();
		ASSERT_THAT(IsNotNull(Engine));

		ScriptModule = Engine->GetModule("OtterAngleScriptTest", asGM_ALWAYS_CREATE);
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

	TEST_METHOD(BasicOperations)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FString")));

		static const char Script[] = R"(
int RunFStringTest()
{
    FString Value = "Otter";
    Value += "Angle";
    if (!Value.Contains("Angle"))
    {
        return -1;
    }

    Value.ReplaceInline("Angle", "Script");
    return Value.Len();
}
)";

		ASSERT_THAT(IsTrue(ScriptModule->AddScriptSection("FStringTests", Script) >= 0));
		ASSERT_THAT(IsTrue(ScriptModule->Build() >= 0));

		asIScriptFunction* Function = ScriptModule->GetFunctionByDecl("int RunFStringTest()");
		ASSERT_THAT(IsNotNull(Function));

		Context = Engine->CreateContext();
		ASSERT_THAT(IsNotNull(Context));
		ASSERT_THAT(IsTrue(Context->Prepare(Function) >= 0));
		auto Result = Context->Execute();
		if (Result == asEXECUTION_EXCEPTION)
		{
			// An exception occurred, let the script writer know what happened so it can be corrected.
			AddError(Context->GetExceptionString());
		}
		AddInfo(FString::Printf(TEXT("Execute result = %d"), Result));
		ASSERT_THAT(IsTrue(Result == asEXECUTION_FINISHED));
		ASSERT_THAT(IsTrue(static_cast<int32>(Context->GetReturnDWord()) == 11));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
