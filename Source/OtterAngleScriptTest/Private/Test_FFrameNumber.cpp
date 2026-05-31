// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Misc/FrameNumber.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFFrameNumberTests,
	"OtterAngleScript.FFrameNumber",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptFrameNumberTest", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FFrameNumber")));
	}

	TEST_METHOD(DefaultConstructor)
	{
		static const char Script[] = R"(
int RunDefaultConstructor()
{
    FFrameNumber F;
    if (F.Value != 0)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameNumberDefaultCtor", Script, "int RunDefaultConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ValueConstructor)
	{
		static const char Script[] = R"(
int RunValueConstructor()
{
    FFrameNumber F(42);
    if (F.Value != 42)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameNumberValueCtor", Script, "int RunValueConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(CopyConstructor)
	{
		static const char Script[] = R"(
int RunCopyConstructor()
{
    FFrameNumber A(10);
    FFrameNumber B(A);
    if (B.Value != 10)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameNumberCopyCtor", Script, "int RunCopyConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(AssignBinding)
	{
		static const char Script[] = R"(
int RunAssignBinding()
{
    FFrameNumber A(5);
    FFrameNumber B;
    B = A;
    if (B.Value != 5)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameNumberAssign", Script, "int RunAssignBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(EqualsBinding)
	{
		static const char Script[] = R"(
int RunEqualsBinding()
{
    FFrameNumber A(7);
    FFrameNumber B(7);
    FFrameNumber C(8);
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
		asIScriptFunction* Function = BuildFunction("FrameNumberEquals", Script, "int RunEqualsBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ComparisonBinding)
	{
		static const char Script[] = R"(
int RunComparisonBinding()
{
    FFrameNumber A(3);
    FFrameNumber B(5);
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
		asIScriptFunction* Function = BuildFunction("FrameNumberComparison", Script, "int RunComparisonBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(AddBinding)
	{
		static const char Script[] = R"(
int RunAddBinding()
{
    FFrameNumber A(3);
    FFrameNumber B(4);
    FFrameNumber C = A + B;
    if (C.Value != 7)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameNumberAdd", Script, "int RunAddBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(SubBinding)
	{
		static const char Script[] = R"(
int RunSubBinding()
{
    FFrameNumber A(10);
    FFrameNumber B(3);
    FFrameNumber C = A - B;
    if (C.Value != 7)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameNumberSub", Script, "int RunSubBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ModBinding)
	{
		static const char Script[] = R"(
int RunModBinding()
{
    FFrameNumber A(10);
    FFrameNumber B(3);
    FFrameNumber C = A % B;
    if (C.Value != 1)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameNumberMod", Script, "int RunModBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(MulFloatBinding)
	{
		static const char Script[] = R"(
int RunMulFloatBinding()
{
    FFrameNumber A(10);
    FFrameNumber B = A * 2.0f;
    if (B.Value != 20)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameNumberMulFloat", Script, "int RunMulFloatBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(DivFloatBinding)
	{
		static const char Script[] = R"(
int RunDivFloatBinding()
{
    FFrameNumber A(10);
    FFrameNumber B = A / 2.0f;
    if (B.Value != 5)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameNumberDivFloat", Script, "int RunDivFloatBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(NegBinding)
	{
		static const char Script[] = R"(
int RunNegBinding()
{
    FFrameNumber A(5);
    FFrameNumber B = -A;
    if (B.Value != -5)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameNumberNeg", Script, "int RunNegBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(AddAssignBinding)
	{
		static const char Script[] = R"(
int RunAddAssignBinding()
{
    FFrameNumber A(3);
    FFrameNumber B(4);
    A += B;
    if (A.Value != 7)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameNumberAddAssign", Script, "int RunAddAssignBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(SubAssignBinding)
	{
		static const char Script[] = R"(
int RunSubAssignBinding()
{
    FFrameNumber A(10);
    FFrameNumber B(3);
    A -= B;
    if (A.Value != 7)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameNumberSubAssign", Script, "int RunSubAssignBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ModAssignBinding)
	{
		static const char Script[] = R"(
int RunModAssignBinding()
{
    FFrameNumber A(10);
    FFrameNumber B(3);
    A %= B;
    if (A.Value != 1)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameNumberModAssign", Script, "int RunModAssignBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(PreIncBinding)
	{
		static const char Script[] = R"(
int RunPreIncBinding()
{
    FFrameNumber A(5);
    FFrameNumber B = ++A;
    if (A.Value != 6 || B.Value != 6)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameNumberPreInc", Script, "int RunPreIncBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(PreDecBinding)
	{
		static const char Script[] = R"(
int RunPreDecBinding()
{
    FFrameNumber A(5);
    FFrameNumber B = --A;
    if (A.Value != 4 || B.Value != 4)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameNumberPreDec", Script, "int RunPreDecBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(PostIncBinding)
	{
		static const char Script[] = R"(
int RunPostIncBinding()
{
    FFrameNumber A(5);
    FFrameNumber B = A++;
    if (A.Value != 6 || B.Value != 5)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameNumberPostInc", Script, "int RunPostIncBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(PostDecBinding)
	{
		static const char Script[] = R"(
int RunPostDecBinding()
{
    FFrameNumber A(5);
    FFrameNumber B = A--;
    if (A.Value != 4 || B.Value != 5)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameNumberPostDec", Script, "int RunPostDecBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ValuePropertyBinding)
	{
		static const char Script[] = R"(
int RunValuePropertyBinding()
{
    FFrameNumber A;
    A.Value = 99;
    if (A.Value != 99)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FrameNumberValueProperty", Script, "int RunValuePropertyBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}
};

#endif
#endif
