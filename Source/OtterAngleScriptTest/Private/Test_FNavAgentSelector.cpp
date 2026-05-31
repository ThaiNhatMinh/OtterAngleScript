// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "AI/Navigation/NavAgentSelector.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFNavAgentSelectorTests,
	"OtterAngleScript.FNavAgentSelector",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptNavAgentSelectorTest", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FNavAgentSelector")));
	}

	TEST_METHOD(DefaultConstructBinding)
	{
		static const char Script[] = R"(
int RunDefaultConstruct()
{
    FNavAgentSelector Selector;
    // Default constructor uses AllAgentsMask (0x7fffffff), so ContainsAnyAgent should be true
    if (!Selector.ContainsAnyAgent())
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("NavAgentSelectorDefaultConstruct", Script, "int RunDefaultConstruct()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ConstructWithBitsBinding)
	{
		static const char Script[] = R"(
int RunConstructWithBits()
{
    FNavAgentSelector Selector(0);
    if (Selector.ContainsAnyAgent())
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("NavAgentSelectorConstructWithBits", Script, "int RunConstructWithBits()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ContainsBinding)
	{
		static const char Script[] = R"(
int RunContains()
{
    FNavAgentSelector Selector(0);
    Selector.Set(3);
    if (!Selector.Contains(3))
    {
        return -1;
    }
    if (Selector.Contains(0))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("NavAgentSelectorContains", Script, "int RunContains()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ContainsAnyAgentBinding)
	{
		static const char Script[] = R"(
int RunContainsAnyAgent()
{
    FNavAgentSelector Empty(0);
    if (Empty.ContainsAnyAgent())
    {
        return -1;
    }
    FNavAgentSelector NonEmpty;
    if (!NonEmpty.ContainsAnyAgent())
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("NavAgentSelectorContainsAnyAgent", Script, "int RunContainsAnyAgent()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(SetBinding)
	{
		static const char Script[] = R"(
int RunSet()
{
    FNavAgentSelector Selector(0);
    Selector.Set(5);
    if (!Selector.Contains(5))
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("NavAgentSelectorSet", Script, "int RunSet()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsInitializedBinding)
	{
		static const char Script[] = R"(
int RunIsInitialized()
{
    FNavAgentSelector Selector(0);
    if (Selector.IsInitialized())
    {
        return -1;
    }
    Selector.MarkInitialized();
    if (!Selector.IsInitialized())
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("NavAgentSelectorIsInitialized", Script, "int RunIsInitialized()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(EmptyBinding)
	{
		static const char Script[] = R"(
int RunEmpty()
{
    FNavAgentSelector Selector;
    Selector.Empty();
    if (Selector.ContainsAnyAgent())
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("NavAgentSelectorEmpty", Script, "int RunEmpty()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsSameBinding)
	{
		static const char Script[] = R"(
int RunIsSame()
{
    FNavAgentSelector A(0);
    FNavAgentSelector B(0);
    if (!A.IsSame(B))
    {
        return -1;
    }
    B.Set(1);
    if (A.IsSame(B))
    {
        return -2;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("NavAgentSelectorIsSame", Script, "int RunIsSame()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetAgentBitsBinding)
	{
		static const char Script[] = R"(
int RunGetAgentBits()
{
    FNavAgentSelector Selector(3);
    Selector.MarkInitialized();
    // GetAgentBits strips the InitializedBit, so result should still be 3
    if (Selector.GetAgentBits() != 3)
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("NavAgentSelectorGetAgentBits", Script, "int RunGetAgentBits()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(PackedBitsPropertyBinding)
	{
		static const char Script[] = R"(
int RunPackedBitsProperty()
{
    FNavAgentSelector Selector(0);
    Selector.Set(0);
    Selector.Set(1);
    if (Selector.PackedBits != 3)
    {
        return -1;
    }
    return 0;
}
)";

		asIScriptFunction* Function = BuildFunction("NavAgentSelectorPackedBitsProperty", Script, "int RunPackedBitsProperty()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}
};

#endif
#endif
