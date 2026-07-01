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

	virtual void BeginDestroy() override;
	virtual bool IsAsset() const override
	{
		return false;
	}

	//~ UClass interface
	virtual void PostInitInstance(UObject* InObj, FObjectInstancingGraph* InstanceGraph) override;

protected:
	friend class FOtterAngelScriptClassGenerator;

	/** The AngelScript module associated with this class */
	asIScriptModule* ScriptModule;
};
