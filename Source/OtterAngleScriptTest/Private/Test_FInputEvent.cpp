// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Input/Events.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFInputEventTests,
	"OtterAngleScript.FInputEvent",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptInputEventTest", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FInputEvent")));
	}

	TEST_METHOD(DefaultConstruct)
	{
		static const char Script[] = R"(
int RunTest()
{
    FInputEvent E;
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("DefaultCtor", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(IsRepeatDefault)
	{
		static const char Script[] = R"(
int RunTest()
{
    FInputEvent E;
    if (E.IsRepeat()) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("IsRepeatDefault", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(ToText)
	{
		static const char Script[] = R"(
int RunTest()
{
    FInputEvent E;
    FText T = E.ToText();
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("ToText", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(IsPointerEvent)
	{
		static const char Script[] = R"(
int RunTest()
{
    FInputEvent E;
    if (E.IsPointerEvent()) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("IsPointerEvent", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(IsKeyEvent)
	{
		static const char Script[] = R"(
int RunTest()
{
    FInputEvent E;
    if (E.IsKeyEvent()) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("IsKeyEvent", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(modifier_accessors)
	{
		static const char Script[] = R"(
int RunTest()
{
    FInputEvent E;
    if (E.IsShiftDown()) { return -1; }
    if (E.IsLeftShiftDown()) { return -2; }
    if (E.IsRightShiftDown()) { return -3; }
    if (E.IsControlDown()) { return -4; }
    if (E.IsLeftControlDown()) { return -5; }
    if (E.IsRightControlDown()) { return -6; }
    if (E.IsAltDown()) { return -7; }
    if (E.IsLeftAltDown()) { return -8; }
    if (E.IsRightAltDown()) { return -9; }
    if (E.IsCommandDown()) { return -10; }
    if (E.IsLeftCommandDown()) { return -11; }
    if (E.IsRightCommandDown()) { return -12; }
    if (E.AreCapsLocked()) { return -13; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("ModifierAccessors", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
