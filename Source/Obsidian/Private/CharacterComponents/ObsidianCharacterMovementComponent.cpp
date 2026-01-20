// Copyright 2026 out of sCope team - intrxx

#include "CharacterComponents/ObsidianCharacterMovementComponent.h"

// ~ Core

// ~ Project
#include "CharacterComponents/Attributes/ObsidianAttributesComponent.h"

UObsidianCharacterMovementComponent::UObsidianCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

float UObsidianCharacterMovementComponent::GetMaxSpeed() const
{
	const AActor* Owner = GetOwner();
	if(!Owner)
	{
		return Super::GetMaxSpeed();
	}

	const UObsidianAttributesComponent* AttributesComp = UObsidianAttributesComponent::FindCommonAttributesComponent(Owner);
	if(!AttributesComp)
	{
		return Super::GetMaxSpeed();
	}

	return AttributesComp->GetMovementSpeed();
}
