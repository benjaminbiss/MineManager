#pragma once

#include "EMM_CellGeologyType.h"

#include "CoreMinimal.h"
#include "FMM_CellLayer.generated.h"

USTRUCT(BlueprintType)
struct MINEMANAGER_API FMM_CellLayer
{
	GENERATED_BODY()

    UPROPERTY()
    int32 Height;

    UPROPERTY()
    EMM_CellGeologyType CellGeoType;

    FMM_CellLayer()
    {
        Height = 3200;
		CellGeoType = EMM_CellGeologyType::Empty;
    }

    FMM_CellLayer(const int32 InDepth, const EMM_CellGeologyType InCellGeoType)
    {
        Height = InDepth;
		CellGeoType = InCellGeoType;
    }
};
