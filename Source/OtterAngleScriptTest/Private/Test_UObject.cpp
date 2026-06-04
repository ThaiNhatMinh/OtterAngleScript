// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "Test_UObject.h"
#include "CQTest.h"
#include "Misc/AssertionMacros.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "UObject/Class.h"
#include "UObject/Object.h"
#include "UObject/Package.h"
#include "angelscript.h"

namespace
{
	UObject* GetFixtureChildObject()
	{
		static UObject* Child = nullptr;
		if (Child == nullptr)
		{
			UPackage* Package = CreatePackage(TEXT("/Temp/OtterAngleScriptTest"));
			Package->AddToRoot();
			UObject* Parent = NewObject<UObject>(Package, TEXT("OtterParent"));
			Parent->AddToRoot();
			Child = NewObject<UObject>(Parent, TEXT("OtterChild"));
			Child->AddToRoot();
		}
		return Child;
	}

	UObject* GetFixtureParentObject()
	{
		GetFixtureChildObject(); // ensure created
		return GetFixtureChildObject()->GetOuter();
	}

	UObject* GetFixturePackageObject()
	{
		GetFixtureChildObject(); // ensure created
		return GetFixtureChildObject()->GetPackage();
	}

	bool RegisterUObjectFixtureBindings(asIScriptEngine* Engine)
	{
		static bool bRegistered = false;
		if (bRegistered) return true;

		int Result = Engine->RegisterGlobalFunction("UObject GetFixtureChildObject()", asFUNCTION(GetFixtureChildObject), asCALL_CDECL);
		check(Result >= 0);
		Result = Engine->RegisterGlobalFunction("UObject GetFixtureParentObject()", asFUNCTION(GetFixtureParentObject), asCALL_CDECL);
		check(Result >= 0);
		Result = Engine->RegisterGlobalFunction("UObject GetFixturePackageObject()", asFUNCTION(GetFixturePackageObject), asCALL_CDECL);
		check(Result >= 0);

		bRegistered = true;
		return true;
	}
}

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

		ASSERT_THAT(IsTrue(RegisterUObjectFixtureBindings(Engine)));

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

	TEST_METHOD(TypeInfo)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("UObject")));
	}

	TEST_METHOD(GetClass)
	{
		static const char Script[] = R"(
int RunGetClass()
{
    UObject Value = GetFixtureChildObject();
    if (Value is null)
    {
        return -1;
    }
    if (Value.GetClass() is null)
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectGetClass", Script, "int RunGetClass()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(IsA)
	{
		static const char Script[] = R"(
int RunIsA()
{
    UObject Value = GetFixtureChildObject();
    if (Value is null)
    {
        return -1;
    }
    if (!Value.IsA(UObject::StaticClass()))
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectIsA", Script, "int RunIsA()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetName)
	{
		static const char Script[] = R"(
int RunGetName()
{
    UObject Value = GetFixtureChildObject();
    if (Value is null)
    {
        return -1;
    }
    if (Value.GetName() != "OtterChild")
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectGetName", Script, "int RunGetName()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetOuter)
	{
		static const char Script[] = R"(
int RunGetOuter()
{
    UObject Value = GetFixtureChildObject();
    if (Value is null)
    {
        return -1;
    }
    if (Value.GetOuter() is null || Value.GetOuter().GetName() != "OtterParent")
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectGetOuter", Script, "int RunGetOuter()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetUniqueID)
	{
		static const char Script[] = R"(
int RunGetUniqueID()
{
    UObject Value = GetFixtureChildObject();
    if (Value is null)
    {
        return -1;
    }
    if (Value.GetUniqueID() == 0)
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectGetUniqueID", Script, "int RunGetUniqueID()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(IsValidLowLevel)
	{
		static const char Script[] = R"(
int RunIsValidLowLevel()
{
    UObject Value = GetFixtureChildObject();
    if (Value is null)
    {
        return -1;
    }
    if (!Value.IsValidLowLevel())
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectIsValidLowLevel", Script, "int RunIsValidLowLevel()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(IsValidLowLevelFast)
	{
		static const char Script[] = R"(
int RunIsValidLowLevelFast()
{
    UObject Value = GetFixtureChildObject();
    if (Value is null)
    {
        return -1;
    }
    if (!Value.IsValidLowLevelFast())
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectIsValidLowLevelFast", Script, "int RunIsValidLowLevelFast()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(IsTemplate)
	{
		static const char Script[] = R"(
int RunIsTemplate()
{
    UObject Value = GetFixtureChildObject();
    if (Value is null)
    {
        return -1;
    }
    if (Value.IsTemplate())
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectIsTemplate", Script, "int RunIsTemplate()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(IsIn)
	{
		static const char Script[] = R"(
int RunIsIn()
{
    UObject Parent = GetFixtureParentObject();
    UObject Child = GetFixtureChildObject();
    if (Parent is null || Child is null)
    {
        return -1;
    }
    if (!Child.IsIn(Parent))
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectIsIn", Script, "int RunIsIn()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(IsInA)
	{
		static const char Script[] = R"(
int RunIsInA()
{
    UObject Child = GetFixtureChildObject();
    if (Child is null)
    {
        return -1;
    }
    if (!Child.IsInA(UObject::StaticClass()()))
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectIsInA", Script, "int RunIsInA()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetTypedOuter)
	{
		static const char Script[] = R"(
int RunGetTypedOuter()
{
    UObject Child = GetFixtureChildObject();
    if (Child is null)
    {
        return -1;
    }
    UObject TypedOuter = Child.GetTypedOuter(UObject::StaticClass()());
    if (TypedOuter is null || TypedOuter.GetName() != "OtterParent")
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectGetTypedOuter", Script, "int RunGetTypedOuter()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetPathName_WithOuter)
	{
		static const char Script[] = R"(
int RunGetPathNameWithOuter()
{
    UObject Parent = GetFixtureParentObject();
    UObject Child = GetFixtureChildObject();
    if (Parent is null || Child is null)
    {
        return -1;
    }
    if (Child.GetPathName(Parent) != "OtterChild")
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectGetPathNameWithOuter", Script, "int RunGetPathNameWithOuter()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetPathName_NoArg)
	{
		static const char Script[] = R"(
int RunGetPathNameNoArg()
{
    UObject Child = GetFixtureChildObject();
    if (Child is null)
    {
        return -1;
    }
    if (!Child.GetPathName().Contains("/Temp/OtterAngleScriptTest.OtterParent:OtterChild"))
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectGetPathNameNoArg", Script, "int RunGetPathNameNoArg()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetFullName_WithOuter)
	{
		static const char Script[] = R"(
int RunGetFullNameWithOuter()
{
    UObject Parent = GetFixtureParentObject();
    UObject Child = GetFixtureChildObject();
    if (Parent is null || Child is null)
    {
        return -1;
    }
    if (!Child.GetFullName(Parent).Contains("/Temp/OtterAngleScriptTest.OtterParent:OtterChild"))
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectGetFullNameWithOuter", Script, "int RunGetFullNameWithOuter()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetPackage)
	{
		static const char Script[] = R"(
int RunGetPackage()
{
    UObject Child = GetFixtureChildObject();
    if (Child is null)
    {
        return -1;
    }
    if (Child.GetPackage() is null || Child.GetPackage().GetName() != GetFixturePackageObject().GetName())
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectGetPackage", Script, "int RunGetPackage()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetOutermost)
	{
		static const char Script[] = R"(
int RunGetOutermost()
{
    UObject Child = GetFixtureChildObject();
    if (Child is null)
    {
        return -1;
    }
    if (Child.GetOutermost() is null || Child.GetOutermost().GetName() != GetFixturePackageObject().GetName())
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectGetOutermost", Script, "int RunGetOutermost()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(IsRooted)
	{
		static const char Script[] = R"(
int RunIsRooted()
{
    UObject Value = GetFixtureChildObject();
    if (Value is null)
    {
        return -1;
    }
    if (!Value.IsRooted())
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectIsRooted", Script, "int RunIsRooted()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(IsNative)
	{
		static const char Script[] = R"(
int RunIsNative()
{
    UObject Value = GetFixtureChildObject();
    if (Value is null)
    {
        return -1;
    }
    if (Value.IsNative())
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectIsNative", Script, "int RunIsNative()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(IsDefaultSubobject)
	{
		static const char Script[] = R"(
int RunIsDefaultSubobject()
{
    UObject Value = GetFixtureChildObject();
    if (Value is null)
    {
        return -1;
    }
    if (Value.IsDefaultSubobject())
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectIsDefaultSubobject", Script, "int RunIsDefaultSubobject()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(IsAsset)
	{
		static const char Script[] = R"(
int RunIsAsset()
{
    UObject Value = GetFixtureChildObject();
    if (Value is null)
    {
        return -1;
    }
    if (Value.IsAsset())
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectIsAsset", Script, "int RunIsAsset()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetWorld_Null)
	{
		static const char Script[] = R"(
int RunGetWorldNull()
{
    UObject Value = GetFixtureChildObject();
    if (Value is null)
    {
        return -1;
    }
    if (Value.GetWorld() !is null)
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectGetWorldNull", Script, "int RunGetWorldNull()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(GetDesc)
	{
		static const char Script[] = R"(
int RunGetDesc()
{
    UObject Value = GetFixtureChildObject();
    if (Value is null)
    {
        return -1;
    }
    if (!Value.GetDesc().IsEmpty())
    {
        return -2;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectGetDesc", Script, "int RunGetDesc()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Test_NewObject)
	{
		static const char Script[] = R"(
int RunNewObject()
{
    UDUMMYUOBJECT Value = NewObject<UDUMMYUOBJECT>(null, "DummyTestUObject");
    if (Value is null)
    {
        return -1;
    }
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectTestNewObject", Script, "int RunNewObject()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
