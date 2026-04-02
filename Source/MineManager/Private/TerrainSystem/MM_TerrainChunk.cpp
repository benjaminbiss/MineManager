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

void AMM_TerrainChunk::GenerateChunkMesh(const int32 Dimensions, const int32 TriSize, UMaterialInterface* TerrainMaterialInterface, const int32 CellSize, const float LineThickness, const float GridOpacity, const FLinearColor GridColor)
{
	// Set the material parameter for the chunk's material instance dynamic
    TerrainMID = UMaterialInstanceDynamic::Create(TerrainMaterialInterface, this);
    TerrainMID->SetScalarParameterValue("CellSize", CellSize);
    TerrainMID->SetScalarParameterValue("LineThickness", LineThickness);
    TerrainMID->SetScalarParameterValue("GridOpacity", GridOpacity);
    TerrainMID->SetVectorParameterValue("GridColor", GridColor);
    ChunkDimensions = Dimensions;
    // Set up the weightmap texture
    WeightmapTexture = UTexture2D::CreateTransient(
        Dimensions, Dimensions, PF_B8G8R8A8
    );
    // Configure sampling to prevent wrap/edge bleeding
    WeightmapTexture->AddressX = TA_Clamp;
    WeightmapTexture->AddressY = TA_Clamp;
    WeightmapTexture->Filter = TF_Bilinear; // smooth sampling but still no wrap
    WeightmapTexture->MipGenSettings = TMGS_NoMipmaps;
    WeightmapTexture->CompressionSettings = TC_VectorDisplacementmap;
    WeightmapTexture->SRGB = false;
    WeightmapTexture->UpdateResource();
    TArray<FColor> Pixels;
    Pixels.SetNum(Dimensions * Dimensions);
    for (int y = 0; y < Dimensions; y++)
    {
        for (int x = 0; x < Dimensions; x++)
        {
            Pixels[y * Dimensions + x] = FColor(255, 0, 0, 0);
        }
    }
    // Define region, pitch and bytes-per-pixel
    FUpdateTextureRegion2D Region(0, 0, 0, 0, ChunkDimensions, ChunkDimensions); // destX, destY, srcX, srcY, width, height
    const uint32 BytesPerPixel = sizeof(FColor);                       // PF_B8G8R8A8 -> 4
    const uint32 Pitch = Dimensions * BytesPerPixel;                   // bytes per row

    // Make a heap copy for the async update and provide a cleanup lambda
    const int64 TotalBytes = static_cast<int64>(Pixels.Num()) * BytesPerPixel;
    uint8* SrcData = new uint8[TotalBytes];
    FMemory::Memcpy(SrcData, Pixels.GetData(), TotalBytes);

    if (WeightmapTexture->GetSizeX() == ChunkDimensions)
    {
        WeightmapTexture->UpdateTextureRegions(
            0,                                      // mip index
            1,                                      // num regions
            &Region,                                // regions
            Pitch,                                  // src pitch (bytes per row)
            BytesPerPixel,                          // src bytes per pixel
            SrcData,                                // source data
            [](uint8* InSrcData, const FUpdateTextureRegion2D* InRegions) // cleanup
            {
                delete[] InSrcData;
            }
        );
    }

    // 1. Set up Empty Data
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
        static const FName WeightmapParamName(TEXT("WeightmapTexture"));
        TerrainMID->SetTextureParameterValue(WeightmapParamName, WeightmapTexture);
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
    double ArraySize = Vertices.Num();
    if (Chunk.Cells.Num() != ArraySize)
    {
        UE_LOG(LogTemp, Warning, TEXT("UpdateChunkMesh | Mismatch between chunk cell count and vertex count."));
        return;
	}

    TArray<FColor> Pixels;
    Pixels.SetNum(ArraySize);
    for (int32 i = 0; i < ArraySize; i++)
    {
        // Assuming the first layer is the empty air layer
        Vertices[i].Z = Chunk.Cells[i].CellLayers[1].Height; // Adjust the height of the vertices according to chunk data
        EMM_CellGeologyType vertType = Chunk.Cells[i].CellLayers[1].CellGeoType; // Update texture weight map based on geology type for visualization
        switch (vertType)
        {
            case EMM_CellGeologyType::CoverSoil:
                Pixels[i] = FColor(255, 0, 0, 0); 
                break;
			case EMM_CellGeologyType::Overburden:
                Pixels[i] = FColor(0, 255, 0, 0);
                break;
            case EMM_CellGeologyType::Rock:
                Pixels[i] = FColor(0, 0, 255, 0); 
				break;
			case EMM_CellGeologyType::Coal:
                Pixels[i] = FColor(0, 0, 0, 255);
            break;
            default:
                Pixels[i] = FColor(0, 0, 0, 0);   
                break;
        }
    }
    // Define region, pitch and bytes-per-pixel
    if (WeightmapTexture->GetSizeX() == ChunkDimensions)
    {
	    FUpdateTextureRegion2D Region(0, 0, 0, 0, ChunkDimensions, ChunkDimensions); // destX, destY, srcX, srcY, width, height
	    const uint32 BytesPerPixel = sizeof(FColor);                       // PF_B8G8R8A8 -> 4
	    const uint32 Pitch = ChunkDimensions * BytesPerPixel;                   // bytes per row

	    // Make a heap copy for the async update and provide a cleanup lambda
	    const int64 TotalBytes = static_cast<int64>(Pixels.Num()) * BytesPerPixel;
	    uint8* SrcData = new uint8[TotalBytes];
	    FMemory::Memcpy(SrcData, Pixels.GetData(), TotalBytes);

	    WeightmapTexture->UpdateTextureRegions(
	        0,                                      // mip index
	        1,                                      // num regions
	        &Region,                                // regions
	        Pitch,                                  // src pitch (bytes per row)
	        BytesPerPixel,                          // src bytes per pixel
	        SrcData,                                // source data
	        [](uint8* InSrcData, const FUpdateTextureRegion2D* InRegions) // cleanup
	        {
	            delete[] InSrcData;
	        }
	    );
	    static const FName WeightmapParamName(TEXT("WeightmapTexture"));
	    TerrainMID->SetTextureParameterValue(WeightmapParamName, WeightmapTexture); // Updates Material Texture
    }
    else
    {
		UE_LOG(LogTemp, Warning, TEXT("UpdateChunkMesh | Weightmap texture size mismatch with chunk dimensions."));
    }
	RecalculateMesh(); // Updates Mesh Vertices and Normals
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
