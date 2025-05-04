// Copyright 2024 out of sCope team - Michał Ogiński


#include "Characters/ObsidianCharacterCreationHero.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"

AObsidianCharacterCreationHero::AObsidianCharacterCreationHero(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	MeshComp->SetCustomDepthStencilValue(ObsidianHighlight::White);
	MeshComp->SetRenderCustomDepth(false);

	UCharacterMovementComponent* CharacterMovement = GetCharacterMovement();
	CharacterMovement->DisableMovement();
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
