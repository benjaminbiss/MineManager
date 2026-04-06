#pragma once

#include "ProceduralMeshComponent.h"

#include "TerrainSystem/FMM_ChunkData.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MM_TerrainChunk.generated.h"

UCLASS()
class MINEMANAGER_API AMM_TerrainChunk : public AActor
{
	GENERATED_BODY()
	
public:	
	AMM_TerrainChunk();
	void GenerateChunkMesh(const int32 Dimensions, UMaterialInterface* TerrainMaterialInterface, const int32 CellDimension, const float LineThickness, const float GridOpacity, const FLinearColor GridColor);
	void UpdateChunkMesh(const FMM_ChunkData& Chunk);

	UPROPERTY()
	UProceduralMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Material")
	UTexture2D* WeightmapTexture;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Material")
	UMaterialInstanceDynamic* TerrainMID;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|ChunkInfo")
	FVector ChunkCord;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|ChunkInfo")
	int32 ChunkDimensions;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|ChunkInfo")
	int32 CellSize;

protected:
	virtual void BeginPlay() override;
	void SetupTerrainMaterial(UMaterialInterface* TerrainMaterialInterface, const int32 CellDimension, const float LineThickness, const float GridOpacity, const FLinearColor GridColor);
	void WriteWeightMapInfo(TArray<FColor> Pixels);

	void SmoothBorderNormals(const FMM_ChunkData& Chunk);
	void RecalculateMesh(const FMM_ChunkData& Chunk, const bool RecalculateNavMesh);
	void UpdateMeshNavigation() const;

	UPROPERTY()
	TArray<int32> Triangles;
	UPROPERTY()
	TArray<FVector> Vertices;
	UPROPERTY()
	TArray<FVector2D> UVs;
	UPROPERTY()
	TArray<FVector> Normals;
	UPROPERTY()
	TArray<FProcMeshTangent> Tangents;	
};
