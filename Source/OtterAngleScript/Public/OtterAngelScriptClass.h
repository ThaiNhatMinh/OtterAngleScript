#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "OtterAngelScriptClass.generated.h"

class asIScriptModule;
class asIScriptContext;
class asIScriptFunction;

UCLASS()
class OTTERANGLESCRIPT_API UOtterAngelScriptClass : public UClass
{
	GENERATED_BODY()

public:
	void Build(asIScriptModule* Module);

	/** Native function used to call the AngelScript functions from C code */
	DECLARE_FUNCTION(CallAngelScriptFunction);

protected:
	/** The AngelScript module associated with this class */
	asIScriptModule* ScriptModule;
	/** The AngelScript context used for executing functions */
	asIScriptContext* ScriptContext;

	TMap<UFunction*, asIScriptFunction*> ScriptFunctions;
};
