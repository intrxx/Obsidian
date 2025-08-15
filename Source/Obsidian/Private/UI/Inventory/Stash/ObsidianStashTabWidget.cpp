// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/Stash/ObsidianStashTabWidget.h"

// ~ Core

// ~ Project

void UObsidianStashTabWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);
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


