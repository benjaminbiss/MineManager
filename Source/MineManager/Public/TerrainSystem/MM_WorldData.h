#pragma once

#include "TerrainSystem/FMM_CellData.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MM_WorldData.generated.h"

UCLASS()
class MINEMANAGER_API AMM_WorldData : public AActor
{
	GENERATED_BODY()
	
public:	
	AMM_WorldData();
	void InitializeWorldDataParameters(const int32 InChunkDimensionsInCells, const int32 InGridCellSize, const int32 InMapDimensionsInChunks, 
		const int32 InWorldDepth, const int32 InSeed, const float InNoiseScale, const float InSurfaceHeightMultiplier, const float InSubsurfaceHeightOffset);
	void GenerateWorldData();
	// This function will create both surface and subsurface height maps, which are 2D arrays of height values for each cell in the grid, used for terrain generation and deformation
	void CreateWorldGeologyInformation(); 
	/* This function will create a 2D array of height values for the surface layer of the terrain, 
	this is the height for the upper height of the overburden layer */
	void CreateSurfaceHeightMap(); 
	/* This function will create a 2D array of height values for the subsurface layers of the terrain,
	this is the lower height of the overburden layer and the upper height of the rock layer */
	void CreateSubsurfaceHeightMap();
	/* This function will create the PCG minable volume */
	void CreatePCGMinableVolume();
	/* This function will create the main cell data array, which is a 1D array of FMM_CellData, 
	where each FMM_CellData contains an array of FMM_CellLayer representing the vertical layers of the terrain at that cell. */
	void CreateCellDataArray();
	/* This function will parse the surface and subsurface height maps and insert each layer into the cell data layers of the provided cell */
	void InsertBaseGeologyLayersIntoCell(FMM_CellData& Cell);
	/* This function will parse the PCG graph and insert each minable section into the rock layers of the cell data layers*/
	void InsertPCGMinableVolumeIntoCell(FMM_CellData& Cell);

protected:
	// 1D array to store cell data, indexed by (x + y * GridSize)
	// Each FMM_CellData contains an array of FMM_CellLayer, representing the vertical depth of each layer and the material type of each layer
	UPROPERTY()
	TArray<FMM_CellData> CellDataArray; 
	UPROPERTY()
	TArray<float> SurfaceHeightMap;
	UPROPERTY()
	TArray<float> SubsurfaceHeightMap;
	
	// World Parameters
	UPROPERTY()
	int32 GridSize;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|WorldData")
	int32 ChunkDimensionsInCells;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|WorldData")
	int32 GridCellSize;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|WorldData")
	int32 MapDimensionsInChunks;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|WorldData")
	int32 WorldDepth;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|WorldData")
	int32 Seed;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|WorldData")
	float NoiseScale;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|WorldData")
	float SurfaceHeightMultiplier;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|WorldData")
	float SubsurfaceHeightOffset;

};
