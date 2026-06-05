// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "Test_UObject.h"
#include "DummyUObject.h"
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
			UObject* Parent = NewObject<UDUMMYUOBJECT>(Package, TEXT("OtterParent"));
			Parent->AddToRoot();
			Child = NewObject<UDUMMYUOBJECT>(Parent, TEXT("OtterChild"));
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

	int32 ExecuteIntFunction(asIScriptFunction* Function, int ExpectResult = asEXECUTION_FINISHED)
	{
		const int Result = ExecuteFunction(Function);
		if (Result == asEXECUTION_EXCEPTION)
		{
			AddError(Context->GetExceptionString());
		}

		if (!Assert.AreEqual(ExpectResult, Result))
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
    if (!Child.IsInA(UObject::StaticClass()))
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
    UObject TypedOuter = Child.GetTypedOuter(UObject::StaticClass());
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
    if (Child.GetFullName(Parent) != "DUMMYUOBJECT OtterChild")
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
	if (Value.GetName() != "DummyTestUObject")
		return -2;

    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectTestNewObject", Script, "int RunNewObject()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

    TEST_METHOD(Test_NewClassObject)
    {
        static const char Script[] = R"(
class FooDerived : UObject
  {
    void CallMe()
    {
    }
  }

int RunNewObject()
{
    UDUMMYUOBJECT Value = NewObject<UDUMMYUOBJECT>(null, "DummyTestUObject");
    if (Value is null)
    {
        return -1;
    }
	if (Value.GetName() != "DummyTestUObject")
		return -2;

    return 1;
}
)";
        asIScriptFunction* Function = BuildFunction("UObjectTestNewObject", Script, "int RunNewObject()");
        ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
    }

	TEST_METHOD(NewObject_WithOuter)
	{
		static const char Script[] = R"(
int RunNewObject()
{
    UObject Outer = GetFixtureParentObject();
    UDUMMYUOBJECT Value = NewObject<UDUMMYUOBJECT>(Outer, "DummyWithOuter");
    if (Value is null)
        return -1;
    if (Value.GetOuter() !is Outer)
        return -2;
    if (Value.GetName() != "DummyWithOuter")
        return -3;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectNewObjectWithOuter", Script, "int RunNewObject()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(NewObject_DefaultName)
	{
		static const char Script[] = R"(
int RunNewObject()
{
    UDUMMYUOBJECT Value = NewObject<UDUMMYUOBJECT>(null);
    if (Value is null)
        return -1;
    // With NAME_None the engine auto-generates a name
    if (Value.GetName().IsEmpty())
        return -2;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectNewObjectDefaultName", Script, "int RunNewObject()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(NewObject_VerifyClass)
	{
		static const char Script[] = R"(
int RunNewObject()
{
    UDUMMYUOBJECT Value = NewObject<UDUMMYUOBJECT>(null, "DummyVerifyClass");
    if (Value is null)
        return -1;
    UClass ExpectedClass = UDUMMYUOBJECT::StaticClass();
    if (Value.GetClass() !is ExpectedClass)
        return -2;
    if (!Value.IsA(UDUMMYUOBJECT::StaticClass()))
        return -3;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectNewObjectVerifyClass", Script, "int RunNewObject()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(NewObject_ExplicitClass)
	{
		static const char Script[] = R"(
int RunNewObject()
{
    UObject Value = NewObject(null, UDUMMYUOBJECT::StaticClass(), "ExplicitClassObj");
    if (Value is null)
        return -1;
    if (Value.GetName() != "ExplicitClassObj")
        return -2;
    if (Value.GetClass() !is UDUMMYUOBJECT::StaticClass())
        return -3;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectNewObjectExplicitClass", Script, "int RunNewObject()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(NewObject_NullClass)
	{
		static const char Script[] = R"(
int RunNewObject()
{
    // Passing null UClass to the non-template NewObject should trigger an exception
    UObject Value = NewObject(null, null, "NullClassObj");
    // Should never reach here if exception is raised
    if (Value is null)
        return -1;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectNewObjectNullClass", Script, "int RunNewObject()");
        TestRunner->AddExpectedError("NewObject with null class");
		// NewObject with null class triggers an AngelScript exception;
		// ExecuteIntFunction returns -1 when an exception occurs
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function, asEXECUTION_EXCEPTION)));
	}

	// --- DuplicateObject tests ---

	TEST_METHOD(DuplicateObject_Basic)
	{
		static const char Script[] = R"(
int RunDuplicateObject()
{
    UDUMMYUOBJECT Source = NewObject<UDUMMYUOBJECT>(null, "SourceObject");
    if (Source is null)
        return -1;
    UDUMMYUOBJECT Dup = DuplicateObject<UDUMMYUOBJECT>(Source, null, "DuplicatedObject");
    if (Dup is null)
        return -2;
    if (Dup.GetName() != "DuplicatedObject")
        return -3;
    // Duplicate should not be the same instance as source
    if (Dup is Source)
        return -4;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectDuplicateObjectBasic", Script, "int RunDuplicateObject()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(DuplicateObject_WithOuter)
	{
		static const char Script[] = R"(
int RunDuplicateObject()
{
    UDUMMYUOBJECT Source = NewObject<UDUMMYUOBJECT>(null, "SourceWithOuter");
    if (Source is null)
        return -1;
    UObject Outer = GetFixtureParentObject();
    UDUMMYUOBJECT Dup = DuplicateObject<UDUMMYUOBJECT>(Source, Outer, "DuplicatedWithOuter");
    if (Dup is null)
        return -2;
    if (Dup.GetOuter() !is Outer)
        return -3;
    if (Dup.GetName() != "DuplicatedWithOuter")
        return -4;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectDuplicateObjectWithOuter", Script, "int RunDuplicateObject()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(DuplicateObject_DefaultName)
	{
		static const char Script[] = R"(
int RunDuplicateObject()
{
    UDUMMYUOBJECT Source = NewObject<UDUMMYUOBJECT>(null, "SourceDefaultName");
    if (Source is null)
        return -1;
    // Duplicate without specifying a name (uses NAME_None → auto-generated)
    UDUMMYUOBJECT Dup = DuplicateObject<UDUMMYUOBJECT>(Source, null);
    if (Dup is null)
        return -2;
    if (Dup.GetName().IsEmpty())
        return -3;
    if (Dup is Source)
        return -4;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectDuplicateObjectDefaultName", Script, "int RunDuplicateObject()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(DuplicateObject_VerifyNotSame)
	{
		static const char Script[] = R"(
int RunDuplicateObject()
{
    UDUMMYUOBJECT Source = NewObject<UDUMMYUOBJECT>(null, "SourceVerifyNotSame");
    if (Source is null)
        return -1;
    UDUMMYUOBJECT Dup = DuplicateObject<UDUMMYUOBJECT>(Source, null, "DuplicatedVerifyNotSame");
    if (Dup is null)
        return -2;
    if (Dup is Source)
        return -3;
    // Source should still be valid after duplication
    if (Source.GetName() != "SourceVerifyNotSame")
        return -4;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectDuplicateObjectVerifyNotSame", Script, "int RunDuplicateObject()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(DuplicateObject_VerifyClass)
	{
		static const char Script[] = R"(
int RunDuplicateObject()
{
    UDUMMYUOBJECT Source = NewObject<UDUMMYUOBJECT>(null, "SourceVerifyClass");
    if (Source is null)
        return -1;
    UDUMMYUOBJECT Dup = DuplicateObject<UDUMMYUOBJECT>(Source, null, "DuplicatedVerifyClass");
    if (Dup is null)
        return -2;
    // Duplicate should have the same class as the source
    if (Dup.GetClass() !is Source.GetClass())
        return -3;
    if (Dup.GetClass() !is UDUMMYUOBJECT::StaticClass())
        return -4;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectDuplicateObjectVerifyClass", Script, "int RunDuplicateObject()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(DuplicateObject_NullSource)
	{
		static const char Script[] = R"(
int RunDuplicateObject()
{
    UDUMMYUOBJECT Null;
    UDUMMYUOBJECT Dup = DuplicateObject<UDUMMYUOBJECT>(Null, null, "NullSourceDup");
    // Should never reach here if exception is raised
    if (Dup is null)
        return -1;
    return 1;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectDuplicateObjectNullSource", Script, "int RunDuplicateObject()");
        TestRunner->AddExpectedError(TEXT("DuplicateObject with invalid source object"));
		// DuplicateObject with null source triggers an AngelScript exception;
		// ExecuteIntFunction returns -1 when an exception occurs
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(Function, asEXECUTION_EXCEPTION)));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
