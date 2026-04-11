#include "Orders/MM_OrderManager.h"

#include "Orders/MM_OrderInteract.h"
#include "TerrainSystem/MM_WorldData.h"

AMM_OrderManager::AMM_OrderManager()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AMM_OrderManager::HandleOnSelectStarted(const FHitResult& HitResult)
{
	//UE_LOG(LogTemp, Log, TEXT("Select Started at world position: %s"), *HitResult.Location.ToString());
	bIsSelecting = true;
	SelectedLocationStart = HitResult.Location;
}

void AMM_OrderManager::HandleOnSelectTriggered(const FHitResult& HitResult)
{
	if (!bIsSelecting)
		return;
	if (SelectedLocationCurrent == HitResult.Location)	
		return;
	
	//UE_LOG(LogTemp, Log, TEXT("Select Currently at world position: %s"), *HitResult.Location.ToString());
	SelectedLocationCurrent = HitResult.Location;
}

void AMM_OrderManager::HandleOnSelectCompleted(const FHitResult& HitResult)
{
	if (!bIsSelecting)
		return;

	//UE_LOG(LogTemp, Log, TEXT("Select Ended at world position: %s"), *HitResult.Location.ToString());
	bIsSelecting = false;
	SelectedLocationCurrent = HitResult.Location;
	
	if (CurrentOrder == nullptr)
		CurrentOrder = CreateInteractOrder();

	CurrentOrder->ExecuteOrder(HitResult);
}

void AMM_OrderManager::HandleOnSecondarySelect(bool bPressed)
{
	bIsSelecting = false;
	//UE_LOG(LogTemp, Log, TEXT("Order Cancelled = true"));

	SelectedLocationStart = FVector();
	SelectedLocationCurrent = FVector();
}

void AMM_OrderManager::BeginPlay()
{
	Super::BeginPlay();	

	bIsSelecting = false;
}

UMM_OrderInteract* AMM_OrderManager::CreateInteractOrder()
{
	UMM_OrderInteract* order = NewObject<UMM_OrderInteract>();
	order->SetWorldData(WorldData);
	return order;
}
