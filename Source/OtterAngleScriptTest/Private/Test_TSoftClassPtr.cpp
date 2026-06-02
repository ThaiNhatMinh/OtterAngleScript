// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "UObject/SoftObjectPtr.h"
#include "angelscript.h"

namespace
{/*
	static UClass* GetObjectStaticClass()
	{
		return UObject::StaticClass();
	}

	static UClass* GetPhysMatStaticClass()
	{
		return UPhysicalMaterial::StaticClass();
	}

	void RegisterTSoftClassPtrFixtures(asIScriptEngine* Engine)
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
	}*/
}

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptTSoftClassPtrTests,
	"OtterAngleScript.TSoftClassPtr",
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
		//RegisterTSoftClassPtrFixtures(Engine);

		ScriptModule = Engine->GetModule("OtterAngleScriptTSoftClassPtrTest", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("TSoftClassPtr<UObject>")));
	}

	TEST_METHOD(DefaultConstructor)
	{
		static const char Script[] = R"(
int RunDefaultConstructor()
{
    TSoftClassPtr<UObject> Ptr;
    if (!Ptr.IsNull())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftClassPtrDefaultConstructor", Script, "int RunDefaultConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(CopyConstructor)
	{
		static const char Script[] = R"(
int RunCopyConstructor()
{
    TSoftClassPtr<UObject> A;
    TSoftClassPtr<UObject> B(A);
    if (!A.opEquals(B))
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftClassPtrCopyConstructor", Script, "int RunCopyConstructor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ConstructFromClass)
	{
		static const char Script[] = R"(
int RunConstructFromClass()
{
    UClass@ Cls = GetObjectStaticClass();
    TSoftClassPtr<UObject> Ptr(Cls);
    if (Ptr.IsNull())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftClassPtrConstructFromClass", Script, "int RunConstructFromClass()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ConstructFromPath)
	{
		static const char Script[] = R"(
int RunConstructFromPath()
{
    FSoftObjectPath Path("/Script/Engine.Actor");
    TSoftClassPtr<UObject> Ptr(Path);
    if (Ptr.IsNull())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftClassPtrConstructFromPath", Script, "int RunConstructFromPath()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(AssignOperator)
	{
		static const char Script[] = R"(
int RunAssignOperator()
{
    UClass@ Cls = GetObjectStaticClass();
    TSoftClassPtr<UObject> A(Cls);
    TSoftClassPtr<UObject> B;
    B = A;
    if (!A.opEquals(B))
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftClassPtrAssignOperator", Script, "int RunAssignOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(AssignFromClass)
	{
		static const char Script[] = R"(
int RunAssignFromClass()
{
    TSoftClassPtr<UObject> Ptr;
    Ptr = GetObjectStaticClass();
    if (Ptr.IsNull())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftClassPtrAssignFromClass", Script, "int RunAssignFromClass()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(AssignFromPath)
	{
		static const char Script[] = R"(
int RunAssignFromPath()
{
    TSoftClassPtr<UObject> Ptr;
    FSoftObjectPath Path("/Script/Engine.Actor");
    Ptr = Path;
    if (Ptr.IsNull())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftClassPtrAssignFromPath", Script, "int RunAssignFromPath()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(EqualsOperator)
	{
		static const char Script[] = R"(
int RunEqualsOperator()
{
    UClass@ Cls = GetObjectStaticClass();
    TSoftClassPtr<UObject> A(Cls);
    TSoftClassPtr<UObject> B(Cls);
    if (!A.opEquals(B))
    {
        return -1;
    }
    TSoftClassPtr<UObject> Empty;
    if (A.opEquals(Empty))
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftClassPtrEqualsOperator", Script, "int RunEqualsOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(EqualsClass)
	{
		static const char Script[] = R"(
int RunEqualsClass()
{
    TSoftClassPtr<UObject> Ptr(GetObjectStaticClass());
    if (!Ptr.opEquals(GetObjectStaticClass()))
    {
        return -1;
    }
    if (Ptr.opEquals(null))
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftClassPtrEqualsClass", Script, "int RunEqualsClass()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsNull)
	{
		static const char Script[] = R"(
int RunIsNull()
{
    TSoftClassPtr<UObject> Ptr;
    if (!Ptr.IsNull())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftClassPtrIsNull", Script, "int RunIsNull()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsValid)
	{
		static const char Script[] = R"(
int RunIsValid()
{
    TSoftClassPtr<UObject> Invalid;
    if (Invalid.IsValid())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftClassPtrIsValid", Script, "int RunIsValid()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsPending)
	{
		static const char Script[] = R"(
int RunIsPending()
{
    TSoftClassPtr<UObject> Ptr;
    if (Ptr.IsPending())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftClassPtrIsPending", Script, "int RunIsPending()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(Reset)
	{
		static const char Script[] = R"(
int RunReset()
{
    UClass@ Cls = GetObjectStaticClass();
    TSoftClassPtr<UObject> Ptr(Cls);
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
		asIScriptFunction* Function = BuildFunction("TSoftClassPtrReset", Script, "int RunReset()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(Get)
	{
		static const char Script[] = R"(
int RunGet()
{
    TSoftClassPtr<UObject> Null;
    UClass@ Cls = Null.Get();
    if (Cls !is null)
    {
        return -1;
    }
    TSoftClassPtr<UObject> Ptr(GetObjectStaticClass());
    Cls = Ptr.Get();
    if (Cls is null)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftClassPtrGet", Script, "int RunGet()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ToString)
	{
		static const char Script[] = R"(
int RunToString()
{
    TSoftClassPtr<UObject> Empty;
    if (Empty.ToString() != "")
    {
        return -1;
    }
    UClass@ Cls = GetObjectStaticClass();
    TSoftClassPtr<UObject> Ptr(Cls);
    if (Ptr.ToString() == "")
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftClassPtrToString", Script, "int RunToString()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetAssetName)
	{
		static const char Script[] = R"(
int RunGetAssetName()
{
    UClass@ Cls = GetObjectStaticClass();
    TSoftClassPtr<UObject> Ptr(Cls);
    if (Ptr.GetAssetName() == "")
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftClassPtrGetAssetName", Script, "int RunGetAssetName()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ToSoftObjectPath)
	{
		static const char Script[] = R"(
int RunToSoftObjectPath()
{
    UClass@ Cls = GetObjectStaticClass();
    TSoftClassPtr<UObject> Ptr(Cls);
    FSoftObjectPath Retrieved = Ptr.ToSoftObjectPath();
    if (!Retrieved.IsValid())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftClassPtrToSoftObjectPath", Script, "int RunToSoftObjectPath()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(MultipleInstantiations)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("TSoftClassPtr<UObject>")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("TSoftClassPtr<UPhysicalMaterial>")));

		static const char Script[] = R"(
int RunMultipleInstantiations()
{
    TSoftClassPtr<UObject> ObjPtr(GetObjectStaticClass());
    TSoftClassPtr<UPhysicalMaterial> PhysPtr(GetPhysMatStaticClass());

    if (ObjPtr.IsNull())
    {
        return -1;
    }
    if (PhysPtr.IsNull())
    {
        return -2;
    }
    if (ObjPtr.Get() is null)
    {
        return -3;
    }
    if (PhysPtr.Get() is null)
    {
        return -4;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("TSoftClassPtrMultipleInstantiations", Script, "int RunMultipleInstantiations()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
