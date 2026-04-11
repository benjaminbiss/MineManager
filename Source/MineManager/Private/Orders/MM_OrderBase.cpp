#include "Orders/MM_OrderBase.h"

void UMM_OrderBase::PreviewOrder(const FHitResult& HitResult)
{
}

void UMM_OrderBase::ExecuteOrder(const FHitResult& HitResult)
{
}

void UMM_OrderBase::CancelOrder()
{
}

void UMM_OrderBase::TickOrder(float DeltaTime)
{
}

EOrderType UMM_OrderBase::GetOrderType() const
{ 
	return EOrderType::None; 
}