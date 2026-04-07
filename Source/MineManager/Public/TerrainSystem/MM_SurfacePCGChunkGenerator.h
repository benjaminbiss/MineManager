#pragma once

#include "TerrainSystem/FMM_ChunkData.h"

#include "PCGData.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MM_SurfacePCGChunkGenerator.generated.h"

class UPCGGraphInstance;
class UPCGGraph;
class UPCGComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSurfaceChunkGenerated, const TArray<double>&, SurfaceValues, const TArray<double>&, SubsurfaceValues);

UCLASS()
class MINEMANAGER_API AMM_SurfacePCGChunkGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	AMM_SurfacePCGChunkGenerator();

	void SetupSurfaceDataPcg(const int32 InChunkSize, const int32 InCellSize, const int32 InMapSize,
		const int32 InWorldDepth, const int32 InSeed, const int32 InSurfaceHeightMultiplier, const int32 InSubsurfaceHeightMultiplier, const int32 InSubsurfaceHeightOffset) const;
	void GenerateChunkData(const FMM_ChunkData& OutChunkData) const;
	void UpdateSeed(const int32 InSeed) const;

	UPROPERTY(BlueprintAssignable)
	FOnSurfaceChunkGenerated OnSurfaceChunkGenerated;

protected:
	void BeginPlay() override;

	UFUNCTION()
	void OnPCGGraphGenerated(UPCGComponent* InComponent);

	UPROPERTY(VisibleAnywhere, Category = "MyParameters|PCG Graphs")
	UPCGComponent* SurfacePCGComponent;
};
