#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MM_GridManager.generated.h"

UCLASS()
class MINEMANAGER_API AMM_GridManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AMM_GridManager();
	void InitializeGridParameters(const int32 InWorldGridDimensionsInCells, const int32 InCellSize);
	FVector WorldPosToGridCoords(const FVector& WorldPosition) const;
	FVector WorldPosAlignedToGrid(const FVector& WorldPosition) const;
	FVector GridCoordsToWorldPos(const FVector& GridPosition) const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "MyParameters|MapGrid")
	int32 WorldGridDimensionsInCells;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|MapGrid")
	int32 CellSize;

};
