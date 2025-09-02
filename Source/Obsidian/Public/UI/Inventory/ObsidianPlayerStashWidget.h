// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// ~ Project


#include "ObsidianTypes/ObsidianItemTypes.h"
#include "UI/ObsidianMainOverlayWidgetBase.h"
#include "ObsidianPlayerStashWidget.generated.h"

struct FObsidianItemWidgetData;
class UObsidianStashButton;
class UObsidianItem;
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
	
	FGameplayTag GetActiveStashTabTag() const;
	
protected:
	virtual void NativeConstruct() override;

	UObsidianStashTabWidget* GetActiveStashTab() const;
	
	void CreateStashTabButton(const FGameplayTag& StashTag, const FText& StashTabName);
	void ShowStashTab(const FGameplayTag& WithStashTag);

	void OnItemStashed(const FObsidianItemWidgetData& ItemWidgetData);

	void OnStashedItemLeftMouseButtonDown(const UObsidianItem* ItemWidget, const FObsidianItemInteractionFlags& InteractionFlags);
	void OnStashedItemMouseEntered(const UObsidianItem* ItemWidget);
	void OnItemMouseLeave();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	TSubclassOf<UObsidianItem> ItemWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TSubclassOf<UObsidianStashButton> StashButtonWidgetClass;

	UPROPERTY()
	TMap<FGameplayTag, UObsidianStashTabWidget*> StashTabsMap;

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
