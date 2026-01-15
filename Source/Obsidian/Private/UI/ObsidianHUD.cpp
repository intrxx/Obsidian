// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/ObsidianHUD.h"

// ~ Core

// ~ Project
#include "Characters/Heroes/ObsidianHero.h"
#include "UI/MainOverlay/ObsidianMainOverlay.h"
#include "UI/ProgressBars/ObsidianHeroHealthBar.h"
#include "UI/WidgetControllers/ObCharacterStatusWidgetController.h"
#include "UI/WidgetControllers/ObInventoryItemsWidgetController.h"
#include "UI/WidgetControllers/ObMainOverlayWidgetController.h"

UObMainOverlayWidgetController* AObsidianHUD::GetMainOverlayWidgetController(const FObsidianWidgetControllerParams& WidgetControllerParams)
{
	// If the overlay controller is a nullptr we need to construct one
	if(MainOverlayWidgetController == nullptr)
	{
		if(ensureMsgf(MainOverlayWidgetControllerClass, TEXT("Main Overlay Widget Controller Class is not set on HUD Class [%s], please fill it out in BP_ObsidianHUD"), *GetNameSafe(this)))
		{
			MainOverlayWidgetController = NewObject<UObMainOverlayWidgetController>(this, MainOverlayWidgetControllerClass);
			MainOverlayWidgetController->SetWidgetControllerParams(WidgetControllerParams);
			
			return MainOverlayWidgetController;
		}
	}
	return MainOverlayWidgetController;
}

UObCharacterStatusWidgetController* AObsidianHUD::GetCharacterStatusWidgetController(const FObsidianWidgetControllerParams& WidgetControllerParams)
{
	// If the character status controller is a nullptr we need to construct one
	if(CharacterStatusWidgetController == nullptr)
	{
		if(ensureMsgf(CharacterStatusWidgetControllerClass, TEXT("Character Status Widget Controller Class is not set on HUD Class [%s], please fill it out in BP_ObsidianHUD"), *GetNameSafe(this)))
		{
			CharacterStatusWidgetController = NewObject<UObCharacterStatusWidgetController>(this, CharacterStatusWidgetControllerClass);
			CharacterStatusWidgetController->SetWidgetControllerParams(WidgetControllerParams);
			CharacterStatusWidgetController->OnWidgetControllerSetupCompleted();

			return CharacterStatusWidgetController;
		}
	}
	return CharacterStatusWidgetController;
}

UObInventoryItemsWidgetController* AObsidianHUD::GetInventoryItemsWidgetController(const FObsidianWidgetControllerParams& WidgetControllerParams)
{
	// If the character status controller is a nullptr we need to construct one
	if(InventoryItemsWidgetController == nullptr)
	{
		if(ensureMsgf(InventoryItemsWidgetControllerClass, TEXT("Inventory Controller Class is not set on HUD Class [%s], please fill it out in BP_ObsidianHUD"), *GetNameSafe(this)))
		{
			InventoryItemsWidgetController = NewObject<UObInventoryItemsWidgetController>(this, InventoryItemsWidgetControllerClass);
			InventoryItemsWidgetController->SetWidgetControllerParams(WidgetControllerParams);
			InventoryItemsWidgetController->OnWidgetControllerSetupCompleted();
			
			return InventoryItemsWidgetController;
		}
	}
	return InventoryItemsWidgetController;
}

void AObsidianHUD::InitOverlay(AObsidianPlayerController* ForPlayerController, AObsidianPlayerState* ForPlayerState)
{
	if(ensureMsgf(MainOverlayWidgetClass, TEXT("Main Overlay Widget Class is not set on HUD Class [%s], please fill it out in BP_ObsidianHUD"), *GetNameSafe(this)))
	{
		MainOverlayWidget = CreateWidget<UObsidianMainOverlay>(GetWorld(), MainOverlayWidgetClass);

		FObsidianWidgetControllerParams Params;
		Params.ObsidianPlayerController = ForPlayerController;
		Params.ObsidianPlayerState = ForPlayerState;
		UObMainOverlayWidgetController* WidgetController = GetMainOverlayWidgetController(Params);

		MainOverlayWidget->SetWidgetController(WidgetController);
		WidgetController->OnWidgetControllerSetupCompleted();

		if(const AObsidianHero* Hero = Cast<AObsidianHero>(GetOwningPawn()))
		{
			if(Hero->IsLocallyControlled())
			{
				if(UObsidianWidgetBase* Widget = Hero->GetHealthBarWidget())
				{
					Widget->SetWidgetController(WidgetController);
				}
			}
		}
		
		MainOverlayWidget->AddToViewport();
	}
}

void AObsidianHUD::ToggleCharacterStatus() const
{
	if(MainOverlayWidget)
	{
		MainOverlayWidget->ToggleCharacterStatus();
	}
}

void AObsidianHUD::ToggleInventory() const
{
	if(MainOverlayWidget)
	{
		MainOverlayWidget->ToggleInventory();
	}
}

void AObsidianHUD::TogglePassiveSkillTree() const
{
	if(MainOverlayWidget)
	{
		MainOverlayWidget->TogglePassiveSkillTree();
	}
}

void AObsidianHUD::TogglePlayerStash(const bool bShowStash) const
{
	if(MainOverlayWidget)
	{
		MainOverlayWidget->TogglePlayerStash(bShowStash);
	}
}

bool AObsidianHUD::IsInventoryOpened() const
{
	if(MainOverlayWidget)
	{
		return MainOverlayWidget->IsInventoryOpen();
	}
	return false;
}

bool AObsidianHUD::IsPlayerStashOpened() const
{
	if(MainOverlayWidget)
	{
		return MainOverlayWidget->IsPlayerStashOpen();
	}
	return false;
}

FGameplayTag AObsidianHUD::GetActiveStashTabTag() const
{
	if(MainOverlayWidget)
	{
		return MainOverlayWidget->GetActivePlayerStashTabTag();
	}
	return FGameplayTag::EmptyTag;
}


