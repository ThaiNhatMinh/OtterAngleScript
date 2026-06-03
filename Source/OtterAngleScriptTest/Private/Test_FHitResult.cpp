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
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("TWeakObjectPtr<UPrimitiveComponent>")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("TWeakObjectPtr<UPhysicalMaterial>")));

		static const char Script[] = R"(
int RunTWeakObjectPtrTest()
{
    TWeakObjectPtr<UPrimitiveComponent> WeakComponent(GetFixtureComponent());
    if (!WeakComponent.IsValid() || WeakComponent.Get() is null)
    {
        return -1;
    }

    TWeakObjectPtr<UPrimitiveComponent> WeakComponentCopy(WeakComponent);
    if (!WeakComponentCopy.opEquals(WeakComponent))
    {
        return -2;
    }

    TWeakObjectPtr<UPhysicalMaterial> WeakMaterial(GetFixturePhysMaterial());
    if (!WeakMaterial.IsValid() || WeakMaterial.Get() is null)
    {
        return -3;
    }

    TWeakObjectPtr<UPhysicalMaterial> WeakMaterialAssigned;
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

	TEST_METHOD(TypeInfo)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FHitResult")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FVector")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FName")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FActorInstanceHandle")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("TWeakObjectPtr<UPrimitiveComponent>")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("TWeakObjectPtr<UPhysicalMaterial>")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("AActor")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("UPrimitiveComponent")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("UPhysicalMaterial")));
	}

	TEST_METHOD(DefaultConstructor)
	{
		static const char Script[] = R"(
int TestDefaultConstructor()
{
    FHitResult Hit;
    if (Hit.IsValidBlockingHit())
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("DefaultConstructor", Script, "int TestDefaultConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(CopyConstructor)
	{
		static const char Script[] = R"(
int TestCopyConstructor()
{
    FHitResult Hit;
    Hit.SetBlockingHit(true);
    Hit.Time = 0.25f;
    Hit.MyItem = 12;
    Hit.Location = FVector(1.0, 2.0, 3.0);
    Hit.Component = TWeakObjectPtr<UPrimitiveComponent>(GetFixtureComponent());
    Hit.PhysMaterial = TWeakObjectPtr<UPhysicalMaterial>(GetFixturePhysMaterial());
    Hit.HitObjectHandle = MakeFixtureActorHandle();
    Hit.BoneName = FName("Head");
    Hit.MyBoneName = FName("Spine_02");

    FHitResult Copy(Hit);
    if (!Copy.IsValidBlockingHit())
    {
        return -1;
    }

    if (Copy.Time != 0.25f)
    {
        return -2;
    }

    if (Copy.MyItem != 12)
    {
        return -3;
    }

    if (Copy.Location.X != 1.0)
    {
        return -4;
    }

    if (!Copy.Component.IsValid() || Copy.Component.Get() is null)
    {
        return -5;
    }

    if (!Copy.PhysMaterial.IsValid() || Copy.PhysMaterial.Get() is null || !Copy.HitObjectHandle.IsValid())
    {
        return -6;
    }

    if (Copy.BoneName.ToString() != "Head" || Copy.MyBoneName.ToString() != "Spine_02")
    {
        return -7;
    }

    return 3;
}
)";

		asIScriptFunction* Function = BuildFunction("CopyConstructor", Script, "int TestCopyConstructor()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 3));
	}

	TEST_METHOD(AssignmentOperator)
	{
		static const char Script[] = R"(
int TestAssignmentOperator()
{
    FHitResult Hit;
    Hit.SetBlockingHit(true);
    Hit.Time = 0.25f;
    Hit.Distance = 42.0f;
    Hit.MyItem = 12;
    Hit.Item = 9;
    Hit.Component = TWeakObjectPtr<UPrimitiveComponent>(GetFixtureComponent());
    Hit.PhysMaterial = TWeakObjectPtr<UPhysicalMaterial>(GetFixturePhysMaterial());
    Hit.HitObjectHandle = MakeFixtureActorHandle();
    Hit.BoneName = FName("Head");
    Hit.MyBoneName = FName("Spine_02");

    FHitResult Assigned;
    Assigned = Hit;
    if (!Assigned.IsValidBlockingHit() || Assigned.Distance != 42.0f || Assigned.Item != 9 || Assigned.MyItem != 12)
    {
        return -1;
    }

    if (!Assigned.Component.IsValid() || Assigned.Component.Get() is null)
    {
        return -2;
    }

    if (!Assigned.PhysMaterial.IsValid() || Assigned.PhysMaterial.Get() is null || !Assigned.HitObjectHandle.IsValid())
    {
        return -3;
    }

    if (Assigned.BoneName.ToString() != "Head" || Assigned.MyBoneName.ToString() != "Spine_02")
    {
        return -4;
    }

    return 2;
}
)";

		asIScriptFunction* Function = BuildFunction("AssignmentOperator", Script, "int TestAssignmentOperator()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 2));
	}

	TEST_METHOD(Init)
	{
		static const char Script[] = R"(
int TestInit()
{
    FHitResult Hit;
    Hit.Init(FVector(10.0, 20.0, 30.0), FVector(40.0, 50.0, 60.0));
    if (Hit.TraceStart.X != 10.0 || Hit.TraceStart.Y != 20.0 || Hit.TraceEnd.Z != 60.0)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("Init", Script, "int TestInit()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Reset)
	{
		static const char Script[] = R"(
int TestReset()
{
    FHitResult Hit;
    Hit.SetBlockingHit(true);
    Hit.Time = 0.5f;
    Hit.Reset(0.5f);
    if (Hit.IsValidBlockingHit())
    {
        return -1;
    }
    if (Hit.Time != 0.5f)
    {
        return -2;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("Reset", Script, "int TestReset()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(IsValidBlockingHit)
	{
		static const char Script[] = R"(
int TestIsValidBlockingHit()
{
    FHitResult Hit;
    Hit.SetBlockingHit(true);
    Hit.Time = 0.25f;
    Hit.ImpactPoint = FVector(1.0, 2.0, 3.0);
    Hit.Normal = FVector(0.0, 0.0, 1.0);
    if (!Hit.IsValidBlockingHit())
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("IsValidBlockingHit", Script, "int TestIsValidBlockingHit()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ToString)
	{
		static const char Script[] = R"(
int TestToString()
{
    FHitResult Hit = MakeFixtureHitResult();
    Hit.SetBlockingHit(true);
    Hit.Time = 0.25f;
    Hit.Location = FVector(1.0, 2.0, 3.0);
    Hit.ImpactPoint = FVector(4.0, 5.0, 6.0);
    Hit.Normal = FVector(0.0, 0.0, 1.0);
    Hit.ImpactNormal = FVector(0.0, 1.0, 0.0);
    FString Description = Hit.ToString();
    if (Description.IsEmpty())
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("ToString", Script, "int TestToString()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(BlockingHit)
	{
		static const char Script[] = R"(
int TestBlockingHit()
{
    FHitResult Hit;
    if (Hit.IsValidBlockingHit())
    {
        return -1;
    }
    Hit.SetBlockingHit(true);
    if (!Hit.IsValidBlockingHit())
    {
        return -2;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("BlockingHit", Script, "int TestBlockingHit()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(StartPenetrating)
	{
		static const char Script[] = R"(
int TestStartPenetrating()
{
    FHitResult Hit;
    Hit.SetStartPenetrating(false);
    if (Hit.IsStartPenetrating())
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("StartPenetrating", Script, "int TestStartPenetrating()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Time)
	{
		static const char Script[] = R"(
int TestTime()
{
    FHitResult Hit;
    Hit.Time = 0.25f;
    if (Hit.Time != 0.25f)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("Time", Script, "int TestTime()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Distance)
	{
		static const char Script[] = R"(
int TestDistance()
{
    FHitResult Hit;
    Hit.Distance = 42.0f;
    if (Hit.Distance != 42.0f)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("Distance", Script, "int TestDistance()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(PenetrationDepth)
	{
		static const char Script[] = R"(
int TestPenetrationDepth()
{
    FHitResult Hit;
    Hit.PenetrationDepth = 1.5f;
    if (Hit.PenetrationDepth != 1.5f)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("PenetrationDepth", Script, "int TestPenetrationDepth()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(FaceIndex)
	{
		static const char Script[] = R"(
int TestFaceIndex()
{
    FHitResult Hit;
    Hit.FaceIndex = 7;
    if (Hit.FaceIndex != 7)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("FaceIndex", Script, "int TestFaceIndex()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Item)
	{
		static const char Script[] = R"(
int TestItem()
{
    FHitResult Hit;
    Hit.Item = 9;
    if (Hit.Item != 9)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("Item", Script, "int TestItem()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(MyItem)
	{
		static const char Script[] = R"(
int TestMyItem()
{
    FHitResult Fixture = MakeFixtureHitResult();
    if (Fixture.MyItem != 11)
    {
        return -1;
    }

    FHitResult Hit;
    Hit.MyItem = 12;
    if (Hit.MyItem != 12)
    {
        return -2;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("MyItem", Script, "int TestMyItem()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ElementIndex)
	{
		static const char Script[] = R"(
int TestElementIndex()
{
    FHitResult Hit;
    Hit.ElementIndex = 3;
    if (Hit.ElementIndex != 3)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("ElementIndex", Script, "int TestElementIndex()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Location)
	{
		static const char Script[] = R"(
int TestLocation()
{
    FHitResult Hit;
    Hit.Location = FVector(1.0, 2.0, 3.0);
    if (Hit.Location.X != 1.0 || Hit.Location.Y != 2.0 || Hit.Location.Z != 3.0)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("Location", Script, "int TestLocation()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ImpactPoint)
	{
		static const char Script[] = R"(
int TestImpactPoint()
{
    FHitResult Hit;
    Hit.ImpactPoint = FVector(4.0, 5.0, 6.0);
    if (Hit.ImpactPoint.X != 4.0 || Hit.ImpactPoint.Y != 5.0 || Hit.ImpactPoint.Z != 6.0)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("ImpactPoint", Script, "int TestImpactPoint()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Normal)
	{
		static const char Script[] = R"(
int TestNormal()
{
    FHitResult Hit;
    Hit.Normal = FVector(0.0, 0.0, 1.0);
    if (Hit.Normal.Z != 1.0)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("Normal", Script, "int TestNormal()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(ImpactNormal)
	{
		static const char Script[] = R"(
int TestImpactNormal()
{
    FHitResult Hit;
    Hit.ImpactNormal = FVector(0.0, 1.0, 0.0);
    if (Hit.ImpactNormal.Y != 1.0)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("ImpactNormal", Script, "int TestImpactNormal()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(TraceStart)
	{
		static const char Script[] = R"(
int TestTraceStart()
{
    FHitResult Hit;
    Hit.TraceStart = FVector(10.0, 20.0, 30.0);
    if (Hit.TraceStart.X != 10.0 || Hit.TraceStart.Y != 20.0)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("TraceStart", Script, "int TestTraceStart()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(TraceEnd)
	{
		static const char Script[] = R"(
int TestTraceEnd()
{
    FHitResult Hit;
    Hit.TraceEnd = FVector(40.0, 50.0, 60.0);
    if (Hit.TraceEnd.Z != 60.0)
    {
        return -1;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("TraceEnd", Script, "int TestTraceEnd()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(Component)
	{
		static const char Script[] = R"(
int TestComponent()
{
    FHitResult Fixture = MakeFixtureHitResult();
    if (!Fixture.Component.IsValid() || Fixture.Component.Get() is null)
    {
        return -1;
    }

    FHitResult Hit;
    Hit.Component = TWeakObjectPtr<UPrimitiveComponent>(GetFixtureComponent());
    if (!Hit.Component.IsValid() || Hit.Component.Get() is null)
    {
        return -2;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("Component", Script, "int TestComponent()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(PhysMaterial)
	{
		static const char Script[] = R"(
int TestPhysMaterial()
{
    FHitResult Fixture = MakeFixtureHitResult();
    if (!Fixture.PhysMaterial.IsValid() || Fixture.PhysMaterial.Get() is null)
    {
        return -1;
    }

    FHitResult Hit;
    Hit.PhysMaterial = TWeakObjectPtr<UPhysicalMaterial>(GetFixturePhysMaterial());
    if (!Hit.PhysMaterial.IsValid() || Hit.PhysMaterial.Get() is null)
    {
        return -2;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("PhysMaterial", Script, "int TestPhysMaterial()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(HitObjectHandle)
	{
		static const char Script[] = R"(
int TestHitObjectHandle()
{
    FHitResult Fixture = MakeFixtureHitResult();
    if (!Fixture.HitObjectHandle.IsValid() || Fixture.HitObjectHandle.FetchActor() is null)
    {
        return -1;
    }

    FHitResult Hit;
    Hit.HitObjectHandle = MakeFixtureActorHandle();
    if (!Hit.HitObjectHandle.IsValid() || Hit.HitObjectHandle.FetchActor() is null)
    {
        return -2;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("HitObjectHandle", Script, "int TestHitObjectHandle()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(BoneName)
	{
		static const char Script[] = R"(
int TestBoneName()
{
    FHitResult Fixture = MakeFixtureHitResult();
    if (Fixture.BoneName.ToString() != "Pelvis")
    {
        return -1;
    }

    FHitResult Hit;
    Hit.BoneName = FName("Head");
    if (Hit.BoneName.ToString() != "Head")
    {
        return -2;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("BoneName", Script, "int TestBoneName()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}

	TEST_METHOD(MyBoneName)
	{
		static const char Script[] = R"(
int TestMyBoneName()
{
    FHitResult Fixture = MakeFixtureHitResult();
    if (Fixture.MyBoneName.ToString() != "Spine_01")
    {
        return -1;
    }

    FHitResult Hit;
    Hit.MyBoneName = FName("Spine_02");
    if (Hit.MyBoneName.ToString() != "Spine_02")
    {
        return -2;
    }
    return 1;
}
)";

		asIScriptFunction* Function = BuildFunction("MyBoneName", Script, "int TestMyBoneName()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 1));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
