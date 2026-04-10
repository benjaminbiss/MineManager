#include "GameModes/MM_GameMode_ManagerMode.h"

#include "Controllers/MM_PlayerController.h"

//#include "Entities/Workers/MM_WorkerManager.h"
#include "Orders/MM_OrderManager.h"
#include "TerrainSystem/MM_WorldData.h"
#include "TerrainSystem/MM_TerrainManager.h"

#include "Tasks/MM_TaskManager.h"

void AMM_GameMode_ManagerMode::BeginPlay()
{
	Super::BeginPlay();

	PlayerControllerInstance = Cast<AMM_PlayerController>(GetWorld()->GetFirstPlayerController());
	FVector PlayerStartLocation = FVector(ChunkSize * MapSize * CellSize * 0.5f, ChunkSize * MapSize * CellSize * 0.5f, 0);
	PlayerControllerInstance->GetPawn()->SetActorLocation(PlayerStartLocation);

	SetupLevel();
}

void AMM_GameMode_ManagerMode::SetupLevel()
{
	SpawnManagers();
}

void AMM_GameMode_ManagerMode::SpawnManagers()
{
	SpawnWorldData();
	SpawnTerrainManager();
	//SpawnOrderManager();
	//SpawnWorkerManager();
	//SpawnTaskManager();
}

void AMM_GameMode_ManagerMode::SpawnWorldData()
{
	if (WorldData)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		WorldDataInstance = GetWorld()->SpawnActor<AMM_WorldData>(WorldData, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		WorldDataInstance->SetActorLabel(TEXT("World Data"));
		WorldDataInstance->SetActorHiddenInGame(true);
		WorldDataInstance->SetOwner(this);
		WorldDataInstance->InitializeWorldDataParameters(ChunkSize, CellSize, MapSize, WorldDepth, Seed, SurfaceHeightMultiplier, SubsurfaceHeightMultiplier, SubsurfaceHeightOffset, CoalSeamThicknessThreshold, CoalSeamHeightOffset, CoalSeamDepth);
	}
}

void AMM_GameMode_ManagerMode::SpawnTerrainManager()
{
	if (TerrainManager)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		TerrainManagerInstance = GetWorld()->SpawnActor<AMM_TerrainManager>(TerrainManager, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		TerrainManagerInstance->SetActorLabel(TEXT("Terrain Manager"));
		TerrainManagerInstance->SetActorHiddenInGame(true);
		TerrainManagerInstance->SetOwner(this);
		TerrainManagerInstance->InitializeTerrainParameters(MapSize, ChunkSize, CellSize);
		TerrainManagerInstance->CreateChunkArray();

		if (WorldDataInstance)
		{
			WorldDataInstance->OnChunkDataGenerated.AddDynamic(TerrainManagerInstance, &AMM_TerrainManager::OnChunkDataGenerated);
		}
	}
}

void AMM_GameMode_ManagerMode::SpawnOrderManager()
{
	if (OrderManager)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		OrderManagerInstance = GetWorld()->SpawnActor<AMM_OrderManager>(OrderManager, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		OrderManagerInstance->SetActorLabel(TEXT("Order Manager"));
		OrderManagerInstance->SetActorHiddenInGame(true);
		OrderManagerInstance->SetOwner(this);

		if (PlayerControllerInstance)
		{
			PlayerControllerInstance->OnSelectedWorldLocationStarted.AddDynamic(OrderManagerInstance, &AMM_OrderManager::HandleOnSelectStarted);
			PlayerControllerInstance->OnSelectedWorldLocationTriggered.AddDynamic(OrderManagerInstance, &AMM_OrderManager::HandleOnSelectTriggered);
			PlayerControllerInstance->OnSelectedWorldLocationCompleted.AddDynamic(OrderManagerInstance, &AMM_OrderManager::HandleOnSelectCompleted);
			PlayerControllerInstance->OnSecondarySelectInput.AddDynamic(OrderManagerInstance, &AMM_OrderManager::HandleOnSecondarySelect);
		}
	}
}

void AMM_GameMode_ManagerMode::SpawnWorkerManager()
{
	//if (WorkerManager)
	//{
	//	FActorSpawnParameters SpawnParams;
	//	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//	WorkerManagerInstance = GetWorld()->SpawnActor<AMM_WorkerManager>(WorkerManager, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	//	WorkerManagerInstance->SetActorLabel(TEXT("Worker Manager"));
	//	WorkerManagerInstance->SetActorHiddenInGame(true);
	//	WorkerManagerInstance->SetOwner(this);
	//}
}

void AMM_GameMode_ManagerMode::SpawnTaskManager()
{
	if (TaskManager)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		TaskManagerInstance = GetWorld()->SpawnActor<AMM_TaskManager>(TaskManager, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		TaskManagerInstance->SetActorLabel(TEXT("Task Manager"));
		TaskManagerInstance->SetActorHiddenInGame(true);
		TaskManagerInstance->SetOwner(this);


	}
}
