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

void ATerrainManager::WorldToChunkLocation(FVector WorldPosition, FIntPoint& OutChunkCoord, FIntPoint& OutVertexCoord)
{
	OutChunkCoord.X = FMath::FloorToInt(WorldPosition.X / ChunkSize);
	OutChunkCoord.Y = FMath::FloorToInt(WorldPosition.Y / ChunkSize);
	
	float localX = WorldPosition.X - OutChunkCoord.X * ChunkSize;
	float localY = WorldPosition.Y - OutChunkCoord.Y * ChunkSize;
	
	OutVertexCoord.X = FMath::RoundToInt(localX / ChunkTriSize);
	OutVertexCoord.Y = FMath::RoundToInt(localY / ChunkTriSize);
}

void ATerrainManager::ChangeVertexHeight(FVector WorldPosition, bool bRaise)
{
	FIntPoint ChunkCoord, VertexCoord;
	WorldToChunkLocation(WorldPosition, ChunkCoord, VertexCoord);

	ATerrainChunk* TargetChunk = TerrainChunks[ChunkCoord.Y * MapSize + ChunkCoord.X];
	TargetChunk->ApplyDeformationToHeightDeltaMap(VertexCoord, bRaise);
}
