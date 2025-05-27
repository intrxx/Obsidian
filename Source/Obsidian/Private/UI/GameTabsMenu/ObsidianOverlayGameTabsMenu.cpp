// Copyright 2024 out of sCope team - Michał Ogiński

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
	// @HACK displaying mouse button down event fixes a bug when we click on the Tabs Menu, move the mouse over and have our mouse movement blocked.
	if(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UObsidianOverlayGameTabsMenu::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnMouseEnterLeaveDelegate.Broadcast(true);
}

void UObsidianOverlayGameTabsMenu::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	OnMouseEnterLeaveDelegate.Broadcast(false);
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
