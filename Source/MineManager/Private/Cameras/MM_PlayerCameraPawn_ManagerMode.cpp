#include "Cameras/MM_PlayerCameraPawn_ManagerMode.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Controllers/MM_PlayerController.h"

AMM_PlayerCameraPawn_ManagerMode::AMM_PlayerCameraPawn_ManagerMode()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Root);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void AMM_PlayerCameraPawn_ManagerMode::BeginPlay()
{
	Super::BeginPlay();
	
	TargetZoomLevel = 4000.f;
	SpringArm->TargetArmLength = TargetZoomLevel;
	SpringArm->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
	SpringArm->bDoCollisionTest = false;
}

void AMM_PlayerCameraPawn_ManagerMode::PossessedBy(AController* NewController)
{
	if (AMM_PlayerController* PC = Cast<AMM_PlayerController>(NewController))
	{
		PC->OnMoveInput.AddDynamic(this, &AMM_PlayerCameraPawn_ManagerMode::HandleMoveInput);
		PC->OnRotateInput.AddDynamic(this, &AMM_PlayerCameraPawn_ManagerMode::HandleRotateInput);
		PC->OnRotateHoldInput.AddDynamic(this, &AMM_PlayerCameraPawn_ManagerMode::HandleMouseRotateInput);
		PC->OnZoomInput.AddDynamic(this, &AMM_PlayerCameraPawn_ManagerMode::HandleZoomInput);
	}
}

// Called every frame
void AMM_PlayerCameraPawn_ManagerMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsRotatingCamera)
		RotateCamera(GetMouseXDelta());

	if (bIsZoomingCamera)
	{
		SmoothZoom(DeltaTime);
	}
}

#pragma region Movement
// Functions to Controller Events Bindings
void AMM_PlayerCameraPawn_ManagerMode::HandleMoveInput(FVector2D Value)
{
	MoveForward(Value.Y);
	MoveRight(Value.X);
}
void AMM_PlayerCameraPawn_ManagerMode::HandleRotateInput(float Value)
{
	RotateCamera(Value);
}
void AMM_PlayerCameraPawn_ManagerMode::HandleMouseRotateInput(bool Value)
{
	bIsRotatingCamera = Value;
}
void AMM_PlayerCameraPawn_ManagerMode::HandleZoomInput(float Value)
{
	TargetZoomLevel = FMath::Clamp(TargetZoomLevel + (Value * ZoomSpeed), MinArmLength, MaxArmLength);
	bIsZoomingCamera = true;
}
// Movement Functions
void AMM_PlayerCameraPawn_ManagerMode::MoveForward(float Value)
{
	if (!FMath::IsNearlyZero(Value))
	{
		FVector Direction = GetActorForwardVector();
		AddActorWorldOffset(Direction * Value * MoveSpeed * GetWorld()->GetDeltaSeconds(), true);
	}
}

void AMM_PlayerCameraPawn_ManagerMode::MoveRight(float Value)
{
	if (!FMath::IsNearlyZero(Value))
	{
		FVector Direction = GetActorRightVector();
		AddActorWorldOffset(Direction * Value * MoveSpeed * GetWorld()->GetDeltaSeconds(), true);
	}
}

void AMM_PlayerCameraPawn_ManagerMode::RotateCamera(float Value)
{
	if (!FMath::IsNearlyZero(Value))
	{
		AddActorWorldRotation(FRotator(0.f, Value * RotationSpeed * GetWorld()->GetDeltaSeconds(), 0.f));
	}
}

void AMM_PlayerCameraPawn_ManagerMode::SmoothZoom(float DeltaTime)
{
	// Smoothly interpolate the current arm length towards the target zoom level
	float CurrentArmLength = SpringArm->TargetArmLength;
	float NewArmLength = FMath::FInterpTo(CurrentArmLength, TargetZoomLevel, DeltaTime, 5.f); // 5.f is the interpolation speed
	SpringArm->TargetArmLength = NewArmLength;
	if (FMath::IsNearlyEqual(NewArmLength, TargetZoomLevel, 1.f)) // If we're close enough to the target, stop zooming
	{
		SpringArm->TargetArmLength = TargetZoomLevel;
		bIsZoomingCamera = false;
	}
}

float AMM_PlayerCameraPawn_ManagerMode::GetMouseXDelta()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
		return 0.f;

	float DeltaX = 0.f;
	float DeltaY = 0.f;
	PC->GetInputMouseDelta(DeltaX, DeltaY);

	return DeltaX;
}
#pragma endregion