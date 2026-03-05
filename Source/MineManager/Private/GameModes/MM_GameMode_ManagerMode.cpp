#include "GameModes/MM_GameMode_ManagerMode.h"

#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"

#include "Controllers/MM_PlayerController.h"
#include "Entities/Workers/MM_WorkerManager.h"
#include "Tasks/MM_TaskManager.h"
#include "TerrainSystem/MM_TerrainManager.h"
#include "Grid/MM_GridManager.h"

void AMM_GameMode_ManagerMode::BeginPlay()
{
	Super::BeginPlay();

	SetupLevel();
}

void AMM_GameMode_ManagerMode::SetupLevel()
{
	SpawnManagers();
}

void AMM_GameMode_ManagerMode::SpawnManagers()
{
	AMM_PlayerController* PlayerController = Cast<AMM_PlayerController>(GetWorld()->GetFirstPlayerController());

	SpawnWorkerManager();
	SpawnTerrainManager(PlayerController);
	SpawnGridManager();
	SpawnTaskManager(PlayerController);
}

void AMM_GameMode_ManagerMode::SpawnWorkerManager()
{
	if (WorkerManager)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		WorkerManagerInstance = GetWorld()->SpawnActor<AMM_WorkerManager>(WorkerManager, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		WorkerManagerInstance->SetActorLabel(TEXT("Worker Manager"));
		WorkerManagerInstance->SetActorHiddenInGame(true);
		WorkerManagerInstance->SetOwner(this);
	}
}

void AMM_GameMode_ManagerMode::SpawnTaskManager(const AMM_PlayerController* PlayerController)
{
	if (TaskManager)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		TaskManagerInstance = GetWorld()->SpawnActor<AMM_TaskManager>(TaskManager, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		TaskManagerInstance->SetActorLabel(TEXT("Task Manager"));
		TaskManagerInstance->SetActorHiddenInGame(true);
		TaskManagerInstance->SetOwner(this);

		if (GridManagerInstance)
		{
			TaskManagerInstance->GridManager = GridManagerInstance;
		}

		if (PlayerController)
		{
			//PlayerController->OnSelectedWorldLocationStarted.AddDynamic(TaskManagerInstance, &AMM_TaskManager::HandleDigTaskRequestStarted);
			//PlayerController->OnSelectedWorldLocationTriggered.AddDynamic(TaskManagerInstance, &AMM_TaskManager::HandleDigTaskRequestTriggered);
			//PlayerController->OnSelectedWorldLocationCompleted.AddDynamic(TaskManagerInstance, &AMM_TaskManager::HandleDigTaskRequestCompleted);
		}
	}
}

void AMM_GameMode_ManagerMode::SpawnTerrainManager(const AMM_PlayerController* PlayerController)
{
	if (TerrainManager)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		TerrainManagerInstance = GetWorld()->SpawnActor<AMM_TerrainManager>(TerrainManager, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		TerrainManagerInstance->SetActorLabel(TEXT("Terrain Manager"));
		TerrainManagerInstance->SetActorHiddenInGame(true);
		TerrainManagerInstance->SetOwner(this);
		TerrainManagerInstance->InitializeTerrainParameters(MapDimensionsInChunks, ChunkDimensionsInCells, GridCellSize, Seed, NoiseScale, HeightMultiplier);
		TerrainManagerInstance->GenerateTerrain();

		if (PlayerController)
		{
			PlayerController->GetPawn()->SetActorLocation(TerrainManagerInstance->GetTerrainCenter());
		}
	}
}

void AMM_GameMode_ManagerMode::SpawnGridManager()
{
	if (GridManager)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GridManagerInstance = GetWorld()->SpawnActor<AMM_GridManager>(GridManager, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		GridManagerInstance->SetActorLabel(TEXT("Grid Manager"));
		GridManagerInstance->SetActorHiddenInGame(true);
		GridManagerInstance->SetOwner(this);
		GridManagerInstance->InitializeGridParameters(MapDimensionsInChunks * ChunkDimensionsInCells, GridCellSize);
	}
}
