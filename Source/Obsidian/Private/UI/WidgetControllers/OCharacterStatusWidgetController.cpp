// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/WidgetControllers/OCharacterStatusWidgetController.h"

#include "AbilitySystem/Data/ObsidianAttributeInfo.h"
#include "CharacterComponents/Attributes/ObsidianHeroAttributesComponent.h"
#include "Obsidian/ObsidianGameplayTags.h"

void UOCharacterStatusWidgetController::OnWidgetControllerSetupCompleted()
{
}

void UOCharacterStatusWidgetController::HandleBindingCallbacks(UObsidianAbilitySystemComponent* ObsidianASC)
{
}

void UOCharacterStatusWidgetController::SetInitialAttributeValues() const
{
	check(AttributeInfo);

	FOAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(ObsidianGameplayTags::Attribute_Strength);
	Info.AttributeValue = AttributesComponent->GetStrength();

	AttributeInfoDelegate.Broadcast(Info);
}
