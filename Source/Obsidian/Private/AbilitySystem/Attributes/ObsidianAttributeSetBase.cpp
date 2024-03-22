// Copyright 2024 Michał Ogiński


#include "AbilitySystem/Attributes/ObsidianAttributeSetBase.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"

UObsidianAttributeSetBase::UObsidianAttributeSetBase()
{
}

UWorld* UObsidianAttributeSetBase::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}

void UObsidianAttributeSetBase::ClampAttribute(const float MaxAttribute, float& Attribute)
{
	Attribute = FMath::Clamp(Attribute, 0.0f, MaxAttribute);
}

UObsidianAbilitySystemComponent* UObsidianAttributeSetBase::GetObsidianAbilitySystemComponent() const
{
	return Cast<UObsidianAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}
