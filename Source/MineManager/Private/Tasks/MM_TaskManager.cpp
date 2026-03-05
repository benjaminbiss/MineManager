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

void AMM_TaskManager::HandleDigTaskRequestStarted(const FVector& WorldPosition, const bool bPressed)
{
	UE_LOG(LogTemp, Warning, TEXT("Dig task request Started at world position: %s"), *WorldPosition.ToString());
	InitialCursorGridCoords = GridManager->WorldPosToGridCoords(WorldPosition);

	PreviewDigTasks.Empty();
	CreatePreviewDigTasks(InitialCursorGridCoords);
}

void AMM_TaskManager::HandleDigTaskRequestTriggered(const FVector& WorldPosition, const bool bPressed)
{
	UE_LOG(LogTemp, Warning, TEXT("Dig task request Updated at world position: %s"), *WorldPosition.ToString());
	CreatePreviewDigTasks(GridManager->WorldPosToGridCoords(WorldPosition));
}

void AMM_TaskManager::HandleDigTaskRequestCompleted(const FVector& WorldPosition, const bool bPressed)
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

	const int32 MinY = FMath::Min(InitialCursorGridCoords.Y, GridPosition.Y);
	const int32 MinX = FMath::Min(InitialCursorGridCoords.X, GridPosition.X);
	const int32 MaxY = FMath::Max(InitialCursorGridCoords.Y, GridPosition.Y);
	const int32 MaxX = FMath::Max(InitialCursorGridCoords.X, GridPosition.X);
	// Find all grid positions within the rectangle defined by InitialCursorGridCoords and GridPosition
	TArray<FVector> CurrentPreviewGridPositions;
	for (int32 y = MinY; y <= MaxY; y++)
	{
		for (int32 x = MinX; x <= MaxX; x++)
		{
			FVector CurrentGridPosition(x, y, InitialCursorGridCoords.Z);
			CurrentPreviewGridPositions.Add(CurrentGridPosition);
		}
	}	
	// Remove preview tasks that are no longer in the current preview area
	for (auto& Task : PreviewDigTasks)
	{
		if (!CurrentPreviewGridPositions.Contains(Task.Key))
		{
			Task.Value->Destroy();
			PreviewDigTasks.Remove(Task.Key);
		}
	}
	// Add preview tasks for new grid positions in the current preview area
	for (const FVector& CurrentGridPosition : CurrentPreviewGridPositions)
	{
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
