// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptTArrayTests,
	"OtterAngleScript.TArray",
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("TArray<int>")));

		static const char Script[] = R"(
int RunCreateAndBasicAccess()
{
    TArray<int> arr;
    if (!arr.IsEmpty())
        return -1;
    if (arr.Num() != 0)
        return -2;

    arr.Add(10);
    arr.Add(20);
    arr.Add(30);
    if (arr.Num() != 3)
        return -3;
    if (arr[0] != 10)
        return -4;
    if (arr[1] != 20)
        return -5;
    if (arr[2] != 30)
        return -6;
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TArrayCreateBasic", Script, "int RunCreateAndBasicAccess()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(LengthConstructor)
	{
		static const char Script[] = R"(
int RunLengthConstructor()
{
    TArray<int> arr(5);
    if (arr.Num() != 5)
        return -1;
    for (uint i = 0; i < arr.Num(); i++)
    {
        if (arr[i] != 0)
            return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TArrayLengthCtor", Script, "int RunLengthConstructor()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ListInitializer)
	{
		static const char Script[] = R"(
int RunListInitializer()
{
    TArray<int> arr = {10, 20, 30};
    if (arr.Num() != 3)
        return -1;
    if (arr[0] != 10)
        return -2;
    if (arr[1] != 20)
        return -3;
    if (arr[2] != 30)
        return -4;
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TArrayListInit", Script, "int RunListInitializer()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Assign)
	{
		static const char Script[] = R"(
int RunAssign()
{
    TArray<int> src = {1, 2, 3};
    TArray<int> dst = src;
    if (dst.Num() != 3)
        return -1;
    if (dst[0] != 1 || dst[1] != 2 || dst[2] != 3)
        return -2;

    // Mutating dst must not affect src.
    dst[0] = 99;
    if (src[0] != 1)
        return -3;
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TArrayAssign", Script, "int RunAssign()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(InsertAndRemove)
	{
		static const char Script[] = R"(
int RunInsertAndRemove()
{
    TArray<int> arr = {1, 2, 3};
    arr.Insert(1, 99);
    if (arr.Num() != 4)
        return -1;
    if (arr[1] != 99)
        return -2;

    arr.RemoveAt(1);
    if (arr.Num() != 3)
        return -3;
    if (arr[1] != 2)
        return -4;

    arr.Pop();
    if (arr.Num() != 2)
        return -5;
    if (arr[arr.Num()-1] != 2)
        return -6;

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TArrayInsertRemove", Script, "int RunInsertAndRemove()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(InsertArrayAt)
	{
		static const char Script[] = R"(
int RunInsertArrayAt()
{
    TArray<int> arr = {1, 4};
    TArray<int> mid = {2, 3};
    arr.Insert(1, mid);
    if (arr.Num() != 4)
        return -1;
    if (arr[0] != 1 || arr[1] != 2 || arr[2] != 3 || arr[3] != 4)
        return -2;
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TArrayInsertArrayAt", Script, "int RunInsertArrayAt()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(ReserveAndResize)
	{
		static const char Script[] = R"(
int RunReserveAndResize()
{
    TArray<int> arr;
    arr.Reserve(10);
    if (arr.Num() != 0)
        return -1;

    arr.SetNum(5);
    if (arr.Num() != 5)
        return -2;

    arr.SetNum(2);
    if (arr.Num() != 2)
        return -3;

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TArrayReserveResize", Script, "int RunReserveAndResize()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(RemoveRange)
	{
		static const char Script[] = R"(
int RunRemoveRange()
{
    TArray<int> arr = {1, 2, 3, 4, 5};
    arr.RemoveAt(1, 3);
    if (arr.Num() != 2)
        return -1;
    if (arr[0] != 1 || arr[1] != 5)
        return -2;
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TArrayRemoveRange", Script, "int RunRemoveRange()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(SortAscAndDesc)
	{
		static const char Script[] = R"(
int RunSortAscAndDesc()
{
    TArray<int> arr = {3, 1, 4, 1, 5};
    arr.sortAsc();
    if (arr[0] != 1 || arr[4] != 5)
        return -1;

    arr.sortDesc();
    if (arr[0] != 5 || arr[4] != 1)
        return -2;

    TArray<int> partial = {5, 3, 2, 4, 1};
    partial.sortAsc(1, 3);
    if (partial[0] != 5)
        return -3;
    if (partial[1] != 2 || partial[2] != 3 || partial[3] != 4)
        return -4;
    if (partial[4] != 1)
        return -5;
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TArraySortAscDesc", Script, "int RunSortAscAndDesc()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Reverse)
	{
		static const char Script[] = R"(
int RunReverse()
{
    TArray<int> arr = {1, 2, 3, 4, 5};
    arr.reverse();
    if (arr[0] != 5 || arr[4] != 1)
        return -1;
    if (arr[2] != 3)
        return -2;
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TArrayReverse", Script, "int RunReverse()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(FindByValue)
	{
		static const char Script[] = R"(
int RunFindByValue()
{
    TArray<int> arr = {10, 20, 30, 20};
    if (arr.Find(20) != 1)
        return -1;
    if (arr.Find(2, 20) != 3)
        return -2;
    if (arr.Find(99) != -1)
        return -3;
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TArrayFindByValue", Script, "int RunFindByValue()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(Equals)
	{
		static const char Script[] = R"(
int RunEquals()
{
    TArray<int> a = {1, 2, 3};
    TArray<int> b = {1, 2, 3};
    TArray<int> c = {1, 2, 4};

    if (!(a == b))
        return -1;
    if (a == c)
        return -2;
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TArrayEquals", Script, "int RunEquals()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}

	TEST_METHOD(IndexOutOfBounds)
	{
		static const char Script[] = R"(
int RunIndexOutOfBounds()
{
    TArray<int> arr = {1, 2, 3};
    return arr[10];
}
)";
		asIScriptFunction* Function = BuildFunction("TArrayOutOfBounds", Script, "int RunIndexOutOfBounds()");
		const int Result = ExecuteFunction(Function);
		ASSERT_THAT(IsTrue(Result == asEXECUTION_EXCEPTION));
	}

	TEST_METHOD(SortWithCallback)
	{
		static const char Script[] = R"(
int RunSortWithCallback()
{
    TArray<int> arr = {5, 1, 4, 2, 3};
    arr.Sort(function(a, b) { return a < b; });
    if (arr[0] != 1 || arr[4] != 5)
        return -1;

    arr.Sort(function(a, b) { return a > b; });
    if (arr[0] != 5 || arr[4] != 1)
        return -2;

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TArraySortCallback", Script, "int RunSortWithCallback()");
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function)));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
