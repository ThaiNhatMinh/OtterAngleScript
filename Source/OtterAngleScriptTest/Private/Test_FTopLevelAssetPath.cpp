// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "UObject/TopLevelAssetPath.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFTopLevelAssetPathTests,
	"OtterAngleScript.FTopLevelAssetPath",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptTopAssetPathTest", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FTopLevelAssetPath")));
	}

	TEST_METHOD(DefaultConstruct)
	{
		static const char Script[] = R"(
int RunTest()
{
    FTopLevelAssetPath P;
    if (P.IsValid()) { return -1; }
    if (!P.IsNull()) { return -2; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("DefaultCtor", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(NameConstruct)
	{
		static const char Script[] = R"(
int RunTest()
{
    FTopLevelAssetPath P("/Game/MyAsset", "MyAsset");
    if (!P.IsValid()) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("NameCtor", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(StringConstruct)
	{
		static const char Script[] = R"(
int RunTest()
{
    FTopLevelAssetPath P("/Game/MyAsset.MyAsset");
    if (!P.IsValid()) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("StringCtor", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(IsValid)
	{
		static const char Script[] = R"(
int RunTest()
{
    FTopLevelAssetPath Valid("/Game/Test", "Test");
    FTopLevelAssetPath Invalid;
    if (!Valid.IsValid()) { return -1; }
    if (Invalid.IsValid()) { return -2; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("IsValid", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(IsNull)
	{
		static const char Script[] = R"(
int RunTest()
{
    FTopLevelAssetPath Valid("/Game/Test", "Test");
    FTopLevelAssetPath Invalid;
    if (Valid.IsNull()) { return -1; }
    if (!Invalid.IsNull()) { return -2; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("IsNull", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(Reset)
	{
		static const char Script[] = R"(
int RunTest()
{
    FTopLevelAssetPath P("/Game/Test", "Test");
    if (!P.IsValid()) { return -1; }
    P.Reset();
    if (P.IsValid()) { return -2; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("Reset", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(PackageName)
	{
		static const char Script[] = R"(
int RunTest()
{
    FTopLevelAssetPath P("/Game/MyAsset", "MyAsset");
    FName Pkg = P.PackageName;
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("PackageName", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(AssetName)
	{
		static const char Script[] = R"(
int RunTest()
{
    FTopLevelAssetPath P("/Game/MyAsset", "MyAsset");
    FName Name = P.AssetName;
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("AssetName", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(ToString)
	{
		static const char Script[] = R"(
int RunTest()
{
    FTopLevelAssetPath P("/Game/MyAsset", "MyAsset");
    FString S = P.ToString();
    if (S.IsEmpty()) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("ToString", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(Compare)
	{
		static const char Script[] = R"(
int RunTest()
{
    FTopLevelAssetPath A("/Game/Test", "A");
    FTopLevelAssetPath B("/Game/Test", "A");
    FTopLevelAssetPath C("/Game/Test", "B");
    if (A.Compare(B) != 0) { return -1; }
    if (A.Compare(C) == 0) { return -2; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("Compare", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(opEquals)
	{
		static const char Script[] = R"(
int RunTest()
{
    FTopLevelAssetPath A("/Game/Test", "Test");
    FTopLevelAssetPath B("/Game/Test", "Test");
    FTopLevelAssetPath C("/Game/Other", "Other");
    if (!(A == B)) { return -1; }
    if (A == C) { return -2; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("OpEquals", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
