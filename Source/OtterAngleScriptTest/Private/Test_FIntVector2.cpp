// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Math/IntVector.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFIntVector2Tests,
	"OtterAngleScript.FIntVector2",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptIntVec2Test", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FIntVector2")));
	}

	TEST_METHOD(DefaultConstruct)
	{
		static const char Script[] = R"(
int RunTest()
{
    FIntVector2 V;
    if (V.X != 0 || V.Y != 0) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("DefaultCtor", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(XYConstruct)
	{
		static const char Script[] = R"(
int RunTest()
{
    FIntVector2 V(3, 7);
    if (V.X != 3 || V.Y != 7) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("XYCtor", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(Properties)
	{
		static const char Script[] = R"(
int RunTest()
{
    FIntVector2 V(1, 2);
    V.X = 10;
    V.Y = 20;
    if (V.X != 10 || V.Y != 20) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("Properties", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(AddBinding)
	{
		static const char Script[] = R"(
int RunTest()
{
    FIntVector2 A(3, 4);
    FIntVector2 B(1, 2);
    FIntVector2 C = A + B;
    if (C.X != 4 || C.Y != 6) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("AddOp", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(SubBinding)
	{
		static const char Script[] = R"(
int RunTest()
{
    FIntVector2 A(5, 8);
    FIntVector2 B(1, 3);
    FIntVector2 C = A - B;
    if (C.X != 4 || C.Y != 5) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("SubOp", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(MulScalar)
	{
		static const char Script[] = R"(
int RunTest()
{
    FIntVector2 A(3, 5);
    FIntVector2 C = A * 2;
    if (C.X != 6 || C.Y != 10) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("MulScalar", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(BitwiseAnd)
	{
		static const char Script[] = R"(
int RunTest()
{
    FIntVector2 A(6, 7);
    FIntVector2 C = A & 3;
    if (C.X != 2 || C.Y != 3) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("BitwiseAnd", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(IsZero)
	{
		static const char Script[] = R"(
int RunTest()
{
    FIntVector2 Zero(0, 0);
    FIntVector2 NonZero(1, 0);
    if (!Zero.IsZero()) { return -1; }
    if (NonZero.IsZero()) { return -2; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("IsZero", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(GetMax)
	{
		static const char Script[] = R"(
int RunTest()
{
    FIntVector2 V(3, 7);
    if (V.GetMax() != 7) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("GetMax", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(GetMin)
	{
		static const char Script[] = R"(
int RunTest()
{
    FIntVector2 V(3, 7);
    if (V.GetMin() != 3) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("GetMin", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(ToString)
	{
		static const char Script[] = R"(
int RunTest()
{
    FIntVector2 V(3, 7);
    FString S = V.ToString();
    if (S.IsEmpty()) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("ToString", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(DivideAndRoundUp)
	{
		static const char Script[] = R"(
int RunTest()
{
    FIntVector2 V = FIntVector2::DivideAndRoundUp(FIntVector2(7, 10), 3);
    if (V.X != 3 || V.Y != 4) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("DivideAndRoundUp", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
