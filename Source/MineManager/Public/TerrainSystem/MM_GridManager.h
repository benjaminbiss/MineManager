#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Microsoft/AllowMicrosoftPlatformTypes.h"
#include "MM_GridManager.generated.h"

UCLASS()
class MINEMANAGER_API AMM_GridManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AMM_GridManager();
	void InitializeGridParameters(const int32 InGridSize, const int32 InCellSize);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Grid")
	int32 GridSize; // Number of cells along one edge of the grid

	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Grid")
	int32 CellSize; // Size of each cell in world units
};
