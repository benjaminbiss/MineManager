#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MM_TaskManager.generated.h"

class AMM_DigTask;
class AMM_GridManager;

UCLASS()
class MINEMANAGER_API AMM_TaskManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AMM_TaskManager();
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()	
	void HandleDigTaskRequestStarted(const FVector& WorldPosition, const bool bPressed);
	UFUNCTION()
	void HandleDigTaskRequestTriggered(const FVector& WorldPosition, const bool bPressed);
	UFUNCTION()
	void HandleDigTaskRequestCompleted(const FVector& WorldPosition, const bool bPressed);


	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Managers")
	AMM_GridManager* GridManager;

protected:
	virtual void BeginPlay() override;
	void CreateDigTasksFromPreview();
	void CreatePreviewDigTasks(const FVector& GridPosition);


	UPROPERTY(EditAnywhere, Category = "MyParameters|Tasks")
	TSubclassOf<AMM_DigTask> DigTaskClass;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Tasks")
	TMap<FVector, TWeakObjectPtr<AMM_DigTask>> DigTasks; // Key: 3D Grid World Position, Value: Task 
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Tasks")
	TMap<FVector, TWeakObjectPtr<AMM_DigTask>> PreviewDigTasks;
	
	bool bCursorDragging = false;
	FVector InitialCursorGridCoords;
};
