// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Math/IntPoint.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFIntPointTests,
	"OtterAngleScript.FIntPoint",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptIntPointTest", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FIntPoint")));
	}

	TEST_METHOD(DefaultConstruct)
	{
		static const char Script[] = R"(
int RunDefaultConstruct()
{
    FIntPoint P;
    if (P.X != 0 || P.Y != 0)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("IntPointDefaultCtor", Script, "int RunDefaultConstruct()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(XYConstruct)
	{
		static const char Script[] = R"(
int RunXYConstruct()
{
    FIntPoint P(3, 7);
    if (P.X != 3 || P.Y != 7)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("IntPointXYCtor", Script, "int RunXYConstruct()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(Properties)
	{
		static const char Script[] = R"(
int RunProperties()
{
    FIntPoint P;
    P.X = 5;
    P.Y = 10;
    if (P.X != 5 || P.Y != 10)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("IntPointProperties", Script, "int RunProperties()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(AddBinding)
	{
		static const char Script[] = R"(
int RunAddBinding()
{
    FIntPoint A(1, 2);
    FIntPoint B(4, 6);
    FIntPoint C = A + B;
    if (C.X != 5 || C.Y != 8)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("IntPointAdd", Script, "int RunAddBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(SubBinding)
	{
		static const char Script[] = R"(
int RunSubBinding()
{
    FIntPoint A(5, 8);
    FIntPoint B(1, 2);
    FIntPoint C = A - B;
    if (C.X != 4 || C.Y != 6)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("IntPointSub", Script, "int RunSubBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(MulScalar)
	{
		static const char Script[] = R"(
int RunMulScalar()
{
    FIntPoint A(2, 3);
    FIntPoint B = A * 3;
    if (B.X != 6 || B.Y != 9)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("IntPointMulScalar", Script, "int RunMulScalar()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IndexOperator)
	{
		static const char Script[] = R"(
int RunIndexOperator()
{
    FIntPoint P(7, 11);
    if (P[0] != 7 || P[1] != 11)
    {
        return -1;
    }
    P[0] = 3;
    if (P[0] != 3)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("IntPointIndexOp", Script, "int RunIndexOperator()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ComponentMin)
	{
		static const char Script[] = R"(
int RunComponentMin()
{
    FIntPoint A(5, 8);
    FIntPoint B(3, 10);
    FIntPoint C = A.ComponentMin(B);
    if (C.X != 3 || C.Y != 8)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("IntPointCompMin", Script, "int RunComponentMin()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetMax)
	{
		static const char Script[] = R"(
int RunGetMax()
{
    FIntPoint P(3, 8);
    if (P.GetMax() != 8)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("IntPointGetMax", Script, "int RunGetMax()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(Size)
	{
		static const char Script[] = R"(
int RunSize()
{
    FIntPoint P(3, 4);
    if (P.Size() != 5)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("IntPointSize", Script, "int RunSize()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(SizeSquared)
	{
		static const char Script[] = R"(
int RunSizeSquared()
{
    FIntPoint P(3, 4);
    if (P.SizeSquared() != 25)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("IntPointSizeSq", Script, "int RunSizeSquared()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ToString)
	{
		static const char Script[] = R"(
int RunToString()
{
    FIntPoint P(1, 2);
    FString S = P.ToString();
    if (S != "X=1 Y=2")
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("IntPointToString", Script, "int RunToString()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(DivideAndRoundUp)
	{
		static const char Script[] = R"(
int RunDivideAndRoundUp()
{
    FIntPoint P(10, 15);
    FIntPoint Result = FIntPoint::DivideAndRoundUp(P, 4);
    if (Result.X != 3 || Result.Y != 4)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("IntPointDivRoundUp", Script, "int RunDivideAndRoundUp()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}
};

#endif
#endif
