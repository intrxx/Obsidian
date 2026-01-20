// Copyright 2026 out of sCope team - intrxx

#include "Characters/ObsidianDummyMeshActor.h"

// ~ Core

// ~ Project
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
		
		UE_LOG(LogTemp, Display, TEXT("Spawned Dummy Mesh for [%s]."), *GetNameSafe(this));
	}
}

void AObsidianDummyMeshActor::Destroyed()
{
	UE_LOG(LogTemp, Display, TEXT("[%s] Dummy Mesh Destroyed."), *GetNameSafe(this));
	
	Super::Destroyed();
}


