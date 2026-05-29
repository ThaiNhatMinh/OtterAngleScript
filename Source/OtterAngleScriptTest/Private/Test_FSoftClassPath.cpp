// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "UObject/SoftObjectPath.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFSoftClassPathTests,
	"OtterAngleScript.FSoftClassPath",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptSoftClassPathTest", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FSoftClassPath")));
	}

	TEST_METHOD(DefaultConstructor)
	{
		static const char Script[] = R"(
int RunDefaultConstructor()
{
    FSoftClassPath Path;
    if (!Path.IsNull())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftClassPathDefaultConstructor", Script, "int RunDefaultConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(CopyConstructor)
	{
		static const char Script[] = R"(
int RunCopyConstructor()
{
    FSoftClassPath A("/Script/Engine.Actor");
    FSoftClassPath B(A);
    if (!A.opEquals(B))
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftClassPathCopyConstructor", Script, "int RunCopyConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(StringConstructor)
	{
		static const char Script[] = R"(
int RunStringConstructor()
{
    FSoftClassPath Path("/Script/Engine.Actor");
    if (Path.IsNull())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftClassPathStringConstructor", Script, "int RunStringConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(AssignOperator)
	{
		static const char Script[] = R"(
int RunAssignOperator()
{
    FSoftClassPath A("/Script/Engine.Actor");
    FSoftClassPath B;
    B = A;
    if (!A.opEquals(B))
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftClassPathAssignOperator", Script, "int RunAssignOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(EqualsOperator)
	{
		static const char Script[] = R"(
int RunEqualsOperator()
{
    FSoftClassPath A("/Script/Engine.Actor");
    FSoftClassPath B("/Script/Engine.Actor");
    FSoftClassPath C("/Script/Engine.Pawn");
    if (!A.opEquals(B))
    {
        return -1;
    }
    if (A.opEquals(C))
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftClassPathEqualsOperator", Script, "int RunEqualsOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsNull)
	{
		static const char Script[] = R"(
int RunIsNull()
{
    FSoftClassPath Path;
    if (!Path.IsNull())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftClassPathIsNull", Script, "int RunIsNull()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsValid)
	{
		static const char Script[] = R"(
int RunIsValid()
{
    FSoftClassPath Invalid;
    if (Invalid.IsValid())
    {
        return -1;
    }
    FSoftClassPath Valid("/Script/Engine.Actor");
    if (!Valid.IsValid())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftClassPathIsValid", Script, "int RunIsValid()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsAsset)
	{
		static const char Script[] = R"(
int RunIsAsset()
{
    FSoftClassPath Path;
    if (Path.IsAsset())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftClassPathIsAsset", Script, "int RunIsAsset()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsSubobject)
	{
		static const char Script[] = R"(
int RunIsSubobject()
{
    FSoftClassPath Path;
    if (Path.IsSubobject())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftClassPathIsSubobject", Script, "int RunIsSubobject()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ToString)
	{
		static const char Script[] = R"(
int RunToString()
{
    FSoftClassPath Path;
    if (Path.ToString() != "")
    {
        return -1;
    }
    FSoftClassPath Valid("/Script/Engine.Actor");
    if (Valid.ToString() == "")
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftClassPathToString", Script, "int RunToString()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetAssetPathString)
	{
		static const char Script[] = R"(
int RunGetAssetPathString()
{
    FSoftClassPath Path;
    FString AssetPath = Path.GetAssetPathString();
    if (AssetPath != "")
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftClassPathGetAssetPathString", Script, "int RunGetAssetPathString()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetLongPackageName)
	{
		static const char Script[] = R"(
int RunGetLongPackageName()
{
    FSoftClassPath Path("/Script/Engine.Actor");
    FString PackageName = Path.GetLongPackageName();
    if (PackageName == "")
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftClassPathGetLongPackageName", Script, "int RunGetLongPackageName()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetAssetName)
	{
		static const char Script[] = R"(
int RunGetAssetName()
{
    FSoftClassPath Path("/Script/Engine.Actor");
    FString AssetName = Path.GetAssetName();
    if (AssetName != "Actor")
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftClassPathGetAssetName", Script, "int RunGetAssetName()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(SetPath)
	{
		static const char Script[] = R"(
int RunSetPath()
{
    FSoftClassPath Path;
    Path.SetPath("/Script/Engine.Actor");
    if (Path.IsNull())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftClassPathSetPath", Script, "int RunSetPath()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(Reset)
	{
		static const char Script[] = R"(
int RunReset()
{
    FSoftClassPath Path("/Script/Engine.Actor");
    if (Path.IsNull())
    {
        return -1;
    }
    Path.Reset();
    if (!Path.IsNull())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftClassPathReset", Script, "int RunReset()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ResolveClass)
	{
		static const char Script[] = R"(
int RunResolveClass()
{
    FSoftClassPath Path;
    UClass ResolvedClass = Path.ResolveClass();
    if (ResolvedClass !is null)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftClassPathResolveClass", Script, "int RunResolveClass()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(TryLoadClass)
	{
		static const char Script[] = R"(
int RunTryLoadClass()
{
    FSoftClassPath Path;
    UClass LoadedClass = Path.TryLoadClass();
    if (LoadedClass !is null)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftClassPathTryLoadClass", Script, "int RunTryLoadClass()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}
};

#endif
#endif
