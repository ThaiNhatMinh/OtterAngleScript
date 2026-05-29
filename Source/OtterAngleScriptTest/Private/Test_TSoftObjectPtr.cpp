// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "UObject/SoftObjectPtr.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptTSoftObjectPtrTests,
	"OtterAngleScript.TSoftObjectPtr",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptTSoftObjectPtrTest", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("TSoftObjectPtr<UObject>")));
	}

	TEST_METHOD(DefaultConstructor)
	{
		static const char Script[] = R"(
int RunDefaultConstructor()
{
    TSoftObjectPtr<UObject> Ptr;
    if (!Ptr.IsNull())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftObjectPtrDefaultConstructor", Script, "int RunDefaultConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(CopyConstructor)
	{
		static const char Script[] = R"(
int RunCopyConstructor()
{
    TSoftObjectPtr<UObject> A;
    TSoftObjectPtr<UObject> B(A);
    if (!A.opEquals(B))
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftObjectPtrCopyConstructor", Script, "int RunCopyConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ConstructFromPath)
	{
		static const char Script[] = R"(
int RunConstructFromPath()
{
    FSoftObjectPath Path("/Game/MyAsset.MyAsset");
    TSoftObjectPtr<UObject> Ptr(Path);
    if (Ptr.IsNull())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftObjectPtrConstructFromPath", Script, "int RunConstructFromPath()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(AssignOperator)
	{
		static const char Script[] = R"(
int RunAssignOperator()
{
    FSoftObjectPath Path("/Game/MyAsset.MyAsset");
    TSoftObjectPtr<UObject> A(Path);
    TSoftObjectPtr<UObject> B;
    B = A;
    if (!A.opEquals(B))
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftObjectPtrAssignOperator", Script, "int RunAssignOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(AssignFromPath)
	{
		static const char Script[] = R"(
int RunAssignFromPath()
{
    TSoftObjectPtr<UObject> Ptr;
    FSoftObjectPath Path("/Game/MyAsset.MyAsset");
    Ptr = Path;
    if (Ptr.IsNull())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftObjectPtrAssignFromPath", Script, "int RunAssignFromPath()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(EqualsOperator)
	{
		static const char Script[] = R"(
int RunEqualsOperator()
{
    FSoftObjectPath PathA("/Game/AssetA.AssetA");
    FSoftObjectPath PathB("/Game/AssetB.AssetB");
    TSoftObjectPtr<UObject> A(PathA);
    TSoftObjectPtr<UObject> B(PathA);
    TSoftObjectPtr<UObject> C(PathB);
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
		asIScriptFunction* Function = BuildFunction("TSoftObjectPtrEqualsOperator", Script, "int RunEqualsOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsNull)
	{
		static const char Script[] = R"(
int RunIsNull()
{
    TSoftObjectPtr<UObject> Ptr;
    if (!Ptr.IsNull())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftObjectPtrIsNull", Script, "int RunIsNull()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsValid)
	{
		static const char Script[] = R"(
int RunIsValid()
{
    TSoftObjectPtr<UObject> Invalid;
    if (Invalid.IsValid())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftObjectPtrIsValid", Script, "int RunIsValid()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsPending)
	{
		static const char Script[] = R"(
int RunIsPending()
{
    TSoftObjectPtr<UObject> Ptr;
    if (Ptr.IsPending())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftObjectPtrIsPending", Script, "int RunIsPending()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(Reset)
	{
		static const char Script[] = R"(
int RunReset()
{
    FSoftObjectPath Path("/Game/MyAsset.MyAsset");
    TSoftObjectPtr<UObject> Ptr(Path);
    if (Ptr.IsNull())
    {
        return -1;
    }
    Ptr.Reset();
    if (!Ptr.IsNull())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftObjectPtrReset", Script, "int RunReset()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(Get)
	{
		static const char Script[] = R"(
int RunGet()
{
    TSoftObjectPtr<UObject> Ptr;
    UObject@ Obj = Ptr.Get();
    if (Obj !is null)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftObjectPtrGet", Script, "int RunGet()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ToString)
	{
		static const char Script[] = R"(
int RunToString()
{
    TSoftObjectPtr<UObject> Empty;
    if (Empty.ToString() != "")
    {
        return -1;
    }
    FSoftObjectPath Path("/Game/MyAsset.MyAsset");
    TSoftObjectPtr<UObject> Ptr(Path);
    if (Ptr.ToString() == "")
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftObjectPtrToString", Script, "int RunToString()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetAssetName)
	{
		static const char Script[] = R"(
int RunGetAssetName()
{
    FSoftObjectPath Path("/Game/MyAsset.MyAsset");
    TSoftObjectPtr<UObject> Ptr(Path);
    if (Ptr.GetAssetName() != "MyAsset")
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftObjectPtrGetAssetName", Script, "int RunGetAssetName()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ToSoftObjectPath)
	{
		static const char Script[] = R"(
int RunToSoftObjectPath()
{
    FSoftObjectPath Path("/Game/MyAsset.MyAsset");
    TSoftObjectPtr<UObject> Ptr(Path);
    FSoftObjectPath Retrieved = Ptr.ToSoftObjectPath();
    if (!Path.opEquals(Retrieved))
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftObjectPtrToSoftObjectPath", Script, "int RunToSoftObjectPath()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}
};

#endif
#endif
