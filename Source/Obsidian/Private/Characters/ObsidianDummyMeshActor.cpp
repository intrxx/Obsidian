// Copyright 2026 out of sCope team - intrxx

#include "Characters/ObsidianDummyMeshActor.h"

#include "Components/PoseableMeshComponent.h"

#include "Obsidian/ObsidianLogCategories.h"


AObsidianDummyMeshActor::AObsidianDummyMeshActor(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	PoseableMeshComp = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("Dummy Death Mesh"));
	PoseableMeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	PoseableMeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	SetRootComponent(PoseableMeshComp);
}

void AObsidianDummyMeshActor::SetupDummyMeshActor(USkeletalMeshComponent* InMeshToCopy, const float InLifeSpan)
{
	check(InMeshToCopy);
	
	DeadMeshToCopy = InMeshToCopy;
	PoseableMeshComp->SetSkinnedAssetAndUpdate(InMeshToCopy->GetSkeletalMeshAsset(), false);

	SetLifeSpan(InLifeSpan);
}

void AObsidianDummyMeshActor::BeginPlay()
{
	Super::BeginPlay();

	if(IsValid(DeadMeshToCopy))
	{
		PoseableMeshComp->CopyPoseFromSkeletalComponent(DeadMeshToCopy);
		
		UE_LOG(ObLogCharacter, Verbose, TEXT("Spawned Dummy Mesh for [%s]."), *GetNameSafe(this));
	}
}

void AObsidianDummyMeshActor::Destroyed()
{
	UE_LOG(ObLogCharacter, Verbose, TEXT("[%s] Dummy Mesh Destroyed."), *GetNameSafe(this));
	
	Super::Destroyed();
}


