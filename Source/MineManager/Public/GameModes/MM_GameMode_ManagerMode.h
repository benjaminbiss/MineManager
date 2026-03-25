#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MM_GameMode_ManagerMode.generated.h"

class AMM_WorkerManager;
class AMM_TaskManager;
class AMM_WorldData;
class AMM_WorldDataVisualizer;
class AMM_TerrainManager;
class AMM_GridManager;

UCLASS()
class MINEMANAGER_API AMM_GameMode_ManagerMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	void SetupLevel();
	void SpawnManagers();
	void SpawnWorldData();
	void SpawnTerrainManager();
	void SpawnGridManager();
	void SpawnWorkerManager();
	void SpawnTaskManager();

public:
	// World Data
	UPROPERTY(EditAnywhere, Category = "MyParameters|Managers")
	TSubclassOf<AMM_WorldData> WorldData;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Managers")
	AMM_WorldData* WorldDataInstance;
	// World Data Visualizer
	UPROPERTY(EditAnywhere, Category = "MyParameters|Managers")
	TSubclassOf<AMM_WorldDataVisualizer> WorldDataVisualizer;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Managers")
	AMM_WorldDataVisualizer* WorldDataVisualizerInstance;

	// Terrain Manager
	UPROPERTY(EditAnywhere, Category = "MyParameters|Managers")
	TSubclassOf<AMM_TerrainManager> TerrainManager;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Managers")
	AMM_TerrainManager* TerrainManagerInstance;
	
	// Grid Manager
	UPROPERTY(EditAnywhere, Category = "MyParameters|Managers")
	TSubclassOf<AMM_GridManager> GridManager;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Managers")
	AMM_GridManager* GridManagerInstance;

	// Worker Manager
	UPROPERTY(EditAnywhere, Category = "MyParameters|Managers")
	TSubclassOf<AMM_WorkerManager> WorkerManager;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Managers")
	AMM_WorkerManager* WorkerManagerInstance;

	// Task Manager
	UPROPERTY(EditAnywhere, Category = "MyParameters|Managers")
	TSubclassOf<AMM_TaskManager> TaskManager;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Managers")
	AMM_TaskManager* TaskManagerInstance;

	// World Parameters
	UPROPERTY(EditAnywhere, Category = "MyParameters|WorldMap|Terrain")
	int32 Seed = 11;
	UPROPERTY(EditAnywhere, Category = "MyParameters|WorldMap|Grid")
	int32 MapSize = 10;
	UPROPERTY(EditAnywhere, Category = "MyParameters|WorldMap|Grid")
	int32 ChunkSize = 32;
	UPROPERTY(EditAnywhere, Category = "MyParameters|WorldMap|Grid")
	int32 CellSize = 100;
	UPROPERTY(EditAnywhere, Category = "MyParameters|WorldMap|Terrain")
	int32 WorldDepth = 32;
	UPROPERTY(EditAnywhere, Category = "MyParameters|WorldMap|Terrain")
	int32 SurfaceHeightMultiplier = 2200;
	UPROPERTY(EditAnywhere, Category = "MyParameters|WorldMap|Terrain")
	int32 SubsurfaceHeightMultiplier = 3200;
	UPROPERTY(EditAnywhere, Category = "MyParameters|WorldMap|Terrain")
	int32 SubsurfaceHeightOffset = 200;
	UPROPERTY(EditAnywhere, Category = "MyParameters|WorldMap|Terrain")
	float CoalSeamThicknessThreshold = 0.6f;
	UPROPERTY(EditAnywhere, Category = "MyParameters|WorldMap|Terrain")
	int32 CoalSeamHeightOffset = 500;
	UPROPERTY(EditAnywhere, Category = "MyParameters|WorldMap|Terrain")
	int32 CoalSeamDepth = 800;
};
