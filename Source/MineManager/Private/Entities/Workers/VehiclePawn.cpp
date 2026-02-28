#include "Entities/Workers/VehiclePawn.h"

#include "GameFramework/FloatingPawnMovement.h"
#include "NavigationInvokerComponent.h"

AVehiclePawn::AVehiclePawn()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));
	MovementComponent->UpdatedComponent = RootComponent;

	NavInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("Invoker"));
}

void AVehiclePawn::BeginPlay()
{
	Super::BeginPlay();
	
	NavInvoker->SetGenerationRadii(NavGenerationRadius, NavRemovalRadius);
}

void AVehiclePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
