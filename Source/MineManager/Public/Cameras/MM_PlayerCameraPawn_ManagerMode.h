
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MM_PlayerCameraPawn_ManagerMode.generated.h"

UCLASS()
class MINEMANAGER_API AMM_PlayerCameraPawn_ManagerMode : public APawn
{
	GENERATED_BODY()

public:
	AMM_PlayerCameraPawn_ManagerMode();

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MyParameters|Components")
	class USceneComponent* Root;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MyParameters|Components")
	class UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MyParameters|Components")
	class USpringArmComponent* SpringArm;

	// Camera Settings
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MyParameters|Movement", meta = (ToolTip = "Units per second"))
	float MoveSpeed = 2000.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MyParameters|Movement", meta = (ToolTip = "Degrees per second"))
	float RotationSpeed = 90.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MyParameters|Movement", meta = (ToolTip = "Units per mouse wheel tick"))
	float ZoomSpeed = 1000.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MyParameters|Movement")
	float TargetZoomLevel;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MyParameters|Movement")
	bool bIsZoomingCamera = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MyParameters|Movement", meta = (ToolTip = "Units"))
	float MinArmLength = 1000.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MyParameters|Movement", meta = (ToolTip = "Units"))
	float MaxArmLength = 10000.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MyParameters|Movement")
	bool bIsRotatingCamera = false;

	// Movement Bindings To Controller Events
	UFUNCTION()
	void HandleMoveInput(FVector2D Value);
	UFUNCTION()
	void HandleRotateInput(float Value);
	UFUNCTION()
	void HandleMouseRotateInput(bool Value);
	UFUNCTION()
	void HandleZoomInput(float Value);

	// Movement Functions
	void MoveForward(float Value);
	void MoveRight(float Value);
	void RotateCamera(float Value);
	void SmoothZoom(float DeltaTime);
	float GetMouseXDelta();

public:	
	virtual void Tick(float DeltaTime) override;

};
