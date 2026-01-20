// Copyright 2026 out of sCope team - intrxx

#include "Characters/ObsidianCharacterCreationHero.h"

// ~ Core
#include "GameFramework/CharacterMovementComponent.h"

// ~ Project
#include "ObsidianTypes/ObsidianCoreTypes.h"

AObsidianCharacterCreationHero::AObsidianCharacterCreationHero(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	MeshComp->SetCustomDepthStencilValue(ObsidianHighlight::White);
	MeshComp->SetRenderCustomDepth(false);

	UCharacterMovementComponent* CharacterMovementComp = GetCharacterMovement();
	CharacterMovementComp->DisableMovement();
}

void AObsidianCharacterCreationHero::StartHighlight()
{
	if(USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetRenderCustomDepth(true);
	}
}

void AObsidianCharacterCreationHero::StopHighlight()
{
	if(USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetRenderCustomDepth(false);
	}
}

void AObsidianCharacterCreationHero::PlayChooseMeAnimMontage()
{
	if(ChooseMeAnimMontage)
	{
		PlayAnimMontage(ChooseMeAnimMontage);
	}
}

void AObsidianCharacterCreationHero::StopPlayingChooseMeAnimMontage()
{
	if(GetCurrentMontage())
	{
		StopAnimMontage();
	}
}
