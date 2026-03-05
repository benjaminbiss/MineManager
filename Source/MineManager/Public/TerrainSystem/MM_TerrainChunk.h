#pragma once

#include "ProceduralMeshComponent.h"

#include "TerrainSystem/FMM_CellData.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MM_TerrainChunk.generated.h"

UCLASS()
class MINEMANAGER_API AMM_TerrainChunk : public AActor
{
	GENERATED_BODY()
	
public:	
	AMM_TerrainChunk();

protected:
	virtual void BeginPlay() override;
	void GenerateHeightMap(int32 Seed, int32 Dimensions, int32 TriSize, float NoiseScale, float HeightMultiplier);
	void SetupHeightDeltaMap(TArray<float> InHeightDeltaMap);
	void UpdateMeshWithHeightDeltaMap();
	void UpdateVertexHeight(int32 VertexIndex, float HeightDelta);
	void CalculateMesh(); // Complete mesh generation including collision
	void RecalculateMesh(); // Purley visual mesh update without collision mesh regeneration, for better performance when applying small deformations
	void UpdateMeshNavigation();

	// Mesh data
public:
	UPROPERTY()
	UProceduralMeshComponent* Mesh;
protected:
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
	UPROPERTY()
	TArray<float> SeededHeightMap;
	UPROPERTY()
	TArray<float> HeightDeltaMap;

	// Chunk data
	UPROPERTY()
	TArray<FMM_CellData> Cells;

public:	
	void GenerateSquareMesh(int32 Seed, int32 Dimensions, int32 TriSize, float NoiseScale, float HeightMultiplier, TArray<float> InHeightDeltaMap);
	void ApplyDeformationToHeightDeltaMap(FIntPoint VertCoord, bool bRaise);

	UPROPERTY()
	FIntPoint ChunkCoord;
	UPROPERTY()
	int32 ChunkDimensions;
};
