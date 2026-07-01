#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "OtterAngelScriptFunction.generated.h"

UCLASS()
class OTTERANGLESCRIPT_API UOtterAngelScriptFunction : public UFunction
{
	GENERATED_BODY()
public:
	int asFuncId;
	class asIScriptModule* asModule;
	class asIScriptFunction* asFunction;
	class asIScriptContext* asContext;
};
