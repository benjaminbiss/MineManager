#include "Cameras/MM_PlayerCameraPawn_ManagerMode.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Controllers/MM_PlayerController.h"

AMM_PlayerCameraPawn_ManagerMode::AMM_PlayerCameraPawn_ManagerMode()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

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
	
	SpringArm->TargetArmLength = 4000.f;
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
	PrimaryActorTick.SetTickFunctionEnable(Value);
}
void AMM_PlayerCameraPawn_ManagerMode::HandleZoomInput(float Value)
{
	ZoomIn(Value);
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

void AMM_PlayerCameraPawn_ManagerMode::ZoomIn(float Value)
{
	if (SpringArm && !FMath::IsNearlyZero(Value))
	{
		SpringArm->TargetArmLength = FMath::Clamp(SpringArm->TargetArmLength + Value * ZoomSpeed, MinArmLength, MaxArmLength);
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