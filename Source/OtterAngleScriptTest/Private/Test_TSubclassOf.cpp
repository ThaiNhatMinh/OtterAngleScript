// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Misc/AssertionMacros.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Templates/SubclassOf.h"
#include "UObject/Class.h"
#include "angelscript.h"

namespace
{
	UClass* GetObjectStaticClass()
	{
		return UObject::StaticClass();
	}

	UClass* GetPhysMatStaticClass()
	{
		return UPhysicalMaterial::StaticClass();
	}

	void RegisterTSubclassOfFixtures(asIScriptEngine* Engine)
	{
		static bool bRegistered = false;
		if (bRegistered)
		{
			return;
		}

		int Result = Engine->RegisterGlobalFunction(
			"UClass@ GetObjectStaticClass()",
			asFUNCTION(GetObjectStaticClass), asCALL_CDECL);
		check(Result >= 0);

		Result = Engine->RegisterGlobalFunction(
			"UClass@ GetPhysMatStaticClass()",
			asFUNCTION(GetPhysMatStaticClass), asCALL_CDECL);
		check(Result >= 0);

		bRegistered = true;
	}
}

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptTSubclassOfTests,
	"OtterAngleScript.TSubclassOf",
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

	int32 ExecuteIntFunction(asIScriptFunction* Function)
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

		const int Result = Context->Execute();
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
		RegisterTSubclassOfFixtures(Engine);

		ScriptModule = Engine->GetModule("OtterAngleScriptTSubclassOfTest", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("TSubclassOf<UObject>")));
	}

	TEST_METHOD(DefaultConstructor)
	{
		static const char Script[] = R"(
int RunDefaultConstructor()
{
    TSubclassOf<UObject> Sub;
    if (Sub.Get() !is null)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSubclassOfDefaultConstructor", Script, "int RunDefaultConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ConstructFromClass)
	{
		static const char Script[] = R"(
int RunConstructFromClass()
{
    UClass@ Cls = GetObjectStaticClass();
    TSubclassOf<UObject> Sub(Cls);
    if (Sub.Get() is null)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSubclassOfConstructFromClass", Script, "int RunConstructFromClass()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(CopyConstructor)
	{
		static const char Script[] = R"(
int RunCopyConstructor()
{
    UClass@ Cls = GetObjectStaticClass();
    TSubclassOf<UObject> A(Cls);
    TSubclassOf<UObject> B(A);
    if (B.Get() is null)
    {
        return -1;
    }
    if (!A.opEquals(B))
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSubclassOfCopyConstructor", Script, "int RunCopyConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(AssignFromTSubclassOf)
	{
		static const char Script[] = R"(
int RunAssignFromTSubclassOf()
{
    UClass@ Cls = GetObjectStaticClass();
    TSubclassOf<UObject> A(Cls);
    TSubclassOf<UObject> B;
    B = A;
    if (B.Get() is null)
    {
        return -1;
    }
    if (!A.opEquals(B))
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSubclassOfAssignFromTSubclassOf", Script, "int RunAssignFromTSubclassOf()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(AssignFromClass)
	{
		static const char Script[] = R"(
int RunAssignFromClass()
{
    TSubclassOf<UObject> Sub;
    Sub = GetObjectStaticClass();
    if (Sub.Get() is null)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSubclassOfAssignFromClass", Script, "int RunAssignFromClass()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(EqualsOperator)
	{
		static const char Script[] = R"(
int RunEqualsOperator()
{
    UClass@ Cls = GetObjectStaticClass();
    TSubclassOf<UObject> A(Cls);
    TSubclassOf<UObject> B(Cls);
    if (!A.opEquals(B))
    {
        return -1;
    }
    TSubclassOf<UObject> Empty;
    if (A.opEquals(Empty))
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSubclassOfEqualsOperator", Script, "int RunEqualsOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(Get)
	{
		static const char Script[] = R"(
int RunGet()
{
    TSubclassOf<UObject> Null;
    if (Null.Get() !is null)
    {
        return -1;
    }
    TSubclassOf<UObject> Sub(GetObjectStaticClass());
    if (Sub.Get() is null)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSubclassOfGet", Script, "int RunGet()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetDefaultObject)
	{
		static const char Script[] = R"(
int RunGetDefaultObject()
{
    TSubclassOf<UObject> Null;
    if (Null.GetDefaultObject() !is null)
    {
        return -1;
    }
    TSubclassOf<UObject> Sub(GetObjectStaticClass());
    if (Sub.GetDefaultObject() is null)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSubclassOfGetDefaultObject", Script, "int RunGetDefaultObject()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(MultipleInstantiations)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("TSubclassOf<UObject>")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("TSubclassOf<UPhysicalMaterial>")));

		static const char Script[] = R"(
int RunMultipleInstantiations()
{
    TSubclassOf<UObject> ObjSub(GetObjectStaticClass());
    TSubclassOf<UPhysicalMaterial> PhysSub(GetPhysMatStaticClass());

    if (ObjSub.Get() is null)
    {
        return -1;
    }
    if (PhysSub.Get() is null)
    {
        return -2;
    }
    if (PhysSub.GetDefaultObject() is null)
    {
        return -3;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSubclassOfMultipleInstantiations", Script, "int RunMultipleInstantiations()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
