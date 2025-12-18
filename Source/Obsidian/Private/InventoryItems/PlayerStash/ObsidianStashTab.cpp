// Copyright 2024 out of sCope team - Michał Ogiński


#include "InventoryItems/PlayerStash//ObsidianStashTab.h"

UObsidianStashTab::UObsidianStashTab(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FString UObsidianStashTab::GetStashTabName() const
{
	return StashTabName;
}

FGameplayTag UObsidianStashTab::GetStashTabTag() const
{
	return StashTabTag;
}

EObsidianStashTabAccessability UObsidianStashTab::GetStashAccessabilityType() const
{
	return StashTabAccessabilityType;
}

EObsidianStashTabType UObsidianStashTab::GetStashTabType() const
{
	return StashTabType;
}

TSubclassOf<UObsidianStashTabWidget> UObsidianStashTab::GetWidgetClass() const
{
	return StashTabWidgetClass;
}

void UObsidianStashTab::SetStashData(const FObsidianStashTabDefinition& InDefinition)
{
	StashTabTag = InDefinition.StashTag;
	StashTabName = InDefinition.StashTabName;
	StashTabAccessabilityType = InDefinition.StashTabAccessabilityType;
	StashTabType = InDefinition.StashTabType;
	StashTabWidgetClass = InDefinition.StashTabWidgetClass;
}



