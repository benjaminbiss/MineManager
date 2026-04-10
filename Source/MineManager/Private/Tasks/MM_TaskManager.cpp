#include "Tasks/MM_TaskManager.h"

#include "Tasks/MM_DigTask.h"
#include "Tasks/MM_CoreSampleTask.h"
//#include "Grid/MM_GridManager.h"

AMM_TaskManager::AMM_TaskManager()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

}

void AMM_TaskManager::HandleDigTaskRequestStarted(const FVector& WorldPosition, const bool bPressed)
{
	//UE_LOG(LogTemp, Warning, TEXT("Dig task request Started at world position: %s"), *WorldPosition.ToString());
	InitialCursorWorldPosition = WorldPosition;
	PreviewDigTasks.Empty();
	CreatePreviewDigTasks(InitialCursorWorldPosition);
}

void AMM_TaskManager::HandleDigTaskRequestTriggered(const FVector& WorldPosition, const bool bPressed)
{
	//UE_LOG(LogTemp, Warning, TEXT("Dig task request Updated at world position: %s"), *WorldPosition.ToString());
	CreatePreviewDigTasks(WorldPosition);
}

void AMM_TaskManager::HandleDigTaskRequestCompleted(const FVector& WorldPosition, const bool bPressed)
{
	//UE_LOG(LogTemp, Warning, TEXT("Dig task request Finished at world position: %s"), *WorldPosition.ToString());
	CreatePreviewDigTasks(WorldPosition);
	CreateDigTasksFromPreview();
}

void AMM_TaskManager::HandleCoreSampleRequestStarted(const FVector& WorldPosition, const bool bPressed)
{
	//UE_LOG(LogTemp, Warning, TEXT("Dig task request Started at world position: %s"), *WorldPosition.ToString());
	InitialCursorWorldPosition = WorldPosition;
	PreviewDigTasks.Empty();
	CreatePreviewCoreSampleTasks(InitialCursorWorldPosition);
}

void AMM_TaskManager::HandleCoreSampleRequestTriggered(const FVector& WorldPosition, const bool bPressed)
{
	//UE_LOG(LogTemp, Warning, TEXT("Dig task request Updated at world position: %s"), *WorldPosition.ToString());
	CreatePreviewCoreSampleTasks(WorldPosition);
}

void AMM_TaskManager::HandleCoreSampleRequestCompleted(const FVector& WorldPosition, const bool bPressed)
{
	//UE_LOG(LogTemp, Warning, TEXT("Dig task request Finished at world position: %s"), *WorldPosition.ToString());
	CreatePreviewCoreSampleTasks(WorldPosition);
	CreateCoreSampleTasksFromPreview();
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

void AMM_TaskManager::CreatePreviewDigTasks(const FVector& WorldPosition)
{
	if (!DigTaskClass)
		return;

	const int32 MinY = FMath::Min(InitialCursorWorldPosition.Y, WorldPosition.Y);
	const int32 MinX = FMath::Min(InitialCursorWorldPosition.X, WorldPosition.X);
	const int32 MaxY = FMath::Max(InitialCursorWorldPosition.Y, WorldPosition.Y);
	const int32 MaxX = FMath::Max(InitialCursorWorldPosition.X, WorldPosition.X);
	// Find all grid positions within the rectangle defined by InitialCursorGridCoords and GridPosition
	TArray<FVector> CurrentPreviewGridPositions;
	for (int32 y = MinY; y <= MaxY; y++)
	{
		for (int32 x = MinX; x <= MaxX; x++)
		{
			FVector CurrentGridPosition(x, y, InitialCursorWorldPosition.Z);
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
			//FVector TaskLocation = GridManager->GridCoordsToWorldPos(CurrentGridPosition);
			//AMM_DigTask* NewTask = GetWorld()->SpawnActor<AMM_DigTask>(DigTaskClass, TaskLocation, FRotator::ZeroRotator);
			//if (NewTask)
			//{
			//	PreviewDigTasks.Add(CurrentGridPosition, NewTask);
			//}
		}
	}
}

void AMM_TaskManager::CreateCoreSampleTasksFromPreview()
{
	if (!CoreSampleTaskClass)
		return;

	for (auto Task : PreviewCoreSampleTasks)
	{
		if (!CoreSampleTasks.Contains(Task.Key))
		{
			CoreSampleTasks.Add(Task.Key, Task.Value);
		}
		else
		{
			PreviewCoreSampleTasks.Remove(Task.Key);
			Task.Value->Destroy();
		}
	}
	PreviewCoreSampleTasks.Empty();
}

void AMM_TaskManager::CreatePreviewCoreSampleTasks(const FVector& WorldPosition)
{
	if (!CoreSampleTaskClass)
		return;

	const int32 MinY = FMath::Min(InitialCursorWorldPosition.Y, WorldPosition.Y);
	const int32 MinX = FMath::Min(InitialCursorWorldPosition.X, WorldPosition.X);
	const int32 MaxY = FMath::Max(InitialCursorWorldPosition.Y, WorldPosition.Y);
	const int32 MaxX = FMath::Max(InitialCursorWorldPosition.X, WorldPosition.X);
	// Find all grid positions within the rectangle defined by InitialCursorGridCoords and GridPosition
	TArray<FVector> CurrentPreviewGridPositions;
	for (int32 y = MinY; y <= MaxY; y++)
	{
		for (int32 x = MinX; x <= MaxX; x++)
		{
			int32 CellHeight;
			FIntPoint Cord = FIntPoint(x, y);
			//GridManager->GridHeightAtGridPosition(Cord, CellHeight);
			FVector CurrentGridPosition(x, y, CellHeight);
			CurrentPreviewGridPositions.Add(CurrentGridPosition);
		}
	}
	// Remove preview tasks that are no longer in the current preview area
	for (auto& Task : PreviewCoreSampleTasks)
	{
		if (!CurrentPreviewGridPositions.Contains(Task.Key))
		{
			Task.Value->Destroy();
			PreviewCoreSampleTasks.Remove(Task.Key);
		}
	}
	// Add preview tasks for new grid positions in the current preview area
	for (const FVector& CurrentGridPosition : CurrentPreviewGridPositions)
	{
		if (!PreviewCoreSampleTasks.Contains(CurrentGridPosition))
		{
			//FVector TaskLocation = GridManager->GridCoordsToWorldPos(CurrentGridPosition);
			//AMM_CoreSampleTask* NewTask = GetWorld()->SpawnActor<AMM_CoreSampleTask>(CoreSampleTaskClass, TaskLocation, FRotator::ZeroRotator);
			//if (NewTask)
			//{
			//	PreviewCoreSampleTasks.Add(CurrentGridPosition, NewTask);
			//}
		}
	}
}
