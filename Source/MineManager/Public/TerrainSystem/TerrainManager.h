#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TerrainManager.generated.h"

class AMM_PlayerController;

UCLASS()
class MINEMANAGER_API ATerrainManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ATerrainManager();

protected:
	virtual void BeginPlay() override;
	void CreateChunk(FIntPoint ChunkCoord);
	void CreateChunkArray();
	void WorldToChunkLocation(FVector WorldPosition, TArray<FIntPoint>& OutChunkCoords, TArray<FIntPoint>& OutVertexCoords);

	UFUNCTION()
	void ChangeVertexHeight(FVector WorldPosition, bool bRaise);

	UPROPERTY(EditAnywhere, Category = "Terrain")
	TSubclassOf<class ATerrainChunk> TerrainChunkClass;
	UPROPERTY(VisibleAnywhere, Category = "Terrain")
	TArray<ATerrainChunk*> TerrainChunks;
	UPROPERTY(EditAnywhere, Category = "Terrain")
	int32 ChunkDimensions;
	UPROPERTY(EditAnywhere, Category = "Terrain")
	int32 ChunkTriSize;
	UPROPERTY(VisibleAnywhere, Category = "Terrain")
	int32 ChunkSize;
	UPROPERTY(EditAnywhere, Category = "Terrain")
	int32 MapSize;
	UPROPERTY(EditAnywhere, Category = "Terrain")
	int32 Seed;
	UPROPERTY(EditAnywhere, Category = "Terrain")
	float NoiseScale;
	UPROPERTY(EditAnywhere, Category = "Terrain")
	float HeightMultiplier;

	AMM_PlayerController* PlayerController;
};
