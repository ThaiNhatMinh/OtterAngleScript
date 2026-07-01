// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterAngelScriptClass.h"
#include "OtterAngleScript.h"
#include "Binding.h"
#include "OtterAngelScriptFunction.h"


bool IsInputParameter(const FProperty* InParam)
{
	const bool bIsParam = InParam->HasAnyPropertyFlags(CPF_Parm);
	const bool bIsReturnParam = InParam->HasAnyPropertyFlags(CPF_ReturnParm);
	const bool bIsReferenceParam = InParam->HasAnyPropertyFlags(CPF_ReferenceParm);
	const bool bIsOutParam = InParam->HasAnyPropertyFlags(CPF_OutParm) && !InParam->HasAnyPropertyFlags(CPF_ConstParm);
	return bIsParam && !bIsReturnParam && (!bIsOutParam || bIsReferenceParam);
}

bool IsOutputParameter(const FProperty* InParam)
{
	const bool bIsParam = InParam->HasAnyPropertyFlags(CPF_Parm);
	const bool bIsReturnParam = InParam->HasAnyPropertyFlags(CPF_ReturnParm);
	const bool bIsOutParam = InParam->HasAnyPropertyFlags(CPF_OutParm) && !InParam->HasAnyPropertyFlags(CPF_ConstParm);
	return bIsParam && !bIsReturnParam && bIsOutParam;
}

DEFINE_FUNCTION(UOtterAngelScriptClass::CallAngelScriptFunction)
{
	// Stores information about inputs and outputs
	FOutParmRec* OutParms = nullptr;

	// Get the function name from the stack
	auto Func = CastChecked<UOtterAngelScriptFunction>(Stack.CurrentNativeFunction);
	auto asFunc = Func->asModule->GetFunctionByIndex(Func->asFuncId);
	Func->asContext->Prepare(Func->asFunction);
	Func->asContext->SetObject(Context);

	// Allocate memory to store our local argument data
	void* LocalStruct = FMemory_Alloca(FMath::Max<int32>(1, Func->GetStructureSize()));
	Func->InitializeStruct(LocalStruct);
	ON_SCOPE_EXIT
	{
		Func->DestroyStruct(LocalStruct);
	};

		// We iterate the fields directly here as we need to process input and output properties in the 
		// correct stack order, as we're potentially popping data off the bytecode stack
	uint32 Index = 0;
	for (TFieldIterator<FProperty> ParamIt(Func); ParamIt; ++ParamIt)
	{
		FProperty* Param = *ParamIt;

		// Skip the return value; it never has data on the bytecode stack and was added to the output params chain before this loop
		if (Param->HasAnyPropertyFlags(CPF_ReturnParm))
		{
			continue;
		}

		// Step the property data to populate the local value
		Stack.MostRecentPropertyAddress = nullptr;
		Stack.MostRecentPropertyContainer = nullptr;
		void* LocalValue = Param->ContainerPtrToValuePtr<void>(LocalStruct);
		Stack.StepCompiledIn(LocalValue, Param->GetClass());

		// Output parameters (even const ones) need to read their data from the property address (if available) rather than the local struct
		void* ValueAddress = LocalValue;
		if (Param->HasAnyPropertyFlags(CPF_OutParm) && Stack.MostRecentPropertyAddress)
		{
			ValueAddress = Stack.MostRecentPropertyAddress;
		}
		Func->asContext->SetArgAddress(Index, ValueAddress);
		++Index;
	}


	// Validate we reached the end of the parameters when stepping the bytecode stack
	if (Stack.Code)
	{
		checkSlow(*Stack.Code == EX_EndFunctionParms);
		++Stack.Code;
	}
	P_FINISH;

	int r = Func->asContext->Execute();
	if (r == asEXECUTION_FINISHED)
	{
		// The return value is only valid if the execution finished successfully
		asDWORD ret = Func->asContext->GetReturnDWord();
	}
}


void UOtterAngelScriptClass::Build(asIScriptModule* InModule)
{
	ScriptModule = InModule;
}

void UOtterAngelScriptClass::BeginDestroy()
{
	Super::BeginDestroy();
}

void UOtterAngelScriptClass::PostInitInstance(UObject* InObj, FObjectInstancingGraph* InstanceGraph)
{
	check(ScriptModule);
	Super::PostInitInstance(InObj, InstanceGraph);
	// Get asIScriptObject from the UObject
	//auto TypeInfo = GetClassTypeInfo(InObj->GetClass());

	//auto Context = ScriptModule->GetEngine()->CreateContext();
	//auto FactoryFunction = TypeInfo->GetFactoryByDecl();
	//Context->Prepare(TypeInfo->GetFactoryFunction());
	//// Prepare the context to call the factory function
	//Context->Prepare(factory);

	//// Execute the call
	//Context->Execute();

	//// Get the object that was created
	//asIScriptObject* obj = *(asIScriptObject**)Context->GetAddressOfReturnValue();
}
