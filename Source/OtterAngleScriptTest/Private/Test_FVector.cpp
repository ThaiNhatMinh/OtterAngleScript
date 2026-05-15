// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Math/Vector.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

namespace
{
	FVector UnrealBuildVector(double X, double Y, double Z)
	{
		return FVector(X, Y, Z);
	}

	bool UnrealAcceptsVector(const FVector& Value)
	{
		return Value == FVector(11.0, 12.0, 13.0);
	}

	double UnrealSumVector(const FVector& Value)
	{
		return Value.X + Value.Y + Value.Z;
	}

	bool RegisterFVectorInteropFunctions(asIScriptEngine* Engine)
	{
		static bool bRegistered = false;
		if (bRegistered)
		{
			return true;
		}

		int Result = Engine->RegisterGlobalFunction(
			"FVector UnrealBuildVector(double X, double Y, double Z)",
			asFUNCTION(UnrealBuildVector),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"bool UnrealAcceptsVector(const FVector &in Value)",
			asFUNCTION(UnrealAcceptsVector),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		Result = Engine->RegisterGlobalFunction(
			"double UnrealSumVector(const FVector &in Value)",
			asFUNCTION(UnrealSumVector),
			asCALL_CDECL);
		if (Result < 0)
		{
			return false;
		}

		bRegistered = true;
		return true;
	}
}

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFVectorTests,
	"OtterAngleScript.FVector",
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
		ASSERT_THAT(IsTrue(RegisterFVectorInteropFunctions(Engine)));

		ScriptModule = Engine->GetModule("OtterAngleScriptVectorTest", asGM_ALWAYS_CREATE);
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

	TEST_METHOD(ConstructorsAndProperties)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FVector")));

		static const char Script[] = R"(
int RunVectorConstructorTest()
{
    FVector DefaultValue;
    if (DefaultValue.X != 0.0 || DefaultValue.Y != 0.0 || DefaultValue.Z != 0.0)
    {
        return -1;
    }

    FVector Value(1.5, 2.5, 3.5);
    if (Value.X != 1.5 || Value.Y != 2.5 || Value.Z != 3.5)
    {
        return -2;
    }

    Value.X = 10.0;
    Value.Y = 20.0;
    Value.Z = 30.0;
    if (Value.X != 10.0 || Value.Y != 20.0 || Value.Z != 30.0)
    {
        return -3;
    }

    FVector Copy(Value);
    if (!Copy.opEquals(Value))
    {
        return -4;
    }

    FVector Assigned;
    Assigned = Value;
    if (!Assigned.opEquals(Value))
    {
        return -5;
    }

    return 10;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorConstructors", Script, "int RunVectorConstructorTest()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 10));
	}

	TEST_METHOD(ScriptCallsUnrealWithVector)
	{
		static const char Script[] = R"(
int RunVectorInteropTest()
{
    FVector Value = UnrealBuildVector(11.0, 12.0, 13.0);
    if (!UnrealAcceptsVector(Value))
    {
        return -1;
    }

    if (UnrealSumVector(Value) != 36.0)
    {
        return -2;
    }

    return int(Value.X + Value.Y + Value.Z);
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorInterop", Script, "int RunVectorInteropTest()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 36));
	}

	TEST_METHOD(UnrealCallsScriptWithVector)
	{
		static const char Script[] = R"(
FVector BuildVectorForUnreal(const FVector &in Value)
{
    FVector Result(Value);
    Result.X = Result.X + 1.0;
    Result.Y = Result.Y + 2.0;
    Result.Z = Result.Z + 3.0;
    return Result;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorUnrealCall", Script, "FVector BuildVectorForUnreal(const FVector &in Value)");
		ASSERT_THAT(IsNotNull(Function));

		FVector Input(2.0, 4.0, 6.0);

		Context = Engine->CreateContext();
		ASSERT_THAT(IsNotNull(Context));
		ASSERT_THAT(IsTrue(Context->Prepare(Function) >= 0));
		ASSERT_THAT(IsTrue(Context->SetArgObject(0, &Input) >= 0));

		const int ExecuteResult = Context->Execute();
		if (ExecuteResult == asEXECUTION_EXCEPTION)
		{
			AddError(Context->GetExceptionString());
		}

		ASSERT_THAT(IsTrue(ExecuteResult == asEXECUTION_FINISHED));

		const FVector* ReturnValue = static_cast<const FVector*>(Context->GetAddressOfReturnValue());
		ASSERT_THAT(IsNotNull(ReturnValue));
		ASSERT_THAT(IsTrue(*ReturnValue == FVector(3.0, 6.0, 9.0)));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
