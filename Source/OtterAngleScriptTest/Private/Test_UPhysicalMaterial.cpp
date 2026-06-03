// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS
#if WITH_EDITOR

#include "CQTest.h"
#include "Misc/AssertionMacros.h"
#include "Modules/ModuleManager.h"
#include "OtterAngleScript.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "PhysicsSettingsEnums.h"
#include "angelscript.h"

namespace
{
	UPhysicalMaterial* GetTestUPhysicalMaterial()
	{
		static UPhysicalMaterial* PhysMaterial = nullptr;
		if (PhysMaterial == nullptr)
		{
			PhysMaterial = NewObject<UPhysicalMaterial>(GetTransientPackage(), NAME_None, RF_Transient);
			PhysMaterial->AddToRoot();
		}
		return PhysMaterial;
	}

	void RegisterUPhysicalMaterialFixture(asIScriptEngine* Engine)
	{
		static bool bRegistered = false;
		if (bRegistered)
		{
			return;
		}

		int Result = Engine->RegisterGlobalFunction("UPhysicalMaterial@ GetTestUPhysicalMaterial()", asFUNCTION(GetTestUPhysicalMaterial), asCALL_CDECL);
		check(Result >= 0);

		bRegistered = true;
	}
}

TEST_CLASS_WITH_FLAGS(
	FOtterAngleScriptUPhysicalMaterialTests,
	"OtterAngleScript.UPhysicalMaterial",
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

	int32 ExecuteIntFunction(asIScriptFunction* Function)
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

		const int Result = Context->Execute();
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
		RegisterUPhysicalMaterialFixture(Engine);

		ScriptModule = Engine->GetModule("OtterAngleScriptUPhysicalMaterialTest", asGM_ALWAYS_CREATE);
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

	TEST_METHOD(TypesRegistered)
	{
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("UPhysicalMaterial")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FPhysicalMaterialStrength")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("FPhysicalMaterialDamageModifier")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("EFrictionCombineMode")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("EPhysicalSurface")));
		ASSERT_THAT(IsNotNull(Engine->GetTypeInfoByDecl("EPhysicalMaterialSoftCollisionMode")));
	}

	TEST_METHOD(FrictionCombineModeEnumBinding)
	{
		static const char Script[] = R"(
int RunFrictionCombineModeTest()
{
    if (EFrictionCombineMode::Average != 0) return -1;
    if (EFrictionCombineMode::Min != 1)     return -2;
    if (EFrictionCombineMode::Multiply != 2) return -3;
    if (EFrictionCombineMode::Max != 3)     return -4;
    return 10;
}
)";
		asIScriptFunction* Function = BuildFunction("FrictionCombineModeTest", Script, "int RunFrictionCombineModeTest()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 10));
	}

	TEST_METHOD(PhysicalSurfaceEnumBinding)
	{
		static const char Script[] = R"(
int RunPhysicalSurfaceTest()
{
    if (EPhysicalSurface::SurfaceType_Default != 0)  return -1;
    if (EPhysicalSurface::SurfaceType1 != 1)         return -2;
    if (EPhysicalSurface::SurfaceType_Max != 63)     return -3;
    return 10;
}
)";
		asIScriptFunction* Function = BuildFunction("PhysicalSurfaceTest", Script, "int RunPhysicalSurfaceTest()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 10));
	}

	TEST_METHOD(SoftCollisionModeEnumBinding)
	{
		static const char Script[] = R"(
int RunSoftCollisionModeTest()
{
    if (EPhysicalMaterialSoftCollisionMode::None != 0)              return -1;
    if (EPhysicalMaterialSoftCollisionMode::RelativeThickness != 1) return -2;
    if (EPhysicalMaterialSoftCollisionMode::AbsoluteThickess != 2)  return -3;
    return 10;
}
)";
		asIScriptFunction* Function = BuildFunction("SoftCollisionModeTest", Script, "int RunSoftCollisionModeTest()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 10));
	}

	TEST_METHOD(SurfacePropertiesBinding)
	{
		static const char Script[] = R"(
int RunSurfacePropertiesTest()
{
    UPhysicalMaterial Mat = GetTestUPhysicalMaterial();
    Mat.Friction = 0.7f;
    Mat.StaticFriction = 0.6f;
    Mat.bOverrideFrictionCombineMode = true;
    Mat.FrictionCombineMode = EFrictionCombineMode::Multiply;
    Mat.Restitution = 0.3f;
    Mat.bOverrideRestitutionCombineMode = true;
    Mat.RestitutionCombineMode = EFrictionCombineMode::Min;

    if (Mat.Friction != 0.7f)                              return -1;
    if (Mat.StaticFriction != 0.6f)                        return -2;
    if (!Mat.bOverrideFrictionCombineMode)                 return -3;
    if (Mat.FrictionCombineMode != EFrictionCombineMode::Multiply) return -4;
    if (Mat.Restitution != 0.3f)                           return -5;
    if (!Mat.bOverrideRestitutionCombineMode)              return -6;
    if (Mat.RestitutionCombineMode != EFrictionCombineMode::Min)   return -7;

    return 10;
}
)";
		asIScriptFunction* Function = BuildFunction("SurfacePropertiesTest", Script, "int RunSurfacePropertiesTest()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 10));
	}

	TEST_METHOD(ObjectPropertiesBinding)
	{
		static const char Script[] = R"(
int RunObjectPropertiesTest()
{
    UPhysicalMaterial Mat = GetTestUPhysicalMaterial();
    Mat.Density = 1.5f;
    Mat.SleepLinearVelocityThreshold = 0.01f;
    Mat.SleepAngularVelocityThreshold = 0.02f;
    Mat.SleepCounterThreshold = 4;
    Mat.RaiseMassToPower = 0.75f;

    if (Mat.Density != 1.5f)                          return -1;
    if (Mat.SleepLinearVelocityThreshold != 0.01f)    return -2;
    if (Mat.SleepAngularVelocityThreshold != 0.02f)   return -3;
    if (Mat.SleepCounterThreshold != 4)               return -4;
    if (Mat.RaiseMassToPower != 0.75f)                return -5;

    return 10;
}
)";
		asIScriptFunction* Function = BuildFunction("ObjectPropertiesTest", Script, "int RunObjectPropertiesTest()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 10));
	}

	TEST_METHOD(SurfaceTypeBinding)
	{
		static const char Script[] = R"(
int RunSurfaceTypeTest()
{
    UPhysicalMaterial Mat = GetTestUPhysicalMaterial();
    Mat.SurfaceType = EPhysicalSurface::SurfaceType1;
    if (Mat.SurfaceType != EPhysicalSurface::SurfaceType1) return -1;

    Mat.Strength.TensileStrength = 10.0f;
    if (Mat.Strength.TensileStrength != 10.0f)             return -2;

    Mat.DamageModifier.DamageThresholdMultiplier = 2.0f;
    if (Mat.DamageModifier.DamageThresholdMultiplier != 2.0f) return -3;

    return 10;
}
)";
		asIScriptFunction* Function = BuildFunction("SurfaceTypeTest", Script, "int RunSurfaceTypeTest()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 10));
	}

	TEST_METHOD(DebugColorBinding)
	{
		static const char Script[] = R"(
int RunDebugColorTest()
{
    UPhysicalMaterial Mat = GetTestUPhysicalMaterial();
    Mat.DebugColor = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);
    if (Mat.DebugColor.R != 1.0f) return -1;
    if (Mat.DebugColor.G != 0.0f) return -2;
    if (Mat.DebugColor.B != 0.0f) return -3;
    return 10;
}
)";
		asIScriptFunction* Function = BuildFunction("DebugColorTest", Script, "int RunDebugColorTest()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 10));
	}

	TEST_METHOD(ExperimentalPropertiesBinding)
	{
		static const char Script[] = R"(
int RunExperimentalPropertiesTest()
{
    UPhysicalMaterial Mat = GetTestUPhysicalMaterial();
    Mat.bShowExperimentalProperties = true;
    Mat.SoftCollisionMode = EPhysicalMaterialSoftCollisionMode::RelativeThickness;
    Mat.SoftCollisionThickness = 0.05f;
    Mat.BaseFrictionImpulse = 1.0f;

    if (!Mat.bShowExperimentalProperties)                                             return -1;
    if (Mat.SoftCollisionMode != EPhysicalMaterialSoftCollisionMode::RelativeThickness) return -2;
    if (Mat.SoftCollisionThickness != 0.05f)                                          return -3;
    if (Mat.BaseFrictionImpulse != 1.0f)                                              return -4;

    return 10;
}
)";
		asIScriptFunction* Function = BuildFunction("ExperimentalPropertiesTest", Script, "int RunExperimentalPropertiesTest()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 10));
	}

	TEST_METHOD(DetermineSurfaceTypeBinding)
	{
		static const char Script[] = R"(
int RunDetermineSurfaceTypeTest()
{
    UPhysicalMaterial Mat = GetTestUPhysicalMaterial();
    Mat.SurfaceType = EPhysicalSurface::SurfaceType1;
    EPhysicalSurface Surface = UPhysicalMaterial::DetermineSurfaceType(Mat);
    if (Surface != EPhysicalSurface::SurfaceType1) return -1;
    return 10;
}
)";
		asIScriptFunction* Function = BuildFunction("DetermineSurfaceTypeTest", Script, "int RunDetermineSurfaceTypeTest()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 10));
	}

	TEST_METHOD(UObjectMethodsBinding)
	{
		static const char Script[] = R"(
int RunUObjectMethodsTest()
{
    UPhysicalMaterial Mat = GetTestUPhysicalMaterial();
    if (!Mat.IsValidLowLevel())    return -1;
    if (Mat.GetName().IsEmpty())   return -2;
    if (Mat.GetClass() is null)    return -3;
    return 10;
}
)";
		asIScriptFunction* Function = BuildFunction("UObjectMethodsTest", Script, "int RunUObjectMethodsTest()");
		ASSERT_THAT(IsTrue(ExecuteIntFunction(Function) == 10));
	}
};

#endif // WITH_EDITOR
#endif // WITH_DEV_AUTOMATION_TESTS
