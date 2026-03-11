#pragma once

#include "EMM_CellGeologyType.h"

#include "CoreMinimal.h"
#include "FMM_CellLayer.generated.h"

USTRUCT(BlueprintType)
struct MINEMANAGER_API FMM_CellLayer
{
	GENERATED_BODY()

    UPROPERTY()
    EMM_CellGeologyType CellGeoType;

    UPROPERTY()
    int32 Depth;
};
