#pragma once

#include "TerrainSystem/FMM_CellLayer.h"

#include "CoreMinimal.h"
#include "FMM_CellData.generated.h"

USTRUCT(BlueprintType)
struct MINEMANAGER_API FMM_CellData
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FMM_CellLayer> CellLayers;
};
