// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "UObject/SoftObjectPath.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFSoftObjectPathTests,
	"OtterAngleScript.FSoftObjectPath",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptSoftObjectPathTest", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FSoftObjectPath")));
	}

	TEST_METHOD(DefaultConstructor)
	{
		static const char Script[] = R"(
int RunDefaultConstructor()
{
    FSoftObjectPath Path;
    if (!Path.IsNull())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftObjectPathDefaultConstructor", Script, "int RunDefaultConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(StringConstructor)
	{
		static const char Script[] = R"(
int RunStringConstructor()
{
    FSoftObjectPath Path("/Script/Engine.Actor");
    if (Path.IsNull())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftObjectPathStringConstructor", Script, "int RunStringConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(CopyConstructor)
	{
		static const char Script[] = R"(
int RunCopyConstructor()
{
    FSoftObjectPath A("/Script/Engine.Actor");
    FSoftObjectPath B(A);
    if (!A.opEquals(B))
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftObjectPathCopyConstructor", Script, "int RunCopyConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(AssignOperator)
	{
		static const char Script[] = R"(
int RunAssignOperator()
{
    FSoftObjectPath A("/Script/Engine.Actor");
    FSoftObjectPath B;
    B = A;
    if (!A.opEquals(B))
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftObjectPathAssignOperator", Script, "int RunAssignOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(AssignFromString)
	{
		static const char Script[] = R"(
int RunAssignFromString()
{
    FSoftObjectPath Path;
    Path = "/Script/Engine.Actor";
    if (Path.IsNull())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftObjectPathAssignFromString", Script, "int RunAssignFromString()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(EqualsOperator)
	{
		static const char Script[] = R"(
int RunEqualsOperator()
{
    FSoftObjectPath A("/Script/Engine.Actor");
    FSoftObjectPath B("/Script/Engine.Actor");
    FSoftObjectPath C("/Script/Engine.Pawn");
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
		asIScriptFunction* Function = BuildFunction("SoftObjectPathEqualsOperator", Script, "int RunEqualsOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsNull)
	{
		static const char Script[] = R"(
int RunIsNull()
{
    FSoftObjectPath Path;
    if (!Path.IsNull())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftObjectPathIsNull", Script, "int RunIsNull()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsValid)
	{
		static const char Script[] = R"(
int RunIsValid()
{
    FSoftObjectPath Invalid;
    if (Invalid.IsValid())
    {
        return -1;
    }
    FSoftObjectPath Valid("/Script/Engine.Actor");
    if (!Valid.IsValid())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftObjectPathIsValid", Script, "int RunIsValid()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsAsset)
	{
		static const char Script[] = R"(
int RunIsAsset()
{
    FSoftObjectPath Path;
    if (Path.IsAsset())
    {
        return -1;
    }
    FSoftObjectPath Asset("/Game/MyAsset.MyAsset");
    if (!Asset.IsAsset())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftObjectPathIsAsset", Script, "int RunIsAsset()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsSubobject)
	{
		static const char Script[] = R"(
int RunIsSubobject()
{
    FSoftObjectPath Path;
    if (Path.IsSubobject())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftObjectPathIsSubobject", Script, "int RunIsSubobject()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ToString)
	{
		static const char Script[] = R"(
int RunToString()
{
    FSoftObjectPath Path;
    if (Path.ToString() != "")
    {
        return -1;
    }
    FSoftObjectPath Valid("/Script/Engine.Actor");
    if (Valid.ToString() == "")
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftObjectPathToString", Script, "int RunToString()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetAssetName)
	{
		static const char Script[] = R"(
int RunGetAssetName()
{
    FSoftObjectPath Path("/Script/Engine.Actor");
    if (Path.GetAssetName() != "Actor")
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftObjectPathGetAssetName", Script, "int RunGetAssetName()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetLongPackageName)
	{
		static const char Script[] = R"(
int RunGetLongPackageName()
{
    FSoftObjectPath Path("/Script/Engine.Actor");
    if (Path.GetLongPackageName() == "")
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftObjectPathGetLongPackageName", Script, "int RunGetLongPackageName()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(SetPath)
	{
		static const char Script[] = R"(
int RunSetPath()
{
    FSoftObjectPath Path;
    Path.SetPath("/Script/Engine.Actor");
    if (Path.IsNull())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftObjectPathSetPath", Script, "int RunSetPath()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(Reset)
	{
		static const char Script[] = R"(
int RunReset()
{
    FSoftObjectPath Path("/Script/Engine.Actor");
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
		asIScriptFunction* Function = BuildFunction("SoftObjectPathReset", Script, "int RunReset()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(LexicalLess)
	{
		static const char Script[] = R"(
int RunLexicalLess()
{
    FSoftObjectPath A("/Script/Engine.Actor");
    FSoftObjectPath B("/Script/Engine.Pawn");
    if (!A.LexicalLess(B))
    {
        return -1;
    }
    if (B.LexicalLess(A))
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftObjectPathLexicalLess", Script, "int RunLexicalLess()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ResolveObject)
	{
		static const char Script[] = R"(
int RunResolveObject()
{
    FSoftObjectPath Path;
    UObject Resolved = Path.ResolveObject();
    if (Resolved !is null)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftObjectPathResolveObject", Script, "int RunResolveObject()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}
};

#endif
#endif
