// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Engine/EngineTypes.h"
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

	Result = Engine->RegisterEnum("EObjectTypeQuery");
	check(Result >= 0);
	const char* ObjectTypeNames[] =
	{
		"ObjectTypeQuery1",  "ObjectTypeQuery2",  "ObjectTypeQuery3",  "ObjectTypeQuery4",
		"ObjectTypeQuery5",  "ObjectTypeQuery6",  "ObjectTypeQuery7",  "ObjectTypeQuery8",
		"ObjectTypeQuery9",  "ObjectTypeQuery10", "ObjectTypeQuery11", "ObjectTypeQuery12",
		"ObjectTypeQuery13", "ObjectTypeQuery14", "ObjectTypeQuery15", "ObjectTypeQuery16",
		"ObjectTypeQuery17", "ObjectTypeQuery18", "ObjectTypeQuery19", "ObjectTypeQuery20",
		"ObjectTypeQuery21", "ObjectTypeQuery22", "ObjectTypeQuery23", "ObjectTypeQuery24",
		"ObjectTypeQuery25", "ObjectTypeQuery26", "ObjectTypeQuery27", "ObjectTypeQuery28",
		"ObjectTypeQuery29", "ObjectTypeQuery30", "ObjectTypeQuery31", "ObjectTypeQuery32"
	};
	for (int Index = 0; Index < 32; ++Index)
	{
		Result = Engine->RegisterEnumValue("EObjectTypeQuery", ObjectTypeNames[Index], Index);
		check(Result >= 0);
	}

	Result = Engine->RegisterEnum("ETraceTypeQuery");
	check(Result >= 0);
	const char* TraceTypeNames[] =
	{
		"TraceTypeQuery1",  "TraceTypeQuery2",  "TraceTypeQuery3",  "TraceTypeQuery4",
		"TraceTypeQuery5",  "TraceTypeQuery6",  "TraceTypeQuery7",  "TraceTypeQuery8",
		"TraceTypeQuery9",  "TraceTypeQuery10", "TraceTypeQuery11", "TraceTypeQuery12",
		"TraceTypeQuery13", "TraceTypeQuery14", "TraceTypeQuery15", "TraceTypeQuery16",
		"TraceTypeQuery17", "TraceTypeQuery18", "TraceTypeQuery19", "TraceTypeQuery20",
		"TraceTypeQuery21", "TraceTypeQuery22", "TraceTypeQuery23", "TraceTypeQuery24",
		"TraceTypeQuery25", "TraceTypeQuery26", "TraceTypeQuery27", "TraceTypeQuery28",
		"TraceTypeQuery29", "TraceTypeQuery30", "TraceTypeQuery31", "TraceTypeQuery32"
	};
	for (int Index = 0; Index < 32; ++Index)
	{
		Result = Engine->RegisterEnumValue("ETraceTypeQuery", TraceTypeNames[Index], Index);
		check(Result >= 0);
	}
}
