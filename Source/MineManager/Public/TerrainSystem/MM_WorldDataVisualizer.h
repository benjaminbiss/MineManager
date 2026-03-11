#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MM_WorldDataVisualizer.generated.h"

UCLASS()
class MINEMANAGER_API AMM_WorldDataVisualizer : public AActor
{
	GENERATED_BODY()
	
public:	
	AMM_WorldDataVisualizer();

protected:
	virtual void BeginPlay() override;

};
