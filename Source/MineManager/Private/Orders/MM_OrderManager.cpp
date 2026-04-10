#include "Orders/MM_OrderManager.h"

AMM_OrderManager::AMM_OrderManager()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AMM_OrderManager::HandleOnSelectStarted(const FHitResult& HitResult)
{
}

void AMM_OrderManager::HandleOnSelectTriggered(const FHitResult& HitResult)
{
}

void AMM_OrderManager::HandleOnSelectCompleted(const FHitResult& HitResult)
{
}

void AMM_OrderManager::HandleOnSecondarySelect(bool bPressed)
{
}

void AMM_OrderManager::BeginPlay()
{
	Super::BeginPlay();
	
}
