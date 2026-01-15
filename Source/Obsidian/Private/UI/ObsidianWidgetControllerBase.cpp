// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/ObsidianWidgetControllerBase.h"

#include "InventoryItems/Crafting/ObsidianCraftingComponent.h"

void UObsidianHeroWidgetControllerBase::SetWidgetControllerParams(const FObsidianWidgetControllerParams& WidgetControllerParams)
{
	ObsidianPlayerController = WidgetControllerParams.ObsidianPlayerController;
	ObsidianLocalPlayer = WidgetControllerParams.ObsidianLocalPlayer;
	ObsidianPlayerState = WidgetControllerParams.ObsidianPlayerState;
	ObsidianAbilitySystemComponent = WidgetControllerParams.ObsidianAbilitySystemComponent;
	AttributesComponent = WidgetControllerParams.AttributesComponent;
	InventoryComponent = WidgetControllerParams.InventoryComponent;
	EquipmentComponent = WidgetControllerParams.EquipmentComponent;
	PlayerStashComponent = WidgetControllerParams.PlayerStashComponent;
	OwnerCraftingComponent = WidgetControllerParams.CraftingComponent.Get();
}

void UObsidianHeroWidgetControllerBase::OnWidgetControllerSetupCompleted()
{
}

void UObsidianHeroWidgetControllerBase::HandleBindingCallbacks(UObsidianAbilitySystemComponent* ObsidianASC)
{
}

