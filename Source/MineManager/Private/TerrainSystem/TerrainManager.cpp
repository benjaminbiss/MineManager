#include "TerrainSystem/TerrainManager.h"

#include "TerrainSystem/TerrainChunk.h"
#include "Controllers/MM_PlayerController.h"

ATerrainManager::ATerrainManager()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ATerrainManager::BeginPlay()
{
	Super::BeginPlay();

	ChunkSize = ChunkDimensions * ChunkTriSize;
	
	PlayerController = Cast<AMM_PlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("ATerrainManager|BeginPlay: PlayerController not found!"));
	}
	else
	{
		PlayerController->OnDebugDeformationInput.AddDynamic(this, &ATerrainManager::ChangeVertexHeight);
	}

	CreateChunkArray();	
}

void ATerrainManager::CreateChunk(FIntPoint ChunkCoord)
{
	ATerrainChunk* NewChunk = GetWorld()->SpawnActor<ATerrainChunk>(TerrainChunkClass, FVector(ChunkCoord.X * ChunkSize, ChunkCoord.Y * ChunkSize, 0), FRotator::ZeroRotator);
	if (NewChunk)
	{
		TerrainChunks.Add(NewChunk);	
		NewChunk->SetOwner(this);
		NewChunk->ChunkCoord = ChunkCoord;
		TArray<float> HeightDeltaMap; // This value will be loaded from save data, this is the persistence height deformation information
		NewChunk->GenerateSquareMesh(Seed, ChunkDimensions + 1, ChunkTriSize, NoiseScale, HeightMultiplier, HeightDeltaMap);
	}
}

void ATerrainManager::CreateChunkArray()
{
	for (int32 y = 0; y < MapSize; y++)
	{
		for (int32 x = 0; x < MapSize; x++)
		{
			FIntPoint ChunkCoord(x, y);
			CreateChunk(ChunkCoord);
		}
	}
}

void ATerrainManager::WorldToChunkLocation(FVector WorldPosition, TArray<FIntPoint>& OutChunkCoords, TArray<FIntPoint>& OutVertexCoords)
{
	FIntPoint ChunkCoord, VertexCoord;
	ChunkCoord.X = FMath::FloorToInt(WorldPosition.X / ChunkSize);
	ChunkCoord.Y = FMath::FloorToInt(WorldPosition.Y / ChunkSize);
	
	float localX = WorldPosition.X - ChunkCoord.X * ChunkSize;
	float localY = WorldPosition.Y - ChunkCoord.Y * ChunkSize;
	
	VertexCoord.X = FMath::RoundToInt(localX / ChunkTriSize);
	VertexCoord.Y = FMath::RoundToInt(localY / ChunkTriSize);

	OutChunkCoords.Add(ChunkCoord);
	OutVertexCoords.Add(VertexCoord);

	if (VertexCoord.X == 0 || VertexCoord.Y == 0 || VertexCoord.X == ChunkDimensions || VertexCoord.Y == ChunkDimensions)
	{
		if (VertexCoord.X == 0)
		{
			FIntPoint AdjacentChunkCoord = ChunkCoord + FIntPoint(-1, 0);
			FIntPoint AdjacentVertexCoord = VertexCoord + FIntPoint(ChunkDimensions, 0);
			OutChunkCoords.Add(AdjacentChunkCoord);
			OutVertexCoords.Add(AdjacentVertexCoord);
		}
		if (VertexCoord.Y == 0)
		{
			FIntPoint AdjacentChunkCoord = ChunkCoord + FIntPoint(0, -1);
			FIntPoint AdjacentVertexCoord = VertexCoord + FIntPoint(0, ChunkDimensions);
			OutChunkCoords.Add(AdjacentChunkCoord);
			OutVertexCoords.Add(AdjacentVertexCoord);
		}
		if (VertexCoord.X == ChunkDimensions)
		{
			FIntPoint AdjacentChunkCoord = ChunkCoord + FIntPoint(1, 0);
			FIntPoint AdjacentVertexCoord = VertexCoord + FIntPoint(-ChunkDimensions, 0);
			OutChunkCoords.Add(AdjacentChunkCoord);
			OutVertexCoords.Add(AdjacentVertexCoord);
		}
		if (VertexCoord.Y == ChunkDimensions)
		{
			FIntPoint AdjacentChunkCoord = ChunkCoord + FIntPoint(0, 1);
			FIntPoint AdjacentVertexCoord = VertexCoord + FIntPoint(0, -ChunkDimensions);
			OutChunkCoords.Add(AdjacentChunkCoord);
			OutVertexCoords.Add(AdjacentVertexCoord);
		}
		if (OutChunkCoords.Num() == 3) // Corner case, we need to add the diagonal adjacent chunk
		{
			if (VertexCoord.X == 0 && VertexCoord.Y == 0)
			{
				FIntPoint AdjacentChunkCoord = ChunkCoord + FIntPoint(-1, -1);
				FIntPoint AdjacentVertexCoord = VertexCoord + FIntPoint(ChunkDimensions, ChunkDimensions);
				OutChunkCoords.Add(AdjacentChunkCoord);
				OutVertexCoords.Add(AdjacentVertexCoord);
			}
			else if (VertexCoord.X == 0 && VertexCoord.Y == ChunkDimensions)
			{
				FIntPoint AdjacentChunkCoord = ChunkCoord + FIntPoint(-1, 1);
				FIntPoint AdjacentVertexCoord = VertexCoord + FIntPoint(ChunkDimensions, -ChunkDimensions);
				OutChunkCoords.Add(AdjacentChunkCoord);
				OutVertexCoords.Add(AdjacentVertexCoord);
			}
			else if (VertexCoord.X == ChunkDimensions && VertexCoord.Y == 0)
			{
				FIntPoint AdjacentChunkCoord = ChunkCoord + FIntPoint(1, -1);
				FIntPoint AdjacentVertexCoord = VertexCoord + FIntPoint(-ChunkDimensions, ChunkDimensions);
				OutChunkCoords.Add(AdjacentChunkCoord);
				OutVertexCoords.Add(AdjacentVertexCoord);
			}
			else if (VertexCoord.X == ChunkDimensions && VertexCoord.Y == ChunkDimensions)
			{
				FIntPoint AdjacentChunkCoord = ChunkCoord + FIntPoint(1, 1);
				FIntPoint AdjacentVertexCoord = VertexCoord + FIntPoint(-ChunkDimensions, -ChunkDimensions);
				OutChunkCoords.Add(AdjacentChunkCoord);
				OutVertexCoords.Add(AdjacentVertexCoord);
			}
		}
	}
}

void ATerrainManager::ChangeVertexHeight(FVector WorldPosition, bool bRaise)
{
	TArray<FIntPoint> ChunkCoords, VertexCoords;
	WorldToChunkLocation(WorldPosition, ChunkCoords, VertexCoords);

	for (int32 i = 0; i < ChunkCoords.Num(); i++)
	{
		if (ChunkCoords[i].X < 0 || ChunkCoords[i].X >= MapSize || ChunkCoords[i].Y < 0 || ChunkCoords[i].Y >= MapSize)
			continue;
		
		ATerrainChunk* TargetChunk = TerrainChunks[ChunkCoords[i].Y * MapSize + ChunkCoords[i].X];
		TargetChunk->ApplyDeformationToHeightDeltaMap(VertexCoords[i], bRaise);
	}
}
