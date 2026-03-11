#include "Tasks/MM_CoreSampleTask.h"

AMM_CoreSampleTask::AMM_CoreSampleTask()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
}

void AMM_CoreSampleTask::BeginPlay()
{
	Super::BeginPlay();
	
}

