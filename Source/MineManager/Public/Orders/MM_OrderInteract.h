#pragma once

#include "CoreMinimal.h"
#include "Orders/MM_OrderBase.h"
#include "MM_OrderInteract.generated.h"

class AMM_WorldData;

UCLASS()
class MINEMANAGER_API UMM_OrderInteract : public UMM_OrderBase
{
	GENERATED_BODY()
	
public:
	void SetWorldData(AMM_WorldData* WorldDataInstance);

	// Implementation for order interface specific overriden for Interact order type
	virtual void PreviewOrder(const FHitResult& HitResult) override;
	virtual void ExecuteOrder(const FHitResult& HitResult) override;
	virtual void CancelOrder() override;
	virtual void TickOrder(float DeltaTime) override;
	virtual EOrderType GetOrderType() const override;

protected:
	AMM_WorldData* WorldData;
};
