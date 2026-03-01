#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MM_GameMode_ManagerMode.generated.h"

class AMM_WorkerManager;
class AMM_TaskManager;
class AMM_TerrainManager;
class AMM_GridManager;
class APlayerStart;

UCLASS()
class MINEMANAGER_API AMM_GameMode_ManagerMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	void SetupLevel();
	void SpawnManagers();
	void SpawnWorkerManager();
	void SpawnTaskManager(class AMM_PlayerController* PlayerController);
	void SpawnTerrainManager(class AMM_PlayerController* PlayerController);
	void SpawnGridManager();

public:
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

	// World Parameters
	UPROPERTY(EditAnywhere, Category = "MyParameters|WorldMap|Grid")
	int32 ChunkDimensionsInCells = 32;
	UPROPERTY(EditAnywhere, Category = "MyParameters|WorldMap|Grid")
	int32 GridCellSize = 100;
	UPROPERTY(EditAnywhere, Category = "MyParameters|WorldMap|Grid")
	int32 MapDimensionsInChunks = 3;
	UPROPERTY(EditAnywhere, Category = "MyParameters|WorldMap|Terrain")
	int32 Seed = 11;
	UPROPERTY(EditAnywhere, Category = "MyParameters|WorldMap|Terrain")
	float NoiseScale = 0.02f;
	UPROPERTY(EditAnywhere, Category = "MyParameters|WorldMap|Terrain")
	float HeightMultiplier = 400.f;
};
