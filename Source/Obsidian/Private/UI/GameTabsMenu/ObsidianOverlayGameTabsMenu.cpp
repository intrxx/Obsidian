// Copyright 2026 out of sCope team - intrxx

#include "UI/GameTabsMenu/ObsidianOverlayGameTabsMenu.h"

// ~ Core
#include "Components/Button.h"

// ~ Project
#include "UI/GameTabsMenu/Subwidgets/ObsidianGameTabButton.h"

void UObsidianOverlayGameTabsMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if(CharacterStatus_GameTabButton && CharacterStatus_GameTabButton->Tab_Button)
	{
		CharacterStatus_GameTabButton->Tab_Button->OnClicked.AddDynamic(this, &ThisClass::OnCharacterStatusButtonClicked);
		OnCharacterStatusTabStatusChangeDelegate.AddUObject(this, &ThisClass::OnCharacterStatusTabStatusChange);
	}

	if(Inventory_GameTabButton && Inventory_GameTabButton->Tab_Button)
	{
		Inventory_GameTabButton->Tab_Button->OnClicked.AddDynamic(this, &ThisClass::OnInventoryButtonClicked);
		OnCharacterStatusTabStatusChangeDelegate.AddUObject(this, &ThisClass::OnInventoryTabStatusChange);
	}

	if(PassiveSkillTree_GameTabButton && PassiveSkillTree_GameTabButton->Tab_Button)
	{
		PassiveSkillTree_GameTabButton->Tab_Button->OnClicked.AddDynamic(this, &ThisClass::OnPassiveSkillTreeButtonClicked);
		OnPassiveSkillTreeTabStatusChangeDelegate.AddUObject(this, &ThisClass::OnPassiveSkillTreeTabStatusChange);
	}
}

void UObsidianOverlayGameTabsMenu::OnCharacterStatusButtonClicked()
{
	OnCharacterStatusButtonClickedDelegate.Broadcast();
}

FReply UObsidianOverlayGameTabsMenu::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// This widgets won't take any input, don't want to pass gameplay input through
	return FReply::Handled();
}

FReply UObsidianOverlayGameTabsMenu::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// This widgets won't take any input, don't want to pass gameplay input through
	return FReply::Handled();
}

void UObsidianOverlayGameTabsMenu::OnCharacterStatusTabStatusChange(bool bIsConstructed)
{
	if(CharacterStatus_GameTabButton)
	{
		CharacterStatus_GameTabButton->bIsCorrespondingTabOpen = bIsConstructed;
	}
}

void UObsidianOverlayGameTabsMenu::OnInventoryButtonClicked()
{
	OnInventoryButtonClickedDelegate.Broadcast();
}

void UObsidianOverlayGameTabsMenu::OnInventoryTabStatusChange(bool bIsConstructed)
{
	if(Inventory_GameTabButton)
	{
		Inventory_GameTabButton->bIsCorrespondingTabOpen = bIsConstructed;
	}
}

void UObsidianOverlayGameTabsMenu::OnPassiveSkillTreeButtonClicked()
{
	OnPassiveSkillTreeButtonClickedDelegate.Broadcast();
}

void UObsidianOverlayGameTabsMenu::OnPassiveSkillTreeTabStatusChange(bool bIsConstructed)
{
	if(PassiveSkillTree_GameTabButton)
	{
		PassiveSkillTree_GameTabButton->bIsCorrespondingTabOpen = bIsConstructed;
	}
}
