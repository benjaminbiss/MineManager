#include "TerrainSystem/MM_GridManager.h"

AMM_GridManager::AMM_GridManager()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AMM_GridManager::InitializeGridParameters(const int32 InGridSize, const int32 InCellSize)
{
	GridSize = InGridSize;
	CellSize = InCellSize;
}

void AMM_GridManager::BeginPlay()
{
	Super::BeginPlay();
	
}
