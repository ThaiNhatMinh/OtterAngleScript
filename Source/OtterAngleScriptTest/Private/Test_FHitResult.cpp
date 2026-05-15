// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptFHitResultTests,
	"OtterAngleScript.FHitResult",
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

		ScriptModule = Engine->GetModule("OtterAngleScriptHitResultTest", asGM_ALWAYS_CREATE);
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

	TEST_METHOD(FVectorBindingWorks)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FVector")));

		static const char Script[] = R"(
int RunFVectorTest()
{
    FVector Value(1.0, 2.0, 3.0);
    if (Value.X != 1.0 || Value.Y != 2.0 || Value.Z != 3.0)
    {
        return -1;
    }

    FVector Copy(Value);
    if (!Copy.opEquals(Value))
    {
        return -2;
    }

    FVector Assigned;
    Assigned = Value;
    Assigned.Z = 7.0;
    if (Assigned.X != 1.0 || Assigned.Y != 2.0 || Assigned.Z != 7.0)
    {
        return -3;
    }

    return 10;
}
)";

		asIScriptFunction* Function = BuildFunction("FVectorTests", Script, "int RunFVectorTest()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 10));
	}

	TEST_METHOD(FHitResultMethodsAndProperties)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FHitResult")));

		static const char Script[] = R"(
int RunFHitResultTest()
{
    FHitResult Hit;
    if (Hit.BlockingHit)
    {
        return -1;
    }

    Hit.BlockingHit = true;
    Hit.StartPenetrating = false;
    Hit.Time = 0.25f;
    Hit.Distance = 42.0f;
    Hit.PenetrationDepth = 1.5f;
    Hit.FaceIndex = 7;
    Hit.Item = 9;
    Hit.ElementIndex = 3;
    Hit.Location = FVector(1.0, 2.0, 3.0);
    Hit.ImpactPoint = FVector(4.0, 5.0, 6.0);
    Hit.Normal = FVector(0.0, 0.0, 1.0);
    Hit.ImpactNormal = FVector(0.0, 1.0, 0.0);

    if (!Hit.BlockingHit || Hit.StartPenetrating)
    {
        return -2;
    }

    if (Hit.Time != 0.25f || Hit.Distance != 42.0f || Hit.PenetrationDepth != 1.5f)
    {
        return -3;
    }

    if (Hit.FaceIndex != 7 || Hit.Item != 9 || Hit.ElementIndex != 3)
    {
        return -4;
    }

    if (Hit.Location.X != 1.0 || Hit.Location.Y != 2.0 || Hit.Location.Z != 3.0)
    {
        return -5;
    }

    if (Hit.ImpactPoint.X != 4.0 || Hit.ImpactPoint.Y != 5.0 || Hit.ImpactPoint.Z != 6.0)
    {
        return -6;
    }

    if (Hit.Normal.Z != 1.0 || Hit.ImpactNormal.Y != 1.0)
    {
        return -7;
    }

    if (!Hit.IsValidBlockingHit())
    {
        return -8;
    }

    FString Description = Hit.ToString();
    if (Description.IsEmpty())
    {
        return -9;
    }

    FHitResult Copy(Hit);
    if (!Copy.BlockingHit || Copy.Location.X != 1.0 || Copy.Time != 0.25f)
    {
        return -10;
    }

    FHitResult Assigned;
    Assigned = Hit;
    if (!Assigned.BlockingHit || Assigned.Distance != 42.0f || Assigned.Item != 9)
    {
        return -11;
    }

    Assigned.Init(FVector(10.0, 20.0, 30.0), FVector(40.0, 50.0, 60.0));
    if (Assigned.TraceStart.X != 10.0 || Assigned.TraceStart.Y != 20.0 || Assigned.TraceEnd.Z != 60.0)
    {
        return -12;
    }

    Assigned.Reset(0.5f);
    if (Assigned.BlockingHit)
    {
        return -13;
    }

    if (Assigned.Time != 0.5f)
    {
        return -14;
    }

    return 15;
}
)";

		asIScriptFunction* Function = BuildFunction("FHitResultTests", Script, "int RunFHitResultTest()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 15));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
