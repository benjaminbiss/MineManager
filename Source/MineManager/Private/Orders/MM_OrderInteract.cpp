#include "Orders/MM_OrderInteract.h"

void UMM_OrderInteract::PreviewOrder(const TArray<FIntVector>& SelectedCells)
{
	// Gather information about the hovered actor or cell based on mouse position and time still to display it in the details panel
	// This will involve some an event dispatch to the UI system to update the details panel with the relevant actor or cell information
	// TODO: 
	// 1. Implement an InformationInterface that actors can implement to provide relevant information to the UI when hovered over or clicked on
	// this will allow for a more flexible and extensible system for displaying information about different types of actors and cells in the game
	// InformationInterface::GetOnHoverInformation() - gather basic information to fill a tooltip box or small contextual info panel
}

void UMM_OrderInteract::ExecuteOrder(const TArray<FIntVector>& SelectedCells)
{
	// Display the gathered worker or object info based on what the player clicked on
	// InformationInterface::GetOnClickedInformation() - gather more details to fill a details panel with more in depth information about the clicked actor or cell,
	// such as worker stats, inventory details, cell resource information etc.
}

void UMM_OrderInteract::CancelOrder()
{
	// Clear any displayed information in the details contextual pannel for the previewOrder
	// Dont handle ExecuteOrder cancellation here as Interact orders because a window should be provided for detail panels which will be closed with the ui X button
}

void UMM_OrderInteract::TickOrder(float DeltaTime)
{
}

EOrderType UMM_OrderInteract::GetOrderType() const
{
	return EOrderType::Interact;
}
