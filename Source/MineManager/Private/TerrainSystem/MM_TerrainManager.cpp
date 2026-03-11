#include "TerrainSystem/MM_TerrainManager.h"

#include "TerrainSystem/MM_TerrainChunk.h"

AMM_TerrainManager::AMM_TerrainManager()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AMM_TerrainManager::GetTerrainCenter(FVector& OutWorldPosition) const
{
	OutWorldPosition.X = (MapDimensionsInChunks * ChunkDimensionsInUnits) / 2.0f;
	OutWorldPosition.Y = (MapDimensionsInChunks * ChunkDimensionsInUnits) / 2.0f;
	OutWorldPosition.Z = 0;
}

void AMM_TerrainManager::BeginPlay()
{
	Super::BeginPlay();

}

void AMM_TerrainManager::InitializeTerrainParameters(const int32 InMapDimensionsInChunks, const int32 InChunkDimensionsInCells, const int32 InCellSize, const int32 InSeed, const float InNoiseScale, const float InHeightMultiplier)
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

void AMM_TerrainManager::CreateChunk(const FVector ChunkCord, UMaterialInstanceDynamic* TerrainMID)
{
	AMM_TerrainChunk* NewChunk = GetWorld()->SpawnActor<AMM_TerrainChunk>(TerrainChunkClass, FVector(ChunkCord.X * ChunkDimensionsInUnits, ChunkCord.Y * ChunkDimensionsInUnits, 0), FRotator::ZeroRotator);
	if (NewChunk)
	{
		TerrainChunks.Add(NewChunk);	
		NewChunk->SetOwner(this);
		NewChunk->ChunkCord = ChunkCord;
		const TArray<float> HeightDeltaMap; // This value will be loaded from save data, this is the persistence height deformation information
		NewChunk->GenerateChunkMesh(Seed, ChunkDimensionsInCells + 1, CellSize, NoiseScale, HeightMultiplier, HeightDeltaMap, TerrainMID);
	
	}
}

void AMM_TerrainManager::CreateChunkArray()
{
	if (TerrainMaterial)
	{
		UMaterialInstanceDynamic* TerrainMID = UMaterialInstanceDynamic::Create(TerrainMaterial, this);
		TerrainMID->SetScalarParameterValue("CellSize", CellSize);
		TerrainMID->SetScalarParameterValue("LineThickness", 0.035f);
		TerrainMID->SetScalarParameterValue("GridOpacity", 8.f);
		TerrainMID->SetVectorParameterValue("GridColor", FLinearColor::Black);

		for (int32 y = 0; y < MapDimensionsInChunks; y++)
		{
			for (int32 x = 0; x < MapDimensionsInChunks; x++)
			{
				const FVector ChunkCord(x, y, 0);
				CreateChunk(ChunkCord, TerrainMID);
			}
		}
	}
}

void AMM_TerrainManager::WorldToChunkLocation(FVector WorldPosition, TArray<FIntPoint>& OutChunkCoords, TArray<FIntPoint>& OutVertexCoords) const
{
	FIntPoint ChunkCord, VertexCord;
	ChunkCord.X = FMath::FloorToInt(WorldPosition.X / ChunkDimensionsInUnits);
	ChunkCord.Y = FMath::FloorToInt(WorldPosition.Y / ChunkDimensionsInUnits);
	
	float LocalX = WorldPosition.X - ChunkCord.X * ChunkDimensionsInUnits;
	float LocalY = WorldPosition.Y - ChunkCord.Y * ChunkDimensionsInUnits;
	
	VertexCord.X = FMath::RoundToInt(LocalX / CellSize);
	VertexCord.Y = FMath::RoundToInt(LocalY / CellSize);

	OutChunkCoords.Add(ChunkCord);
	OutVertexCoords.Add(VertexCord);

	if (VertexCord.X == 0 || VertexCord.Y == 0 || VertexCord.X == ChunkDimensionsInCells || VertexCord.Y == ChunkDimensionsInCells)
	{
		if (VertexCord.X == 0)
		{
			FIntPoint AdjacentChunkCord = ChunkCord + FIntPoint(-1, 0);
			FIntPoint AdjacentVertexCord = VertexCord + FIntPoint(ChunkDimensionsInCells, 0);
			OutChunkCoords.Add(AdjacentChunkCord);
			OutVertexCoords.Add(AdjacentVertexCord);
		}
		if (VertexCord.Y == 0)
		{
			FIntPoint AdjacentChunkCord = ChunkCord + FIntPoint(0, -1);
			FIntPoint AdjacentVertexCord = VertexCord + FIntPoint(0, ChunkDimensionsInCells);
			OutChunkCoords.Add(AdjacentChunkCord);
			OutVertexCoords.Add(AdjacentVertexCord);
		}
		if (VertexCord.X == ChunkDimensionsInCells)
		{
			FIntPoint AdjacentChunkCord = ChunkCord + FIntPoint(1, 0);
			FIntPoint AdjacentVertexCord = VertexCord + FIntPoint(-ChunkDimensionsInCells, 0);
			OutChunkCoords.Add(AdjacentChunkCord);
			OutVertexCoords.Add(AdjacentVertexCord);
		}
		if (VertexCord.Y == ChunkDimensionsInCells)
		{
			FIntPoint AdjacentChunkCord = ChunkCord + FIntPoint(0, 1);
			FIntPoint AdjacentVertexCord = VertexCord + FIntPoint(0, -ChunkDimensionsInCells);
			OutChunkCoords.Add(AdjacentChunkCord);
			OutVertexCoords.Add(AdjacentVertexCord);
		}
		if (OutChunkCoords.Num() == 3) // Corner case, we need to add the diagonal adjacent chunk
		{
			if (VertexCord.X == 0 && VertexCord.Y == 0)
			{
				FIntPoint AdjacentChunkCord = ChunkCord + FIntPoint(-1, -1);
				FIntPoint AdjacentVertexCord = VertexCord + FIntPoint(ChunkDimensionsInCells, ChunkDimensionsInCells);
				OutChunkCoords.Add(AdjacentChunkCord);
				OutVertexCoords.Add(AdjacentVertexCord);
			}
			else if (VertexCord.X == 0 && VertexCord.Y == ChunkDimensionsInCells)
			{
				FIntPoint AdjacentChunkCord = ChunkCord + FIntPoint(-1, 1);
				FIntPoint AdjacentVertexCord = VertexCord + FIntPoint(ChunkDimensionsInCells, -ChunkDimensionsInCells);
				OutChunkCoords.Add(AdjacentChunkCord);
				OutVertexCoords.Add(AdjacentVertexCord);
			}
			else if (VertexCord.X == ChunkDimensionsInCells && VertexCord.Y == 0)
			{
				FIntPoint AdjacentChunkCord = ChunkCord + FIntPoint(1, -1);
				FIntPoint AdjacentVertexCord = VertexCord + FIntPoint(-ChunkDimensionsInCells, ChunkDimensionsInCells);
				OutChunkCoords.Add(AdjacentChunkCord);
				OutVertexCoords.Add(AdjacentVertexCord);
			}
			else if (VertexCord.X == ChunkDimensionsInCells && VertexCord.Y == ChunkDimensionsInCells)
			{
				FIntPoint AdjacentChunkCord = ChunkCord + FIntPoint(1, 1);
				FIntPoint AdjacentVertexCord = VertexCord + FIntPoint(-ChunkDimensionsInCells, -ChunkDimensionsInCells);
				OutChunkCoords.Add(AdjacentChunkCord);
				OutVertexCoords.Add(AdjacentVertexCord);
			}
		}
	}
}

void AMM_TerrainManager::ChangeVertexHeight(const FVector& WorldPosition, const bool bRaise)
{
	TArray<FIntPoint> ChunkCoords, VertexCoords;
	WorldToChunkLocation(WorldPosition, ChunkCoords, VertexCoords);

	for (int32 i = 0; i < ChunkCoords.Num(); i++)
	{
		if (ChunkCoords[i].X < 0 || ChunkCoords[i].X >= MapDimensionsInChunks || ChunkCoords[i].Y < 0 || ChunkCoords[i].Y >= MapDimensionsInChunks)
			continue;
		
		AMM_TerrainChunk* TargetChunk = TerrainChunks[ChunkCoords[i].Y * MapDimensionsInChunks + ChunkCoords[i].X];
		//TargetChunk->ApplyDeformationToHeightDeltaMap(VertexCoords[i], bRaise);
	}
}
