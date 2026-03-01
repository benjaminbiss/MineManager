#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MM_WorkerAIController.generated.h"

class UNavigationSystemV1;

UCLASS()
class MINEMANAGER_API AMM_WorkerAIController : public AAIController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	void MaintainHeightAboveGround();

public:
	UFUNCTION()
	void MovePawnToLocation(FVector TargetLocation);
};
