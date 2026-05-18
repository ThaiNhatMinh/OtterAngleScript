// Copyright Epic Games, Inc. All Rights Reserved.

#include "Binding.h"
#include "Engine/EngineTypes.h"
#include "Misc/AssertionMacros.h"
#include "angelscript.h"

void Bind_ETraceTypeQuery(asIScriptEngine* Engine)
{
	check(Engine != nullptr);

	int Result = Engine->RegisterEnum("ETraceTypeQuery");
	check(Result >= 0);

	const char* Names[] =
	{
		"TraceTypeQuery1", "TraceTypeQuery2", "TraceTypeQuery3", "TraceTypeQuery4",
		"TraceTypeQuery5", "TraceTypeQuery6", "TraceTypeQuery7", "TraceTypeQuery8",
		"TraceTypeQuery9", "TraceTypeQuery10", "TraceTypeQuery11", "TraceTypeQuery12",
		"TraceTypeQuery13", "TraceTypeQuery14", "TraceTypeQuery15", "TraceTypeQuery16",
		"TraceTypeQuery17", "TraceTypeQuery18", "TraceTypeQuery19", "TraceTypeQuery20",
		"TraceTypeQuery21", "TraceTypeQuery22", "TraceTypeQuery23", "TraceTypeQuery24",
		"TraceTypeQuery25", "TraceTypeQuery26", "TraceTypeQuery27", "TraceTypeQuery28",
		"TraceTypeQuery29", "TraceTypeQuery30", "TraceTypeQuery31", "TraceTypeQuery32"
	};

	for (int Index = 0; Index < 32; ++Index)
	{
		Result = Engine->RegisterEnumValue("ETraceTypeQuery", Names[Index], Index);
		check(Result >= 0);
	}
}
