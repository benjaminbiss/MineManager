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

void AMM_TerrainChunk::GenerateChunkMesh(const int32 Dimensions, UMaterialInterface* TerrainMaterialInterface, const int32 CellDimension, const float LineThickness, const float GridOpacity, const FLinearColor GridColor)
{
	// 1. Initialize Class Variables
    ChunkDimensions = Dimensions;
	CellSize = CellDimension;
	SetupTerrainMaterial(TerrainMaterialInterface, CellDimension, LineThickness, GridOpacity, GridColor);
    
    // 2. Set up Empty Data
    Vertices.Empty();
    Triangles.Empty();
    UVs.Empty();
    Normals.Empty();
    Tangents.Empty();

	// 3. Generate Vertices
    Vertices.Empty();
    UVs.Empty();

	//UE_LOG(LogTemp, Log, TEXT("Generating chunk mesh with dimensions: %d x %d"), ChunkDimensions, ChunkDimensions);

    for (int32 y = 0; y < ChunkDimensions; y++)
    {
        for (int32 x = 0; x < ChunkDimensions; x++)
        {
            Vertices.Add(FVector(x * CellSize, y * CellSize, 0));
            UVs.Add(FVector2D(static_cast<float>(x) / (ChunkDimensions - 1), static_cast<float>(y) / (ChunkDimensions - 1)));
        }
    }
	
	// 4. Generate Triangles
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
    if (TerrainMID && WeightmapTexture)
    {
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

void AMM_TerrainChunk::SetupTerrainMaterial(UMaterialInterface* TerrainMaterialInterface, const int32 CellDimension, const float LineThickness, const float GridOpacity, const FLinearColor GridColor)
{
    // Set the material parameter for the chunk's material instance dynamic
    TerrainMID = UMaterialInstanceDynamic::Create(TerrainMaterialInterface, this);
    TerrainMID->SetScalarParameterValue("CellSize", CellSize);
    TerrainMID->SetScalarParameterValue("LineThickness", LineThickness);
    TerrainMID->SetScalarParameterValue("GridOpacity", GridOpacity);
    TerrainMID->SetVectorParameterValue("GridColor", GridColor);
    TerrainMID->SetVectorParameterValue("ChunkOrigin", ChunkCord);

	//Set up the weight map with a +1 padding to prevent error when setting region 32x32 of 32x32 texture
    WeightmapTexture = UTexture2D::CreateTransient(
        ChunkDimensions, ChunkDimensions, PF_B8G8R8A8
    );
    // Configure sampling to prevent wrap/edge bleeding
    WeightmapTexture->AddressX = TA_Clamp;
    WeightmapTexture->AddressY = TA_Clamp;
    WeightmapTexture->Filter = TF_Bilinear; // smooth sampling but still no wrap
    WeightmapTexture->MipGenSettings = TMGS_NoMipmaps;
    WeightmapTexture->CompressionSettings = TC_VectorDisplacementmap;
    WeightmapTexture->SRGB = false;
    WeightmapTexture->UpdateResource();
    TerrainMID->SetTextureParameterValue("WeightmapTexture", WeightmapTexture);
    // Initialize the weightmap with default values (e.g., all zeros)
    TArray<FColor> Pixels;
    Pixels.SetNum(ChunkDimensions * ChunkDimensions);
    for (int32 i = 0; i < Pixels.Num(); i++)
    {
        Pixels[i] = FColor(255, 0, 0, 0); // default to grass
    }
    // Write the initial pixel data to the texture
    WriteWeightMapInfo(Pixels);
}

void AMM_TerrainChunk::WriteWeightMapInfo(TArray<FColor> Pixels)
{
    if (!WeightmapTexture || !WeightmapTexture->IsValidLowLevel() || !WeightmapTexture->GetResource())
    {
        UE_LOG(LogTemp, Error, TEXT("WriteWeightMapInfo | Invalid texture on chunk %s"), *GetName());
        return;
    }

    // Define region, pitch and bytes-per-pixel
    uint32 InDestX = 0; 
    uint32 InDestY = 0;
    int32 InSrcX = 0;
    int32 InSrcY = 0;
    uint32 InWidth = static_cast<uint32>(ChunkDimensions);
    uint32 InHeight = static_cast<uint32>(ChunkDimensions);
    // Validate pixel array is exactly the right size
    if (Pixels.Num() != (int32)(InWidth * InHeight))
    {
        UE_LOG(LogTemp, Error, TEXT("WriteWeightMapInfo | Pixel count %d doesn't match texture %dx%d=%d"),
            Pixels.Num(), InWidth, InHeight, InWidth * InHeight);
        return;
    }
    // Heap allocate region so it survives until render thread is done
	FUpdateTextureRegion2D* Region = new FUpdateTextureRegion2D(InDestX, InDestY, InSrcX, InSrcY, InWidth, InHeight);

    // Make a heap copy for the async update and provide a cleanup lambda
    const uint32 BytesPerPixel = sizeof(FColor);                                 // PF_B8G8R8A8 -> 4
    const uint32 Pitch = static_cast<uint32>(ChunkDimensions) * BytesPerPixel;   // bytes per row
    const int64 TotalBytes = static_cast<int64>(Pixels.Num()) * BytesPerPixel;
    uint8* SrcData = new uint8[TotalBytes];
    FMemory::Memcpy(SrcData, Pixels.GetData(), TotalBytes);
        
    WeightmapTexture->UpdateTextureRegions(
        0,                                      // mip index
        1,                                      // num regions
        Region,                                 // regions
        Pitch,                                  // src pitch (bytes per row)
        BytesPerPixel,                          // src bytes per pixel
        SrcData,                                // source data
        [](uint8* InSrcData, const FUpdateTextureRegion2D* InRegions) // cleanup
        {
            delete[] InSrcData;
        }
    );
    //UE_LOG(LogTemp, Log, TEXT("UpdateChunkMesh | Weight map texture updated."));        
}

void AMM_TerrainChunk::UpdateChunkMesh(const FMM_ChunkData& Chunk)
{
	//UE_LOG(LogTemp, Log, TEXT("Updating chunk mesh for chunk at (%d, %d)"), Chunk.CoordX, Chunk.CoordY);
    double ArraySize = Vertices.Num();
    //UE_LOG(LogTemp, Log, TEXT("Number of vertices: %f, Number of cells: %d"), ArraySize, Chunk.Cells.Num());
    TArray<FColor> Pixels;
    Pixels.SetNum(WeightmapTexture->GetSizeX() * WeightmapTexture->GetSizeY());
    const int32 CellStride = ChunkDimensions + 2; // 34
    for (int32 y = 0; y < ChunkDimensions; y++) 
    {
        for (int32 x = 0; x < ChunkDimensions; x++)
        {
			int32 vertIndex = y * ChunkDimensions + x; // 32x32 mesh
			int32 chunkIndex = (y + 1) * CellStride + (x + 1); // 34x34 grid, skip ghost border

			Vertices[vertIndex].Z = Chunk.Cells[chunkIndex].CellLayers[1].Height; // UPDATES VERTEX HEIGHT BASED ON HEIGHT OF LAYER BELOW AIR LAYER [0]
			EMM_CellGeologyType vertType = Chunk.Cells[chunkIndex].CellLayers[1].CellGeoType; // UPDATES VERTEX COLOR BASED ON GEOLOGY TYPE
            switch (vertType)
            {
            case EMM_CellGeologyType::CoverSoil:
                Pixels[vertIndex] = FColor(255, 0, 0, 0);
                break;
            case EMM_CellGeologyType::Overburden:
                Pixels[vertIndex] = FColor(0, 255, 0, 0);
                break;
            case EMM_CellGeologyType::Rock:
                Pixels[vertIndex] = FColor(0, 0, 255, 0);
                break;
            case EMM_CellGeologyType::Coal:
                Pixels[vertIndex] = FColor(0, 0, 0, 255);
                break;
            default:
                Pixels[vertIndex] = FColor(0, 0, 0, 0);
                break;
            }
        }
    }
    WriteWeightMapInfo(Pixels);
	RecalculateMesh(Chunk, true); // Updates Mesh Vertices and Normals
}

void AMM_TerrainChunk::SmoothBorderNormals(const FMM_ChunkData& Chunk)
{
    const int32 CellStride = ChunkDimensions + 2; // 34

    // Helper to get height at any position in the 34x34 cell grid
    // cellX and cellY are in 0..33 space
    auto GetHeight = [&](int32 cellX, int32 cellY) -> float
        {
            int32 idx = cellY * CellStride + cellX;
            return Chunk.Cells[idx].CellLayers[1].Height;
        };

    // Central difference normal - samples all 4 neighbours symmetrically
    // cellX/cellY are in 34x34 space (so real mesh verts start at 1,1)
    auto ComputeNormal = [&](int32 cellX, int32 cellY) -> FVector
        {
            float hRight = GetHeight(cellX + 1, cellY);
            float hLeft = GetHeight(cellX - 1, cellY);
            float hUp = GetHeight(cellX, cellY + 1);
            float hDown = GetHeight(cellX, cellY - 1);

            FVector dx = FVector(2.f * CellSize, 0.f, hRight - hLeft);
            FVector dy = FVector(0.f, 2.f * CellSize, hUp - hDown);
            return FVector::CrossProduct(dx, dy).GetSafeNormal();
        };


    for (int32 i = 0; i < ChunkDimensions; i++)
    {
        Normals[i] = ComputeNormal(i + 1, 1); // Bottom border (mesh y == 0) → cell row 1
        Normals[(ChunkDimensions - 1) * ChunkDimensions + i] = ComputeNormal(i + 1, ChunkDimensions); // Top border (mesh y == ChunkDimensions-1) → cell row ChunkDimensions
        Normals[i * ChunkDimensions] = ComputeNormal(1, i + 1); // Left border (mesh x == 0) → cell col 1
        Normals[i * ChunkDimensions + (ChunkDimensions - 1)] = ComputeNormal(ChunkDimensions, i + 1); // Right border (mesh x == ChunkDimensions-1) → cell col ChunkDimensions
    }
}

void AMM_TerrainChunk::RecalculateMesh(const FMM_ChunkData& Chunk, const bool RecalculateNavMesh)
{
    // Recalculate normals after height changes
    UKismetProceduralMeshLibrary::CalculateTangentsForMesh(
        Vertices,
        Triangles,
        UVs,
        Normals,
        Tangents
    );
    SmoothBorderNormals(Chunk);
    // Update the mesh section with new vertices and normals
    Mesh->UpdateMeshSection_LinearColor(
        0,
        Vertices,
        Normals,
        UVs,
        TArray<FLinearColor>(),
        Tangents
    );    

    if (RecalculateNavMesh)
		UpdateMeshNavigation();
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
