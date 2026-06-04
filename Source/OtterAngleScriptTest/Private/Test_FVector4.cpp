// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Math/Vector4.h"
#include "Math/Color.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

namespace
{
	static bool UnrealAcceptsVector4(const FVector4& Value)
	{
		return Value.Equals(FVector4(1.0, 2.0, 3.0, 4.0), KINDA_SMALL_NUMBER);
	}
}

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFVector4Tests,
	"OtterAngleScript.FVector4",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptVec4Test", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FVector4")));
	}

	TEST_METHOD(DefaultConstruct)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4 V;
    if (V.X != 0.0 || V.Y != 0.0 || V.Z != 0.0 || V.W != 1.0) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("DefaultCtor", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(XYZWConstruct)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4 V(1.0, 2.0, 3.0, 4.0);
    if (V.X != 1.0 || V.Y != 2.0 || V.Z != 3.0 || V.W != 4.0) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("XYZWCtor", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(Properties)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4 V(1.0, 2.0, 3.0, 4.0);
    V.X = 10.0;
    V.Y = 20.0;
    V.Z = 30.0;
    V.W = 40.0;
    if (V.X != 10.0 || V.Y != 20.0 || V.Z != 30.0 || V.W != 40.0) { return -1; }
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
    FVector4 A(1.0, 2.0, 3.0, 4.0);
    FVector4 B(5.0, 6.0, 7.0, 8.0);
    FVector4 C = A + B;
    if (C.X != 6.0 || C.Y != 8.0 || C.Z != 10.0 || C.W != 12.0) { return -1; }
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
    FVector4 A(5.0, 6.0, 7.0, 8.0);
    FVector4 B(1.0, 2.0, 3.0, 4.0);
    FVector4 C = A - B;
    if (C.X != 4.0 || C.Y != 4.0 || C.Z != 4.0 || C.W != 4.0) { return -1; }
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
    FVector4 A(1.0, 2.0, 3.0, 4.0);
    FVector4 B = A * 2.0;
    if (B.X != 2.0 || B.Y != 4.0 || B.Z != 6.0 || B.W != 8.0) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("MulScalar", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(opEquals)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4 A(1.0, 2.0, 3.0, 4.0);
    FVector4 B(1.0, 2.0, 3.0, 4.0);
    FVector4 C(1.0, 2.0, 3.0, 5.0);
    if (!(A == B)) { return -1; }
    if (A == C) { return -2; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("EqualsOp", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(IndexOperator)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4 V(1.0, 2.0, 3.0, 4.0);
    if (V[0] != 1.0) { return -1; }
    if (V[1] != 2.0) { return -2; }
    if (V[2] != 3.0) { return -3; }
    if (V[3] != 4.0) { return -4; }
    V[0] = 10.0;
    if (V.X != 10.0) { return -5; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("IndexOp", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(Size)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4 V(3.0, 4.0, 0.0, 0.0);
    if (!FMath::IsNearlyEqual(V.Size3(), 5.0, 0.001)) { return -1; }
    if (!FMath::IsNearlyEqual(V.SizeSquared3(), 25.0, 0.001)) { return -2; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("Size", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(ContainsNaN)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4 V(1.0, 2.0, 3.0, 4.0);
    if (V.ContainsNaN()) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("ContainsNaN", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(ToString)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4 V(1.0, 2.0, 3.0, 4.0);
    FString S = V.ToString();
    if (S.IsEmpty()) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("ToString", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(static_Zero)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4 Zero = FVector4::Zero();
    if (Zero.X != 0.0 || Zero.Y != 0.0 || Zero.Z != 0.0 || Zero.W != 0.0) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("StaticZero", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(static_One)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4 One = FVector4::One();
    if (One.X != 1.0 || One.Y != 1.0 || One.Z != 1.0 || One.W != 1.0) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("StaticOne", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(CrossOperator)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4 A(1.0, 0.0, 0.0, 0.0);
    FVector4 B(0.0, 1.0, 0.0, 0.0);
    FVector4 C = A ^ B;
    if (C.X != 0.0 || C.Y != 0.0 || C.Z != 1.0) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("CrossOp", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(ComponentMin)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4 A(1.0, 5.0, 2.0, 6.0);
    FVector4 B(3.0, 2.0, 4.0, 1.0);
    FVector4 MinV = A.ComponentMin(B);
    if (MinV.X != 1.0 || MinV.Y != 2.0 || MinV.Z != 2.0 || MinV.W != 1.0) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("ComponentMin", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}

	TEST_METHOD(Set)
	{
		static const char Script[] = R"(
int RunTest()
{
    FVector4 V;
    V.Set(5.0, 6.0, 7.0, 8.0);
    if (V.X != 5.0 || V.Y != 6.0 || V.Z != 7.0 || V.W != 8.0) { return -1; }
    return 0;
}
)";
		asIScriptFunction* F = BuildFunction("SetMethod", Script, "int RunTest()");
		ASSERT_THAT(IsNotNull(F));
		ASSERT_THAT(AreEqual(0, ExecuteIntFunction(F)));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
