// Copyright 2026 out of sCope team - intrxx

#include "UI/InventoryItems/Stash/ObsidianStashTabWidget.h"

void UObsidianStashTabWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);
}

FGameplayTag UObsidianStashTabWidget::GetStashTabTag() const
{
	return StashTabTag;
}

void UObsidianStashTabWidget::ShowStashTab()
{
	SetVisibility(ESlateVisibility::Visible);
	bActive = true;
}

void UObsidianStashTabWidget::HideStashTab()
{
	SetVisibility(ESlateVisibility::Collapsed);
	bActive = false;
}

bool UObsidianStashTabWidget::IsTabActive()
{
	return bActive;
}


