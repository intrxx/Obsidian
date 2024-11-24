// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/ObsidianWidgetControllerBase.h"

void UObsidianHeroWidgetControllerBase::SetWidgetControllerParams(const FObsidianWidgetControllerParams& WidgetControllerParams)
{
	PlayerController = WidgetControllerParams.PlayerController;
	PlayerState = WidgetControllerParams.PlayerState;
	AbilitySystemComponent = WidgetControllerParams.AbilitySystemComponent;
	AttributesComponent = WidgetControllerParams.AttributesComponent;
	InventoryComponent = WidgetControllerParams.InventoryComponent;
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
