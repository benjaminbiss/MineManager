#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MM_CoreSampleTask.generated.h"

UCLASS()
class MINEMANAGER_API AMM_CoreSampleTask : public AActor
{
	GENERATED_BODY()
	
public:	
	AMM_CoreSampleTask();

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, Category = "MyParameters|Mesh")
	UStaticMeshComponent* Mesh;

};
