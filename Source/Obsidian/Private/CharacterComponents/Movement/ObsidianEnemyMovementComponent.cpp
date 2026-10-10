// Copyright 2026 out of sCope - intrxx

#include "CharacterComponents/Movement/ObsidianEnemyMovementComponent.h"

#include "CharacterComponents/Attributes/ObsidianAttributesComponent.h"


UObsidianEnemyMovementComponent::UObsidianEnemyMovementComponent(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
}

float UObsidianEnemyMovementComponent::GetMaxSpeed() const
{
	const AActor* Owner = GetOwner();
	if(!Owner)
	{
		return Super::GetMaxSpeed();
	}

	const UObsidianAttributesComponent* CommonAttributes = UObsidianAttributesComponent::FindCommonAttributesComponent(Owner);
	if(!CommonAttributes)
	{
		return Super::GetMaxSpeed();
	}

	return CommonAttributes->GetMovementSpeed();
}
