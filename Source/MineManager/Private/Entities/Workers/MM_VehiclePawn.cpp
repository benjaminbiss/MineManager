#include "Entities/Workers/MM_VehiclePawn.h"

#include "GameFramework/FloatingPawnMovement.h"
#include "NavigationInvokerComponent.h"

AMM_VehiclePawn::AMM_VehiclePawn()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	ModelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	ModelMesh->SetupAttachment(RootComponent);

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));
	MovementComponent->UpdatedComponent = RootComponent;

	NavInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("Invoker"));
}

void AMM_VehiclePawn::BeginPlay()
{
	Super::BeginPlay();
	
	NavInvoker->SetGenerationRadii(NavGenerationRadius, NavRemovalRadius);
	ModelMesh->SetStaticMesh(VehicleMesh);
}