// Copyright 2026 out of sCope team - intrxx

#include "UI/InventoryItems/Stash/ObsidianStashButton.h"


void UObsidianStashButton::NativeConstruct()
{
	Super::NativeConstruct();

	OnClicked().AddUObject(this, &ThisClass::OnStashButtonClicked);
}

void UObsidianStashButton::NativeDestruct()
{
	Super::NativeDestruct();

	OnClicked().Clear();
}

void UObsidianStashButton::InitializeStashButton(const FGameplayTag& StashTag, const FText& StashTabName)
{
	CorrespondingStashTag = StashTag;

	SetButtonText(StashTabName);
}

void UObsidianStashButton::OnStashButtonClicked()
{
	OnStashTabButtonPressedDelegate.Broadcast(CorrespondingStashTag);
}


