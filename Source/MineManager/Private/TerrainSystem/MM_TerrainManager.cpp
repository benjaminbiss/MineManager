#include "TerrainSystem/MM_TerrainManager.h"

#include "TerrainSystem/MM_TerrainChunk.h"

AMM_TerrainManager::AMM_TerrainManager()
{
	PrimaryActorTick.bCanEverTick = false;

}

FVector AMM_TerrainManager::GetTerrainCenter() const
{
	float CenterX = (MapDimensionsInChunks * ChunkDimensionsInUnits) / 2.0f;
	float CenterY = (MapDimensionsInChunks * ChunkDimensionsInUnits) / 2.0f;
	return FVector(CenterX, CenterY, 0);
}

void AMM_TerrainManager::BeginPlay()
{
	Super::BeginPlay();

}

void AMM_TerrainManager::InitializeTerrainParameters(int32 InMapDimensionsInChunks, int32 InChunkDimensionsInCells, int32 InCellSize, int32 InSeed, float InNoiseScale, float InHeightMultiplier)
{
	MapDimensionsInChunks = InMapDimensionsInChunks;
	ChunkDimensionsInCells = InChunkDimensionsInCells;
	CellSize = InCellSize;
	Seed = InSeed;
	NoiseScale = InNoiseScale;
	HeightMultiplier = InHeightMultiplier;
	ChunkDimensionsInUnits = ChunkDimensionsInCells * CellSize;

	
}

void AMM_TerrainManager::GenerateTerrain()
{
	CreateChunkArray();
}

void AMM_TerrainManager::CreateChunk(FIntPoint ChunkCoord)
{
	AMM_TerrainChunk* NewChunk = GetWorld()->SpawnActor<AMM_TerrainChunk>(TerrainChunkClass, FVector(ChunkCoord.X * ChunkDimensionsInUnits, ChunkCoord.Y * ChunkDimensionsInUnits, 0), FRotator::ZeroRotator);
	if (NewChunk)
	{
		TerrainChunks.Add(NewChunk);	
		NewChunk->SetOwner(this);
		NewChunk->ChunkCoord = ChunkCoord;
		TArray<float> HeightDeltaMap; // This value will be loaded from save data, this is the persistence height deformation information
		NewChunk->GenerateSquareMesh(Seed, ChunkDimensionsInCells + 1, CellSize, NoiseScale, HeightMultiplier, HeightDeltaMap);
	}
}

void AMM_TerrainManager::CreateChunkArray()
{
	for (int32 y = 0; y < MapDimensionsInChunks; y++)
	{
		for (int32 x = 0; x < MapDimensionsInChunks; x++)
		{
			FIntPoint ChunkCoord(x, y);
			CreateChunk(ChunkCoord);
		}
	}
}

void AMM_TerrainManager::WorldToChunkLocation(FVector WorldPosition, TArray<FIntPoint>& OutChunkCoords, TArray<FIntPoint>& OutVertexCoords)
{
	FIntPoint ChunkCoord, VertexCoord;
	ChunkCoord.X = FMath::FloorToInt(WorldPosition.X / ChunkDimensionsInUnits);
	ChunkCoord.Y = FMath::FloorToInt(WorldPosition.Y / ChunkDimensionsInUnits);
	
	float localX = WorldPosition.X - ChunkCoord.X * ChunkDimensionsInUnits;
	float localY = WorldPosition.Y - ChunkCoord.Y * ChunkDimensionsInUnits;
	
	VertexCoord.X = FMath::RoundToInt(localX / CellSize);
	VertexCoord.Y = FMath::RoundToInt(localY / CellSize);

	OutChunkCoords.Add(ChunkCoord);
	OutVertexCoords.Add(VertexCoord);

	if (VertexCoord.X == 0 || VertexCoord.Y == 0 || VertexCoord.X == ChunkDimensionsInCells || VertexCoord.Y == ChunkDimensionsInCells)
	{
		if (VertexCoord.X == 0)
		{
			FIntPoint AdjacentChunkCoord = ChunkCoord + FIntPoint(-1, 0);
			FIntPoint AdjacentVertexCoord = VertexCoord + FIntPoint(ChunkDimensionsInCells, 0);
			OutChunkCoords.Add(AdjacentChunkCoord);
			OutVertexCoords.Add(AdjacentVertexCoord);
		}
		if (VertexCoord.Y == 0)
		{
			FIntPoint AdjacentChunkCoord = ChunkCoord + FIntPoint(0, -1);
			FIntPoint AdjacentVertexCoord = VertexCoord + FIntPoint(0, ChunkDimensionsInCells);
			OutChunkCoords.Add(AdjacentChunkCoord);
			OutVertexCoords.Add(AdjacentVertexCoord);
		}
		if (VertexCoord.X == ChunkDimensionsInCells)
		{
			FIntPoint AdjacentChunkCoord = ChunkCoord + FIntPoint(1, 0);
			FIntPoint AdjacentVertexCoord = VertexCoord + FIntPoint(-ChunkDimensionsInCells, 0);
			OutChunkCoords.Add(AdjacentChunkCoord);
			OutVertexCoords.Add(AdjacentVertexCoord);
		}
		if (VertexCoord.Y == ChunkDimensionsInCells)
		{
			FIntPoint AdjacentChunkCoord = ChunkCoord + FIntPoint(0, 1);
			FIntPoint AdjacentVertexCoord = VertexCoord + FIntPoint(0, -ChunkDimensionsInCells);
			OutChunkCoords.Add(AdjacentChunkCoord);
			OutVertexCoords.Add(AdjacentVertexCoord);
		}
		if (OutChunkCoords.Num() == 3) // Corner case, we need to add the diagonal adjacent chunk
		{
			if (VertexCoord.X == 0 && VertexCoord.Y == 0)
			{
				FIntPoint AdjacentChunkCoord = ChunkCoord + FIntPoint(-1, -1);
				FIntPoint AdjacentVertexCoord = VertexCoord + FIntPoint(ChunkDimensionsInCells, ChunkDimensionsInCells);
				OutChunkCoords.Add(AdjacentChunkCoord);
				OutVertexCoords.Add(AdjacentVertexCoord);
			}
			else if (VertexCoord.X == 0 && VertexCoord.Y == ChunkDimensionsInCells)
			{
				FIntPoint AdjacentChunkCoord = ChunkCoord + FIntPoint(-1, 1);
				FIntPoint AdjacentVertexCoord = VertexCoord + FIntPoint(ChunkDimensionsInCells, -ChunkDimensionsInCells);
				OutChunkCoords.Add(AdjacentChunkCoord);
				OutVertexCoords.Add(AdjacentVertexCoord);
			}
			else if (VertexCoord.X == ChunkDimensionsInCells && VertexCoord.Y == 0)
			{
				FIntPoint AdjacentChunkCoord = ChunkCoord + FIntPoint(1, -1);
				FIntPoint AdjacentVertexCoord = VertexCoord + FIntPoint(-ChunkDimensionsInCells, ChunkDimensionsInCells);
				OutChunkCoords.Add(AdjacentChunkCoord);
				OutVertexCoords.Add(AdjacentVertexCoord);
			}
			else if (VertexCoord.X == ChunkDimensionsInCells && VertexCoord.Y == ChunkDimensionsInCells)
			{
				FIntPoint AdjacentChunkCoord = ChunkCoord + FIntPoint(1, 1);
				FIntPoint AdjacentVertexCoord = VertexCoord + FIntPoint(-ChunkDimensionsInCells, -ChunkDimensionsInCells);
				OutChunkCoords.Add(AdjacentChunkCoord);
				OutVertexCoords.Add(AdjacentVertexCoord);
			}
		}
	}
}

void AMM_TerrainManager::GridToCellLocation(FIntPoint GridPosition, FIntPoint& OutCellCoord)
{

}

void AMM_TerrainManager::GridToVertices(FIntPoint GridPosition, TMap<FIntPoint, FIntPoint>& VertexAndChunkCoords)
{

}

void AMM_TerrainManager::ChangeVertexHeight(FVector WorldPosition, bool bRaise)
{
	TArray<FIntPoint> ChunkCoords, VertexCoords;
	WorldToChunkLocation(WorldPosition, ChunkCoords, VertexCoords);

	for (int32 i = 0; i < ChunkCoords.Num(); i++)
	{
		if (ChunkCoords[i].X < 0 || ChunkCoords[i].X >= MapDimensionsInChunks || ChunkCoords[i].Y < 0 || ChunkCoords[i].Y >= MapDimensionsInChunks)
			continue;
		
		AMM_TerrainChunk* TargetChunk = TerrainChunks[ChunkCoords[i].Y * MapDimensionsInChunks + ChunkCoords[i].X];
		TargetChunk->ApplyDeformationToHeightDeltaMap(VertexCoords[i], bRaise);
	}
}
