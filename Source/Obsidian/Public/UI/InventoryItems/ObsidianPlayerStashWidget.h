// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"
#include "UI/ObsidianMainOverlayWidgetBase.h"

#include "ObsidianPlayerStashWidget.generated.h"

struct FObsidianItemWidgetData;

class UObsidianSlot_ItemSlot;
class UObsidianStashButton;
class UObsidianItem;
class UOverlay;
class UScrollBox;
class UObsidianStashTabWidget;
class UObsidianGridPanel;
class UObInventoryItemsWidgetController;

/**
 * 
 */
UCLASS(meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API UObsidianPlayerStashWidget : public UObsidianMainOverlayWidgetBase
{
	GENERATED_BODY()

public:
	FGameplayTag GetActiveStashTabTag() const;

	void CloseStash();

protected:
	UObsidianStashTabWidget* GetActiveStashTab() const { return ActiveStashTab; }

	void CreateStashTabButton(const FGameplayTag& InStashTag, const FText& InStashTabName);
	void ShowStashTab(const FGameplayTag& InWithStashTag);

	void OnItemStashed(const FObsidianItemWidgetData& InItemWidgetData);
	void OnItemChanged(const FObsidianItemWidgetData& InItemWidgetData);
	void OnItemRemoved(const FObsidianItemWidgetData& InItemWidgetData);

	void HighlightSlotPlacement(const FGameplayTagContainer& InWithTags);
	void StopHighlightSlotPlacement();

	void SavePlayerStash();

	void OnUsableContextFiredForStash(const TMultiMap<FGameplayTag, FObsidianItemPosition>& InMatchingItemPositions);
	void ClearUsableItemHighlight();

	//~ Begin UObsidianWidgetBase Interface
	virtual void HandleWidgetControllerSet() override;
	//~ End UObsidianWidgetBase Interface

	//~ Begin UUserWidget Interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	//~ End UUserWidget Interface

	//~ Begin UObsidianMainOverlayWidgetBase Interface
	virtual void PreCloseButtonPressed() override;
	//~ End UObsidianMainOverlayWidgetBase Interface

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	TSubclassOf<UObsidianItem> ItemWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TSubclassOf<UObsidianStashButton> StashButtonWidgetClass;

	UPROPERTY()
	TMap<FGameplayTag, UObsidianStashTabWidget*> StashTabsMap;

	UPROPERTY()
	TObjectPtr<UObsidianStashTabWidget> ActiveStashTab;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> StashTabsList_ScrollBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> StashTab_Overlay;

	bool bStashChanged = false;

private:
	void CreateStashTabs();

	UPROPERTY()
	TObjectPtr<UObInventoryItemsWidgetController> InventoryItemsWidgetController;

	UPROPERTY()
	TArray<UObsidianSlot_ItemSlot*> CachedHighlightedSlot;

	TMultiMap<FGameplayTag, FObsidianItemPosition> CachedItemsToHighlight;
};
