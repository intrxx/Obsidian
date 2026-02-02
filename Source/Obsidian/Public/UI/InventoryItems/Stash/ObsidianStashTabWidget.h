// Copyright 2026 out of sCope team - intrxx

#pragma once

#include <CoreMinimal.h>

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "UI/ObsidianWidgetBase.h"
#include "ObsidianStashTabWidget.generated.h"

struct FObsidianItemWidgetData;
class UObsidianItem;
class UObInventoryItemsWidgetController;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianStashTabWidget : public UObsidianWidgetBase
{
	GENERATED_BODY()
	
public:
	virtual void AddItemToStash(UObsidianItem* InItemWidget, const FObsidianItemWidgetData& ItemWidgetData) {};
	virtual void HandleItemChanged(const FObsidianItemWidgetData& ItemWidgetData) {};
	virtual void HandleItemRemoved(const FObsidianItemWidgetData& ItemWidgetData) {};
	
	virtual void HandleHighlightingItems(const TArray<FObsidianItemPosition>& ItemsToHighlight) {};
	virtual void ClearUsableItemHighlight() {};
	
	FGameplayTag GetStashTabTag() const;
	
	void ShowStashTab();
	void HideStashTab();

	bool IsTabActive();
	
protected:
	virtual void NativeConstruct() override;

protected:
	bool bActive = false;

	UPROPERTY()
	FGameplayTag StashTabTag = FGameplayTag::EmptyTag;
};
