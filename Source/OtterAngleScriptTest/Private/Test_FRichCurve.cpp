// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Curves/RichCurve.h"
#include "Curves/KeyHandle.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFRichCurveTests,
	"OtterAngleScript.FRichCurve",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptRichCurveTest", asGM_ALWAYS_CREATE);
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

	TEST_METHOD(TypeRegistered_FRichCurve)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FRichCurve")));
	}

	TEST_METHOD(TypeRegistered_FRichCurveKey)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FRichCurveKey")));
	}

	TEST_METHOD(TypeRegistered_FKeyHandle)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FKeyHandle")));
	}

	TEST_METHOD(DefaultConstruct)
	{
		static const char Script[] = R"(
int RunDefaultConstruct()
{
    FRichCurve Curve;
    if (!Curve.IsEmpty())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("RichCurveDefaultCtor", Script, "int RunDefaultConstruct()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(AddKey)
	{
		static const char Script[] = R"(
int RunAddKey()
{
    FRichCurve Curve;
    FKeyHandle Handle = Curve.AddKey(0.0f, 1.0f);
    if (!Handle.IsValid())
    {
        return -1;
    }
    if (Curve.GetNumKeys() != 1)
    {
        return -2;
    }
    if (Curve.GetKeyTime(Handle) != 0.0f)
    {
        return -3;
    }
    if (Curve.GetKeyValue(Handle) != 1.0f)
    {
        return -4;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("RichCurveAddKey", Script, "int RunAddKey()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(Eval)
	{
		static const char Script[] = R"(
int RunEval()
{
    FRichCurve Curve;
    Curve.AddKey(0.0f, 10.0f);
    Curve.AddKey(1.0f, 20.0f);
    float ValAtZero = Curve.Eval(0.0f);
    if (ValAtZero != 10.0f)
    {
        return -1;
    }
    float ValAtOne = Curve.Eval(1.0f);
    if (ValAtOne != 20.0f)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("RichCurveEval", Script, "int RunEval()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetNumKeys)
	{
		static const char Script[] = R"(
int RunGetNumKeys()
{
    FRichCurve Curve;
    Curve.AddKey(0.0f, 1.0f);
    Curve.AddKey(1.0f, 2.0f);
    Curve.AddKey(2.0f, 3.0f);
    if (Curve.GetNumKeys() != 3)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("RichCurveGetNumKeys", Script, "int RunGetNumKeys()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsEmpty)
	{
		static const char Script[] = R"(
int RunIsEmpty()
{
    FRichCurve Curve;
    if (!Curve.IsEmpty())
    {
        return -1;
    }
    Curve.AddKey(0.0f, 1.0f);
    if (Curve.IsEmpty())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("RichCurveIsEmpty", Script, "int RunIsEmpty()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(Reset)
	{
		static const char Script[] = R"(
int RunReset()
{
    FRichCurve Curve;
    Curve.AddKey(0.0f, 1.0f);
    Curve.Reset();
    if (!Curve.IsEmpty())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("RichCurveReset", Script, "int RunReset()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(DeleteKey)
	{
		static const char Script[] = R"(
int RunDeleteKey()
{
    FRichCurve Curve;
    FKeyHandle Handle = Curve.AddKey(0.0f, 1.0f);
    Curve.DeleteKey(Handle);
    if (!Curve.IsEmpty())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("RichCurveDeleteKey", Script, "int RunDeleteKey()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}
};

#endif
#endif
