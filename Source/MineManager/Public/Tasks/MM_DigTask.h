#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MM_DigTask.generated.h"

UCLASS()
class MINEMANAGER_API AMM_DigTask : public AActor
{
	GENERATED_BODY()
	
public:	
	AMM_DigTask();

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, Category = "MyParameters|Mesh")
	UStaticMeshComponent* Mesh;

public:	
	virtual void Tick(float DeltaTime) override;

};
