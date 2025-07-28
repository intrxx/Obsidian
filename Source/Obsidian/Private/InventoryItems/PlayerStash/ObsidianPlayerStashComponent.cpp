// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/PlayerStash/ObsidianPlayerStashComponent.h"

// ~ Core

// ~ Project

UObsidianPlayerStashComponent::UObsidianPlayerStashComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

}

int32 UObsidianPlayerStashComponent::FindAllStacksForGivenItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef)
{
	//TODO Implement
	return 0;
}

int32 UObsidianPlayerStashComponent::FindAllStacksForGivenItem(const UObsidianInventoryItemInstance* ItemInstance)
{
	//TODO Implement
	return 0;
}


