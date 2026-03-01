#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MM_WorkerManager.generated.h"

UCLASS()
class MINEMANAGER_API AMM_WorkerManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AMM_WorkerManager();

protected:
	virtual void BeginPlay() override;

public:	

};
