#pragma once

#include "CoreMinimal.h"
#include "FMM_CellLayer.generated.h"

USTRUCT(BlueprintType)
struct MINEMANAGER_API FMM_CellLayer
{
	GENERATED_BODY()

    UPROPERTY()
    FName LayerType;

    UPROPERTY()
    int32 Depth;
};
