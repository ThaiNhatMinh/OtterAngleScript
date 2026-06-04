// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Framework/Commands/InputChord.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFInputChordTests,
	"OtterAngleScript.FInputChord",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptInputChordTest", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FInputChord")));
	}

	TEST_METHOD(DefaultConstruct)
	{
		static const char Script[] = R"(
int RunDefaultConstruct()
{
    FInputChord Chord;
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("InputChordDefaultCtor", Script, "int RunDefaultConstruct()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(KeyConstruct)
	{
		static const char Script[] = R"(
int RunKeyConstruct()
{
    FKey SpaceKey = FKey("SpaceBar");
    FInputChord Chord(SpaceKey);
    if (Chord.get_bShift())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("InputChordKeyCtor", Script, "int RunKeyConstruct()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FullConstruct)
	{
		static const char Script[] = R"(
int RunFullConstruct()
{
    FKey SpaceKey = FKey("SpaceBar");
    FInputChord Chord(SpaceKey, true, true, false, false);
    if (!Chord.get_bShift())
    {
        return -1;
    }
    if (!Chord.get_bCtrl())
    {
        return -2;
    }
    if (Chord.get_bAlt())
    {
        return -3;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("InputChordFullCtor", Script, "int RunFullConstruct()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(opEquals)
	{
		static const char Script[] = R"(
int RunOpEquals()
{
    FKey SpaceKey = FKey("SpaceBar");
    FInputChord A(SpaceKey);
    FInputChord B(SpaceKey);
    FInputChord C;
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
		asIScriptFunction* Function = BuildFunction("InputChordOpEquals", Script, "int RunOpEquals()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetKey)
	{
		static const char Script[] = R"(
int RunGetKey()
{
    FKey SpaceKey = FKey("SpaceBar");
    FInputChord Chord(SpaceKey);
    FKey Result = Chord.get_Key();
    if (Result != SpaceKey)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("InputChordGetKey", Script, "int RunGetKey()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(bShift)
	{
		static const char Script[] = R"(
int RunBShift()
{
    FKey SpaceKey = FKey("SpaceBar");
    FInputChord Chord(SpaceKey, true, false, false, false);
    if (!Chord.get_bShift())
    {
        return -1;
    }
    Chord.set_bShift(false);
    if (Chord.get_bShift())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("InputChordBShift", Script, "int RunBShift()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(NeedsControl)
	{
		static const char Script[] = R"(
int RunNeedsControl()
{
    FKey SpaceKey = FKey("SpaceBar");
    FInputChord Chord(SpaceKey, false, true, false, false);
    if (!Chord.NeedsControl())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("InputChordNeedsCtrl", Script, "int RunNeedsControl()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsValidChord)
	{
		static const char Script[] = R"(
int RunIsValidChord()
{
    FInputChord Default;
    if (Default.IsValidChord())
    {
        return -1;
    }
    FKey SpaceKey = FKey("SpaceBar");
    FInputChord Chord(SpaceKey);
    if (!Chord.IsValidChord())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("InputChordIsValid", Script, "int RunIsValidChord()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ERelationshipType_None)
	{
		static const char Script[] = R"(
int RunERelationshipType_None()
{
    FInputChord A;
    FInputChord B;
    ERelationshipType Rel = A.GetRelationship(B);
    if (Rel != ERelationshipType::None)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("InputChordRelNone", Script, "int RunERelationshipType_None()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}
};

#endif
#endif
