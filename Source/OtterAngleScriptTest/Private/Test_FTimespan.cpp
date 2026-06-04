// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Misc/Timespan.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFTimespanTests,
	"OtterAngleScript.FTimespan",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptTimespanTest", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FTimespan")));
	}

	TEST_METHOD(DefaultConstruct)
	{
		static const char Script[] = R"(
int RunTest()
{
    FTimespan T;
    if (!T.IsZero()) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("DefaultCtor", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(TicksConstruct)
	{
		static const char Script[] = R"(
int RunTest()
{
    // TicksPerDay = 864000000000
    FTimespan T(int64(864000000000));
    if (T.GetDays() != 1) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("TicksCtor", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(HMSConstruct)
	{
		static const char Script[] = R"(
int RunTest()
{
    FTimespan T(2, 30, 0);
    if (T.GetHours() != 2 || T.GetMinutes() != 30 || T.GetSeconds() != 0) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("HMSCtor", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(GetDays)
	{
		static const char Script[] = R"(
int RunTest()
{
    FTimespan T(3, 0, 0, 0);
    if (T.GetDays() != 3) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("GetDays", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(GetHours)
	{
		static const char Script[] = R"(
int RunTest()
{
    FTimespan T(0, 5, 0, 0);
    if (T.GetHours() != 5) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("GetHours", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(GetMinutes)
	{
		static const char Script[] = R"(
int RunTest()
{
    FTimespan T(0, 0, 15, 0);
    if (T.GetMinutes() != 15) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("GetMinutes", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(GetSeconds)
	{
		static const char Script[] = R"(
int RunTest()
{
    FTimespan T(0, 0, 0, 45);
    if (T.GetSeconds() != 45) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("GetSeconds", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(GetTicks)
	{
		static const char Script[] = R"(
int RunTest()
{
    // TicksPerMinute = 600000000
    FTimespan T(0, 0, 1, 0);
    if (T.GetTicks() != int64(600000000)) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("GetTicks", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(GetTotalSeconds)
	{
		static const char Script[] = R"(
int RunTest()
{
    FTimespan T(0, 0, 1, 30);
    if (T.GetTotalSeconds() != 90.0) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("GetTotalSeconds", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(IsZero)
	{
		static const char Script[] = R"(
int RunTest()
{
    FTimespan Zero;
    FTimespan NonZero(0, 0, 5, 0);
    if (!Zero.IsZero()) { return -1; }
    if (NonZero.IsZero()) { return -2; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("IsZero", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(ToString)
	{
		static const char Script[] = R"(
int RunTest()
{
    FTimespan T(1, 2, 30, 0);
    FString S = T.ToString();
    if (S.IsEmpty()) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("ToString", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(opEquals)
	{
		static const char Script[] = R"(
int RunTest()
{
    FTimespan A(0, 0, 5, 0);
    FTimespan B(0, 0, 5, 0);
    FTimespan C(0, 0, 10, 0);
    if (!(A == B)) { return -1; }
    if (A == C) { return -2; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("OpEquals", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(opCmp)
	{
		static const char Script[] = R"(
int RunTest()
{
    FTimespan A(0, 0, 5, 0);
    FTimespan B(0, 0, 10, 0);
    if (!(A < B)) { return -1; }
    if (!(B > A)) { return -2; }
    if (!(A <= A)) { return -3; }
    if (!(B >= B)) { return -4; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("OpCmp", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(opAdd)
	{
		static const char Script[] = R"(
int RunTest()
{
    FTimespan A(0, 0, 5, 0);
    FTimespan B(0, 0, 10, 0);
    FTimespan C = A + B;
    if (C.GetMinutes() != 15) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("OpAdd", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(opSub)
	{
		static const char Script[] = R"(
int RunTest()
{
    FTimespan A(0, 0, 10, 0);
    FTimespan B(0, 0, 3, 0);
    FTimespan C = A - B;
    if (C.GetMinutes() != 7) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("OpSub", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(opNeg)
	{
		static const char Script[] = R"(
int RunTest()
{
    FTimespan A(0, 0, 5, 0);
    FTimespan B = -A;
    if (B.GetMinutes() != -5) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("OpNeg", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(static_FromSeconds)
	{
		static const char Script[] = R"(
int RunTest()
{
    FTimespan T = FTimespan::FromSeconds(120.0);
    if (T.GetMinutes() != 2) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("FromSeconds", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(static_MaxValue)
	{
		static const char Script[] = R"(
int RunTest()
{
    FTimespan T = FTimespan::MaxValue();
    if (T.IsZero()) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("MaxValue", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(static_MinValue)
	{
		static const char Script[] = R"(
int RunTest()
{
    FTimespan T = FTimespan::MinValue();
    if (T.IsZero()) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("MinValue", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
