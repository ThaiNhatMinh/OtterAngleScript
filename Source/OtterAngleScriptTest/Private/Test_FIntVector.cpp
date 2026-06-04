// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Math/IntVector.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFIntVectorTests,
	"OtterAngleScript.FIntVector",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptIntVecTest", asGM_ALWAYS_CREATE);
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FIntVector")));
	}

	TEST_METHOD(DefaultConstruct)
	{
		static const char Script[] = R"(
int RunDefaultConstruct()
{
    FIntVector V;
    if (V.X != 0 || V.Y != 0 || V.Z != 0)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("IntVecDefaultCtor", Script, "int RunDefaultConstruct()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(XYZConstruct)
	{
		static const char Script[] = R"(
int RunXYZConstruct()
{
    FIntVector V(1, 2, 3);
    if (V.X != 1 || V.Y != 2 || V.Z != 3)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("IntVecXYZCtor", Script, "int RunXYZConstruct()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(Properties)
	{
		static const char Script[] = R"(
int RunProperties()
{
    FIntVector V;
    V.X = 5;
    V.Y = 10;
    V.Z = 15;
    if (V.X != 5 || V.Y != 10 || V.Z != 15)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("IntVecProperties", Script, "int RunProperties()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(AddBinding)
	{
		static const char Script[] = R"(
int RunAddBinding()
{
    FIntVector A(1, 2, 3);
    FIntVector B(4, 5, 6);
    FIntVector C = A + B;
    if (C.X != 5 || C.Y != 7 || C.Z != 9)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("IntVecAdd", Script, "int RunAddBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(SubBinding)
	{
		static const char Script[] = R"(
int RunSubBinding()
{
    FIntVector A(5, 7, 9);
    FIntVector B(1, 2, 3);
    FIntVector C = A - B;
    if (C.X != 4 || C.Y != 5 || C.Z != 6)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("IntVecSub", Script, "int RunSubBinding()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(MulScalar)
	{
		static const char Script[] = R"(
int RunMulScalar()
{
    FIntVector A(2, 3, 4);
    FIntVector B = A * 3;
    if (B.X != 6 || B.Y != 9 || B.Z != 12)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("IntVecMulScalar", Script, "int RunMulScalar()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(BitwiseAnd)
	{
		static const char Script[] = R"(
int RunBitwiseAnd()
{
    FIntVector A(6, 7, 8);
    FIntVector B = A & 3;
    if (B.X != 2 || B.Y != 3 || B.Z != 0)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("IntVecBitAnd", Script, "int RunBitwiseAnd()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(IsZero)
	{
		static const char Script[] = R"(
int RunIsZero()
{
    FIntVector A(0, 0, 0);
    FIntVector B(1, 0, 0);
    if (!A.IsZero())
    {
        return -1;
    }
    if (B.IsZero())
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("IntVecIsZero", Script, "int RunIsZero()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetMax)
	{
		static const char Script[] = R"(
int RunGetMax()
{
    FIntVector V(1, 5, 3);
    if (V.GetMax() != 5)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("IntVecGetMax", Script, "int RunGetMax()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetMin)
	{
		static const char Script[] = R"(
int RunGetMin()
{
    FIntVector V(4, 2, 7);
    if (V.GetMin() != 2)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("IntVecGetMin", Script, "int RunGetMin()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(Size)
	{
		static const char Script[] = R"(
int RunSize()
{
    FIntVector V(3, 4, 0);
    if (V.Size() != 5)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("IntVecSize", Script, "int RunSize()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(ToString)
	{
		static const char Script[] = R"(
int RunToString()
{
    FIntVector V(1, 2, 3);
    FString S = V.ToString();
    if (S != "X=1 Y=2 Z=3")
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("IntVecToString", Script, "int RunToString()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}
};

#endif
#endif
