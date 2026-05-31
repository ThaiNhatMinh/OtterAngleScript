// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "AlphaBlend.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFAlphaBlendTests,
	"OtterAngleScript.FAlphaBlend",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptFAlphaBlendTest", asGM_ALWAYS_CREATE);
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

	TEST_METHOD(TypesRegistered)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FAlphaBlend")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FAlphaBlendArgs")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("EAlphaBlendOption")));
	}

	TEST_METHOD(EAlphaBlendOptionEnumValues)
	{
		static const char Script[] = R"(
int RunEAlphaBlendOptionTest()
{
    EAlphaBlendOption Val = EAlphaBlendOption::Linear;
    if (Val != EAlphaBlendOption::Linear)
    {
        return -1;
    }
    Val = EAlphaBlendOption::Cubic;
    if (Val != EAlphaBlendOption::Cubic)
    {
        return -2;
    }
    Val = EAlphaBlendOption::Custom;
    if (Val != EAlphaBlendOption::Custom)
    {
        return -3;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("EAlphaBlendOptionTest", Script, "int RunEAlphaBlendOptionTest()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FAlphaBlendArgsDefaultConstruct)
	{
		static const char Script[] = R"(
int RunFAlphaBlendArgsDefaultConstruct()
{
    FAlphaBlendArgs Args;
    if (Args.BlendOption != EAlphaBlendOption::Linear)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAlphaBlendArgsDefaultConstruct", Script, "int RunFAlphaBlendArgsDefaultConstruct()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FAlphaBlendArgsConstructFromBlendTime)
	{
		static const char Script[] = R"(
int RunFAlphaBlendArgsConstructFromBlendTime()
{
    FAlphaBlendArgs Args(0.5f);
    if (Args.BlendTime != 0.5f)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAlphaBlendArgsConstructFromBlendTime", Script, "int RunFAlphaBlendArgsConstructFromBlendTime()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FAlphaBlendArgsConstructFromFAlphaBlend)
	{
		static const char Script[] = R"(
int RunFAlphaBlendArgsConstructFromFAlphaBlend()
{
    FAlphaBlend Blend(0.3f);
    FAlphaBlendArgs Args(Blend);
    if (Args.BlendTime != 0.3f)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAlphaBlendArgsConstructFromFAlphaBlend", Script, "int RunFAlphaBlendArgsConstructFromFAlphaBlend()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FAlphaBlendArgsCopyAndAssign)
	{
		static const char Script[] = R"(
int RunFAlphaBlendArgsCopyAndAssign()
{
    FAlphaBlendArgs Original(0.7f);
    Original.BlendOption = EAlphaBlendOption::Sinusoidal;

    FAlphaBlendArgs Copy(Original);
    if (Copy.BlendTime != 0.7f || Copy.BlendOption != EAlphaBlendOption::Sinusoidal)
    {
        return -1;
    }

    FAlphaBlendArgs Assigned;
    Assigned = Original;
    if (Assigned.BlendTime != 0.7f || Assigned.BlendOption != EAlphaBlendOption::Sinusoidal)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAlphaBlendArgsCopyAndAssign", Script, "int RunFAlphaBlendArgsCopyAndAssign()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FAlphaBlendArgsProperties)
	{
		static const char Script[] = R"(
int RunFAlphaBlendArgsProperties()
{
    FAlphaBlendArgs Args;
    Args.BlendTime = 1.5f;
    Args.BlendOption = EAlphaBlendOption::CubicInOut;

    if (Args.BlendTime != 1.5f)
    {
        return -1;
    }
    if (Args.BlendOption != EAlphaBlendOption::CubicInOut)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAlphaBlendArgsProperties", Script, "int RunFAlphaBlendArgsProperties()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FAlphaBlendDefaultConstruct)
	{
		static const char Script[] = R"(
int RunFAlphaBlendDefaultConstruct()
{
    FAlphaBlend Blend;
    // Default blend time is 0.2f
    if (Blend.GetBlendTime() != 0.2f)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAlphaBlendDefaultConstruct", Script, "int RunFAlphaBlendDefaultConstruct()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FAlphaBlendConstructFromBlendTime)
	{
		static const char Script[] = R"(
int RunFAlphaBlendConstructFromBlendTime()
{
    FAlphaBlend Blend(0.5f);
    if (Blend.GetBlendTime() != 0.5f)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAlphaBlendConstructFromBlendTime", Script, "int RunFAlphaBlendConstructFromBlendTime()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FAlphaBlendConstructFromOtherAndTime)
	{
		static const char Script[] = R"(
int RunFAlphaBlendConstructFromOtherAndTime()
{
    FAlphaBlend Original(0.4f);
    FAlphaBlend Blend(Original, 0.9f);
    if (Blend.GetBlendTime() != 0.9f)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAlphaBlendConstructFromOtherAndTime", Script, "int RunFAlphaBlendConstructFromOtherAndTime()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FAlphaBlendConstructFromArgs)
	{
		static const char Script[] = R"(
int RunFAlphaBlendConstructFromArgs()
{
    FAlphaBlendArgs Args(0.6f);
    Args.BlendOption = EAlphaBlendOption::Sinusoidal;
    FAlphaBlend Blend(Args);
    if (Blend.GetBlendTime() != 0.6f)
    {
        return -1;
    }
    if (Blend.GetBlendOption() != EAlphaBlendOption::Sinusoidal)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAlphaBlendConstructFromArgs", Script, "int RunFAlphaBlendConstructFromArgs()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FAlphaBlendCopyAndAssign)
	{
		static const char Script[] = R"(
int RunFAlphaBlendCopyAndAssign()
{
    FAlphaBlend Original(0.8f);
    Original.SetBlendOption(EAlphaBlendOption::QuadraticInOut);

    FAlphaBlend Copy(Original);
    if (Copy.GetBlendTime() != 0.8f || Copy.GetBlendOption() != EAlphaBlendOption::QuadraticInOut)
    {
        return -1;
    }

    FAlphaBlend Assigned;
    Assigned = Original;
    if (Assigned.GetBlendTime() != 0.8f || Assigned.GetBlendOption() != EAlphaBlendOption::QuadraticInOut)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAlphaBlendCopyAndAssign", Script, "int RunFAlphaBlendCopyAndAssign()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FAlphaBlendSetBlendOption)
	{
		static const char Script[] = R"(
int RunFAlphaBlendSetBlendOption()
{
    FAlphaBlend Blend;
    Blend.SetBlendOption(EAlphaBlendOption::HermiteCubic);
    if (Blend.GetBlendOption() != EAlphaBlendOption::HermiteCubic)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAlphaBlendSetBlendOption", Script, "int RunFAlphaBlendSetBlendOption()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FAlphaBlendSetBlendTime)
	{
		static const char Script[] = R"(
int RunFAlphaBlendSetBlendTime()
{
    FAlphaBlend Blend;
    Blend.SetBlendTime(1.2f);
    if (Blend.GetBlendTime() != 1.2f)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAlphaBlendSetBlendTime", Script, "int RunFAlphaBlendSetBlendTime()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FAlphaBlendSetValueRange)
	{
		static const char Script[] = R"(
int RunFAlphaBlendSetValueRange()
{
    FAlphaBlend Blend;
    Blend.SetValueRange(0.0f, 10.0f);
    if (Blend.GetBeginValue() != 0.0f)
    {
        return -1;
    }
    if (Blend.GetDesiredValue() != 10.0f)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAlphaBlendSetValueRange", Script, "int RunFAlphaBlendSetValueRange()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FAlphaBlendSetDesiredValue)
	{
		static const char Script[] = R"(
int RunFAlphaBlendSetDesiredValue()
{
    FAlphaBlend Blend;
    Blend.SetDesiredValue(5.0f);
    if (Blend.GetDesiredValue() != 5.0f)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAlphaBlendSetDesiredValue", Script, "int RunFAlphaBlendSetDesiredValue()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FAlphaBlendSetAlpha)
	{
		static const char Script[] = R"(
int RunFAlphaBlendSetAlpha()
{
    FAlphaBlend Blend;
    Blend.SetAlpha(0.75f);
    if (Blend.GetAlpha() != 0.75f)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAlphaBlendSetAlpha", Script, "int RunFAlphaBlendSetAlpha()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FAlphaBlendUpdate)
	{
		static const char Script[] = R"(
int RunFAlphaBlendUpdate()
{
    FAlphaBlend Blend(1.0f);
    Blend.SetValueRange(0.0f, 1.0f);
    Blend.Update(0.5f);
    // After updating half the blend time, alpha should be non-zero and blend should not be complete
    if (Blend.IsComplete())
    {
        return -1;
    }
    if (Blend.GetAlpha() <= 0.0f)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAlphaBlendUpdate", Script, "int RunFAlphaBlendUpdate()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FAlphaBlendIsComplete)
	{
		static const char Script[] = R"(
int RunFAlphaBlendIsComplete()
{
    FAlphaBlend Blend(0.5f);
    Blend.SetValueRange(0.0f, 1.0f);
    // Update beyond blend time to complete it
    Blend.Update(1.0f);
    if (!Blend.IsComplete())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAlphaBlendIsComplete", Script, "int RunFAlphaBlendIsComplete()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FAlphaBlendGetBlendTimeRemaining)
	{
		static const char Script[] = R"(
int RunFAlphaBlendGetBlendTimeRemaining()
{
    FAlphaBlend Blend(1.0f);
    Blend.SetValueRange(0.0f, 1.0f);
    Blend.Update(0.3f);
    float Remaining = Blend.GetBlendTimeRemaining();
    if (Remaining <= 0.0f)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAlphaBlendGetBlendTimeRemaining", Script, "int RunFAlphaBlendGetBlendTimeRemaining()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FAlphaBlendReset)
	{
		static const char Script[] = R"(
int RunFAlphaBlendReset()
{
    FAlphaBlend Blend(0.5f);
    Blend.SetValueRange(0.0f, 1.0f);
    Blend.Update(1.0f);
    if (!Blend.IsComplete())
    {
        return -1;
    }
    Blend.Reset();
    if (Blend.IsComplete())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAlphaBlendReset", Script, "int RunFAlphaBlendReset()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FAlphaBlendResetAlpha)
	{
		static const char Script[] = R"(
int RunFAlphaBlendResetAlpha()
{
    FAlphaBlend Blend(1.0f);
    Blend.SetValueRange(0.0f, 1.0f);
    Blend.Update(0.5f);
    float AlphaBefore = Blend.GetAlpha();
    Blend.ResetAlpha();
    // ResetAlpha keeps the blended value but resets alpha state; blend should continue from current value
    float AlphaAfter = Blend.GetAlpha();
    if (AlphaBefore <= 0.0f || AlphaAfter < 0.0f)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAlphaBlendResetAlpha", Script, "int RunFAlphaBlendResetAlpha()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FAlphaBlendGetBlendedValue)
	{
		static const char Script[] = R"(
int RunFAlphaBlendGetBlendedValue()
{
    FAlphaBlend Blend(0.5f);
    Blend.SetValueRange(0.0f, 10.0f);
    Blend.Update(1.0f);
    // After completion the blended value should reach the desired value
    if (Blend.GetBlendedValue() != 10.0f)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FAlphaBlendGetBlendedValue", Script, "int RunFAlphaBlendGetBlendedValue()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FAlphaBlend_AlphaToBlendOptionGlobal)
	{
		static const char Script[] = R"(
int RunAlphaToBlendOptionGlobal()
{
    // Linear blend: output should equal input
    float Out = FAlphaBlend_AlphaToBlendOption(0.5f, EAlphaBlendOption::Linear);
    if (Out != 0.5f)
    {
        return -1;
    }
    // Alpha 0 should always return 0 regardless of blend type
    float OutZero = FAlphaBlend_AlphaToBlendOption(0.0f, EAlphaBlendOption::Cubic);
    if (OutZero != 0.0f)
    {
        return -2;
    }
    // Alpha 1 should always return 1 regardless of blend type
    float OutOne = FAlphaBlend_AlphaToBlendOption(1.0f, EAlphaBlendOption::Cubic);
    if (OutOne != 1.0f)
    {
        return -3;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("AlphaToBlendOptionGlobal", Script, "int RunAlphaToBlendOptionGlobal()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
