#pragma once

#include "ProceduralMeshComponent.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TerrainChunk.generated.h"

UCLASS()
class MINEMANAGER_API ATerrainChunk : public AActor
{
	GENERATED_BODY()
	
public:	
	ATerrainChunk();

protected:
	virtual void BeginPlay() override;
	void GenerateHeightMap(int32 Seed, int32 Dimensions, int32 TriSize, float NoiseScale, float HeightMultiplier);
	void SetupHeightDeltaMap(TArray<float> InHeightDeltaMap);
	void UpdateMeshWithHeightDeltaMap();
	void UpdateVertexHeight(int32 VertexIndex, float HeightDelta);
	void RecalculateMesh();
	void GenerateTriangles();

	// Mesh data
	UPROPERTY()
	UProceduralMeshComponent* Mesh;
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

public:	
	void GenerateSquareMesh(int32 Seed, int32 Dimensions, int32 TriSize, float NoiseScale, float HeightMultiplier, TArray<float> InHeightDeltaMap);
	void ApplyDeformationToHeightDeltaMap(FIntPoint VertCoord, bool bRaise);

	UPROPERTY()
	FIntPoint ChunkCoord;
	UPROPERTY()
	int32 ChunkDimensions;
};
