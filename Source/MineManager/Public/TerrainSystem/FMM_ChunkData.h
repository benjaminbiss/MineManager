#pragma once

#include "TerrainSystem/FMM_CellData.h"

#include "CoreMinimal.h"
#include "FMM_ChunkData.generated.h"

USTRUCT(BlueprintType)
struct MINEMANAGER_API FMM_ChunkData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 CoordX;
	UPROPERTY()
	int32 CoordY;

	UPROPERTY()
	TArray<FMM_CellData> Cells;

	FMM_ChunkData()
	{
		CoordX = 0;
		CoordY = 0;
	}

	FMM_ChunkData(const int32 InCoordX, const int32 InCoordY)
	{
		CoordX = InCoordX;
		CoordY = InCoordY;
	}
};
