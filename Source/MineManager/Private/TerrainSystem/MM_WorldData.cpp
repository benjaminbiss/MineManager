#include "TerrainSystem/MM_WorldData.h"

#include "TerrainSystem/FMM_ChunkData.h"
#include "TerrainSystem/FMM_CellLayer.h"
#include "TerrainSystem/FMM_CellData.h"
#include "TerrainSystem/EMM_CellGeologyType.h"
#include "TerrainSystem/MM_SurfacePCGChunkGenerator.h"

AMM_WorldData::AMM_WorldData()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AMM_WorldData::Tick(float DeltaTime)
{
	ElapsedGenerationTime += DeltaTime;
}

void AMM_WorldData::InitializeWorldDataParameters(const int32 InChunkSize, const int32 InCellSize, const int32 InMapSize,
	const int32 InWorldDepth, const int32 InSeed, const int32 InSurfaceHeightMultiplier, const int32 InSubsurfaceHeightMultiplier, const int32 InSubsurfaceHeightOffset,
	const float InCoalSeamThicknessThreshold, const int32 InCoalSeamHeightOffset, const int32 InCoalSeamDepth)
{
	Seed = InSeed;
	MapSize = InMapSize;
	ChunkSize = InChunkSize;
	CellSize = InCellSize;
	WorldDepth = InWorldDepth;
	SurfaceHeightMultiplier = InSurfaceHeightMultiplier;
	SubsurfaceHeightMultiplier = InSubsurfaceHeightMultiplier;
	SubsurfaceHeightOffset = InSubsurfaceHeightOffset;
	CoalSeamThicknessThreshold = InCoalSeamThicknessThreshold;
	CoalSeamHeightOffset = InCoalSeamHeightOffset;
	CoalSeamDepth = InCoalSeamDepth;

	CreateSurfaceGenerator();

	RegenerateWorldData();
}

void AMM_WorldData::CreateSurfaceGenerator()
{
	SurfacePCGChunkGenerator = GetWorld()->SpawnActor<AMM_SurfacePCGChunkGenerator>(SurfacePCGChunkGeneratorClass);
	SurfacePCGChunkGenerator->SetActorHiddenInGame(true);
	SurfacePCGChunkGenerator->SetActorLabel(TEXT("Surface PCG Chunk Generator"));
	SurfacePCGChunkGenerator->SetOwner(this);
	SurfacePCGChunkGenerator->SetupSurfaceDataPcg(ChunkSize, CellSize, MapSize, WorldDepth, Seed, SurfaceHeightMultiplier, SubsurfaceHeightMultiplier, SubsurfaceHeightOffset);
	SurfacePCGChunkGenerator->OnSurfaceChunkGenerated.AddDynamic(this, &AMM_WorldData::WriteSurfaceDataToChunkData);
}

// Triggers a full world regeneration
void AMM_WorldData::RegenerateWorldData()
{
	GeneratedChunkCount = 0;
	GenerateWorldData();
	SurfacePCGChunkGenerator->UpdateSeed(FMath::Rand());
}

void AMM_WorldData::GenerateWorldData()
{	
	if (GeneratedChunkCount < MapSize*MapSize)
	{
		int32 x = GeneratedChunkCount % MapSize;
		int32 y = GeneratedChunkCount / MapSize;
		RequestChunkGeneration(x, y);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("All chunk data generated."));
		UE_LOG(LogTemp, Log, TEXT("Generation Time: %f."), ElapsedGenerationTime);
		//FTimerHandle RegenerationTimerHandle;
		//GetWorld()->GetTimerManager().SetTimer(RegenerationTimerHandle, this, &AMM_WorldData::RegenerateWorldData, 3.0f, false);
	}
}

void AMM_WorldData::RequestChunkGeneration(int32 X, int32 Y)
{
	FMM_ChunkData Chunk = FMM_ChunkData(X, Y);
	int32 ChunkIndex = X + Y * MapSize;	
	if (ChunkDataArray.Num() <= ChunkIndex)
	{
		// Ensure the array is large enough to hold the chunk at the given index
		ChunkDataArray.SetNum(ChunkIndex + 1);
	}	
	ChunkDataArray[ChunkIndex] = Chunk;
	SurfacePCGChunkGenerator->GenerateChunkData(Chunk);
}

void AMM_WorldData::WriteSurfaceDataToChunkData(const TArray<double>& SurfaceValues, const TArray<double>& SubsurfaceValues)
{
	FMM_ChunkData& CurrentChunkData = ChunkDataArray[GeneratedChunkCount]; // Get the most recently added chunk data, which is the one currently being generated
	GeneratedChunkCount++;
	UE_LOG(LogTemp, Log, TEXT("Received surface height data for chunk (%d, %d)"), CurrentChunkData.CoordX, CurrentChunkData.CoordY);

	int32 NumPoints = SurfaceValues.Num() == SubsurfaceValues.Num() ? SurfaceValues.Num() : 0;
	for (int32 i = 0; i < NumPoints; i++)
	{
		double SurfaceValue = SurfaceValues[i];
		double SubsurfaceValue = SubsurfaceValues[i];
		int32 x = SurfaceValues.Num() % ChunkSize;
		int32 y = SurfaceValues.Num() / ChunkSize;
		FMM_CellData CellData = FMM_CellData(x, y);
		// Add layer of type Empty and MaxLayerHeight = 3200, to simplify rendering and mining logic by ensuring every cell has at least one layer representing the surface height
		FMM_CellLayer Layer = FMM_CellLayer();
		CellData.CellLayers.Add(Layer);
		// Compare surface and subsurface values to determine the geology type of the layer
		if (SurfaceValue > SubsurfaceValue) // Soil -> Overburden - > Rock
		{
			Layer = FMM_CellLayer(SurfaceValue, EMM_CellGeologyType::CoverSoil);
			CellData.CellLayers.Add(Layer);
			Layer = FMM_CellLayer(SurfaceValue - 100, EMM_CellGeologyType::Overburden);
			CellData.CellLayers.Add(Layer);
			Layer = FMM_CellLayer(SubsurfaceValue, EMM_CellGeologyType::Rock);
			CellData.CellLayers.Add(Layer);
		}
		if (SurfaceValue == SubsurfaceValue) // Overburden - > Rock
		{
			Layer = FMM_CellLayer(SurfaceValue, EMM_CellGeologyType::Overburden);
			CellData.CellLayers.Add(Layer);
			Layer = FMM_CellLayer(SurfaceValue - 100, EMM_CellGeologyType::Rock);
			CellData.CellLayers.Add(Layer);
		}
		if (SurfaceValue < SubsurfaceValue) // Rock
		{
			Layer = FMM_CellLayer(SubsurfaceValue, EMM_CellGeologyType::Rock);
			CellData.CellLayers.Add(Layer);
		}
		CurrentChunkData.Cells.Add(CellData);
	}
	OnChunkDataGenerated.Broadcast(CurrentChunkData);
	GenerateWorldData();
}