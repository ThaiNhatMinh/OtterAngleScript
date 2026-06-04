// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Misc/DateTime.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFDateTimeTests,
	"OtterAngleScript.FDateTime",
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

	int64 ExecuteInt64Function(asIScriptFunction* Function)
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

		return Context->GetReturnQWord();
	}

	BEFORE_EACH()
	{
		FOtterAngleScriptModule& Module = FModuleManager::LoadModuleChecked<FOtterAngleScriptModule>("OtterAngleScript");
		Engine = Module.GetScriptEngine();
		ASSERT_THAT(IsNotNull(Engine));

		ScriptModule = Engine->GetModule("OtterAngleScriptDateTimeTest", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FDateTime")));
	}

	TEST_METHOD(DefaultConstruct)
	{
		static const char Script[] = R"(
int64 RunDefaultConstruct()
{
    FDateTime Dt;
    return Dt.GetTicks();
}
)";
		asIScriptFunction* Function = BuildFunction("FDateTimeDefaultCtor", Script, "int64 RunDefaultConstruct()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteInt64Function(Function) == 0));
	}

	TEST_METHOD(TicksConstruct)
	{
		static const char Script[] = R"(
int RunTicksConstruct()
{
    FDateTime Dt(1000000);
    if (Dt.GetTicks() != 1000000)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FDateTimeTicksCtor", Script, "int RunTicksConstruct()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(YMDHMSConstruct)
	{
		static const char Script[] = R"(
int RunYMDHMSConstruct()
{
    FDateTime Dt(2024, 1, 15, 10, 30, 45, 0);
    if (Dt.GetYear() != 2024)
    {
        return -1;
    }
    if (Dt.GetMonth() != 1)
    {
        return -2;
    }
    if (Dt.GetDay() != 15)
    {
        return -3;
    }
    if (Dt.GetHour() != 10)
    {
        return -4;
    }
    if (Dt.GetMinute() != 30)
    {
        return -5;
    }
    if (Dt.GetSecond() != 45)
    {
        return -6;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FDateTimeYMDHMSCtor", Script, "int RunYMDHMSConstruct()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetYear)
	{
		static const char Script[] = R"(
int RunGetYear()
{
    FDateTime Dt(2023, 6, 15);
    if (Dt.GetYear() != 2023)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FDateTimeGetYear", Script, "int RunGetYear()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetMonth)
	{
		static const char Script[] = R"(
int RunGetMonth()
{
    FDateTime Dt(2023, 6, 15);
    if (Dt.GetMonth() != 6)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FDateTimeGetMonth", Script, "int RunGetMonth()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetDay)
	{
		static const char Script[] = R"(
int RunGetDay()
{
    FDateTime Dt(2023, 6, 15);
    if (Dt.GetDay() != 15)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FDateTimeGetDay", Script, "int RunGetDay()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetHour)
	{
		static const char Script[] = R"(
int RunGetHour()
{
    FDateTime Dt(2023, 6, 15, 14, 30, 0);
    if (Dt.GetHour() != 14)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FDateTimeGetHour", Script, "int RunGetHour()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetMinute)
	{
		static const char Script[] = R"(
int RunGetMinute()
{
    FDateTime Dt(2023, 6, 15, 14, 30, 0);
    if (Dt.GetMinute() != 30)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FDateTimeGetMinute", Script, "int RunGetMinute()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetSecond)
	{
		static const char Script[] = R"(
int RunGetSecond()
{
    FDateTime Dt(2023, 6, 15, 14, 30, 45);
    if (Dt.GetSecond() != 45)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FDateTimeGetSecond", Script, "int RunGetSecond()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetTicks)
	{
		static const char Script[] = R"(
int64 RunGetTicks()
{
    FDateTime Dt(5000000);
    return Dt.GetTicks();
}
)";
		asIScriptFunction* Function = BuildFunction("FDateTimeGetTicks", Script, "int64 RunGetTicks()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteInt64Function(Function) == 5000000));
	}

	TEST_METHOD(IsMorning)
	{
		static const char Script[] = R"(
int RunIsMorning()
{
    FDateTime Morning(2023, 6, 15, 8, 0, 0);
    if (!Morning.IsMorning())
    {
        return -1;
    }
    FDateTime Afternoon(2023, 6, 15, 14, 0, 0);
    if (Afternoon.IsMorning())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FDateTimeIsMorning", Script, "int RunIsMorning()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ToIso8601)
	{
		static const char Script[] = R"(
int RunToIso8601()
{
    FDateTime Dt(2023, 6, 15, 10, 30, 0);
    FString Str = Dt.ToIso8601();
    if (Str.IsEmpty())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FDateTimeToIso8601", Script, "int RunToIso8601()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ToString)
	{
		static const char Script[] = R"(
int RunToString()
{
    FDateTime Dt(2023, 6, 15, 10, 30, 0);
    FString Str = Dt.ToString();
    if (Str.IsEmpty())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FDateTimeToString", Script, "int RunToString()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ToUnixTimestamp)
	{
		static const char Script[] = R"(
int64 RunToUnixTimestamp()
{
    FDateTime Dt(2023, 6, 15, 10, 30, 0);
    return Dt.ToUnixTimestamp();
}
)";
		asIScriptFunction* Function = BuildFunction("FDateTimeToUnixTimestamp", Script, "int64 RunToUnixTimestamp()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteInt64Function(Function) > 0));
	}

	TEST_METHOD(opEquals)
	{
		static const char Script[] = R"(
int RunOpEquals()
{
    FDateTime A(2023, 6, 15);
    FDateTime B(2023, 6, 15);
    FDateTime C(2024, 1, 1);
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
		asIScriptFunction* Function = BuildFunction("FDateTimeOpEquals", Script, "int RunOpEquals()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(opCmp)
	{
		static const char Script[] = R"(
int RunOpCmp()
{
    FDateTime Early(2023, 6, 15);
    FDateTime Late(2024, 1, 1);
    if (!(Early < Late))
    {
        return -1;
    }
    if (!(Late > Early))
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FDateTimeOpCmp", Script, "int RunOpCmp()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(static_Now)
	{
		static const char Script[] = R"(
int RunStaticNow()
{
    FDateTime Now = FDateTime.Now();
    if (Now.GetTicks() <= 0)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FDateTimeStaticNow", Script, "int RunStaticNow()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(static_Today)
	{
		static const char Script[] = R"(
int RunStaticToday()
{
    FDateTime Today = FDateTime.Today();
    if (Today.GetTicks() <= 0)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FDateTimeStaticToday", Script, "int RunStaticToday()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(static_UtcNow)
	{
		static const char Script[] = R"(
int RunStaticUtcNow()
{
    FDateTime UtcNow = FDateTime.UtcNow();
    if (UtcNow.GetTicks() <= 0)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FDateTimeStaticUtcNow", Script, "int RunStaticUtcNow()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(static_MaxValue)
	{
		static const char Script[] = R"(
int64 RunStaticMaxValue()
{
    FDateTime Max = FDateTime.MaxValue();
    return Max.GetTicks();
}
)";
		asIScriptFunction* Function = BuildFunction("FDateTimeStaticMaxValue", Script, "int64 RunStaticMaxValue()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteInt64Function(Function) > 0));
	}

	TEST_METHOD(static_MinValue)
	{
		static const char Script[] = R"(
int64 RunStaticMinValue()
{
    FDateTime Min = FDateTime.MinValue();
    return Min.GetTicks();
}
)";
		asIScriptFunction* Function = BuildFunction("FDateTimeStaticMinValue", Script, "int64 RunStaticMinValue()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteInt64Function(Function) == 0));
	}

	TEST_METHOD(static_DaysInMonth)
	{
		static const char Script[] = R"(
int RunStaticDaysInMonth()
{
    int Days = FDateTime.DaysInMonth(2024, 2);
    if (Days != 29)
    {
        return -1;
    }
    Days = FDateTime.DaysInMonth(2023, 2);
    if (Days != 28)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FDateTimeStaticDaysInMonth", Script, "int RunStaticDaysInMonth()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(static_IsLeapYear)
	{
		static const char Script[] = R"(
int RunStaticIsLeapYear()
{
    if (!FDateTime.IsLeapYear(2024))
    {
        return -1;
    }
    if (FDateTime.IsLeapYear(2023))
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FDateTimeStaticIsLeapYear", Script, "int RunStaticIsLeapYear()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}
};

#endif
#endif
