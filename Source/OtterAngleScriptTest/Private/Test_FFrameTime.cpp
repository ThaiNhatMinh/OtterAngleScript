// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Misc/FrameNumber.h"
#include "Misc/FrameTime.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFFrameTimeTests,
	"OtterAngleScript.FFrameTime",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptFrameTimeTest", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FFrameTime")));
	}

	TEST_METHOD(DefaultConstructor)
	{
		static const char Script[] = R"(
int RunDefaultConstructor()
{
    FFrameTime T;
    if (T.GetFrame().Value != 0 || T.GetSubFrame() != 0.0f)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameTimeDefaultCtor", Script, "int RunDefaultConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ConstructFromInt)
	{
		static const char Script[] = R"(
int RunConstructFromInt()
{
    FFrameTime T(5);
    if (T.GetFrame().Value != 5 || T.GetSubFrame() != 0.0f)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameTimeConstructFromInt", Script, "int RunConstructFromInt()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ConstructFromFrameNumber)
	{
		static const char Script[] = R"(
int RunConstructFromFrameNumber()
{
    FFrameNumber F(10);
    FFrameTime T(F);
    if (T.GetFrame().Value != 10 || T.GetSubFrame() != 0.0f)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameTimeConstructFromFN", Script, "int RunConstructFromFrameNumber()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ConstructFromFrameNumberAndSubFrame)
	{
		static const char Script[] = R"(
int RunConstructFromFrameNumberAndSubFrame()
{
    FFrameNumber F(3);
    FFrameTime T(F, 0.5f);
    if (T.GetFrame().Value != 3)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameTimeConstructFNSF", Script, "int RunConstructFromFrameNumberAndSubFrame()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FrameNumberPropertyBinding)
	{
		static const char Script[] = R"(
int RunFrameNumberPropertyBinding()
{
    FFrameTime T(7);
    if (T.FrameNumber.Value != 7)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameTimeFrameNumberProp", Script, "int RunFrameNumberPropertyBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetSubFrameBinding)
	{
		static const char Script[] = R"(
int RunGetSubFrameBinding()
{
    FFrameNumber F(2);
    FFrameTime T(F, 0.25f);
    if (T.GetSubFrame() <= 0.0f)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameTimeGetSubFrame", Script, "int RunGetSubFrameBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FloorToFrameBinding)
	{
		static const char Script[] = R"(
int RunFloorToFrameBinding()
{
    FFrameNumber F(4);
    FFrameTime T(F, 0.9f);
    if (T.FloorToFrame().Value != 4)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameTimeFloorToFrame", Script, "int RunFloorToFrameBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(CeilToFrameBinding)
	{
		static const char Script[] = R"(
int RunCeilToFrameBinding()
{
    FFrameNumber F(4);
    FFrameTime T(F, 0.5f);
    if (T.CeilToFrame().Value != 5)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameTimeCeilToFrame", Script, "int RunCeilToFrameBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(RoundToFrameBinding)
	{
		static const char Script[] = R"(
int RunRoundToFrameBinding()
{
    FFrameNumber F(4);
    FFrameTime T(F, 0.6f);
    if (T.RoundToFrame().Value != 5)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameTimeRoundToFrame", Script, "int RunRoundToFrameBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(AsDecimalBinding)
	{
		static const char Script[] = R"(
int RunAsDecimalBinding()
{
    FFrameTime T(10);
    if (T.AsDecimal() != 10.0)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameTimeAsDecimal", Script, "int RunAsDecimalBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FromDecimalBinding)
	{
		static const char Script[] = R"(
int RunFromDecimalBinding()
{
    FFrameTime T = FFrameTime::FromDecimal(5.0);
    if (T.GetFrame().Value != 5)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameTimeFromDecimal", Script, "int RunFromDecimalBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(EqualsBinding)
	{
		static const char Script[] = R"(
int RunEqualsBinding()
{
    FFrameTime A(5);
    FFrameTime B(5);
    FFrameTime C(6);
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
		asIScriptFunction* Function = BuildFunction("FrameTimeEquals", Script, "int RunEqualsBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ComparisonBinding)
	{
		static const char Script[] = R"(
int RunComparisonBinding()
{
    FFrameTime A(3);
    FFrameTime B(5);
    if (!(A < B))
    {
        return -1;
    }
    if (!(B > A))
    {
        return -2;
    }
    if (!(A <= A))
    {
        return -3;
    }
    if (!(B >= B))
    {
        return -4;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameTimeComparison", Script, "int RunComparisonBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(AddBinding)
	{
		static const char Script[] = R"(
int RunAddBinding()
{
    FFrameTime A(3);
    FFrameTime B(4);
    FFrameTime C = A + B;
    if (C.GetFrame().Value != 7)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameTimeAdd", Script, "int RunAddBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(SubBinding)
	{
		static const char Script[] = R"(
int RunSubBinding()
{
    FFrameTime A(10);
    FFrameTime B(3);
    FFrameTime C = A - B;
    if (C.GetFrame().Value != 7)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameTimeSub", Script, "int RunSubBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(MulDoubleBinding)
	{
		static const char Script[] = R"(
int RunMulDoubleBinding()
{
    FFrameTime A(4);
    FFrameTime B = A * 2.0;
    if (B.GetFrame().Value != 8)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameTimeMulDouble", Script, "int RunMulDoubleBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(DivDoubleBinding)
	{
		static const char Script[] = R"(
int RunDivDoubleBinding()
{
    FFrameTime A(10);
    FFrameTime B = A / 2.0;
    if (B.GetFrame().Value != 5)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameTimeDivDouble", Script, "int RunDivDoubleBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(NegBinding)
	{
		static const char Script[] = R"(
int RunNegBinding()
{
    FFrameTime A(5);
    FFrameTime B = -A;
    if (B.GetFrame().Value != -5)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameTimeNeg", Script, "int RunNegBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(AddAssignBinding)
	{
		static const char Script[] = R"(
int RunAddAssignBinding()
{
    FFrameTime A(3);
    FFrameTime B(4);
    A += B;
    if (A.GetFrame().Value != 7)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameTimeAddAssign", Script, "int RunAddAssignBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(SubAssignBinding)
	{
		static const char Script[] = R"(
int RunSubAssignBinding()
{
    FFrameTime A(10);
    FFrameTime B(3);
    A -= B;
    if (A.GetFrame().Value != 7)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameTimeSubAssign", Script, "int RunSubAssignBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}
};

#endif
#endif
