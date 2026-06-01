// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptTMapTests,
	"OtterAngleScript.TMap",
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("TMap<int, int>")));

		static const char Script[] = R"(
int RunCreateAndBasicAccess()
{
    TMap<int, int> map;
    if (!map.isEmpty())
        return -1;
    if (map.length() != 0)
        return -2;

    // Insert entries
    map.set(1, 100);
    map.set(2, 200);
    map.set(3, 300);

    if (map.length() != 3)
        return -3;

    // Check contains
    if (!map.contains(1))
        return -4;
    if (!map.contains(2))
        return -5;
    if (!map.contains(3))
        return -6;
    if (map.contains(99))
        return -7;

    // Access by key
    if (map[1] != 100)
        return -8;
    if (map[2] != 200)
        return -9;
    if (map[3] != 300)
        return -10;

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TMapCreateBasic", Script, "int RunCreateAndBasicAccess()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(UpdateExistingKey)
	{
		static const char Script[] = R"(
int RunUpdateExistingKey()
{
    TMap<int, int> map;
    map.set(1, 100);
    map.set(2, 200);

    // Update existing key
    map.set(1, 999);
    if (map.length() != 2)
        return -1;
    if (map[1] != 999)
        return -2;

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TMapUpdate", Script, "int RunUpdateExistingKey()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Remove)
	{
		static const char Script[] = R"(
int RunRemove()
{
    TMap<int, int> map;
    map.set(1, 100);
    map.set(2, 200);
    map.set(3, 300);

    // Remove existing key
    if (!map.remove(2))
        return -1;
    if (map.length() != 2)
        return -2;
    if (map.contains(2))
        return -3;

    // Remove non-existing key
    if (map.remove(99))
        return -4;

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TMapRemove", Script, "int RunRemove()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Clear)
	{
		static const char Script[] = R"(
int RunClear()
{
    TMap<int, int> map;
    map.set(1, 100);
    map.set(2, 200);

    map.clear();
    if (!map.isEmpty())
        return -1;
    if (map.length() != 0)
        return -2;
    if (map.contains(1))
        return -3;

    // Can add after clear
    map.set(3, 300);
    if (map.length() != 1)
        return -4;
    if (map[3] != 300)
        return -5;

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TMapClear", Script, "int RunClear()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(CopyConstructAndAssign)
	{
		static const char Script[] = R"(
int RunCopyConstructAndAssign()
{
    TMap<int, int> src;
    src.set(1, 100);
    src.set(2, 200);

    // Copy construct
    TMap<int, int> dst = src;
    if (dst.length() != 2)
        return -1;
    if (dst[1] != 100 || dst[2] != 200)
        return -2;

    // Mutating dst should not affect src
    dst.set(1, 999);
    if (src[1] != 100)
        return -3;

    // Assignment
    TMap<int, int> assigned;
    assigned = src;
    if (assigned.length() != 2)
        return -4;
    if (assigned[1] != 100 || assigned[2] != 200)
        return -5;

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TMapCopy", Script, "int RunCopyConstructAndAssign()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(AccessByIndex)
	{
		static const char Script[] = R"(
int RunAccessByIndex()
{
    TMap<int, int> map;
    map.set(10, 100);
    map.set(20, 200);
    map.set(30, 300);

    // Access by position
    bool found10 = false, found20 = false, found30 = false;
    for (uint i = 0; i < map.length(); i++)
    {
        int k = map.getKey(i);
        int v = map.getValue(i);

        if (k == 10 && v == 100) found10 = true;
        if (k == 20 && v == 200) found20 = true;
        if (k == 30 && v == 300) found30 = true;
    }

    if (!found10 || !found20 || !found30)
        return -1;

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TMapIndexAccess", Script, "int RunAccessByIndex()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ForRangeIteration)
	{
		static const char Script[] = R"(
int RunForRangeIteration()
{
    TMap<int, int> map;
    map.set(1, 10);
    map.set(2, 20);
    map.set(3, 30);

    // For-range iterates over keys
    int sum = 0;
    for (int key : map)
    {
        sum += key;
    }
    if (sum != 6)
        return -1;

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TMapForRange", Script, "int RunForRangeIteration()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(RemoveByIndex)
	{
		static const char Script[] = R"(
int RunRemoveByIndex()
{
    TMap<int, int> map;
    map.set(1, 100);
    map.set(2, 200);
    map.set(3, 300);

    if (map.length() != 3)
        return -1;

    map.removeIndex(1);
    if (map.length() != 2)
        return -2;

    // The remaining entries should still be accessible
    if (!map.contains(1) && !map.contains(3))
        return -3;

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TMapRemoveIndex", Script, "int RunRemoveByIndex()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(CapacityConstructor)
	{
		static const char Script[] = R"(
int RunCapacityConstructor()
{
    TMap<int, int> map(10);
    if (map.length() != 0)
        return -1;

    map.set(1, 100);
    if (map.length() != 1)
        return -2;
    if (map[1] != 100)
        return -3;

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TMapCapacityCtor", Script, "int RunCapacityConstructor()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(FStringKey)
	{
		static const char Script[] = R"(
int RunFStringKey()
{
    TMap<FString, int> map;
    map.set("one", 1);
    map.set("two", 2);
    map.set("three", 3);

    if (map.length() != 3)
        return -1;

    if (!map.contains("one"))
        return -2;
    if (!map.contains("two"))
        return -3;
    if (!map.contains("three"))
        return -4;

    if (map["one"] != 1)
        return -5;
    if (map["two"] != 2)
        return -6;
    if (map["three"] != 3)
        return -7;

    // Update existing string key
    map.set("two", 22);
    if (map["two"] != 22)
        return -8;

    // Remove by string key
    map.remove("three");
    if (map.length() != 2)
        return -9;
    if (map.contains("three"))
        return -10;

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TMapFStringKey", Script, "int RunFStringKey()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
