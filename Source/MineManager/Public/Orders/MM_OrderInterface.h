#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MM_OrderInterface.generated.h"

UINTERFACE(MinimalAPI)
class UMM_OrderInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MINEMANAGER_API IMM_OrderInterface
{
	GENERATED_BODY()

public:
    // Mandatory implementations for every order type
	virtual void PreviewOrder(const FHitResult& HitResult) = 0;
    virtual void ExecuteOrder(const FHitResult& HitResult) = 0;
    virtual void CancelOrder() = 0;
    virtual void TickOrder(float DeltaTime) = 0;
    virtual EOrderType GetOrderType() const = 0;

    // Optional overrides with default behavior
    //virtual bool CanExecuteOnCell(FIntVector CellCoord) const { return true; }
    //virtual void OnOrderQueued() {}
    //virtual void OnOrderStarted() {}
    //virtual void OnOrderCompleted() {}
};

UENUM()
enum class EOrderType : uint8
{
	None, // Default state, no order
	Interact, // Interact with cell info, workers etc. (default order type when player clicks on anything to gather details or info)
	Survey, // Survey a cell to reveal its contents, will be executed by workers
	Dig, // Dig a cell to extract resources, will be executed by workers
	Dump, // Dump resources in that cell, will be executed by workers
};