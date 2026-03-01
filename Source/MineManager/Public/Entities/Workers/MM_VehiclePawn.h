#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MM_VehiclePawn.generated.h"

class UFloatingPawnMovement;
class UNavigationInvokerComponent;

UCLASS()
class MINEMANAGER_API AMM_VehiclePawn : public APawn
{
	GENERATED_BODY()

public:
	AMM_VehiclePawn();

protected:
	virtual void BeginPlay() override;	

	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Components")
	UFloatingPawnMovement* MovementComponent;

	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Components")
	UNavigationInvokerComponent* NavInvoker;

	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Components")
	UStaticMeshComponent* ModelMesh;

	UPROPERTY(EditAnywhere, Category = "MyParameters|Mesh")
	UStaticMesh* VehicleMesh;

	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Navigation Invoker")
	float NavGenerationRadius = 3000.0f;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Navigation Invoker")
	float NavRemovalRadius = 5000.0f;
};
