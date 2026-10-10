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

void UObsidianStashButton::InitializeStashButton(const FGameplayTag& InStashTag, const FText& InStashTabName)
{
	CorrespondingStashTag = InStashTag;

	SetButtonText(InStashTabName);
}

void UObsidianStashButton::OnStashButtonClicked()
{
	OnStashTabButtonPressedDelegate.Broadcast(CorrespondingStashTag);
}


