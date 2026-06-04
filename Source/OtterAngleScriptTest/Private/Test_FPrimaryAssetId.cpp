// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "UObject/PrimaryAssetId.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFPrimaryAssetIdTests,
	"OtterAngleScript.FPrimaryAssetId",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptPrimAssetIdTest", asGM_ALWAYS_CREATE);
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

	TEST_METHOD(TypeRegistered_PrimaryAssetType)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FPrimaryAssetType")));
	}

	TEST_METHOD(TypeRegistered_PrimaryAssetId)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FPrimaryAssetId")));
	}

	TEST_METHOD(PrimaryAssetTypeDefault)
	{
		static const char Script[] = R"(
int RunPrimaryAssetTypeDefault()
{
    FPrimaryAssetType Type;
    if (Type.IsValid())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("PrimAssetTypeDefault", Script, "int RunPrimaryAssetTypeDefault()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(PrimaryAssetTypeFromString)
	{
		static const char Script[] = R"(
int RunPrimaryAssetTypeFromString()
{
    FPrimaryAssetType Type("MyAssetType");
    if (!Type.IsValid())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("PrimAssetTypeFromStr", Script, "int RunPrimaryAssetTypeFromString()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(PrimaryAssetTypeIsValid)
	{
		static const char Script[] = R"(
int RunPrimaryAssetTypeIsValid()
{
    FPrimaryAssetType Type("Valid");
    if (!Type.IsValid())
    {
        return -1;
    }
    FPrimaryAssetType Empty;
    if (Empty.IsValid())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("PrimAssetTypeIsValid", Script, "int RunPrimaryAssetTypeIsValid()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(PrimaryAssetTypeToString)
	{
		static const char Script[] = R"(
int RunPrimaryAssetTypeToString()
{
    FPrimaryAssetType Type("MyType");
    FString S = Type.ToString();
    if (S != "MyType")
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("PrimAssetTypeToString", Script, "int RunPrimaryAssetTypeToString()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(PrimaryAssetIdDefault)
	{
		static const char Script[] = R"(
int RunPrimaryAssetIdDefault()
{
    FPrimaryAssetId Id;
    if (Id.IsValid())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("PrimAssetIdDefault", Script, "int RunPrimaryAssetIdDefault()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(PrimaryAssetIdFromTypeAndName)
	{
		static const char Script[] = R"(
int RunPrimaryAssetIdFromTypeAndName()
{
    FPrimaryAssetType Type("MyType");
    FPrimaryAssetId Id(Type, FName("MyName"));
    if (!Id.IsValid())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("PrimAssetIdFromTypeName", Script, "int RunPrimaryAssetIdFromTypeAndName()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(PrimaryAssetIdFromString)
	{
		static const char Script[] = R"(
int RunPrimaryAssetIdFromString()
{
    FPrimaryAssetId Id("MyType");
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("PrimAssetIdFromStr", Script, "int RunPrimaryAssetIdFromString()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(PrimaryAssetIdIsValid)
	{
		static const char Script[] = R"(
int RunPrimaryAssetIdIsValid()
{
    FPrimaryAssetType Type("MyType");
    FPrimaryAssetId Id(Type, FName("MyName"));
    if (!Id.IsValid())
    {
        return -1;
    }
    FPrimaryAssetId Empty;
    if (Empty.IsValid())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("PrimAssetIdIsValid", Script, "int RunPrimaryAssetIdIsValid()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(PrimaryAssetIdToString)
	{
		static const char Script[] = R"(
int RunPrimaryAssetIdToString()
{
    FPrimaryAssetType Type("MyType");
    FPrimaryAssetId Id(Type, FName("MyName"));
    FString S = Id.ToString();
    if (S != "MyType MyName")
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("PrimAssetIdToString", Script, "int RunPrimaryAssetIdToString()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(PrimaryAssetIdFromStringStatic)
	{
		static const char Script[] = R"(
int RunPrimaryAssetIdFromStringStatic()
{
    FPrimaryAssetId Id = FPrimaryAssetId::FromString("MyType MyName");
    if (!Id.IsValid())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("PrimAssetIdFromStrStatic", Script, "int RunPrimaryAssetIdFromStringStatic()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}
};

#endif
#endif
