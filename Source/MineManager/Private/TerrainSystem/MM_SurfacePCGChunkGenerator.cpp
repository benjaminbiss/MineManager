#include "TerrainSystem/MM_SurfacePCGChunkGenerator.h"

#include "PCGGraph.h"
#include "PCGComponent.h"
#include "Data/PCGPointData.h"
#include "Data/PCGPointArrayData.h"
#include "Metadata/Accessors/PCGAttributeAccessorKeys.h"
#include "Metadata/Accessors/IPCGAttributeAccessor.h"
#include "Metadata/Accessors/PCGAttributeAccessorHelpers.h"


AMM_SurfacePCGChunkGenerator::AMM_SurfacePCGChunkGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(Root);

	SurfacePCGComponent = CreateDefaultSubobject<UPCGComponent>(TEXT("SurfacePCGComponent"));
}

void AMM_SurfacePCGChunkGenerator::SetupSurfaceDataPcg(const int32 InChunkSize, const int32 InCellSize, const int32 InMapSize,
	const int32 InWorldDepth, const int32 InSeed, const int32 InSurfaceHeightMultiplier, const int32 InSubsurfaceHeightMultiplier, const int32 InSubsurfaceHeightOffset) const
{
	UPCGGraphInstance* Instance = SurfacePCGComponent->GetGraphInstance();
	Instance->SetGraphParameter(FName("Seed"), InSeed);
	Instance->SetGraphParameter(FName("ChunkSize"), InChunkSize);
	Instance->SetGraphParameter(FName("CellSize"), InCellSize);
	Instance->SetGraphParameter(FName("SurfaceHeightMultiplier"), InSurfaceHeightMultiplier);
	Instance->SetGraphParameter(FName("SubsurfaceHeightMultiplier"), InSubsurfaceHeightMultiplier);
	Instance->SetGraphParameter(FName("SubsurfaceHeightOffset"), InSubsurfaceHeightOffset);
}

void AMM_SurfacePCGChunkGenerator::GenerateChunkData(const FMM_ChunkData& OutChunkData) const
{
	UE_LOG(LogTemp, Log, TEXT("Generating surface data for chunk (%d, %d)"), OutChunkData.CoordX, OutChunkData.CoordY);

	UPCGGraphInstance* Instance = SurfacePCGComponent->GetGraphInstance();
	Instance->SetGraphParameter(FName("ChunkX"), OutChunkData.CoordX);
	Instance->SetGraphParameter(FName("ChunkY"), OutChunkData.CoordY);

	SurfacePCGComponent->GenerateLocal(EPCGComponentGenerationTrigger::GenerateAtRuntime, true);
}

void AMM_SurfacePCGChunkGenerator::BeginPlay()
{
	Super::BeginPlay();

	SurfacePCGComponent->OnPCGGraphGeneratedDelegate.AddUObject(this, &AMM_SurfacePCGChunkGenerator::OnPCGGraphGenerated);
}

void AMM_SurfacePCGChunkGenerator::OnPCGGraphGenerated(UPCGComponent* InComponent)
{
	//UE_LOG(LogTemp, Log, TEXT("PCG Graph generated for component: %s"), *GetNameSafe(InComponent));
	FPCGDataCollection DataCollection = SurfacePCGComponent->GetGeneratedGraphOutput();
	for (const FPCGTaggedData& TaggedData : DataCollection.TaggedData)
	{
		// Check if this specific item has your tag
		if (TaggedData.Tags.Contains(TEXT("OutData")))
		{
			//UE_LOG(LogTemp, Log, TEXT("Found tagged data with tag 'OutData'"));
			const UPCGData* PCGData = TaggedData.Data.Get();
			const UPCGPointArrayData* PointDataArr = Cast<UPCGPointArrayData>(PCGData);
			const UPCGPointData* FlattenedData = PointDataArr->ToPointData(nullptr);
			if (FlattenedData)
			{
				if (!FlattenedData || !FlattenedData->Metadata) continue;

				const TArray<FPCGPoint>& Points = FlattenedData->GetPoints();
				const int32 NumPoints = Points.Num();
				if (NumPoints == 0) continue;

				// 1. Create the Accessor for the specific attribute name and type
				const FPCGAttributePropertySelector Selector(FName("SurfaceHeight"));
				const TUniquePtr<const IPCGAttributeAccessor> Accessor = PCGAttributeAccessorHelpers::CreateConstAccessor(PCGData, Selector);

				if (Accessor.IsValid())
				{
					// 2. Create the "Keys" - this tells the accessor which points we want to read
					FPCGAttributeAccessorKeysPoints Keys(Points);
					// 3. Prepare a local array to hold all the results
					TArray<double> HeightValues;
					HeightValues.SetNumUninitialized(NumPoints);
					// 4. Batch Read
					if (Accessor->GetRange<double>(HeightValues, 0, Keys))
					{
						OnSurfaceChunkGenerated.Broadcast(HeightValues);
						// 5.Iterate local array and the Points array in parallel
						for (int32 i = 0; i < NumPoints; ++i)
						{
							const FPCGPoint& CurrentPoint = Points[i];
							double CurrentHeight = HeightValues[i];
							//UE_LOG(LogTemp, Log, TEXT("Point %d: SurfaceHeight = %f"), i, CurrentHeight);
						}
					}
				}
			}
		}
	}
	// TODO: Broadcast the generated data to the world data actor, which will insert it into the corresponding chunk data
}
