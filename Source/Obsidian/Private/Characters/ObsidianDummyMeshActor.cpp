// Copyright 2024 out of sCope team - Michał Ogiński


#include "Characters/ObsidianDummyMeshActor.h"

#include "Components/PoseableMeshComponent.h"

AObsidianDummyMeshActor::AObsidianDummyMeshActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	PoseableMeshComp = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("Dummy Death Mesh"));
	PoseableMeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	PoseableMeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	SetRootComponent(PoseableMeshComp);
}

void AObsidianDummyMeshActor::SetupDummyMeshActor(USkeletalMeshComponent* InMeshToCopy, const float LifeSpan)
{
	check(InMeshToCopy);
	
	DeadMeshToCopy = InMeshToCopy;
	PoseableMeshComp->SetSkinnedAssetAndUpdate(InMeshToCopy->GetSkeletalMeshAsset(), false);

	SetLifeSpan(LifeSpan);
}

void AObsidianDummyMeshActor::BeginPlay()
{
	Super::BeginPlay();

	if(IsValid(DeadMeshToCopy))
	{
		PoseableMeshComp->CopyPoseFromSkeletalComponent(DeadMeshToCopy);
		
		UE_LOG(LogTemp, Error, TEXT("Spawned Dummy Mesh."));
	}
}

void AObsidianDummyMeshActor::Destroyed()
{
	Super::Destroyed();

	UE_LOG(LogTemp, Error, TEXT("Dummy Mesh Destroyed."));
}


