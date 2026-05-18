// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Math/Vector.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"

void Bind_EAxisType(asIScriptEngine* Engine)
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
}
