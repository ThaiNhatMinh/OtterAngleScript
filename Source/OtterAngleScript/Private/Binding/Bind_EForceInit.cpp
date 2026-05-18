// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Misc/CoreMiscDefines.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"

void Bind_EForceInit(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = Engine->RegisterEnum("EForceInitType");
	check(Result >= 0);

	Result = Engine->RegisterEnumValue("EForceInitType", "ForceInit", static_cast<int>(ForceInit));
	check(Result >= 0);
	Result = Engine->RegisterEnumValue("EForceInitType", "ForceInitToZero", static_cast<int>(ForceInitToZero));
	check(Result >= 0);
}
