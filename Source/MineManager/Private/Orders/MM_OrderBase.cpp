#include "Orders/MM_OrderBase.h"

void UMM_OrderBase::PreviewOrder(const TArray<FIntVector>& SelectedCells)
{
}

void UMM_OrderBase::ExecuteOrder(const TArray<FIntVector>& SelectedCells)
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