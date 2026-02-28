#include "TerrainSystem/TerrainChunk.h"

#include "KismetProceduralMeshLibrary.h"
#include "NavigationSystem.h"

#include "ThirdParty/FastNoiseLite.h"

ATerrainChunk::ATerrainChunk()
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

void ATerrainChunk::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATerrainChunk::GenerateSquareMesh(int32 Seed, int32 Dimensions, int32 TriSize, float NoiseScale, float HeightMultiplier, TArray<float> InHeightDeltaMap)
{
    // 1. Setup Empty Data
    Vertices.Empty();
    Triangles.Empty();
    UVs.Empty();
    Normals.Empty();
    Tangents.Empty();
	// 2. Generate Verticies and Apply Height Delta Map
	GenerateHeightMap(Seed, Dimensions, TriSize, NoiseScale, HeightMultiplier);
	SetupHeightDeltaMap(InHeightDeltaMap);
	UpdateMeshWithHeightDeltaMap();
	// 3. Generate Triangles
    for (int32 y = 0; y < ChunkDimensions - 1; y++)
    {
        for (int32 x = 0; x < ChunkDimensions - 1; x++)
        {
            int32 i0 = (y * ChunkDimensions) + x;
            int32 i1 = i0 + 1;
            int32 i2 = i0 + ChunkDimensions;
            int32 i3 = i2 + 1;

            // First triangle
            Triangles.Add(i0);
            Triangles.Add(i2);
            Triangles.Add(i1);

            // Second triangle
            Triangles.Add(i1);
            Triangles.Add(i2);
            Triangles.Add(i3);
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
    // 5. Create the mesh section 
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
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (NavSys)
    {
        NavSys->OnComponentRegistered(Mesh);
        FTimerHandle Timer;
        GetWorld()->GetTimerManager().SetTimer(
            Timer,
            this,
            &ATerrainChunk::UpdateMeshNavigation,
            0.05f,   // 50 ms delay
            false
        );
    }
}

void ATerrainChunk::GenerateHeightMap(int32 Seed, int32 Dimensions, int32 TriSize, float NoiseScale, float HeightMultiplier)
{
	ChunkDimensions = Dimensions;

    FastNoiseLite Noise;
    Noise.SetSeed(Seed);
    Noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    Noise.SetFrequency(NoiseScale);

    Vertices.Empty();
    UVs.Empty();

    for (int32 y = 0; y < ChunkDimensions; y++)
    {
        for (int32 x = 0; x < ChunkDimensions; x++)
        {
            float GlobalX = (ChunkCoord.X * (ChunkDimensions - 1) + x);
            float GlobalY = (ChunkCoord.Y * (ChunkDimensions - 1) + y);

            float NoiseValue = Noise.GetNoise(GlobalX, GlobalY);
            float Height = NoiseValue * HeightMultiplier;
            SeededHeightMap.Add(Height);

            Vertices.Add(FVector(x * TriSize, y * TriSize, Height));
            UVs.Add(FVector2D((float)x / (ChunkDimensions - 1), (float)y / (ChunkDimensions - 1)));
        }
    }
}

void ATerrainChunk::SetupHeightDeltaMap(TArray<float> InHeightDeltaMap)
{
    // Create a new height delta map if the input is empty or has a different size
    if (InHeightDeltaMap.IsEmpty() || InHeightDeltaMap.Num() != Vertices.Num())
    {		
        for (int32 i = 0; i < Vertices.Num(); i++)
        {
            HeightDeltaMap.Add(0.0f);
		}
        return;
	}
	HeightDeltaMap = InHeightDeltaMap;
}

void ATerrainChunk::UpdateMeshWithHeightDeltaMap()
{
    for (int32 i = 0; i < Vertices.Num(); i++)
    {
		UpdateVertexHeight(i, HeightDeltaMap[i]);
    }
}

void ATerrainChunk::UpdateVertexHeight(int32 VertexIndex, float HeightDelta)
{
    Vertices[VertexIndex].Z = SeededHeightMap[VertexIndex] + HeightDeltaMap[VertexIndex];
}

void ATerrainChunk::CalculateMesh()
{
    // Rebuild collision
    Mesh->ContainsPhysicsTriMeshData(true);
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
}

void ATerrainChunk::RecalculateMesh()
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
    UpdateMeshNavigation();
}

void ATerrainChunk::UpdateMeshNavigation()
{
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (NavSys)
    {
        NavSys->UpdateComponentInNavOctree(*Mesh);
	}
}

void ATerrainChunk::ApplyDeformationToHeightDeltaMap(FIntPoint VertCoord, bool bRaise)
{
	int32 index = VertCoord.Y * ChunkDimensions + VertCoord.X;
	HeightDeltaMap[index] += bRaise ? 25.0f : -25.0f;
	UpdateVertexHeight(index, HeightDeltaMap[index]);
    RecalculateMesh();
}
