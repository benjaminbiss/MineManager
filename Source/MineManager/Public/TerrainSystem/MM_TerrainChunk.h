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
	void GenerateChunkMesh(const int32 Dimensions, const int32 TriSize, UMaterialInstanceDynamic* TerrainMidInst);
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

protected:
	virtual void BeginPlay() override;

	void UpdateVertexHeight(const int32 VertexIndex, float HeightDelta);
	void CalculateMesh(); // Complete mesh generation including collision
	void RecalculateMesh(); // Purley visual mesh update without collision mesh regeneration, for better performance when applying small deformations
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
