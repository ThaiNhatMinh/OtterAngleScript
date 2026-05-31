// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "UObject/WeakObjectPtr.h"
#include "angelscript.h"

namespace
{
	AActor* GetFixtureActorForWeakPtr()
	{
		static AActor* Actor = nullptr;
		if (Actor == nullptr)
		{
			Actor = NewObject<AActor>(GetTransientPackage(), NAME_None, RF_Transient);
			Actor->AddToRoot();
		}
		return Actor;
	}

	UPrimitiveComponent* GetFixtureComponentForWeakPtr()
	{
		static UBoxComponent* Component = nullptr;
		if (Component == nullptr)
		{
			Component = NewObject<UBoxComponent>(GetFixtureActorForWeakPtr(), NAME_None, RF_Transient);
			Component->AddToRoot();
		}
		return Component;
	}

	UPhysicalMaterial* GetFixturePhysMaterialForWeakPtr()
	{
		static UPhysicalMaterial* PhysMaterial = nullptr;
		if (PhysMaterial == nullptr)
		{
			PhysMaterial = NewObject<UPhysicalMaterial>(GetTransientPackage(), NAME_None, RF_Transient);
			PhysMaterial->AddToRoot();
		}
		return PhysMaterial;
	}

	void RegisterWeakPtrTestFixtures(asIScriptEngine* Engine)
	{
		static bool bRegistered = false;
		if (bRegistered)
		{
			return;
		}

		int Result = Engine->RegisterGlobalFunction(
			"UPrimitiveComponent@ GetWeakPtrFixtureComponent()",
			asFUNCTION(GetFixtureComponentForWeakPtr), asCALL_CDECL);
		check(Result >= 0);

		Result = Engine->RegisterGlobalFunction(
			"UPhysicalMaterial@ GetWeakPtrFixturePhysMaterial()",
			asFUNCTION(GetFixturePhysMaterialForWeakPtr), asCALL_CDECL);
		check(Result >= 0);

		bRegistered = true;
	}
}

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptTWeakObjectPtrTests,
	"OtterAngleScript.TWeakObjectPtr",
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
		RegisterWeakPtrTestFixtures(Engine);

		ScriptModule = Engine->GetModule("OtterAngleScriptTWeakObjectPtrTest", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("TWeakObjectPtr<UObject>")));
	}

	TEST_METHOD(DefaultConstructor)
	{
		static const char Script[] = R"(
int RunDefaultConstructor()
{
    TWeakObjectPtr<UObject> Ptr;
    if (!Ptr.IsExplicitlyNull())
    {
        return -1;
    }
    if (Ptr.IsValid())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TWeakObjectPtrDefaultConstructor", Script, "int RunDefaultConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ConstructFromObject)
	{
		static const char Script[] = R"(
int RunConstructFromObject()
{
    UPrimitiveComponent@ Comp = GetWeakPtrFixtureComponent();
    TWeakObjectPtr<UPrimitiveComponent> Ptr(Comp);
    if (!Ptr.IsValid())
    {
        return -1;
    }
    if (Ptr.Get() is null)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TWeakObjectPtrConstructFromObject", Script, "int RunConstructFromObject()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(CopyConstructor)
	{
		static const char Script[] = R"(
int RunCopyConstructor()
{
    UPrimitiveComponent@ Comp = GetWeakPtrFixtureComponent();
    TWeakObjectPtr<UPrimitiveComponent> A(Comp);
    TWeakObjectPtr<UPrimitiveComponent> B(A);
    if (!B.IsValid())
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
		asIScriptFunction* Function = BuildFunction("TWeakObjectPtrCopyConstructor", Script, "int RunCopyConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(AssignFromWeakPtr)
	{
		static const char Script[] = R"(
int RunAssignFromWeakPtr()
{
    UPrimitiveComponent@ Comp = GetWeakPtrFixtureComponent();
    TWeakObjectPtr<UPrimitiveComponent> A(Comp);
    TWeakObjectPtr<UPrimitiveComponent> B;
    B = A;
    if (!B.IsValid() || !A.opEquals(B))
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TWeakObjectPtrAssignFromWeakPtr", Script, "int RunAssignFromWeakPtr()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(AssignFromObject)
	{
		static const char Script[] = R"(
int RunAssignFromObject()
{
    TWeakObjectPtr<UPrimitiveComponent> Ptr;
    Ptr = GetWeakPtrFixtureComponent();
    if (!Ptr.IsValid())
    {
        return -1;
    }
    if (Ptr.Get() is null)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TWeakObjectPtrAssignFromObject", Script, "int RunAssignFromObject()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(EqualsOperator)
	{
		static const char Script[] = R"(
int RunEqualsOperator()
{
    UPrimitiveComponent@ Comp = GetWeakPtrFixtureComponent();
    TWeakObjectPtr<UPrimitiveComponent> A(Comp);
    TWeakObjectPtr<UPrimitiveComponent> B(Comp);
    if (!A.opEquals(B))
    {
        return -1;
    }
    TWeakObjectPtr<UPrimitiveComponent> Empty;
    if (A.opEquals(Empty))
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TWeakObjectPtrEqualsOperator", Script, "int RunEqualsOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(EqualsRawObject)
	{
		static const char Script[] = R"(
int RunEqualsRawObject()
{
    UPrimitiveComponent@ Comp = GetWeakPtrFixtureComponent();
    TWeakObjectPtr<UPrimitiveComponent> Ptr(Comp);
    if (!Ptr.opEquals(Comp))
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TWeakObjectPtrEqualsRawObject", Script, "int RunEqualsRawObject()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsValid)
	{
		static const char Script[] = R"(
int RunIsValid()
{
    TWeakObjectPtr<UObject> Invalid;
    if (Invalid.IsValid())
    {
        return -1;
    }
    UPrimitiveComponent@ Comp = GetWeakPtrFixtureComponent();
    TWeakObjectPtr<UPrimitiveComponent> Valid(Comp);
    if (!Valid.IsValid())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TWeakObjectPtrIsValid", Script, "int RunIsValid()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsExplicitlyNull)
	{
		static const char Script[] = R"(
int RunIsExplicitlyNull()
{
    TWeakObjectPtr<UObject> Ptr;
    if (!Ptr.IsExplicitlyNull())
    {
        return -1;
    }
    UPrimitiveComponent@ Comp = GetWeakPtrFixtureComponent();
    TWeakObjectPtr<UPrimitiveComponent> Valid(Comp);
    if (Valid.IsExplicitlyNull())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TWeakObjectPtrIsExplicitlyNull", Script, "int RunIsExplicitlyNull()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsStale)
	{
		static const char Script[] = R"(
int RunIsStale()
{
    TWeakObjectPtr<UObject> Ptr;
    // A default-constructed (null) pointer is not stale
    if (Ptr.IsStale())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TWeakObjectPtrIsStale", Script, "int RunIsStale()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(Reset)
	{
		static const char Script[] = R"(
int RunReset()
{
    UPrimitiveComponent@ Comp = GetWeakPtrFixtureComponent();
    TWeakObjectPtr<UPrimitiveComponent> Ptr(Comp);
    if (!Ptr.IsValid())
    {
        return -1;
    }
    Ptr.Reset();
    if (Ptr.IsValid())
    {
        return -2;
    }
    if (!Ptr.IsExplicitlyNull())
    {
        return -3;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TWeakObjectPtrReset", Script, "int RunReset()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(Get)
	{
		static const char Script[] = R"(
int RunGet()
{
    TWeakObjectPtr<UObject> Null;
    if (Null.Get() !is null)
    {
        return -1;
    }
    UPrimitiveComponent@ Comp = GetWeakPtrFixtureComponent();
    TWeakObjectPtr<UPrimitiveComponent> Ptr(Comp);
    if (Ptr.Get() is null)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TWeakObjectPtrGet", Script, "int RunGet()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(HasSameIndexAndSerialNumber)
	{
		static const char Script[] = R"(
int RunHasSameIndexAndSerialNumber()
{
    UPrimitiveComponent@ Comp = GetWeakPtrFixtureComponent();
    TWeakObjectPtr<UPrimitiveComponent> A(Comp);
    TWeakObjectPtr<UPrimitiveComponent> B(Comp);
    if (!A.HasSameIndexAndSerialNumber(B))
    {
        return -1;
    }
    TWeakObjectPtr<UPrimitiveComponent> Empty;
    if (A.HasSameIndexAndSerialNumber(Empty))
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TWeakObjectPtrHasSameIndexAndSerialNumber", Script, "int RunHasSameIndexAndSerialNumber()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(MultipleInstantiations)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("TWeakObjectPtr<UPrimitiveComponent>")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("TWeakObjectPtr<UPhysicalMaterial>")));

		static const char Script[] = R"(
int RunMultipleInstantiations()
{
    UPrimitiveComponent@ Comp = GetWeakPtrFixtureComponent();
    UPhysicalMaterial@ Mat = GetWeakPtrFixturePhysMaterial();

    TWeakObjectPtr<UPrimitiveComponent> WeakComp(Comp);
    TWeakObjectPtr<UPhysicalMaterial> WeakMat(Mat);

    if (!WeakComp.IsValid() || WeakComp.Get() is null)
    {
        return -1;
    }
    if (!WeakMat.IsValid() || WeakMat.Get() is null)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TWeakObjectPtrMultipleInstantiations", Script, "int RunMultipleInstantiations()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
