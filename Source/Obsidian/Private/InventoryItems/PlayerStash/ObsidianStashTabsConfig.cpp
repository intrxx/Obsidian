// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/PlayerStash/ObsidianStashTabsConfig.h"


UObsidianStashTabsConfig::UObsidianStashTabsConfig(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
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
