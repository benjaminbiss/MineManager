#include "Controllers/MM_WorkerAIController.h"

#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"

void AMM_WorkerAIController::BeginPlay()
{
    Super::BeginPlay();

}

void AMM_WorkerAIController::MaintainHeightAboveGround()
{
    if (GetMoveStatus() == EPathFollowingStatus::Idle)
    {
		UE_LOG(LogTemp, Warning, TEXT("Worker is idle, skipping height adjustment."));
        return; // No movement, so no need to adjust height
	}

    FTimerHandle GroundCheckTimer;
    GetWorld()->GetTimerManager().SetTimer(
        GroundCheckTimer,
        this,
        &AMM_WorkerAIController::MaintainHeightAboveGround,
        0.1f, // Adjust the interval as needed
        false
	);

    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (NavSys)
    {
    	FNavLocation Projected;
		FVector CurrentLocation = GetPawn()->GetActorLocation();
    	FVector CurrentXY = FVector(CurrentLocation.X, CurrentLocation.Y, 0.0f);
    	NavSys->ProjectPointToNavigation(CurrentXY, Projected);
    	float GroundZ = Projected.Location.Z;
    	FVector NewLocation = FVector(CurrentLocation.X, CurrentLocation.Y, GroundZ);
		GetPawn()->SetActorLocation(NewLocation);
    }
}

void AMM_WorkerAIController::MovePawnToLocation(FVector TargetLocation)
{
    FNavLocation Projected;
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

    MoveToLocation(TargetLocation);     
    MaintainHeightAboveGround();        
}
