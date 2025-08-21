// Copyright 2024 out of sCope team - Michał Ogiński


#include "InventoryItems/PlayerStash//ObsidianStashTab.h"

UObsidianStashTab::UObsidianStashTab(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FGameplayTag UObsidianStashTab::GetStashTabTag() const
{
	return StashTabTag;
}

void UObsidianStashTab::SetStashTabTag(const FGameplayTag& InTag)
{
	StashTabTag = InTag;
}


