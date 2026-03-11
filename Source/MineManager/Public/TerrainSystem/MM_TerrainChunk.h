#pragma once

#include "ProceduralMeshComponent.h"

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
	//void GenerateHeightMap(const int32 Seed, const int32 Dimensions, const int32 TriSize, const float NoiseScale, const float HeightMultiplier);
	//void SetupHeightDeltaMap(const TArray<float>& InHeightDeltaMap);
	//void UpdateMeshWithHeightDeltaMap();
	void UpdateVertexHeight(const int32 VertexIndex, float HeightDelta);
	void CalculateMesh(); // Complete mesh generation including collision
	void RecalculateMesh(); // Purley visual mesh update without collision mesh regeneration, for better performance when applying small deformations
	void UpdateMeshNavigation() const;

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
	//UPROPERTY()
	//TArray<float> SeededHeightMap;
	//UPROPERTY()
	//TArray<float> HeightDeltaMap;

	// Chunk data
	//UPROPERTY()
	//TArray<FMM_CellData> Cells;

public:	
	void GenerateChunkMesh(const int32 Seed, const int32 Dimensions, const int32 TriSize, const float NoiseScale, const float HeightMultiplier, const TArray<float>& InHeightDeltaMap, UMaterialInstanceDynamic* TerrainMidInst);
	//void ApplyDeformationToHeightDeltaMap(const FIntPoint VertCord, const bool bRaise);
	//void GetHeightAtPoint(const FIntPoint& VertCord, int32& OutHeight) const;

	UPROPERTY()
	UProceduralMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|Material")
	UMaterialInstanceDynamic* TerrainMID;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|ChunkInfo")
	FVector ChunkCord;
	UPROPERTY(VisibleAnywhere, Category = "MyParameters|ChunkInfo")
	int32 ChunkDimensions;
};
