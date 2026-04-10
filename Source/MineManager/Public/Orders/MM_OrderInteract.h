#pragma once

#include "CoreMinimal.h"
#include "Orders/MM_OrderBase.h"
#include "MM_OrderInteract.generated.h"

UCLASS()
class MINEMANAGER_API UMM_OrderInteract : public UMM_OrderBase
{
	GENERATED_BODY()
	
public:
	// Implementation for order interface specific overriden for Interact order type
	virtual void PreviewOrder(const TArray<FIntVector>& SelectedCells) override;
	virtual void ExecuteOrder(const TArray<FIntVector>& SelectedCells) override;
	virtual void CancelOrder() override;
	virtual void TickOrder(float DeltaTime) override;
	virtual EOrderType GetOrderType() const override;


};
