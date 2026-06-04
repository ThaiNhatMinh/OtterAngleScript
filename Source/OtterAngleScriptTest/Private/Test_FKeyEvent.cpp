// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Input/Events.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFKeyEventTests,
	"OtterAngleScript.FKeyEvent",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptKeyEventTest", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FKeyEvent")));
	}

	TEST_METHOD(DefaultConstruct)
	{
		static const char Script[] = R"(
int RunTest()
{
    FKeyEvent E;
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("DefaultCtor", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(GetKeyOnDefault)
	{
		static const char Script[] = R"(
int RunTest()
{
    FKeyEvent E;
    FKey K = E.GetKey();
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("GetKeyDefault", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(GetCharacterOnDefault)
	{
		static const char Script[] = R"(
int RunTest()
{
    FKeyEvent E;
    uint Ch = E.GetCharacter();
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("GetCharDefault", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
