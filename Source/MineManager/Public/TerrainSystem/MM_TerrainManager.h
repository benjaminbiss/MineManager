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
	void InitializeTerrainParameters(const int32 InMapDimensionsInChunks, const int32 InChunkDimensionsInCells, const int32 InCellSize, const int32 InSeed, const float InNoiseScale, const float InHeightMultiplier);
	void GenerateTerrain();
	
	FVector GetTerrainCenter() const;

protected:
	virtual void BeginPlay() override;
	void CreateChunk(const FIntPoint ChunkCord);
	void CreateChunkArray();
	// Any World Position converted to the nearest vertex position, and the corresponding chunk coordinate
	void WorldToChunkLocation(FVector WorldPosition, TArray<FIntPoint>& OutChunkCoords, TArray<FIntPoint>& OutVertexCoords) const;
	// Grid position corresponds to the index of the vertex at the cells lower left corner, returns a map of the cell's 4 vertices and their corresponding chunk coordinates
	void GridToCellLocation(FIntPoint GridPosition, FIntPoint& OutCellCord);
	void GridToVertices(FIntPoint GridPosition, TMap<FIntPoint, FIntPoint>& VertexAndChunkCoords);


	UFUNCTION()
	void ChangeVertexHeight(const FVector& WorldPosition, const bool bRaise);

	UPROPERTY(EditAnywhere, Category = "MyParameters|Terrain")
	TSubclassOf<class AMM_TerrainChunk> TerrainChunkClass;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Terrain")
	TArray<AMM_TerrainChunk*> TerrainChunks;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Terrain")
	int32 ChunkDimensionsInCells;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Terrain")
	int32 CellSize;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Terrain")
	int32 ChunkDimensionsInUnits;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Terrain")
	int32 MapDimensionsInChunks;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Terrain")
	int32 Seed;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Terrain")
	float NoiseScale;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Terrain")
	float HeightMultiplier;

	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Navigation Invoker")
	ANavMeshBoundsVolume* NavMeshBoundsVolume;
};
