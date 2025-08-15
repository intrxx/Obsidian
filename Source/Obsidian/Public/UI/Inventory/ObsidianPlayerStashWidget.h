// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// ~ Project


#include "UI/ObsidianMainOverlayWidgetBase.h"
#include "ObsidianPlayerStashWidget.generated.h"

class UObsidianStashButton;
class UOverlay;
class UScrollBox;
class UObsidianStashTabWidget;
class UObsidianGrid;
class UObsidianInventoryItemsWidgetController;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianPlayerStashWidget : public UObsidianMainOverlayWidgetBase
{
	GENERATED_BODY()

public:
	virtual void HandleWidgetControllerSet() override;
	
protected:
	virtual void NativeConstruct() override;

	void CreateStashTabButton(const FGameplayTag& StashTag, const FText& StashTabName);
	void ShowStashTab(const FGameplayTag& WithStashTag);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TSubclassOf<UObsidianStashButton> StashButtonWidgetClass;

	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UObsidianStashTabWidget>> StashTabsMap;

	UPROPERTY()
	TObjectPtr<UObsidianStashTabWidget> ActiveStashTab;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> StashTabsList_ScrollBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> StashTab_Overlay;

private:
	void CreateStashTabs();
	
private:
	UPROPERTY()
	TObjectPtr<UObsidianInventoryItemsWidgetController> InventoryItemsWidgetController;
};
