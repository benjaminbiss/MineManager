#include "TerrainSystem/MM_WorldData.h"

#include "TerrainSystem/FMM_ChunkData.h"
#include "TerrainSystem/FMM_CellLayer.h"
#include "TerrainSystem/FMM_CellData.h"
#include "TerrainSystem/EMM_CellGeologyType.h"
#include "TerrainSystem/MM_SurfacePCGChunkGenerator.h"

AMM_WorldData::AMM_WorldData()
{
	PrimaryActorTick.bCanEverTick = false;

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

	SurfacePCGChunkGenerator = GetWorld()->SpawnActor<AMM_SurfacePCGChunkGenerator>(SurfacePCGChunkGeneratorClass);
	SurfacePCGChunkGenerator->SetActorHiddenInGame(true);
	SurfacePCGChunkGenerator->SetActorLabel(TEXT("Surface PCG Chunk Generator"));
	SurfacePCGChunkGenerator->SetOwner(this);
	SurfacePCGChunkGenerator->SetupSurfaceDataPcg(ChunkSize, CellSize, MapSize, WorldDepth, Seed, SurfaceHeightMultiplier, SubsurfaceHeightMultiplier, SubsurfaceHeightOffset);
	SurfacePCGChunkGenerator->OnSurfaceChunkGenerated.AddDynamic(this, &AMM_WorldData::WriteSurfaceDataToChunkData);

	GenerateWorldData();
}

void AMM_WorldData::GenerateWorldData()
{
	int32 ChunkCount = ChunkDataArray.Num();
	if (ChunkCount < MapSize*MapSize)
	{
		int32 x = ChunkCount % MapSize;
		int32 y = ChunkCount / MapSize;
		RequestChunkGeneration(x, y);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("All chunk data generated."));
	}
}

void AMM_WorldData::RequestChunkGeneration(int32 X, int32 Y)
{
	FMM_ChunkData Chunk = FMM_ChunkData(X, Y);
	ChunkDataArray.Add(Chunk);
	SurfacePCGChunkGenerator->GenerateChunkData(Chunk);
}

void AMM_WorldData::WriteSurfaceDataToChunkData(const TArray<double>& HeighValues)
{
	FMM_ChunkData& CurrentChunkData = ChunkDataArray.Last(); // Get the most recently added chunk data, which is the one currently being generated
	UE_LOG(LogTemp, Log, TEXT("Received surface height data for chunk (%d, %d)"), CurrentChunkData.CoordX, CurrentChunkData.CoordY);

	for (int32 i = 0; i < HeighValues.Num(); i++)
	{
		double HeightValue = HeighValues[i];
		int32 x = HeighValues.Num() % ChunkSize;
		int32 y = HeighValues.Num() / ChunkSize;
		FMM_CellData CellData = FMM_CellData(x, y);
		// Add layer of type Empty and MaxLayerHeight = 3200, to simplify rendering and mining logic by ensuring every cell has at least one layer representing the surface height
		FMM_CellLayer EmptyLayer = FMM_CellLayer();
		CellData.CellLayers.Add(EmptyLayer);
		// Add layer of type Empty and MaxLayerHeight = HeightValue, representing the surface height at this cell
		FMM_CellLayer CellLayer = FMM_CellLayer(HeightValue, EMM_CellGeologyType::CoverSoil);
		CellData.CellLayers.Add(CellLayer);
		CurrentChunkData.Cells.Add(CellData);
	}
	OnChunkDataGenerated.Broadcast(CurrentChunkData);
	GenerateWorldData();
}