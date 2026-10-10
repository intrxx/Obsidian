// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "ObsidianWidgetControllerBase.h"

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
class UObsidianItemLabelOverlay;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API AObsidianHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	UObMainOverlayWidgetController* GetMainOverlayWidgetController(const FObsidianWidgetControllerParams& InWidgetControllerParams);
	UObCharacterStatusWidgetController* GetCharacterStatusWidgetController(const FObsidianWidgetControllerParams& InWidgetControllerParams);
	UObInventoryItemsWidgetController* GetInventoryItemsWidgetController(const FObsidianWidgetControllerParams& InWidgetControllerParams);

	UObsidianMainOverlay* GetMainOverlay()
	{
		return MainOverlayWidget;
	}

	void InitOverlay(AObsidianPlayerController* InForPlayerController, AObsidianPlayerState* InForPlayerState);

	void ToggleCharacterStatus() const;
	void ToggleInventory() const;
	void TogglePassiveSkillTree() const;
	
	void TogglePlayerStash(const bool bInShowStash) const;

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
