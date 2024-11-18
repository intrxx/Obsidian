// Copyright 2024 out of sCope team - Michał Ogiński

#include "CharacterComponents/ObsidianCharacterMovementComponent.h"
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
