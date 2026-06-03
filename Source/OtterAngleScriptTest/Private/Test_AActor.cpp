// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Misc/AssertionMacros.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "angelscript.h"

namespace
{
	AActor* GetFixtureActor1()
	{
		static AActor* Actor = nullptr;
		if (Actor == nullptr)
		{
			Actor = NewObject<AActor>(GetTransientPackage(), NAME_None, RF_Transient);
			Actor->AddToRoot();
		}

		return Actor;
	}

	bool RegisterActorFixtureBindings(asIScriptEngine* Engine)
	{
		static bool bRegistered = false;
		if (bRegistered)
		{
			return true;
		}

		int Result = Engine->RegisterGlobalFunction(
			"AActor GetFixtureActor()",
			asFUNCTION(GetFixtureActor1),
			asCALL_CDECL);
		check(Result >= 0);

		bRegistered = true;
		return true;
	}
}

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptActorTests,
	"OtterAngleScript.Actor",
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

		ASSERT_THAT(IsTrue(RegisterActorFixtureBindings(Engine)));

		ScriptModule = Engine->GetModule("OtterAngleScriptActorTest", asGM_ALWAYS_CREATE);
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

	TEST_METHOD(TypeInfo)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("AActor")));
	}

	TEST_METHOD(NullActor)
	{
		static const char Script[] = R"(
int RunNullActor()
{
    AActor@ Null;
    if (Null !is null)
    {
        return -1;
    }

    if (Null.GetClass() !is null)
    {
        return -2;
    }

    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("ActorNull", Script, "int RunNullActor()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(FixtureActorIsNotNull)
	{
		static const char Script[] = R"(
int RunFixtureActorNotNull()
{
    AActor@ Actor = GetFixtureActor();
    if (Actor is null)
    {
        return -1;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("ActorNotNull", Script, "int RunFixtureActorNotNull()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetClass)
	{
		static const char Script[] = R"(
int RunGetClass()
{
    AActor@ Actor = GetFixtureActor();
    if (Actor is null)
    {
        return -1;
    }
    if (Actor.GetClass() is null)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("ActorGetClass", Script, "int RunGetClass()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetName)
	{
		static const char Script[] = R"(
int RunGetName()
{
    AActor@ Actor = GetFixtureActor();
    if (Actor is null)
    {
        return -1;
    }
    if (Actor.GetName().Len() == 0)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("ActorGetName", Script, "int RunGetName()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetActorLocation)
	{
		static const char Script[] = R"(
int RunGetActorLocation()
{
    AActor@ Actor = GetFixtureActor();
    if (Actor is null)
    {
        return -1;
    }
    FVector Location = Actor.GetActorLocation();
    if (Location.Length() > 0.0f)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("ActorGetLocation", Script, "int RunGetActorLocation()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(SetActorLocation)
	{
		static const char Script[] = R"(
int RunSetActorLocation()
{
    AActor@ Actor = GetFixtureActor();
    if (Actor is null)
    {
        return -1;
    }
    FVector TargetLocation = FVector(100.0f, 200.0f, 300.0f);
    bool bSuccess = Actor.SetActorLocation(TargetLocation, false);
    if (!bSuccess)
    {
        return -2;
    }
    FVector NewLocation = Actor.GetActorLocation();
    if ((NewLocation - TargetLocation).Length() > 0.1f)
    {
        return -3;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("ActorSetLocation", Script, "int RunSetActorLocation()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetActorRotation)
	{
		static const char Script[] = R"(
int RunGetActorRotation()
{
    AActor@ Actor = GetFixtureActor();
    if (Actor is null)
    {
        return -1;
    }
    FRotator Rotation = Actor.GetActorRotation();
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("ActorGetRotation", Script, "int RunGetActorRotation()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(SetActorRotation)
	{
		static const char Script[] = R"(
int RunSetActorRotation()
{
    AActor@ Actor = GetFixtureActor();
    if (Actor is null)
    {
        return -1;
    }
    FRotator TargetRotation = FRotator(90.0f, 45.0f, 0.0f);
    bool bSuccess = Actor.SetActorRotation(TargetRotation);
    if (!bSuccess)
    {
        return -2;
    }
    FRotator NewRotation = Actor.GetActorRotation();
    if ((NewRotation - TargetRotation).Length() > 0.1f)
    {
        return -3;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("ActorSetRotation", Script, "int RunSetActorRotation()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetActorScale3D)
	{
		static const char Script[] = R"(
int RunGetActorScale3D()
{
    AActor@ Actor = GetFixtureActor();
    if (Actor is null)
    {
        return -1;
    }
    FVector Scale = Actor.GetActorScale3D();
    if ((Scale - FVector(1.0f, 1.0f, 1.0f)).Length() > 0.01f)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("ActorGetScale", Script, "int RunGetActorScale3D()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(SetActorScale3D)
	{
		static const char Script[] = R"(
int RunSetActorScale3D()
{
    AActor@ Actor = GetFixtureActor();
    if (Actor is null)
    {
        return -1;
    }
    FVector TargetScale = FVector(2.0f, 2.0f, 2.0f);
    Actor.SetActorScale3D(TargetScale);
    FVector NewScale = Actor.GetActorScale3D();
    if ((NewScale - TargetScale).Length() > 0.01f)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("ActorSetScale", Script, "int RunSetActorScale3D()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetVelocity)
	{
		static const char Script[] = R"(
int RunGetVelocity()
{
    AActor@ Actor = GetFixtureActor();
    if (Actor is null)
    {
        return -1;
    }
    FVector Velocity = Actor.GetVelocity();
    // A newly created actor should have zero velocity.
    if (Velocity.Length() > 0.0f)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("ActorGetVelocity", Script, "int RunGetVelocity()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetActorForwardVector)
	{
		static const char Script[] = R"(
int RunGetActorForwardVector()
{
    AActor@ Actor = GetFixtureActor();
    if (Actor is null)
    {
        return -1;
    }
    FVector Forward = Actor.GetActorForwardVector();
    if (Forward.Length() < 0.9f)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("ActorForwardVector", Script, "int RunGetActorForwardVector()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetActorRightVector)
	{
		static const char Script[] = R"(
int RunGetActorRightVector()
{
    AActor@ Actor = GetFixtureActor();
    if (Actor is null)
    {
        return -1;
    }
    FVector Right = Actor.GetActorRightVector();
    if (Right.Length() < 0.9f)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("ActorRightVector", Script, "int RunGetActorRightVector()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetActorUpVector)
	{
		static const char Script[] = R"(
int RunGetActorUpVector()
{
    AActor@ Actor = GetFixtureActor();
    if (Actor is null)
    {
        return -1;
    }
    FVector Up = Actor.GetActorUpVector();
    if (Up.Length() < 0.9f)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("ActorUpVector", Script, "int RunGetActorUpVector()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}

	TEST_METHOD(GetWorld)
	{
		static const char Script[] = R"(
int RunGetWorld()
{
    AActor@ Actor = GetFixtureActor();
    if (Actor is null)
    {
        return -1;
    }
    // For a transient actor with no outer world, GetWorld() should be null.
    if (Actor.GetWorld() !is null)
    {
        return -2;
    }
    return 0;
}
)";
		asIScriptFunction* Function = BuildFunction("ActorGetWorld", Script, "int RunGetWorld()");
		ASSERT_THAT(IsNotNull(Function));
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 0));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
