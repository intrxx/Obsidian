// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "ObsidianWidgetControllerBase.h"

#include "GameFramework/HUD.h"
#include "ObsidianHUD.generated.h"

struct FObsidianHeroWidgetControllerParams;

class UObInventoryItemsWidgetController;
class UObCharacterStatusWidgetController;
class UObsidianHeroAttributesComponent;
class UObMainOverlayWidgetController;
class UObsidianMainOverlay;
class UObsidianWidgetBase;
class AObsidianPlayerController;
class AObsidianPlayerState;
class UObsidianAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API AObsidianHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	UObMainOverlayWidgetController* GetMainOverlayWidgetController(const FObsidianWidgetControllerParams& WidgetControllerParams);
	UObCharacterStatusWidgetController* GetCharacterStatusWidgetController(const FObsidianWidgetControllerParams& WidgetControllerParams);
	UObInventoryItemsWidgetController* GetInventoryItemsWidgetController(const FObsidianWidgetControllerParams& WidgetControllerParams);

	UObsidianMainOverlay* GetMainOverlay()
	{
		return MainOverlayWidget;
	}

	void InitOverlay(AObsidianPlayerController* PC, AObsidianPlayerState* PS, UObsidianAbilitySystemComponent* ASC, UObsidianHeroAttributesComponent* AC);

	void ToggleCharacterStatus() const;
	void ToggleInventory() const;
	void TogglePassiveSkillTree() const;
	
	void TogglePlayerStash(const bool bShowStash) const;

	bool IsInventoryOpened() const;
	bool IsPlayerStashOpened() const;
	FGameplayTag GetActiveStashTabTag() const;
	
public:
	UPROPERTY()
	TObjectPtr<UObsidianMainOverlay> MainOverlayWidget;
	
private:
	UPROPERTY(EditAnywhere, Category = "ObsidianUI|MainOverlay")
	TSubclassOf<UObsidianMainOverlay> MainOverlayWidgetClass;

	/**
	 * Widget Controllers 
	 */
	
	UPROPERTY()
	TObjectPtr<UObMainOverlayWidgetController> MainOverlayWidgetController;

	UPROPERTY(EditAnywhere, Category = "ObsidianUI|MainOverlay")
	TSubclassOf<UObMainOverlayWidgetController> MainOverlayWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UObCharacterStatusWidgetController> CharacterStatusWidgetController;

	UPROPERTY(EditAnywhere, Category = "ObsidianUI|CharacterStatus")
	TSubclassOf<UObCharacterStatusWidgetController> CharacterStatusWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UObInventoryItemsWidgetController> InventoryItemsWidgetController;
	
	UPROPERTY(EditAnywhere, Category = "ObsidianUI|Inventory")
	TSubclassOf<UObInventoryItemsWidgetController> InventoryItemsWidgetControllerClass;
};
