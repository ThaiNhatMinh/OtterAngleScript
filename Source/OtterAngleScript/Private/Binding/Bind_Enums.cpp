// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Engine/EngineTypes.h"
#include "InputCoreTypes.h"
#include "Math/Vector.h"
#include "Misc/CoreMiscDefines.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"

void Bind_Enums(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = Engine->RegisterEnum("EAxisType");
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAxisType", "None", static_cast<int>(EAxis::None));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAxisType", "X", static_cast<int>(EAxis::X));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAxisType", "Y", static_cast<int>(EAxis::Y));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EAxisType", "Z", static_cast<int>(EAxis::Z));
	check(Result >= 0);

	Result = Engine->RegisterEnum("EForceInitType");
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EForceInitType", "ForceInit", static_cast<int>(ForceInit));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EForceInitType", "ForceInitToZero", static_cast<int>(ForceInitToZero));
	check(Result >= 0);

	Result = Engine->RegisterEnum("EPairedAxis");
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EPairedAxis", "Unpaired", static_cast<int>(EPairedAxis::Unpaired));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EPairedAxis", "X", static_cast<int>(EPairedAxis::X));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EPairedAxis", "Y", static_cast<int>(EPairedAxis::Y));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EPairedAxis", "Z", static_cast<int>(EPairedAxis::Z));
	check(Result >= 0);
}
