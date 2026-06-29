// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterAngelScriptClass.h"
#include "OtterAngleScript.h"
#include "Binding.h"
#include "OtterAngelScriptFunction.h"

DEFINE_FUNCTION(UOtterAngelScriptClass::CallAngelScriptFunction)
{
	// Get the function name from the stack
	auto Func = CastChecked<UOtterAngelScriptFunction>(Stack.CurrentNativeFunction);
	ScriptContext->Prepare(Func->asFunction);
}


void UOtterAngelScriptClass::Build(asIScriptModule* InModule)
{
	ScriptModule = InModule;
	ScriptContext = ScriptModule->GetEngine()->CreateContext();
}