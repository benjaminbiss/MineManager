#include "TerrainSystem/MM_WorldData.h"

AMM_WorldData::AMM_WorldData()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AMM_WorldData::InitializeWorldDataParameters(const int32 InChunkDimensionsInCells, const int32 InGridCellSize, 
	const int32 InMapDimensionsInChunks, const int32 InWorldDepth, const int32 InSeed, const float InNoiseScale, const float InSurfaceHeightMultiplier, const float InSubsurfaceHeightOffset)
{
	ChunkDimensionsInCells = InChunkDimensionsInCells;
	GridCellSize = InGridCellSize;
	MapDimensionsInChunks = InMapDimensionsInChunks;
	WorldDepth = InWorldDepth;
	Seed = InSeed;
	NoiseScale = InNoiseScale;
	SurfaceHeightMultiplier = InSurfaceHeightMultiplier;
	SubsurfaceHeightOffset = InSubsurfaceHeightOffset;

	GridSize = MapDimensionsInChunks * ChunkDimensionsInCells; // Total number of cells along one axis of the grid

	GenerateWorldData();
}

void AMM_WorldData::GenerateWorldData()
{
	CreateWorldGeologyInformation();
	CreateCellDataArray();
}

void AMM_WorldData::CreateWorldGeologyInformation()
{
	CreateSurfaceHeightMap();
	CreateSubsurfaceHeightMap();
	CreatePCGMinableVolume();
}

void AMM_WorldData::CreateSurfaceHeightMap()
{
    //FastNoiseLite Noise;
    //Noise.SetSeed(Seed);
    //Noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    //Noise.SetFrequency(NoiseScale);

    for (int32 y = 0; y < GridSize; y++)
    {
        for (int32 x = 0; x < GridSize; x++)
        {
            //const float NoiseValue = Noise.GetNoise(x, y);
            //float Height = NoiseValue * SurfaceHeightMultiplier;
            //SurfaceHeightMap.Add(Height);
        }
    }
}

void AMM_WorldData::CreateSubsurfaceHeightMap()
{
	/*FastNoiseLite Noise;
	Noise.SetSeed(Seed + 1);
	Noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	Noise.SetFrequency(NoiseScale + .01f);*/

	for (int32 y = 0; y < GridSize; y++)
	{
		for (int32 x = 0; x < GridSize; x++)
		{
			//const float NoiseValue = Noise.GetNoise(x, y);
			//float Height = NoiseValue * SurfaceHeightMultiplier - SubsurfaceHeightOffset;
			//SurfaceHeightMap.Add(Height);
		}
	}
}

void AMM_WorldData::CreatePCGMinableVolume()
{
}

void AMM_WorldData::CreateCellDataArray()
{
	for (int32 y = 0; y < GridSize; y++)
	{
		for (int32 x = 0; x < GridSize; x++)
		{
			FMM_CellData CellData;
			CellData.Index = x + y * GridSize; // Calculate the 1D index for the cell at (x, y)
			InsertBaseGeologyLayersIntoCell(CellData);
			InsertPCGMinableVolumeIntoCell(CellData);
			CellDataArray.Add(CellData);
		}
	}
}

void AMM_WorldData::InsertBaseGeologyLayersIntoCell(FMM_CellData& Cell)
{
	
}

void AMM_WorldData::InsertPCGMinableVolumeIntoCell(FMM_CellData& Cell)
{

}