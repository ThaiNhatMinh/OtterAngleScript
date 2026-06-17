#pragma once

#include "CoreMinimal.h"
#include "DummyUObject.generated.h"


UCLASS()
class UDUMMYUOBJECT : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
    TArray<int> ArrayOfInt;
    UPROPERTY(BlueprintReadWrite)
    TArray<float> ArrayOfFloat;

	void CallMe()
	{
		ArrayOfFloat.Add(3.14f);
	}
};