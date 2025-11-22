// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
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
	virtual void AddItemToStash(UObsidianItem* InItemWidget, const float ItemSlotPadding = 0.0f) {};
	
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
