#include "TerrainSystem/MM_WorldData.h"

#include "TerrainSystem/FMM_CellLayer.h"
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

	int32 GeneratorCount = FMath::Floor(MapSize / 2);
	for (int32 i = 0; i < GeneratorCount; i++)
		CreateSurfaceGenerator();

	RegenerateWorldData();
}

FMM_ChunkData& AMM_WorldData::GetChunkData(FVector Location)
{
	int32 ChunkIndex = GetChunkIndex(Location);
	if (ChunkIndex < 0 || ChunkIndex >= ChunkDataArray.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("GetCellData | Received an invalid chunk index: %d"), ChunkIndex);		
		return ChunkDataArray[0];
	}
	return ChunkDataArray[ChunkIndex];
}

FMM_CellData& AMM_WorldData::GetCellData(FVector Location)
{
	//UE_LOG(LogTemp, Log, TEXT("Chunk: %d - Cell: %d"), GetChunkIndex(Location), GetCellIndex(Location));
	int32 ChunkIndex = GetChunkIndex(Location);
	int32 CellIndex = GetCellIndex(Location);
	if (ChunkIndex < 0 || ChunkIndex >= ChunkDataArray.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("GetCellData | Received an invalid chunk index: %d"), ChunkIndex);
		return ChunkDataArray[0].Cells[0];
	}
	else if (CellIndex < 0 || CellIndex >= ChunkDataArray[ChunkIndex].Cells.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("GetCellData | Received an invalid cell index: %d"), CellIndex);
		return ChunkDataArray[0].Cells[0];
	}
	return ChunkDataArray[ChunkIndex].Cells[CellIndex];
}

void AMM_WorldData::CreateSurfaceGenerator()
{
	AMM_SurfacePCGChunkGenerator* generator = GetWorld()->SpawnActor<AMM_SurfacePCGChunkGenerator>(SurfacePCGChunkGeneratorClass);
	SurfacePCGChunkGenerator.Add(generator, FIntPoint(-1, -1));
	generator->SetActorHiddenInGame(true);
	generator->SetActorLabel(TEXT("Surface PCG Chunk Generator"));
	generator->SetOwner(this);
	generator->SetupSurfaceDataPcg(ChunkSize, CellSize, MapSize, WorldDepth, Seed, SurfaceHeightMultiplier, SubsurfaceHeightMultiplier, SubsurfaceHeightOffset);
	//generator->OnSurfaceChunkGenerated.AddDynamic(this, &AMM_WorldData::WriteSurfaceDataToChunkData);
	generator->OnPCGGraphGeneration.AddDynamic(this, &AMM_WorldData::WriteSurfaceDataToChunkData);
}

// Triggers a full world regeneration
void AMM_WorldData::RegenerateWorldData()
{
	GeneratedChunkCount = 0;
	GenerateWorldData();
	//SurfacePCGChunkGenerator->UpdateSeed(FMath::Rand());
}

void AMM_WorldData::GenerateWorldData()
{	
	bool complete = true;
	for (auto& pair : SurfacePCGChunkGenerator)
	{		
		if (pair.Value != FIntPoint(-1, -1))
		{
			complete = false;
			continue; // Skip generators that are currently active
		}
		if (GeneratedChunkCount < MapSize*MapSize)
		{
			int32 x = GeneratedChunkCount % MapSize;
			int32 y = GeneratedChunkCount / MapSize;
			FMM_ChunkData Chunk = FMM_ChunkData(x, y);
			int32 ChunkIndex = x + (y * MapSize);
			if (ChunkDataArray.Num() <= ChunkIndex)
			{
				// Ensure the array is large enough to hold the chunk at the given index
				ChunkDataArray.SetNum(ChunkIndex + 1);
			}
			ChunkDataArray[ChunkIndex] = Chunk;
			pair.Key->GenerateChunkData(Chunk);
			pair.Value = FIntPoint(x, y); // Mark the generator as active
			GeneratedChunkCount++;
		}
	}
	if (complete)
	{
		UE_LOG(LogTemp, Log, TEXT("All chunk data generated."));
		UE_LOG(LogTemp, Log, TEXT("Generation Time: %f."), ElapsedGenerationTime);
		return;
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
	//SurfacePCGChunkGenerator->GenerateChunkData(Chunk);
}

void AMM_WorldData::WriteSurfaceDataToChunkData(const AMM_SurfacePCGChunkGenerator* Generator, const TArray<double>& SurfaceValues, const TArray<double>& SubsurfaceValues)
{
	auto GeneratorCoordinates = SurfacePCGChunkGenerator.Find(Generator);
	int32 index = GeneratorCoordinates->X + GeneratorCoordinates->Y * MapSize;
	if (index < 0 || index >= ChunkDataArray.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("WriteSurfaceDataToChunkData | Received an invalid chunk of index: %d, %d"), GeneratorCoordinates->X, GeneratorCoordinates->Y);
		return;
	}
	SurfacePCGChunkGenerator[Generator] = FIntPoint(-1, -1); // Mark the generator as inactive
	FMM_ChunkData& CurrentChunkData = ChunkDataArray[index]; // Get the most recently added chunk data, which is the one currently being generated
	//UE_LOG(LogTemp, Log, TEXT("Received surface height data for chunk (%d, %d)"), CurrentChunkData.CoordX, CurrentChunkData.CoordY);

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

int32 AMM_WorldData::GetChunkIndex(FVector Location)
{
	int32 X = FMath::Floor(Location.X / (ChunkSize * CellSize));
	int32 Y = FMath::Floor(Location.Y / (ChunkSize * CellSize));
	return X + Y * MapSize;
}

int32 AMM_WorldData::GetCellIndex(FVector Location)
{
	int32 X = FMath::Floor(Location.X / CellSize);
	int32 Y = FMath::Floor(Location.Y / CellSize);
	X = X % ChunkSize;
	Y = Y % ChunkSize;
	return X + Y * ChunkSize;
}
