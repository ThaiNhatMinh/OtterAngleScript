// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptTSetTests,
	"OtterAngleScript.TSet",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
{
	asIScriptEngine*  Engine = nullptr;
	asIScriptModule*  ScriptModule = nullptr;
	asIScriptContext* Context = nullptr;

	asIScriptFunction* BuildFunction(const char* SectionName, const char* Script, const char* Declaration)
	{
		if (!Assert.IsTrue(ScriptModule->AddScriptSection(SectionName, Script) >= 0))
			return nullptr;
		if (!Assert.IsTrue(ScriptModule->Build() >= 0))
			return nullptr;
		asIScriptFunction* Function = ScriptModule->GetFunctionByDecl(Declaration);
		if (!Assert.IsNotNull(Function))
			return nullptr;
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
			return -1;
		if (!Assert.IsTrue(Context->Prepare(Function) >= 0))
			return -1;
		return Context->Execute();
	}

	int32 ExecuteIntFunction(asIScriptFunction* Function)
	{
		const int Result = ExecuteFunction(Function);
		if (Result == asEXECUTION_EXCEPTION)
			AddError(Context->GetExceptionString());
		if (!Assert.IsTrue(Result == asEXECUTION_FINISHED))
			return -1;
		return static_cast<int32>(Context->GetReturnDWord());
	}

	BEFORE_EACH()
	{
		FOtterAngleScriptModule& Module = FModuleManager::LoadModuleChecked<FOtterAngleScriptModule>("OtterAngleScript");
		Engine = Module.GetScriptEngine();
		ASSERT_THAT(IsNotNull(Engine));

		ScriptModule = Engine->GetModule("OtterAngleScriptTest", asGM_ALWAYS_CREATE);
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

	TEST_METHOD(CreateAndBasicAccess)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("TSet<int>")));

		static const char Script[] = R"(
int RunCreateAndBasicAccess()
{
    TSet<int> set;
    if (!set.isEmpty())
        return -1;
    if (set.length() != 0)
        return -2;

    // Add unique elements
    if (!set.add(10))
        return -3;
    if (!set.add(20))
        return -4;
    if (!set.add(30))
        return -5;
    if (set.length() != 3)
        return -6;

    // Contains
    if (!set.contains(10))
        return -7;
    if (!set.contains(20))
        return -8;
    if (!set.contains(30))
        return -9;
    if (set.contains(99))
        return -10;

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSetCreateBasic", Script, "int RunCreateAndBasicAccess()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(UniquenessEnforced)
	{
		static const char Script[] = R"(
int RunUniquenessEnforced()
{
    TSet<int> set;
    if (!set.add(10))
        return -1;
    if (!set.add(20))
        return -2;
    if (!set.add(30))
        return -3;

    // Adding duplicates should return false and not change size
    if (set.add(10))
        return -4;
    if (set.add(20))
        return -5;
    if (set.add(30))
        return -6;

    if (set.length() != 3)
        return -7;

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSetUniqueness", Script, "int RunUniquenessEnforced()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(RemoveElements)
	{
		static const char Script[] = R"(
int RunRemoveElements()
{
    TSet<int> set;
    set.add(10);
    set.add(20);
    set.add(30);

    if (set.length() != 3)
        return -1;

    // Remove existing element
    if (!set.remove(20))
        return -2;
    if (set.length() != 2)
        return -3;
    if (set.contains(20))
        return -4;

    // Remove non-existing element
    if (set.remove(99))
        return -5;

    // Remove remaining elements
    if (!set.remove(10))
        return -6;
    if (!set.remove(30))
        return -7;
    if (!set.isEmpty())
        return -8;

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSetRemove", Script, "int RunRemoveElements()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(RemoveByIndex)
	{
		static const char Script[] = R"(
int RunRemoveByIndex()
{
    TSet<int> set;
    set.add(10);
    set.add(20);
    set.add(30);

    if (set.length() != 3)
        return -1;

    // Remove by index
    set.removeIndex(1);
    if (set.length() != 2)
        return -2;

    // The set should still contain the remaining two elements
    if (!set.contains(10))
        return -3;
    if (!set.contains(30))
        return -4;

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSetRemoveIndex", Script, "int RunRemoveByIndex()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Clear)
	{
		static const char Script[] = R"(
int RunClear()
{
    TSet<int> set;
    set.add(10);
    set.add(20);
    set.add(30);

    if (set.length() != 3)
        return -1;

    set.clear();
    if (!set.isEmpty())
        return -2;
    if (set.length() != 0)
        return -3;

    // Can add after clear
    if (!set.add(99))
        return -4;
    if (set.length() != 1)
        return -5;

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSetClear", Script, "int RunClear()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(CopyConstructAndAssign)
	{
		static const char Script[] = R"(
int RunCopyConstructAndAssign()
{
    TSet<int> src;
    src.add(10);
    src.add(20);
    src.add(30);

    // Copy construct
    TSet<int> dst = src;
    if (dst.length() != 3)
        return -1;
    if (!dst.contains(10) || !dst.contains(20) || !dst.contains(30))
        return -2;

    // Mutating dst should not affect src
    dst.remove(20);
    if (src.contains(20))
        return -3;
    if (src.length() != 3)
        return -4;

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSetCopy", Script, "int RunCopyConstructAndAssign()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Equality)
	{
		static const char Script[] = R"(
int RunEquality()
{
    TSet<int> a;
    a.add(1);
    a.add(2);
    a.add(3);

    TSet<int> b;
    b.add(1);
    b.add(2);
    b.add(3);

    if (!(a == b))
        return -1;

    b.remove(3);
    if (a == b)
        return -2;

    b.add(4);
    if (a == b)
        return -3;

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSetEquality", Script, "int RunEquality()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Iteration)
	{
		static const char Script[] = R"(
int RunIteration()
{
    TSet<int> set;
    set.add(10);
    set.add(20);
    set.add(30);

    int sum = 0;
    for (int val : set)
    {
        sum += val;
    }

    // Sum of all unique elements
    if (sum != 60)
        return -1;

    // Iterate with index
    uint count = 0;
    for (uint i = 0; i < set.length(); i++)
    {
        if (set.contains(set[i]))
            count++;
    }
    if (count != 3)
        return -2;

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSetIteration", Script, "int RunIteration()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(CapacityConstructor)
	{
		static const char Script[] = R"(
int RunCapacityConstructor()
{
    TSet<int> set(10);
    if (set.length() != 0)
        return -1;

    set.add(1);
    set.add(2);
    if (set.length() != 2)
        return -2;

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSetCapacityCtor", Script, "int RunCapacityConstructor()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ContainsEmpty)
	{
		static const char Script[] = R"(
int RunContainsEmpty()
{
    TSet<int> set;
    if (set.contains(42))
        return -1;
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSetContainsEmpty", Script, "int RunContainsEmpty()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
