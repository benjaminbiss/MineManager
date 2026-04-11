#pragma once

#include "Orders/MM_OrderInterface.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MM_OrderBase.generated.h"


UCLASS(Abstract)
class MINEMANAGER_API UMM_OrderBase : public UObject, public IMM_OrderInterface
{
	GENERATED_BODY()
	
public:
	// Default implementations for order interface, can be overridden by specific order types as needed
	virtual void PreviewOrder(const FHitResult& HitResult) override;
	virtual void ExecuteOrder(const FHitResult& HitResult) override;
	virtual void CancelOrder() override;
	virtual void TickOrder(float DeltaTime) override;
	virtual EOrderType GetOrderType() const override;

protected:
	bool IsPreviewingOrder;
};
