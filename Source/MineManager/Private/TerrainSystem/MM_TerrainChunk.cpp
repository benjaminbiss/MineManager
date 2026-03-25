#include "TerrainSystem/MM_TerrainChunk.h"

#include "KismetProceduralMeshLibrary.h"
#include "NavigationSystem.h"

AMM_TerrainChunk::AMM_TerrainChunk()
{
	PrimaryActorTick.bCanEverTick = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->bUseAsyncCooking = true; // Enable async cooking for better performance when generating meshes at runtime
    Mesh->bNavigationRelevant = true;
    Mesh->SetCanEverAffectNavigation(true);
    Mesh->bUseComplexAsSimpleCollision = true;
}

void AMM_TerrainChunk::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMM_TerrainChunk::GenerateChunkMesh(const int32 Dimensions, const int32 TriSize, UMaterialInstanceDynamic* TerrainMidInst)
{
	// Set the material parameter for the chunk's material instance dynamic
    TerrainMID = TerrainMidInst;
    ChunkDimensions = Dimensions;

    // 1. Setup Empty Data
    Vertices.Empty();
    Triangles.Empty();
    UVs.Empty();
    Normals.Empty();
    Tangents.Empty();

	// 2. Generate Vertices
    Vertices.Empty();
    UVs.Empty();

    for (int32 y = 0; y < ChunkDimensions; y++)
    {
        for (int32 x = 0; x < ChunkDimensions; x++)
        {
            Vertices.Add(FVector(x * TriSize, y * TriSize, 0));
            UVs.Add(FVector2D(static_cast<float>(x) / (ChunkDimensions - 1), static_cast<float>(y) / (ChunkDimensions - 1)));
        }
    }

	// 3. Generate Triangles
    for (int32 y = 0; y < ChunkDimensions - 1; y++)
    {
        for (int32 x = 0; x < ChunkDimensions - 1; x++)
        {
            int32 I0 = (y * ChunkDimensions) + x;
            int32 I1 = I0 + 1;
            int32 I2 = I0 + ChunkDimensions;
            int32 I3 = I2 + 1;

            // First triangle
            Triangles.Add(I0);
            Triangles.Add(I2);
            Triangles.Add(I1);

            // Second triangle
            Triangles.Add(I1);
            Triangles.Add(I2);
            Triangles.Add(I3);
        }
    }
	// 4. Calculate normals and tangents for proper lighting
    UKismetProceduralMeshLibrary::CalculateTangentsForMesh(
        Vertices,
        Triangles,
        UVs,
        Normals,
        Tangents
    );
    // 5. Create the mesh section and apply material
    Mesh->CreateMeshSection_LinearColor(
        0,
        Vertices,
        Triangles,
        Normals,
        UVs,
        TArray<FLinearColor>(),
        Tangents,
        true // enable collision
    );
    if (TerrainMID)
    {
        TerrainMID->SetVectorParameterValue("ChunkOrigin", ChunkCord);
		Mesh->SetMaterial(0, TerrainMID);
    }
    // 6. Set collision behavior
    //Mesh->bUseComplexAsSimpleCollision = true;
    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Mesh->SetCollisionObjectType(ECC_WorldStatic);
    Mesh->SetCollisionResponseToAllChannels(ECR_Block);
    // 7. Mark as navigation-relevant
    //Mesh->SetCanEverAffectNavigation(true);
    Mesh->bFillCollisionUnderneathForNavmesh = true;
    // 8. Update internal states (bounds, render, physics)
    Mesh->UpdateBounds();
    Mesh->MarkRenderStateDirty();
    Mesh->RecreatePhysicsState();
    // 9. Register with navigation system
    UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetCurrent(GetWorld());
    if (NavigationSystem)
    {
        NavigationSystem->OnComponentRegistered(Mesh);
        FTimerHandle Timer;
        GetWorld()->GetTimerManager().SetTimer(
            Timer,
            this,
            &AMM_TerrainChunk::UpdateMeshNavigation,
            0.05f,   // 50 ms delay
            false
        );
    }
}

void AMM_TerrainChunk::UpdateChunkMesh(const FMM_ChunkData& Chunk)
{
    for (int32 i = 0; i < Vertices.Num(); i++)
    {
        Vertices[i].Z = Chunk.Cells[i].CellLayers[1].Height; // Assuming the first layer is the empty air layer
    }
	RecalculateMesh();
}

void AMM_TerrainChunk::UpdateVertexHeight(const int32 VertexIndex, float HeightDelta)
{
    //Vertices[VertexIndex].Z = SeededHeightMap[VertexIndex] + HeightDeltaMap[VertexIndex];
}

void AMM_TerrainChunk::CalculateMesh()
{
    UKismetProceduralMeshLibrary::CalculateTangentsForMesh(
        Vertices,
        Triangles,
        UVs,
        Normals,
        Tangents
    );
    Mesh->CreateMeshSection_LinearColor(
        0,
        Vertices,
        Triangles,
        Normals,
        UVs,
        TArray<FLinearColor>(),
        Tangents,
        true
    );
    UpdateMeshNavigation();
}

void AMM_TerrainChunk::RecalculateMesh()
{
    // Recalculate normals after height changes
    UKismetProceduralMeshLibrary::CalculateTangentsForMesh(
        Vertices,
        Triangles,
        UVs,
        Normals,
        Tangents
    );
    // Update the mesh section with new vertices and normals
    Mesh->UpdateMeshSection_LinearColor(
        0,
        Vertices,
        Normals,
        UVs,
        TArray<FLinearColor>(),
        Tangents
    );    
}

void AMM_TerrainChunk::UpdateMeshNavigation() const
{
    UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetCurrent(GetWorld());
    if (NavigationSystem)
    {
        NavigationSystem->UpdateComponentInNavOctree(*Mesh);
	}
}

//void AMM_TerrainChunk::ApplyDeformationToHeightDeltaMap(const FIntPoint VertCord, const bool bRaise)
//{
//	int32 index = VertCord.Y * ChunkDimensions + VertCord.X;
//	HeightDeltaMap[index] += bRaise ? 25.0f : -25.0f;
//	UpdateVertexHeight(index, HeightDeltaMap[index]);
//    CalculateMesh();
//}

//void AMM_TerrainChunk::GetHeightAtPoint(const FIntPoint& VertCord, int32& OutHeight) const
//{
//    int32 index = VertCord.Y * ChunkDimensions + VertCord.X;
//	OutHeight = Vertices[index].Z;
//}
