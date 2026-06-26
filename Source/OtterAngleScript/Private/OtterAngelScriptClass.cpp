// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterAngelScriptClass.h"
#include "OtterAngleScript.h"

DEFINE_FUNCTION(UOtterAngelScriptClass::CallAngelScriptFunction)
{
	// Get the function name from the stack
	FName FunctionName = Stack.CurrentNativeFunction->GetFName();
}


void UOtterAngelScriptClass::Build(asIScriptModule* InModule)
{
}