// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Chaos/ChaosEngineInterface.h"
#include "Misc/AssertionMacros.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "PhysicsSettingsEnums.h"
#include "UObject/Class.h"
#include "angelscript.h"

#include <new>

namespace
{
	// --- UPhysicalMaterial TEnumAsByte getters/setters ---

	int UPhysicalMaterial_GetFrictionCombineMode(const UPhysicalMaterial* Mat)
	{
		return static_cast<int>(Mat->FrictionCombineMode.GetValue());
	}

	void UPhysicalMaterial_SetFrictionCombineMode(UPhysicalMaterial* Mat, int Value)
	{
		Mat->FrictionCombineMode = static_cast<EFrictionCombineMode::Type>(Value);
	}

	int UPhysicalMaterial_GetRestitutionCombineMode(const UPhysicalMaterial* Mat)
	{
		return static_cast<int>(Mat->RestitutionCombineMode.GetValue());
	}

	void UPhysicalMaterial_SetRestitutionCombineMode(UPhysicalMaterial* Mat, int Value)
	{
		Mat->RestitutionCombineMode = static_cast<EFrictionCombineMode::Type>(Value);
	}

	int UPhysicalMaterial_GetSurfaceType(const UPhysicalMaterial* Mat)
	{
		return static_cast<int>(Mat->SurfaceType.GetValue());
	}

	void UPhysicalMaterial_SetSurfaceType(UPhysicalMaterial* Mat, int Value)
	{
		Mat->SurfaceType = static_cast<EPhysicalSurface>(Value);
	}

	int UPhysicalMaterial_GetSoftCollisionMode(const UPhysicalMaterial* Mat)
	{
		return static_cast<int>(Mat->SoftCollisionMode);
	}

	void UPhysicalMaterial_SetSoftCollisionMode(UPhysicalMaterial* Mat, int Value)
	{
		Mat->SoftCollisionMode = static_cast<EPhysicalMaterialSoftCollisionMode>(Value);
	}

	// --- UPhysicalMaterial UObject base method wrappers ---

	FName UPhysicalMaterial_GetFName(const UPhysicalMaterial* Value)
	{
		return Value->GetFName();
	}

	FString UPhysicalMaterial_GetName(const UPhysicalMaterial* Value)
	{
		return Value->GetName();
	}

	FString UPhysicalMaterial_GetPathName(const UPhysicalMaterial* Value)
	{
		return Value->GetPathName();
	}

	FString UPhysicalMaterial_GetFullName(const UPhysicalMaterial* Value)
	{
		return Value->GetFullName();
	}

	UObject* UPhysicalMaterial_GetOuter(const UPhysicalMaterial* Value)
	{
		return Value->GetOuter();
	}

	UClass* UPhysicalMaterial_GetClass(const UPhysicalMaterial* Value)
	{
		return Value->GetClass();
	}

	bool UPhysicalMaterial_IsA(const UPhysicalMaterial* Value, const UClass* SomeBase)
	{
		return Value->IsA(SomeBase);
	}

	bool UPhysicalMaterial_IsValidLowLevel(const UPhysicalMaterial* Value)
	{
		return Value->IsValidLowLevel();
	}

	bool UPhysicalMaterial_IsAsset(const UPhysicalMaterial* Value)
	{
		return Value->IsAsset();
	}
}

void Bind_UPhysicalMaterial(asIScriptEngine* Engine)
{
	check(Engine != nullptr);
	int Result;

	// --- EFrictionCombineMode enum ---

	Result = Engine->RegisterEnum("EFrictionCombineMode");
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EFrictionCombineMode", "Average", static_cast<int>(EFrictionCombineMode::Average));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EFrictionCombineMode", "Min", static_cast<int>(EFrictionCombineMode::Min));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EFrictionCombineMode", "Multiply", static_cast<int>(EFrictionCombineMode::Multiply));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EFrictionCombineMode", "Max", static_cast<int>(EFrictionCombineMode::Max));
	check(Result >= 0);

	// --- EPhysicalSurface enum ---

	Result = Engine->RegisterEnum("EPhysicalSurface");
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EPhysicalSurface", "SurfaceType_Default", static_cast<int>(SurfaceType_Default));
	check(Result >= 0);
	{
		char NameBuf[32];
		for (int i = 1; i <= 62; ++i)
		{
			FCStringAnsi::Snprintf(NameBuf, sizeof(NameBuf), "SurfaceType%d", i);
			Result = Engine->RegisterEnumValue("EPhysicalSurface", NameBuf, i);
			check(Result >= 0);
		}
	}
	Result = Engine->RegisterEnumValue("EPhysicalSurface", "SurfaceType_Max", static_cast<int>(SurfaceType_Max));
	check(Result >= 0);

	// --- EPhysicalMaterialSoftCollisionMode enum ---

	Result = Engine->RegisterEnum("EPhysicalMaterialSoftCollisionMode");
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EPhysicalMaterialSoftCollisionMode", "None", static_cast<int>(EPhysicalMaterialSoftCollisionMode::None));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EPhysicalMaterialSoftCollisionMode", "RelativeThickness", static_cast<int>(EPhysicalMaterialSoftCollisionMode::RelativeThickness));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EPhysicalMaterialSoftCollisionMode", "AbsoluteThickess", static_cast<int>(EPhysicalMaterialSoftCollisionMode::AbsoluteThickess));
	check(Result >= 0);

	// --- FPhysicalMaterialStrength ---

	REGISTER_PROPERTY(FPhysicalMaterialStrength, "float TensileStrength", TensileStrength);
	REGISTER_PROPERTY(FPhysicalMaterialStrength, "float CompressionStrength", CompressionStrength);
	REGISTER_PROPERTY(FPhysicalMaterialStrength, "float ShearStrength", ShearStrength);

	// --- FPhysicalMaterialDamageModifier ---

	REGISTER_PROPERTY(FPhysicalMaterialDamageModifier, "float DamageThresholdMultiplier", DamageThresholdMultiplier);

	// --- UPhysicalMaterial: UObject base methods ---

	REGISTER_METHOD(UPhysicalMaterial, "FName GetFName() const", asFUNCTION(UPhysicalMaterial_GetFName), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UPhysicalMaterial, "FString GetName() const", asFUNCTION(UPhysicalMaterial_GetName), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UPhysicalMaterial, "FString GetPathName() const", asFUNCTION(UPhysicalMaterial_GetPathName), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UPhysicalMaterial, "FString GetFullName() const", asFUNCTION(UPhysicalMaterial_GetFullName), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UPhysicalMaterial, "UObject GetOuter() const", asFUNCTION(UPhysicalMaterial_GetOuter), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UPhysicalMaterial, "UClass GetClass() const", asFUNCTION(UPhysicalMaterial_GetClass), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UPhysicalMaterial, "bool IsA(UClass SomeBase) const", asFUNCTION(UPhysicalMaterial_IsA), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UPhysicalMaterial, "bool IsValidLowLevel() const", asFUNCTION(UPhysicalMaterial_IsValidLowLevel), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UPhysicalMaterial, "bool IsAsset() const", asFUNCTION(UPhysicalMaterial_IsAsset), asCALL_CDECL_OBJFIRST);

	// --- UPhysicalMaterial: Surface properties ---

	REGISTER_PROPERTY(UPhysicalMaterial, "float Friction", Friction);
	REGISTER_PROPERTY(UPhysicalMaterial, "float StaticFriction", StaticFriction);
	REGISTER_PROPERTY(UPhysicalMaterial, "bool bOverrideFrictionCombineMode", bOverrideFrictionCombineMode);
	REGISTER_PROPERTY(UPhysicalMaterial, "float Restitution", Restitution);
	REGISTER_PROPERTY(UPhysicalMaterial, "bool bOverrideRestitutionCombineMode", bOverrideRestitutionCombineMode);

	REGISTER_METHOD(UPhysicalMaterial, "EFrictionCombineMode get_FrictionCombineMode() const", asFUNCTION(UPhysicalMaterial_GetFrictionCombineMode), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UPhysicalMaterial, "void set_FrictionCombineMode(EFrictionCombineMode Value)", asFUNCTION(UPhysicalMaterial_SetFrictionCombineMode), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UPhysicalMaterial, "EFrictionCombineMode get_RestitutionCombineMode() const", asFUNCTION(UPhysicalMaterial_GetRestitutionCombineMode), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UPhysicalMaterial, "void set_RestitutionCombineMode(EFrictionCombineMode Value)", asFUNCTION(UPhysicalMaterial_SetRestitutionCombineMode), asCALL_CDECL_OBJFIRST);

	// --- UPhysicalMaterial: Object properties ---

	REGISTER_PROPERTY(UPhysicalMaterial, "float Density", Density);
	REGISTER_PROPERTY(UPhysicalMaterial, "float SleepLinearVelocityThreshold", SleepLinearVelocityThreshold);
	REGISTER_PROPERTY(UPhysicalMaterial, "float SleepAngularVelocityThreshold", SleepAngularVelocityThreshold);
	REGISTER_PROPERTY(UPhysicalMaterial, "int SleepCounterThreshold", SleepCounterThreshold);
	REGISTER_PROPERTY(UPhysicalMaterial, "float RaiseMassToPower", RaiseMassToPower);

	// --- UPhysicalMaterial: Physical properties ---

	REGISTER_METHOD(UPhysicalMaterial, "EPhysicalSurface get_SurfaceType() const", asFUNCTION(UPhysicalMaterial_GetSurfaceType), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UPhysicalMaterial, "void set_SurfaceType(EPhysicalSurface Value)", asFUNCTION(UPhysicalMaterial_SetSurfaceType), asCALL_CDECL_OBJFIRST);
	REGISTER_PROPERTY(UPhysicalMaterial, "FPhysicalMaterialStrength Strength", Strength);
	REGISTER_PROPERTY(UPhysicalMaterial, "FPhysicalMaterialDamageModifier DamageModifier", DamageModifier);

	// --- UPhysicalMaterial: Debug properties ---

	REGISTER_PROPERTY(UPhysicalMaterial, "FLinearColor DebugColor", DebugColor);

	// --- UPhysicalMaterial: Experimental properties ---

	REGISTER_PROPERTY(UPhysicalMaterial, "bool bShowExperimentalProperties", bShowExperimentalProperties);
	REGISTER_METHOD(UPhysicalMaterial, "EPhysicalMaterialSoftCollisionMode get_SoftCollisionMode() const", asFUNCTION(UPhysicalMaterial_GetSoftCollisionMode), asCALL_CDECL_OBJFIRST);
	REGISTER_METHOD(UPhysicalMaterial, "void set_SoftCollisionMode(EPhysicalMaterialSoftCollisionMode Value)", asFUNCTION(UPhysicalMaterial_SetSoftCollisionMode), asCALL_CDECL_OBJFIRST);
	REGISTER_PROPERTY(UPhysicalMaterial, "float SoftCollisionThickness", SoftCollisionThickness);
	REGISTER_PROPERTY(UPhysicalMaterial, "float BaseFrictionImpulse", BaseFrictionImpulse);

	// --- UPhysicalMaterial: Static functions ---

	Result = Engine->SetDefaultNamespace("UPhysicalMaterial");
	check(Result >= 0);

	Result = Engine->RegisterGlobalFunction(
		"EPhysicalSurface DetermineSurfaceType(UPhysicalMaterial PhysicalMaterial)",
		asFUNCTION(UPhysicalMaterial::DetermineSurfaceType),
		asCALL_CDECL);
	check(Result >= 0);

	Result = Engine->RegisterGlobalFunction(
		"void SetEngineDefaultPhysMaterial(UPhysicalMaterial Material)",
		asFUNCTION(UPhysicalMaterial::SetEngineDefaultPhysMaterial),
		asCALL_CDECL);
	check(Result >= 0);

	Result = Engine->RegisterGlobalFunction(
		"void SetEngineDefaultDestructiblePhysMaterial(UPhysicalMaterial Material)",
		asFUNCTION(UPhysicalMaterial::SetEngineDefaultDestructiblePhysMaterial),
		asCALL_CDECL);
	check(Result >= 0);

#if WITH_EDITOR
	Result = Engine->RegisterGlobalFunction(
		"void RebuildPhysicalMaterials()",
		asFUNCTION(UPhysicalMaterial::RebuildPhysicalMaterials),
		asCALL_CDECL);
	check(Result >= 0);
#endif

	Result = Engine->SetDefaultNamespace("");
	check(Result >= 0);
}
