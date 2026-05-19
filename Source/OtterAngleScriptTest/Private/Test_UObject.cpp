// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Misc/AssertionMacros.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "UObject/Object.h"
#include "UObject/Package.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptUObjectTests,
	"OtterAngleScript.UObject",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptUObjectTest", asGM_ALWAYS_CREATE);
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

	TEST_METHOD(BasicQueries)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("UObject")));

		static const char Script[] = R"(
int RunBasicQueries()
{
    UObject@ Value = GetFixtureChildObject();
    if (Value is null)
    {
        return -1;
    }

    if (Value.GetClass() is null)
    {
        return -2;
    }

    if (!Value.IsA(GetFixtureUObjectClass()))
    {
        return -3;
    }

    if (Value.GetName() != "OtterChild")
    {
        return -4;
    }

    if (Value.GetOuter() is null || Value.GetOuter().GetName() != "OtterParent")
    {
        return -5;
    }

    if (Value.GetUniqueID() == 0)
    {
        return -6;
    }

    if (!Value.IsValidLowLevel())
    {
        return -7;
    }

    if (!Value.IsValidLowLevelFast())
    {
        return -8;
    }

    if (Value.IsTemplate())
    {
        return -9;
    }

    return 10;
}
)";

		asIScriptFunction* Function = BuildFunction("UObjectBasicQueries", Script, "int RunBasicQueries()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 10));
	}

	TEST_METHOD(OuterQueries)
	{
		static const char Script[] = R"(
int RunOuterQueries()
{
    UObject@ Parent = GetFixtureParentObject();
    UObject@ Child = GetFixtureChildObject();
    if (Parent is null || Child is null)
    {
        return -1;
    }

    if (!Child.IsIn(Parent))
    {
        return -2;
    }

    if (!Child.IsInA(GetFixtureUObjectClass()))
    {
        return -3;
    }

    UObject@ TypedOuter = Child.GetTypedOuter(GetFixtureUObjectClass());
    if (TypedOuter is null || TypedOuter.GetName() != "OtterParent")
    {
        return -4;
    }

    if (Child.GetPathName(Parent) != "OtterChild")
    {
        return -5;
    }

    if (!Child.GetPathName().Contains("OtterParent.OtterChild"))
    {
        return -6;
    }

    if (!Child.GetFullName(Parent).Contains("OtterChild"))
    {
        return -7;
    }

    if (Child.GetPackage() is null || Child.GetPackage().GetName() != GetFixturePackageObject().GetName())
    {
        return -8;
    }

    if (Child.GetOutermost() is null || Child.GetOutermost().GetName() != GetFixturePackageObject().GetName())
    {
        return -9;
    }

    return 11;
}
)";

		asIScriptFunction* Function = BuildFunction("UObjectOuterQueries", Script, "int RunOuterQueries()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 11));
	}

	TEST_METHOD(StateQueries)
	{
		static const char Script[] = R"(
int RunStateQueries()
{
    UObject@ Value = GetFixtureChildObject();
    if (Value is null)
    {
        return -1;
    }

    if (!Value.IsRooted())
    {
        return -2;
    }

    if (Value.IsNative())
    {
        return -3;
    }

    if (Value.IsDefaultSubobject())
    {
        return -4;
    }

    if (Value.IsAsset())
    {
        return -5;
    }

    if (Value.GetWorld() !is null)
    {
        return -6;
    }

    if (!Value.GetDesc().IsEmpty())
    {
        return -7;
    }

    return 8;
}
)";

		asIScriptFunction* Function = BuildFunction("UObjectStateQueries", Script, "int RunStateQueries()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 8));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
