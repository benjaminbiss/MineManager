#pragma once

#include "TerrainSystem/FMM_ChunkData.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MM_WorldData.generated.h"

class AMM_SurfacePCGChunkGenerator;

class UPCGComponent;
class UPCGGraphInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChunkDataGenerated, const FMM_ChunkData&, Chunk);

UCLASS()
class MINEMANAGER_API AMM_WorldData : public AActor
{
	GENERATED_BODY()
	
public:	
	AMM_WorldData();

	void InitializeWorldDataParameters(const int32 InChunkSize, const int32 InCellSize, const int32 InMapSize, 
		const int32 InWorldDepth, const int32 InSeed, const int32 InSurfaceHeightMultiplier, const int32 InSubsurfaceHeightMultiplier, const int32 InSubsurfaceHeightOffset,
		const float InCoalSeamThicknessThreshold, const int32 InCoalSeamHeightOffset, const int32 InCoalSeamDepth);

	UPROPERTY(BlueprintCallable)
	FOnChunkDataGenerated OnChunkDataGenerated;

protected:
	void GenerateWorldData();
	// Tells the PCG graph to generate data for the chunk at the given coordinates
	void RequestChunkGeneration(int32 X, int32 Y);
	// Binds to the PCG graph's custom delegate emitted when generation complete event, and inserts the generated surface data into the corresponding chunk data
	UFUNCTION()
	void WriteSurfaceDataToChunkData(const TArray<double>& HeighValues);

	// 1D array to store cell data, indexed by (x + y * GridSize)
	// Each FMM_CellData contains an array of FMM_CellLayer, representing the vertical depth of each layer and the material type of each layer
	UPROPERTY()
	TArray<FMM_ChunkData> ChunkDataArray;
	UPROPERTY()
	TArray<float> SurfaceHeightMap;
	UPROPERTY()
	TArray<float> SubsurfaceHeightMap;

	// PCG Graphs
	UPROPERTY(EditDefaultsOnly, Category = "MyParameters|PCG Graphs")
	TSubclassOf<AMM_SurfacePCGChunkGenerator> SurfacePCGChunkGeneratorClass;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|PCG Graphs")
	AMM_SurfacePCGChunkGenerator* SurfacePCGChunkGenerator;

	// World Parameters
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|WorldData")
	int32 Seed;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|WorldData")
	int32 MapSize;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|WorldData")
	int32 ChunkSize;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|WorldData")
	int32 CellSize;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|WorldData")
	int32 WorldDepth;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|WorldData")
	float SurfaceHeightMultiplier;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|WorldData")
	float SubsurfaceHeightMultiplier;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|WorldData")
	int32 SubsurfaceHeightOffset;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|WorldData")
	float CoalSeamThicknessThreshold;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|WorldData")
	int32 CoalSeamHeightOffset;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|WorldData")
	int32 CoalSeamDepth;
};
