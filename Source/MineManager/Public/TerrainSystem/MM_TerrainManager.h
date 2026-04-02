#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MM_TerrainManager.generated.h"

class AMM_TerrainChunk;
class ANavMeshBoundsVolume;

UCLASS()
class MINEMANAGER_API AMM_TerrainManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AMM_TerrainManager();
	void InitializeTerrainParameters(const int32 InMapDimensionsInChunks, const int32 InChunkDimensionsInCells, const int32 InCellSize);
	void CreateChunkArray();
	void GetTerrainCenter(FVector& OutWorldPosition) const;
	UFUNCTION()
	void OnChunkDataGenerated(const FMM_ChunkData& Chunk);

protected:
	virtual void BeginPlay() override;
	void CreateChunk(const FVector& ChunkCord);
	// Any World Position converted to the nearest vertex position, and the corresponding chunk coordinate
	void WorldToChunkLocation(FVector WorldPosition, TArray<FIntPoint>& OutChunkCoords, TArray<FIntPoint>& OutVertexCoords) const;

	UFUNCTION()
	void ChangeVertexHeight(const FVector& WorldPosition, const bool bRaise);

	UPROPERTY(EditAnywhere, Category = "MyParameters|Terrain")
	TSubclassOf<class AMM_TerrainChunk> TerrainChunkClass;
	UPROPERTY(EditAnywhere, Category = "MyParameters|Material")
	UMaterialInterface* TerrainMaterial;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Terrain")
	TArray<AMM_TerrainChunk*> TerrainChunks;
	UPROPERTY(EditAnywhere, Category = "MyParameters|Terrain")
	int32 CellSize;
	UPROPERTY(EditAnywhere, Category = "MyParameters|Terrain")
	int32 ChunkDimensionsInCells;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Terrain")
	int32 ChunkDimensionsInUnits;
	UPROPERTY(EditAnywhere, Category = "MyParameters|Terrain")
	int32 MapDimensionsInChunks;
	UPROPERTY(EditAnywhere, Category = "MyParameters|Terrain")
	int32 Seed;
	UPROPERTY(EditAnywhere, Category = "MyParameters|Terrain")
	float HeightMultiplier;
	UPROPERTY(EditAnywhere, Category = "MyParameters|Grid")
	float LineThickness = 0.035f;
	UPROPERTY(EditAnywhere, Category = "MyParameters|Grid")
	float GridOpacity = 8.f;
	UPROPERTY(EditAnywhere, Category = "MyParameters|Grid")
	FLinearColor GridColor = FLinearColor::Black;


	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Navigation Invoker")
	ANavMeshBoundsVolume* NavMeshBoundsVolume;
};
