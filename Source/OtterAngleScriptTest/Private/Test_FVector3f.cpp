// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Math/Vector.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFVector3fTests,
	"OtterAngleScript.FVector3f",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptVec3fTest", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FVector3f")));
	}

	TEST_METHOD(DefaultConstruct)
	{
		static const char Script[] = R"(
int RunDefaultConstruct()
{
    FVector3f V;
    if (V.X != 0.0f || V.Y != 0.0f || V.Z != 0.0f)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("Vec3fDefaultCtor", Script, "int RunDefaultConstruct()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(XYZConstruct)
	{
		static const char Script[] = R"(
int RunXYZConstruct()
{
    FVector3f V(1.0f, 2.0f, 3.0f);
    if (V.X != 1.0f || V.Y != 2.0f || V.Z != 3.0f)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("Vec3fXYZCtor", Script, "int RunXYZConstruct()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(Properties)
	{
		static const char Script[] = R"(
int RunProperties()
{
    FVector3f V(4.0f, 5.0f, 6.0f);
    if (V.X != 4.0f) return -1;
    if (V.Y != 5.0f) return -2;
    if (V.Z != 6.0f) return -3;
    V.X = 10.0f;
    V.Y = 20.0f;
    V.Z = 30.0f;
    if (V.X != 10.0f || V.Y != 20.0f || V.Z != 30.0f)
    {
        return -4;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("Vec3fProperties", Script, "int RunProperties()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(AddBinding)
	{
		static const char Script[] = R"(
int RunAddBinding()
{
    FVector3f A(1.0f, 2.0f, 3.0f);
    FVector3f B(4.0f, 5.0f, 6.0f);
    FVector3f C = A + B;
    if (C.X != 5.0f || C.Y != 7.0f || C.Z != 9.0f)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("Vec3fAdd", Script, "int RunAddBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(SubBinding)
	{
		static const char Script[] = R"(
int RunSubBinding()
{
    FVector3f A(4.0f, 5.0f, 6.0f);
    FVector3f B(1.0f, 2.0f, 3.0f);
    FVector3f C = A - B;
    if (C.X != 3.0f || C.Y != 3.0f || C.Z != 3.0f)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("Vec3fSub", Script, "int RunSubBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(MulScalar)
	{
		static const char Script[] = R"(
int RunMulScalar()
{
    FVector3f V(2.0f, 3.0f, 4.0f);
    FVector3f R = V * 2.0f;
    if (R.X != 4.0f || R.Y != 6.0f || R.Z != 8.0f)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("Vec3fMulScalar", Script, "int RunMulScalar()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(Equals)
	{
		static const char Script[] = R"(
int RunEquals()
{
    FVector3f A(1.0f, 2.0f, 3.0f);
    FVector3f B(1.0f, 2.0f, 3.0f);
    FVector3f C(4.0f, 5.0f, 6.0f);
    if (!(A == B))
    {
        return -1;
    }
    if (A == C)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("Vec3fEquals", Script, "int RunEquals()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(CrossDot)
	{
		static const char Script[] = R"(
int RunCrossDot()
{
    FVector3f A(1.0f, 0.0f, 0.0f);
    FVector3f B(0.0f, 1.0f, 0.0f);
    FVector3f Cross = A ^ B;
    if (Cross.X != 0.0f || Cross.Y != 0.0f || Cross.Z != 1.0f)
    {
        return -1;
    }
    float Dot = A | B;
    if (Dot != 0.0f)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("Vec3fCrossDot", Script, "int RunCrossDot()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsZero)
	{
		static const char Script[] = R"(
int RunIsZero()
{
    FVector3f Zero;
    if (!Zero.IsZero())
    {
        return -1;
    }
    FVector3f NonZero(1.0f, 2.0f, 3.0f);
    if (NonZero.IsZero())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("Vec3fIsZero", Script, "int RunIsZero()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(Normalize)
	{
		static const char Script[] = R"(
int RunNormalize()
{
    FVector3f V(3.0f, 0.0f, 0.0f);
    bool Result = V.Normalize();
    if (!Result)
    {
        return -1;
    }
    if (V.X != 1.0f || V.Y != 0.0f || V.Z != 0.0f)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("Vec3fNormalize", Script, "int RunNormalize()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ToString)
	{
		static const char Script[] = R"(
int RunToString()
{
    FVector3f V(1.0f, 2.0f, 3.0f);
    FString Str = V.ToString();
    if (Str.IsEmpty())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("Vec3fToString", Script, "int RunToString()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}
};

#endif
#endif
