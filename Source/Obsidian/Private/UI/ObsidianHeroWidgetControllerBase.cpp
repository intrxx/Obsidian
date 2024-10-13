// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/ObsidianHeroWidgetControllerBase.h"

void UObsidianHeroWidgetControllerBase::SetWidgetControllerParams(const FObsidianHeroWidgetControllerParams& WidgetControllerParams)
{
	PlayerController = WidgetControllerParams.PlayerController;
	PlayerState = WidgetControllerParams.PlayerState;
	AbilitySystemComponent = WidgetControllerParams.AbilitySystemComponent;
	AttributesComponent = WidgetControllerParams.AttributesComponent;
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
