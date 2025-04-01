// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/ObsidianWidgetControllerBase.h"

void UObsidianHeroWidgetControllerBase::SetWidgetControllerParams(const FObsidianWidgetControllerParams& WidgetControllerParams)
{
	ObsidianPlayerController = WidgetControllerParams.ObsidianPlayerController;
	ObsidianPlayerState = WidgetControllerParams.ObsidianPlayerState;
	ObsidianAbilitySystemComponent = WidgetControllerParams.ObsidianAbilitySystemComponent;
	AttributesComponent = WidgetControllerParams.AttributesComponent;
	InventoryComponent = WidgetControllerParams.InventoryComponent;
	EquipmentComponent = WidgetControllerParams.EquipmentComponent;
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
