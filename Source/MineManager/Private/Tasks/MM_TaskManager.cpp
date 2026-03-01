#include "Tasks/MM_TaskManager.h"

#include "Tasks/MM_DigTask.h"
#include "Grid/MM_GridManager.h"

AMM_TaskManager::AMM_TaskManager()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

}

void AMM_TaskManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMM_TaskManager::HandleDigTaskRequestStarted(FVector WorldPosition, bool bPressed)
{
	UE_LOG(LogTemp, Warning, TEXT("Dig task request Started at world position: %s"), *WorldPosition.ToString());
	InitialCursorGridCoords = GridManager->WorldPosToGridCoords(WorldPosition);

	PreviewDigTasks.Empty();
	CreatePreviewDigTasks(InitialCursorGridCoords);
}

void AMM_TaskManager::HandleDigTaskRequestTriggered(FVector WorldPosition, bool bPressed)
{
	UE_LOG(LogTemp, Warning, TEXT("Dig task request Updated at world position: %s"), *WorldPosition.ToString());
	CreatePreviewDigTasks(GridManager->WorldPosToGridCoords(WorldPosition));
}

void AMM_TaskManager::HandleDigTaskRequestCompleted(FVector WorldPosition, bool bPressed)
{
	UE_LOG(LogTemp, Warning, TEXT("Dig task request Finished at world position: %s"), *WorldPosition.ToString());
	CreatePreviewDigTasks(GridManager->WorldPosToGridCoords(WorldPosition));
	CreateDigTasksFromPreview();
}

void AMM_TaskManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMM_TaskManager::CreateDigTasksFromPreview()
{
	if (!DigTaskClass)
		return;

	for (auto Task : PreviewDigTasks)
	{
		if (!DigTasks.Contains(Task.Key))
		{		
			DigTasks.Add(Task.Key, Task.Value);			
		}
		else
		{
			PreviewDigTasks.Remove(Task.Key);
			Task.Value->Destroy();
		}
	}
	PreviewDigTasks.Empty();
}

void AMM_TaskManager::CreatePreviewDigTasks(const FVector& GridPosition)
{
	if (!DigTaskClass)
		return;

	int32 MinY = InitialCursorGridCoords.Y;
	int32 MinX = InitialCursorGridCoords.X;
	int32 MaxY = GridPosition.Y;
	int32 MaxX = GridPosition.X;
	for (int32 y = MinY; y < MaxY; y++)
	{
		for (int32 x = MinX; x < MaxX; x++)
		{
			FVector CurrentGridPosition(x, y, InitialCursorGridCoords.Z);
			if (!PreviewDigTasks.Contains(CurrentGridPosition))
			{
				FVector TaskLocation = GridManager->GridCoordsToWorldPos(CurrentGridPosition);
				AMM_DigTask* NewTask = GetWorld()->SpawnActor<AMM_DigTask>(DigTaskClass, TaskLocation, FRotator::ZeroRotator);
				if (NewTask)
				{
					PreviewDigTasks.Add(CurrentGridPosition, NewTask);
				}
			}
		}
	}	
}
