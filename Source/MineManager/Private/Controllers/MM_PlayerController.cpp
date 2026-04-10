#include "Controllers/MM_PlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void AMM_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (MappingContext)
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}

	SetShowMouseCursor(true);
}

void AMM_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(InputComponent);

	EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMM_PlayerController::Move);

	EIC->BindAction(RotateAction, ETriggerEvent::Triggered, this, &AMM_PlayerController::Rotate);

	EIC->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &AMM_PlayerController::Zoom);

	EIC->BindAction(SelectAction, ETriggerEvent::Started, this, &AMM_PlayerController::SelectStarted);
	EIC->BindAction(SelectAction, ETriggerEvent::Triggered, this, &AMM_PlayerController::SelectTriggered);
	EIC->BindAction(SelectAction, ETriggerEvent::Completed, this, &AMM_PlayerController::SelectCompleted);

	EIC->BindAction(SecondarySelectAction, ETriggerEvent::Started, this, &AMM_PlayerController::SecondarySelect);
	//EIC->BindAction(SecondarySelectAction, ETriggerEvent::Triggered, this, &AMM_PlayerController::SecondarySelect);
	//EIC->BindAction(SecondarySelectAction, ETriggerEvent::Completed, this, &AMM_PlayerController::SecondarySelect);

	EIC->BindAction(RotateHoldAction, ETriggerEvent::Started, this, &AMM_PlayerController::RotateHold);
	EIC->BindAction(RotateHoldAction, ETriggerEvent::Completed, this, &AMM_PlayerController::RotateHold);
}


void AMM_PlayerController::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	OnMoveInput.Broadcast(MovementVector);
}

void AMM_PlayerController::Rotate(const FInputActionValue& Value)
{
	float RotationValue = Value.Get<float>();
	OnRotateInput.Broadcast(RotationValue);
}

void AMM_PlayerController::Zoom(const FInputActionValue& Value)
{
	float ZoomValue = Value.Get<float>();
	OnZoomInput.Broadcast(ZoomValue);
}

void AMM_PlayerController::SelectStarted(const FInputActionValue& Value)
{
	FVector WorldOrigin, WorldDirection;
	DeprojectMousePositionToWorld(WorldOrigin, WorldDirection);

	FVector Start = WorldOrigin;
	FVector End = WorldOrigin + WorldDirection * 100000.f;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.bTraceComplex = true;

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
	{
		bool bPressed = Value.Get<bool>();	
		OnSelectedWorldLocationStarted.Broadcast(HitResult);
	}
}
void AMM_PlayerController::SelectTriggered(const FInputActionValue& Value)
{
	//bool bPressed = Value.Get<bool>();
	//OnSelectInput.Broadcast(bPressed);

	FVector WorldOrigin, WorldDirection;
	DeprojectMousePositionToWorld(WorldOrigin, WorldDirection);

	FVector Start = WorldOrigin;
	FVector End = WorldOrigin + WorldDirection * 100000.f;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.bTraceComplex = true;

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
	{
		bool bPressed = Value.Get<bool>();	
		OnSelectedWorldLocationTriggered.Broadcast(HitResult);
	}
}
void AMM_PlayerController::SelectCompleted(const FInputActionValue& Value)
{
	//bool bPressed = Value.Get<bool>();
	//OnSelectInput.Broadcast(bPressed);

	FVector WorldOrigin, WorldDirection;
	DeprojectMousePositionToWorld(WorldOrigin, WorldDirection);

	FVector Start = WorldOrigin;
	FVector End = WorldOrigin + WorldDirection * 50000.f;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.bTraceComplex = true;

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
	{
		bool bPressed = Value.Get<bool>();
		OnSelectedWorldLocationCompleted.Broadcast(HitResult);
	}
}

void AMM_PlayerController::SecondarySelect(const FInputActionValue& Value)
{
	bool bPressed = Value.Get<bool>();
	OnSecondarySelectInput.Broadcast(bPressed);
}

void AMM_PlayerController::RotateHold(const FInputActionValue& Value)
{
	bool bPressed = Value.Get<bool>();
	OnRotateHoldInput.Broadcast(bPressed);
}