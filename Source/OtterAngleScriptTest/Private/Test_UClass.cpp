// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "UObject/Class.h"
#include "angelscript.h"

namespace
{
	UClass* GetFixtureUObjectClass()
	{
		return UObject::StaticClass();
	}
}

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptUClassTests,
	"OtterAngleScript.UClass",
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

		static bool bRegistered = false;
		if (!bRegistered)
		{
			int Result = Engine->RegisterGlobalFunction("UClass GetFixtureUObjectClass()", asFUNCTION(GetFixtureUObjectClass), asCALL_CDECL);
			check(Result >= 0);
			bRegistered = true;
		}

		ScriptModule = Engine->GetModule("OtterAngleScriptUClassTest", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("UClass")));
	}

	TEST_METHOD(GetFNameBinding)
	{
		static const char Script[] = R"(
int RunGetFName()
{
    UClass Cls = GetFixtureUObjectClass();
    if (Cls is null)
    {
        return -1;
    }
    FName Fn = Cls.GetFName();
    if (Fn.IsNone())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("UClassGetFName", Script, "int RunGetFName()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetNameBinding)
	{
		static const char Script[] = R"(
int RunGetName()
{
    UClass Cls = GetFixtureUObjectClass();
    if (Cls is null)
    {
        return -1;
    }
    FString Str = Cls.GetName();
    if (Str.IsEmpty())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("UClassGetName", Script, "int RunGetName()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetPathNameBinding)
	{
		static const char Script[] = R"(
int RunGetPathName()
{
    UClass Cls = GetFixtureUObjectClass();
    if (Cls is null)
    {
        return -1;
    }
    FString Str = Cls.GetPathName();
    if (Str.IsEmpty())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("UClassGetPathName", Script, "int RunGetPathName()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetFullNameBinding)
	{
		static const char Script[] = R"(
int RunGetFullName()
{
    UClass Cls = GetFixtureUObjectClass();
    if (Cls is null)
    {
        return -1;
    }
    FString Str = Cls.GetFullName();
    if (Str.IsEmpty())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("UClassGetFullName", Script, "int RunGetFullName()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetSuperClassBinding)
	{
		static const char Script[] = R"(
int RunGetSuperClass()
{
    UClass Cls = GetFixtureUObjectClass();
    if (Cls is null)
    {
        return -1;
    }
    UClass Super = Cls.GetSuperClass();
    if (Super !is null)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("UClassGetSuperClass", Script, "int RunGetSuperClass()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsChildOfBinding)
	{
		static const char Script[] = R"(
int RunIsChildOf()
{
    UClass Cls = GetFixtureUObjectClass();
    if (Cls is null)
    {
        return -1;
    }
    if (!Cls.IsChildOf(Cls))
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("UClassIsChildOf", Script, "int RunIsChildOf()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetDefaultObjectBinding)
	{
		static const char Script[] = R"(
int RunGetDefaultObject()
{
    UClass Cls = GetFixtureUObjectClass();
    if (Cls is null)
    {
        return -1;
    }
    UObject Obj = Cls.GetDefaultObject();
    if (Obj is null)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("UClassGetDefaultObject", Script, "int RunGetDefaultObject()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetPropertiesSizeBinding)
	{
		static const char Script[] = R"(
int RunGetPropertiesSize()
{
    UClass Cls = GetFixtureUObjectClass();
    if (Cls is null)
    {
        return -1;
    }
    int Size = Cls.GetPropertiesSize();
    if (Size <= 0)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("UClassGetPropertiesSize", Script, "int RunGetPropertiesSize()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsNativeBinding)
	{
		static const char Script[] = R"(
int RunIsNative()
{
    UClass Cls = GetFixtureUObjectClass();
    if (Cls is null)
    {
        return -1;
    }
    if (!Cls.IsNative())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("UClassIsNative", Script, "int RunIsNative()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}
};

#endif
#endif
