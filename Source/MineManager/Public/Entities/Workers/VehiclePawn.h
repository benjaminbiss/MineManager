#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "VehiclePawn.generated.h"

class UFloatingPawnMovement;
class UNavigationInvokerComponent;

UCLASS()
class MINEMANAGER_API AVehiclePawn : public APawn
{
	GENERATED_BODY()

public:
	AVehiclePawn();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Components")
	UFloatingPawnMovement* MovementComponent;

	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Components")
	UNavigationInvokerComponent* NavInvoker;

	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Navigation Invoker")
	float NavGenerationRadius = 3000.0f;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Navigation Invoker")
	float NavRemovalRadius = 5000.0f;

public:	
	virtual void Tick(float DeltaTime) override;

};
