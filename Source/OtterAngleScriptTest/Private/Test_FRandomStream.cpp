// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Math/RandomStream.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFRandomStreamTests,
	"OtterAngleScript.FRandomStream",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptRandomStreamTest", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FRandomStream")));
	}

	TEST_METHOD(DefaultConstruct)
	{
		static const char Script[] = R"(
int RunDefaultConstruct()
{
    FRandomStream Stream;
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("RandomStreamDefaultCtor", Script, "int RunDefaultConstruct()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(SeedConstruct)
	{
		static const char Script[] = R"(
int RunSeedConstruct()
{
    FRandomStream Stream(42);
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("RandomStreamSeedCtor", Script, "int RunSeedConstruct()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(SeedValueMatch)
	{
		static const char Script[] = R"(
int RunSeedValueMatch()
{
    FRandomStream Stream(42);
    int Result1 = Stream.RandRange(0, 100);
    FRandomStream Other(42);
    int Result2 = Other.RandRange(0, 100);
    if (Result1 != Result2)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("RandomStreamSeedMatch", Script, "int RunSeedValueMatch()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetInitialSeed)
	{
		static const char Script[] = R"(
int RunGetInitialSeed()
{
    FRandomStream Stream(42);
    if (Stream.GetInitialSeed() != 42)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("RandomStreamInitSeed", Script, "int RunGetInitialSeed()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetCurrentSeed)
	{
		static const char Script[] = R"(
int RunGetCurrentSeed()
{
    FRandomStream Stream(42);
    int Initial = Stream.GetCurrentSeed();
    Stream.RandRange(0, 100);
    int Current = Stream.GetCurrentSeed();
    if (Current == Initial)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("RandomStreamCurrSeed", Script, "int RunGetCurrentSeed()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FRandRange)
	{
		static const char Script[] = R"(
int RunFRandRange()
{
    FRandomStream Stream(42);
    double Value = Stream.FRandRange(5.0, 10.0);
    if (Value < 5.0 || Value > 10.0)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("RandomStreamFRandRange", Script, "int RunFRandRange()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(RandRange)
	{
		static const char Script[] = R"(
int RunRandRange()
{
    FRandomStream Stream(42);
    int Value = Stream.RandRange(1, 6);
    if (Value < 1 || Value > 6)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("RandomStreamRandRange", Script, "int RunRandRange()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetFraction)
	{
		static const char Script[] = R"(
int RunGetFraction()
{
    FRandomStream Stream(42);
    float Fraction = Stream.GetFraction();
    if (Fraction < 0.0f || Fraction >= 1.0f)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("RandomStreamGetFrac", Script, "int RunGetFraction()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}
};

#endif
#endif
