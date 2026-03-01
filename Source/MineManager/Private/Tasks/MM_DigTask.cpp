#include "Tasks/MM_DigTask.h"

AMM_DigTask::AMM_DigTask()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
}

void AMM_DigTask::BeginPlay()
{
	Super::BeginPlay();
		
}

void AMM_DigTask::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

