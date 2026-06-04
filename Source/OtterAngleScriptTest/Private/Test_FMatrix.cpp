// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"
#include "Math/Plane.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFMatrixTests,
	"OtterAngleScript.FMatrix",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptMatrixTest", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FMatrix")));
	}

	TEST_METHOD(DefaultConstruct)
	{
		static const char Script[] = R"(
int RunDefaultConstruct()
{
    FMatrix M;
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FMatrixDefaultCtor", Script, "int RunDefaultConstruct()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(VectorConstruct)
	{
		static const char Script[] = R"(
int RunVectorConstruct()
{
    FVector X(1.0, 0.0, 0.0);
    FVector Y(0.0, 1.0, 0.0);
    FVector Z(0.0, 0.0, 1.0);
    FVector W(0.0, 0.0, 0.0);
    FMatrix M(X, Y, Z, W);
    if (M.GetColumn(0).X != 1.0 || M.GetColumn(1).Y != 1.0 || M.GetColumn(2).Z != 1.0)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FMatrixVectorCtor", Script, "int RunVectorConstruct()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(opEquals)
	{
		static const char Script[] = R"(
int RunOpEquals()
{
    FVector X(1.0, 0.0, 0.0);
    FVector Y(0.0, 1.0, 0.0);
    FVector Z(0.0, 0.0, 1.0);
    FVector W(0.0, 0.0, 0.0);
    FMatrix A(X, Y, Z, W);
    FMatrix B(X, Y, Z, W);
    FMatrix C;
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
		asIScriptFunction* Function = BuildFunction("FMatrixOpEquals", Script, "int RunOpEquals()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(opMul)
	{
		static const char Script[] = R"(
int RunOpMul()
{
    FVector X(1.0, 0.0, 0.0);
    FVector Y(0.0, 1.0, 0.0);
    FVector Z(0.0, 0.0, 1.0);
    FVector W(0.0, 0.0, 0.0);
    FMatrix A(X, Y, Z, W);
    FMatrix B = A * A;
    if (!(B == A))
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FMatrixOpMul", Script, "int RunOpMul()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(Determinant)
	{
		static const char Script[] = R"(
int RunDeterminant()
{
    FVector X(2.0, 0.0, 0.0);
    FVector Y(0.0, 3.0, 0.0);
    FVector Z(0.0, 0.0, 4.0);
    FVector W(0.0, 0.0, 0.0);
    FMatrix M(X, Y, Z, W);
    double Det = M.Determinant();
    if (Det != 24.0)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FMatrixDeterminant", Script, "int RunDeterminant()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(Inverse)
	{
		static const char Script[] = R"(
int RunInverse()
{
    FVector X(2.0, 0.0, 0.0);
    FVector Y(0.0, 3.0, 0.0);
    FVector Z(0.0, 0.0, 4.0);
    FVector W(1.0, 2.0, 3.0);
    FMatrix M(X, Y, Z, W);
    FMatrix Inv = M.Inverse();
    FMatrix Identity = M * Inv;
    FVector IX(1.0, 0.0, 0.0);
    FVector IY(0.0, 1.0, 0.0);
    FVector IZ(0.0, 0.0, 1.0);
    FVector IW(0.0, 0.0, 0.0);
    FMatrix ExpectedIdentity(IX, IY, IZ, IW);
    if (!Identity.Equals(ExpectedIdentity, 0.001))
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FMatrixInverse", Script, "int RunInverse()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(Rotator)
	{
		static const char Script[] = R"(
int RunRotator()
{
    FVector X(1.0, 0.0, 0.0);
    FVector Y(0.0, 1.0, 0.0);
    FVector Z(0.0, 0.0, 1.0);
    FVector W(0.0, 0.0, 0.0);
    FMatrix M(X, Y, Z, W);
    FRotator R = M.Rotator();
    if (R.Pitch != 0.0 || R.Yaw != 0.0 || R.Roll != 0.0)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FMatrixRotator", Script, "int RunRotator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ToString)
	{
		static const char Script[] = R"(
int RunToString()
{
    FMatrix M;
    FString Str = M.ToString();
    if (Str.IsEmpty())
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FMatrixToString", Script, "int RunToString()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(Identity)
	{
		static const char Script[] = R"(
int RunIdentity()
{
    FMatrix M = FMatrix.Identity;
    FVector IX(1.0, 0.0, 0.0);
    FVector IY(0.0, 1.0, 0.0);
    FVector IZ(0.0, 0.0, 1.0);
    FVector IW(0.0, 0.0, 0.0);
    FMatrix Expected(IX, IY, IZ, IW);
    if (!(M == Expected))
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("FMatrixIdentity", Script, "int RunIdentity()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}
};

#endif
#endif
