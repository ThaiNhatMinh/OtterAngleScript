#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "OtterAngelScriptFunction.generated.h"

UCLASS()
class OTTERANGLESCRIPT_API UOtterAngelScriptFunction : public UFunction
{
	GENERATED_BODY()
public:
	class asIScriptFunction* asFunction;
};
