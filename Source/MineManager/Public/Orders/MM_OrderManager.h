#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MM_OrderManager.generated.h"

UCLASS()
class MINEMANAGER_API AMM_OrderManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AMM_OrderManager();

	UFUNCTION()
	void HandleOnSelectStarted(const FHitResult& HitResult);
	void HandleOnSelectTriggered(const FHitResult& HitResult);
	void HandleOnSelectCompleted(const FHitResult& HitResult);
	void HandleOnSecondarySelect(bool bPressed);

protected:
	virtual void BeginPlay() override;

	// This is the Order state that will be dispatched when a player on click is detected. 
	// It will be set by the UI when the player clicks on an order button
	// Default is Interact Order
	class AMM_OrderBase* CurrentOrder;
};
