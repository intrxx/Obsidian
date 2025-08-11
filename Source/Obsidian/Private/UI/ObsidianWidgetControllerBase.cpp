// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/ObsidianWidgetControllerBase.h"

// ~ Core

// ~ Project

void UObsidianHeroWidgetControllerBase::SetWidgetControllerParams(const FObsidianWidgetControllerParams& WidgetControllerParams)
{
	ObsidianPlayerController = WidgetControllerParams.ObsidianPlayerController;
	ObsidianPlayerState = WidgetControllerParams.ObsidianPlayerState;
	ObsidianAbilitySystemComponent = WidgetControllerParams.ObsidianAbilitySystemComponent;
	AttributesComponent = WidgetControllerParams.AttributesComponent;
	InventoryComponent = WidgetControllerParams.InventoryComponent;
	EquipmentComponent = WidgetControllerParams.EquipmentComponent;
	PlayerStashComponent = WidgetControllerParams.PlayerStashComponent;
}

void UObsidianHeroWidgetControllerBase::OnWidgetControllerSetupCompleted()
{
}

void UObsidianHeroWidgetControllerBase::HandleBindingCallbacks(UObsidianAbilitySystemComponent* ObsidianASC)
{
}

void UObsidianHeroWidgetControllerBase::SetInitialAttributeValues() const
{
}
