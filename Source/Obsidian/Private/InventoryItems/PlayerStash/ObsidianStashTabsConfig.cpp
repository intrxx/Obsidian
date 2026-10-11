// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/PlayerStash/ObsidianStashTabsConfig.h"


UObsidianStashTabsConfig::UObsidianStashTabsConfig(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
}

int32 UObsidianStashTabsConfig::StashTabCount() const
{
	return StashTabs.Num();
}
