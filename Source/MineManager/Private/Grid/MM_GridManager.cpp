#include "Grid/MM_GridManager.h"

AMM_GridManager::AMM_GridManager()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AMM_GridManager::BeginPlay()
{
	Super::BeginPlay();
		
}

void AMM_GridManager::InitializeGridParameters(const int32 InWorldGridDimensionsInCells, const int32 InCellSize)
{
	WorldGridDimensionsInCells = InWorldGridDimensionsInCells;
	CellSize = InCellSize;
}

FVector AMM_GridManager::WorldPosToGridCoords(const FVector& WorldPosition) const
{
	FVector GridPosition;
	GridPosition.X = FMath::FloorToInt(WorldPosition.X / CellSize);
	GridPosition.Y = FMath::FloorToInt(WorldPosition.Y / CellSize);
	GridPosition.Z = FMath::FloorToInt(WorldPosition.Z / CellSize);
	return GridPosition;	
}

FVector AMM_GridManager::WorldPosAlignedToGrid(const FVector& WorldPosition) const
{
	FVector GridAlignedPosition = WorldPosToGridCoords(WorldPosition);
	GridAlignedPosition.X *= CellSize;
	GridAlignedPosition.Y *= CellSize;
	GridAlignedPosition.Z *= CellSize;
	return GridAlignedPosition;
}

FVector AMM_GridManager::GridCoordsToWorldPos(const FVector& GridPosition) const
{
	FVector GridAlignedPosition = GridPosition;
	GridAlignedPosition.X *= CellSize;
	GridAlignedPosition.Y *= CellSize;
	GridAlignedPosition.Z *= CellSize;
	return GridAlignedPosition;
}
