// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/PlayerStash/ObsidianStashTabsConfig.h"

// ~ Core

// ~ Project
UObsidianStashTabsConfig::UObsidianStashTabsConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TArray<FObsidianStashTabDefinition> UObsidianStashTabsConfig::GetStashTabDefinitions() const
{
	return StashTabs;
}

int32 UObsidianStashTabsConfig::StashTabCount() const
{
	return StashTabs.Num();
}
