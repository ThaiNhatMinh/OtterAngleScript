// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Components/BoxComponent.h"
#include "Engine/ActorInstanceHandle.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Actor.h"
#include "Misc/AssertionMacros.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "angelscript.h"

namespace
{
	AActor* GetFixtureActor()
	{
		static AActor* Actor = nullptr;
		if (Actor == nullptr)
		{
			Actor = NewObject<AActor>(GetTransientPackage(), NAME_None, RF_Transient);
			Actor->AddToRoot();
		}

		return Actor;
	}

	UPrimitiveComponent* GetFixtureComponent()
	{
		static UBoxComponent* Component = nullptr;
		if (Component == nullptr)
		{
			Component = NewObject<UBoxComponent>(GetFixtureActor(), NAME_None, RF_Transient);
			Component->AddToRoot();
		}

		return Component;
	}

	UPhysicalMaterial* GetFixturePhysMaterial()
	{
		static UPhysicalMaterial* PhysMaterial = nullptr;
		if (PhysMaterial == nullptr)
		{
			PhysMaterial = NewObject<UPhysicalMaterial>(GetTransientPackage(), NAME_None, RF_Transient);
			PhysMaterial->AddToRoot();
		}

		return PhysMaterial;
	}

	FActorInstanceHandle MakeFixtureActorHandle()
	{
		return FActorInstanceHandle(GetFixtureActor());
	}

	FHitResult MakeFixtureHitResult()
	{
		FHitResult Hit;
		Hit.MyItem = 11;
		Hit.PhysMaterial = GetFixturePhysMaterial();
		Hit.HitObjectHandle = MakeFixtureActorHandle();
		Hit.Component = GetFixtureComponent();
		Hit.BoneName = FName(TEXT("Pelvis"));
		Hit.MyBoneName = FName(TEXT("Spine_01"));
		return Hit;
	}

	void RegisterFixtureBindings(asIScriptEngine* Engine)
	{
		static bool bRegistered = false;
		if (bRegistered)
		{
			return;
		}

		int Result = Engine->RegisterGlobalFunction("AActor@ GetFixtureActor()", asFUNCTION(GetFixtureActor), asCALL_CDECL);
		check(Result >= 0);
		Result = Engine->RegisterGlobalFunction("UPrimitiveComponent@ GetFixtureComponent()", asFUNCTION(GetFixtureComponent), asCALL_CDECL);
		check(Result >= 0);
		Result = Engine->RegisterGlobalFunction("UPhysicalMaterial@ GetFixturePhysMaterial()", asFUNCTION(GetFixturePhysMaterial), asCALL_CDECL);
		check(Result >= 0);
		Result = Engine->RegisterGlobalFunction("FActorInstanceHandle MakeFixtureActorHandle()", asFUNCTION(MakeFixtureActorHandle), asCALL_CDECL);
		check(Result >= 0);
		Result = Engine->RegisterGlobalFunction("FHitResult MakeFixtureHitResult()", asFUNCTION(MakeFixtureHitResult), asCALL_CDECL);
		check(Result >= 0);

		bRegistered = true;
	}
}

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
		RegisterFixtureBindings(Engine);

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
    Assigned.X = 5.0;
    Assigned.Y = 6.0;
    Assigned.Z = 7.0;
    if (Assigned.X != 5.0 || Assigned.Y != 6.0 || Assigned.Z != 7.0)
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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FName")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FActorInstanceHandle")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("TWeakObjectPtr_UPrimitiveComponent")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("TWeakObjectPtr_UPhysicalMaterial")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("AActor")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("UPrimitiveComponent")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("UPhysicalMaterial")));

		static const char Script[] = R"(
int RunFHitResultTest()
{
    FHitResult Fixture = MakeFixtureHitResult();
    if (Fixture.MyItem != 11)
    {
        return -1;
    }
    if (!Fixture.Component.IsValid() || Fixture.Component.Get() is null)
    {
        return -2;
    }
    if (!Fixture.PhysMaterial.IsValid() || Fixture.PhysMaterial.Get() is null)
    {
        return -3;
    }
    if (!Fixture.HitObjectHandle.IsValid() || Fixture.HitObjectHandle.FetchActor() is null)
    {
        return -4;
    }
    if (Fixture.BoneName.ToString() != "Pelvis" || Fixture.MyBoneName.ToString() != "Spine_01")
    {
        return -5;
    }

    FHitResult Hit;
    if (Hit.BlockingHit)
    {
        return -6;
    }

    Hit.BlockingHit = true;
    Hit.StartPenetrating = false;
    Hit.Time = 0.25f;
    Hit.Distance = 42.0f;
    Hit.PenetrationDepth = 1.5f;
    Hit.FaceIndex = 7;
    Hit.Item = 9;
    Hit.MyItem = 12;
    Hit.ElementIndex = 3;
    Hit.Location = FVector(1.0, 2.0, 3.0);
    Hit.ImpactPoint = FVector(4.0, 5.0, 6.0);
    Hit.Normal = FVector(0.0, 0.0, 1.0);
    Hit.ImpactNormal = FVector(0.0, 1.0, 0.0);
    Hit.Component = TWeakObjectPtr_UPrimitiveComponent(GetFixtureComponent());
    Hit.PhysMaterial = TWeakObjectPtr_UPhysicalMaterial(GetFixturePhysMaterial());
    Hit.HitObjectHandle = MakeFixtureActorHandle();
    Hit.BoneName = FName("Head");
    Hit.MyBoneName = FName("Spine_02");

    if (!Hit.BlockingHit || Hit.StartPenetrating)
    {
        return -7;
    }

    if (Hit.Time != 0.25f || Hit.Distance != 42.0f || Hit.PenetrationDepth != 1.5f)
    {
        return -8;
    }

    if (Hit.FaceIndex != 7 || Hit.Item != 9 || Hit.MyItem != 12 || Hit.ElementIndex != 3)
    {
        return -9;
    }

    if (Hit.Location.X != 1.0 || Hit.Location.Y != 2.0 || Hit.Location.Z != 3.0)
    {
        return -10;
    }

    if (Hit.ImpactPoint.X != 4.0 || Hit.ImpactPoint.Y != 5.0 || Hit.ImpactPoint.Z != 6.0)
    {
        return -11;
    }

    if (Hit.Normal.Z != 1.0 || Hit.ImpactNormal.Y != 1.0)
    {
        return -12;
    }

    if (!Hit.Component.IsValid() || Hit.Component.Get() is null)
    {
        return -13;
    }

    if (!Hit.PhysMaterial.IsValid() || Hit.PhysMaterial.Get() is null)
    {
        return -14;
    }

    if (!Hit.HitObjectHandle.IsValid() || Hit.HitObjectHandle.FetchActor() is null)
    {
        return -15;
    }

    if (Hit.BoneName.ToString() != "Head" || Hit.MyBoneName.ToString() != "Spine_02")
    {
        return -16;
    }

    if (!Hit.IsValidBlockingHit())
    {
        return -17;
    }

    FString Description = Hit.ToString();
    if (Description.IsEmpty())
    {
        return -18;
    }

    FHitResult Copy(Hit);
    if (!Copy.BlockingHit || Copy.Location.X != 1.0 || Copy.Time != 0.25f || Copy.MyItem != 12)
    {
        return -19;
    }

    if (!Copy.Component.IsValid() || Copy.Component.Get() is null)
    {
        return -20;
    }

    if (!Copy.PhysMaterial.IsValid() || Copy.PhysMaterial.Get() is null || !Copy.HitObjectHandle.IsValid())
    {
        return -21;
    }

    if (Copy.BoneName.ToString() != "Head" || Copy.MyBoneName.ToString() != "Spine_02")
    {
        return -22;
    }

    FHitResult Assigned;
    Assigned = Hit;
    if (!Assigned.BlockingHit || Assigned.Distance != 42.0f || Assigned.Item != 9 || Assigned.MyItem != 12)
    {
        return -23;
    }

    if (!Assigned.Component.IsValid() || Assigned.Component.Get() is null)
    {
        return -24;
    }

    if (!Assigned.PhysMaterial.IsValid() || Assigned.PhysMaterial.Get() is null || !Assigned.HitObjectHandle.IsValid())
    {
        return -25;
    }

    if (Assigned.BoneName.ToString() != "Head" || Assigned.MyBoneName.ToString() != "Spine_02")
    {
        return -26;
    }

    Assigned.Init(FVector(10.0, 20.0, 30.0), FVector(40.0, 50.0, 60.0));
    if (Assigned.TraceStart.X != 10.0 || Assigned.TraceStart.Y != 20.0 || Assigned.TraceEnd.Z != 60.0)
    {
        return -27;
    }

    Assigned.Reset(0.5f);
    if (Assigned.BlockingHit)
    {
        return -28;
    }

    if (Assigned.Time != 0.5f)
    {
        return -29;
    }

    return 30;
}
)";

		asIScriptFunction* Function = BuildFunction("FHitResultTests", Script, "int RunFHitResultTest()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 30));
	}

	TEST_METHOD(FNameBindingWorks)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FName")));

		static const char Script[] = R"(
int RunFNameTest()
{
    FName EmptyName;
    if (!EmptyName.IsNone())
    {
        return -1;
    }

    FName BoneName("Pelvis");
    if (BoneName.IsNone())
    {
        return -2;
    }

    if (BoneName.ToString() != "Pelvis")
    {
        return -3;
    }

    FName Copy(BoneName);
    if (!Copy.opEquals(BoneName))
    {
        return -4;
    }

    FName Assigned;
    Assigned = BoneName;
    if (!Assigned.opEquals(BoneName))
    {
        return -5;
    }

    return 6;
}
)";

		asIScriptFunction* Function = BuildFunction("FNameTests", Script, "int RunFNameTest()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 6));
	}

	TEST_METHOD(FActorInstanceHandleBindingWorks)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FActorInstanceHandle")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("AActor")));

		static const char Script[] = R"(
int RunFActorInstanceHandleTest()
{
    AActor@ Actor = GetFixtureActor();
    if (Actor is null)
    {
        return -1;
    }

    FActorInstanceHandle InvalidHandle;
    if (InvalidHandle.IsValid())
    {
        return -2;
    }

    FActorInstanceHandle Handle(Actor);
    if (!Handle.IsValid())
    {
        return -3;
    }

    if (Handle.FetchActor() is null)
    {
        return -4;
    }

    FActorInstanceHandle Copy(Handle);
    if (!Copy.opEquals(Handle))
    {
        return -5;
    }

    FActorInstanceHandle Assigned;
    Assigned = Handle;
    if (!Assigned.opEquals(Handle))
    {
        return -6;
    }

    return 7;
}
)";

		asIScriptFunction* Function = BuildFunction("FActorInstanceHandleTests", Script, "int RunFActorInstanceHandleTest()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 7));
	}

	TEST_METHOD(TWeakObjectPtrBindingWorks)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("TWeakObjectPtr_UPrimitiveComponent")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("TWeakObjectPtr_UPhysicalMaterial")));

		static const char Script[] = R"(
int RunTWeakObjectPtrTest()
{
    TWeakObjectPtr_UPrimitiveComponent WeakComponent(GetFixtureComponent());
    if (!WeakComponent.IsValid() || WeakComponent.Get() is null)
    {
        return -1;
    }

    TWeakObjectPtr_UPrimitiveComponent WeakComponentCopy(WeakComponent);
    if (!WeakComponentCopy.opEquals(WeakComponent))
    {
        return -2;
    }

    TWeakObjectPtr_UPhysicalMaterial WeakMaterial(GetFixturePhysMaterial());
    if (!WeakMaterial.IsValid() || WeakMaterial.Get() is null)
    {
        return -3;
    }

    TWeakObjectPtr_UPhysicalMaterial WeakMaterialAssigned;
    WeakMaterialAssigned = GetFixturePhysMaterial();
    if (!WeakMaterialAssigned.IsValid() || WeakMaterialAssigned.Get() is null)
    {
        return -4;
    }

    return 5;
}
)";

		asIScriptFunction* Function = BuildFunction("TWeakObjectPtrTests", Script, "int RunTWeakObjectPtrTest()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 5));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
