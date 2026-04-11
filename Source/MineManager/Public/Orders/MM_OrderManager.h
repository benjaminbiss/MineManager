#pragma once

#include "Orders/MM_OrderInteract.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MM_OrderManager.generated.h"

class AMM_WorldData; 

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOrderDispatch, const UMM_OrderBase*, NewOrder);

UCLASS()
class MINEMANAGER_API AMM_OrderManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AMM_OrderManager();

	UFUNCTION() void HandleOnSelectStarted(const FHitResult& HitResult);
	UFUNCTION() void HandleOnSelectTriggered(const FHitResult& HitResult);
	UFUNCTION() void HandleOnSelectCompleted(const FHitResult& HitResult);
	UFUNCTION() void HandleOnSecondarySelect(bool bPressed);

	UPROPERTY(BlueprintAssignable, Category = "MyParameters|Orders")
	FOnOrderDispatch OnOrderDispatch;
	
	AMM_WorldData* WorldData;

protected:
	virtual void BeginPlay() override;

	UMM_OrderInteract* CreateInteractOrder();

	// This is the Order state that will be dispatched when a player on click is detected. 
	// It will be set by the UI when the player clicks on an order button
	// Default is Interact Order
	class UMM_OrderBase* CurrentOrder;

	bool bIsSelecting;
	FVector SelectedLocationStart;
	FVector SelectedLocationCurrent;
};
