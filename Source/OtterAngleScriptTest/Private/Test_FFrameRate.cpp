// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Misc/FrameNumber.h"
#include "Misc/FrameRate.h"
#include "Misc/FrameTime.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFFrameRateTests,
	"OtterAngleScript.FFrameRate",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptFrameRateTest", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FFrameRate")));
	}

	TEST_METHOD(DefaultConstructor)
	{
		static const char Script[] = R"(
int RunDefaultConstructor()
{
    FFrameRate R;
    if (R.Numerator != 60000 || R.Denominator != 1)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameRateDefaultCtor", Script, "int RunDefaultConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ConstructNumeratorDenominator)
	{
		static const char Script[] = R"(
int RunConstructNumeratorDenominator()
{
    FFrameRate R(30, 1);
    if (R.Numerator != 30 || R.Denominator != 1)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameRateNumeratorDenominatorCtor", Script, "int RunConstructNumeratorDenominator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(NumeratorPropertyBinding)
	{
		static const char Script[] = R"(
int RunNumeratorPropertyBinding()
{
    FFrameRate R(24, 1);
    R.Numerator = 30;
    if (R.Numerator != 30)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameRateNumeratorProp", Script, "int RunNumeratorPropertyBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(DenominatorPropertyBinding)
	{
		static const char Script[] = R"(
int RunDenominatorPropertyBinding()
{
    FFrameRate R(24, 1);
    R.Denominator = 2;
    if (R.Denominator != 2)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameRateDenominatorProp", Script, "int RunDenominatorPropertyBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(EqualsBinding)
	{
		static const char Script[] = R"(
int RunEqualsBinding()
{
    FFrameRate A(30, 1);
    FFrameRate B(30, 1);
    FFrameRate C(24, 1);
    if (!(A == B))
    {
        return -1;
    }
    if (A == C)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameRateEquals", Script, "int RunEqualsBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsValidBinding)
	{
		static const char Script[] = R"(
int RunIsValidBinding()
{
    FFrameRate Valid(30, 1);
    FFrameRate Invalid(30, 0);
    if (!Valid.IsValid())
    {
        return -1;
    }
    if (Invalid.IsValid())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameRateIsValid", Script, "int RunIsValidBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(AsIntervalBinding)
	{
		static const char Script[] = R"(
int RunAsIntervalBinding()
{
    FFrameRate R(2, 1);
    // interval = 1/2 = 0.5s per frame
    if (R.AsInterval() != 0.5)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameRateAsInterval", Script, "int RunAsIntervalBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(AsDecimalBinding)
	{
		static const char Script[] = R"(
int RunAsDecimalBinding()
{
    FFrameRate R(30, 1);
    if (R.AsDecimal() != 30.0)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameRateAsDecimal", Script, "int RunAsDecimalBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(AsSecondsBinding)
	{
		static const char Script[] = R"(
int RunAsSecondsBinding()
{
    FFrameRate R(1, 1);
    FFrameTime T(5);
    // 5 frames at 1fps = 5 seconds
    if (R.AsSeconds(T) != 5.0)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameRateAsSeconds", Script, "int RunAsSecondsBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(AsFrameTimeBinding)
	{
		static const char Script[] = R"(
int RunAsFrameTimeBinding()
{
    FFrameRate R(1, 1);
    // 3 seconds at 1fps = frame 3
    FFrameTime T = R.AsFrameTime(3.0);
    if (T.GetFrame().Value != 3)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameRateAsFrameTime", Script, "int RunAsFrameTimeBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(AsFrameNumberBinding)
	{
		static const char Script[] = R"(
int RunAsFrameNumberBinding()
{
    FFrameRate R(1, 1);
    // 7 seconds at 1fps = frame 7
    FFrameNumber N = R.AsFrameNumber(7.0);
    if (N.Value != 7)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameRateAsFrameNumber", Script, "int RunAsFrameNumberBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsMultipleOfBinding)
	{
		static const char Script[] = R"(
int RunIsMultipleOfBinding()
{
    FFrameRate R60(60, 1);
    FFrameRate R30(30, 1);
    // 30fps is a multiple of 60fps (every 60fps frame aligns to a 30fps frame)
    if (!R30.IsMultipleOf(R60))
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameRateIsMultipleOf", Script, "int RunIsMultipleOfBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsFactorOfBinding)
	{
		static const char Script[] = R"(
int RunIsFactorOfBinding()
{
    FFrameRate R60(60, 1);
    FFrameRate R30(30, 1);
    // 60fps is a factor of 30fps
    if (!R60.IsFactorOf(R30))
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameRateIsFactorOf", Script, "int RunIsFactorOfBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ReciprocalBinding)
	{
		static const char Script[] = R"(
int RunReciprocalBinding()
{
    FFrameRate R(30, 1);
    FFrameRate Rec = R.Reciprocal();
    if (Rec.Numerator != 1 || Rec.Denominator != 30)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameRateReciprocal", Script, "int RunReciprocalBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ToPrettyTextBinding)
	{
		static const char Script[] = R"(
int RunToPrettyTextBinding()
{
    FFrameRate R(30, 1);
    FText T = R.ToPrettyText();
    if (T.ToString().IsEmpty())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameRateToPrettyText", Script, "int RunToPrettyTextBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(MaxSecondsBinding)
	{
		static const char Script[] = R"(
int RunMaxSecondsBinding()
{
    FFrameRate R(30, 1);
    if (R.MaxSeconds() <= 0.0)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameRateMaxSeconds", Script, "int RunMaxSecondsBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(MulBinding)
	{
		static const char Script[] = R"(
int RunMulBinding()
{
    FFrameRate A(3, 1);
    FFrameRate B(2, 1);
    FFrameRate C = A * B;
    if (C.Numerator != 6 || C.Denominator != 1)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameRateMul", Script, "int RunMulBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(DivBinding)
	{
		static const char Script[] = R"(
int RunDivBinding()
{
    FFrameRate A(60, 1);
    FFrameRate B(2, 1);
    FFrameRate C = A / B;
    if (C.Numerator != 60 || C.Denominator != 2)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameRateDiv", Script, "int RunDivBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(TransformTimeBinding)
	{
		static const char Script[] = R"(
int RunTransformTimeBinding()
{
    FFrameRate Src(24, 1);
    FFrameRate Dst(48, 1);
    FFrameTime T(12);
    // frame 12 at 24fps -> frame 24 at 48fps
    FFrameTime Result = FFrameRate::TransformTime(T, Src, Dst);
    if (Result.GetFrame().Value != 24)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameRateTransformTime", Script, "int RunTransformTimeBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(SnapBinding)
	{
		static const char Script[] = R"(
int RunSnapBinding()
{
    FFrameRate R30(30, 1);
    FFrameRate R60(60, 1);
    FFrameTime T(3);
    // frame 3 at 30fps snapped to 60fps and back should round-trip
    FFrameTime Result = FFrameRate::Snap(T, R30, R60);
    if (Result.GetFrame().Value != 3)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameRateSnap", Script, "int RunSnapBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}
};

#endif
#endif
