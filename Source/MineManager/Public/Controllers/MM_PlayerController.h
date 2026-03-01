#pragma once

#include "InputActionValue.h"

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MM_PlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMoveCameraInputSignature, FVector2D, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRotateCameraInputSignature, float, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FZoomCameraInputSignature, float, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSelectInputSignature, bool, bPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSecondarySelectInputSignature, bool, bPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRotateHoldInputSignature, bool, bPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSelectedWorldLocationInputSignature, FVector, WorldLocation, bool, bPressed);

UCLASS()
class MINEMANAGER_API AMM_PlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

public:
    UPROPERTY(EditDefaultsOnly, Category = "MyParameters|Input")
    class UInputMappingContext* MappingContext;

    UPROPERTY(EditDefaultsOnly, Category = "MyParameters|Input")
    class UInputAction* MoveAction;

    UPROPERTY(EditDefaultsOnly, Category = "MyParameters|Input")
    class UInputAction* RotateAction;

    UPROPERTY(EditDefaultsOnly, Category = "MyParameters|Input")
    class UInputAction* ZoomAction;

    UPROPERTY(EditDefaultsOnly, Category = "MyParameters|Input")
    class UInputAction* SelectAction;

    UPROPERTY(EditDefaultsOnly, Category = "MyParameters|Input")
    class UInputAction* SecondarySelectAction;

    UPROPERTY(EditDefaultsOnly, Category = "MyParameters|Input")
    class UInputAction* RotateHoldAction;

    UPROPERTY(BlueprintAssignable, Category = "MyParameters|Input")
    FMoveCameraInputSignature OnMoveInput;

    UPROPERTY(BlueprintAssignable, Category = "MyParameters|Input")
    FRotateCameraInputSignature OnRotateInput;

    UPROPERTY(BlueprintAssignable, Category = "MyParameters|Input")
    FZoomCameraInputSignature OnZoomInput;

    UPROPERTY(BlueprintAssignable, Category = "MyParameters|Input")
    FSelectInputSignature OnSelectInput;

    UPROPERTY(BlueprintAssignable, Category = "MyParameters|Input")
    FSelectInputSignature OnSecondarySelectInput;

    UPROPERTY(BlueprintAssignable, Category = "MyParameters|Input")
    FRotateHoldInputSignature OnRotateHoldInput;

	UPROPERTY(BlueprintAssignable, Category = "MyParameters|Input")
    FSelectedWorldLocationInputSignature OnSelectedWorldLocationStarted;
    UPROPERTY(BlueprintAssignable, Category = "MyParameters|Input")
    FSelectedWorldLocationInputSignature OnSelectedWorldLocationTriggered;
    UPROPERTY(BlueprintAssignable, Category = "MyParameters|Input")
    FSelectedWorldLocationInputSignature OnSelectedWorldLocationCompleted;

    void Move(const FInputActionValue& Value);
    void Rotate(const FInputActionValue& Value);
    void Zoom(const FInputActionValue& Value);
    void SelectStarted(const FInputActionValue& Value);
    void SelectTriggered(const FInputActionValue& Value);
    void SelectCompleted(const FInputActionValue& Value);
	void SecondarySelect(const FInputActionValue& Value);
    void RotateHold(const FInputActionValue& Value);
};
