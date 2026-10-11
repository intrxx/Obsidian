// Copyright 2026 out of sCope team - intrxx


#include "InventoryItems/PlayerStash//ObsidianStashTab.h"


UObsidianStashTab::UObsidianStashTab(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
}

void UObsidianStashTab::SetStashData(const FObsidianStashTabDefinition& InDefinition)
{
	StashTabTag = InDefinition.StashTag;
	StashTabName = InDefinition.StashTabName;
	StashTabAccessabilityType = InDefinition.StashTabAccessabilityType;
	StashTabType = InDefinition.StashTabType;
	StashTabWidgetClass = InDefinition.StashTabWidgetClass;
}



