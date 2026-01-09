// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>


#include "UI/ObsidianWidgetBase.h"
#include "ObsidianItem.generated.h"

class UCommonTextBlock;
class USizeBox;
class UObsidianItem;
class UImage;

/**
 * Item Widget that is displayed in the inventory.
 */
UCLASS()
class OBSIDIAN_API UObsidianItem : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	void InitializeItemWidget(const FIntPoint& InItemGridSpan, UTexture2D* ItemImage, const int32 CurrentStack = 0);
	void InitializeItemWidget(const FIntPoint& InItemGridSpan, UTexture2D* ItemImage, const bool bIsForSwapSlot = false);
	
	void AddCurrentStackCount(const int32 StackCountToAdd);
	void OverrideCurrentStackCount(const int32 NewStackCount);
	
	FSlateBrush GetItemImage() const;
	FVector2D GetItemWidgetSize() const;

	void SetBlockadeItemProperties();
	void ResetBlockadeItemProperties();
	
	void SetUsingItemProperties();
	void ResetUsingItemProperties();

	void HighlightItem();
	void ResetHighlight();
	
protected:
	virtual void NativeConstruct() override;
	
protected:
	/** Opacity to set when item is added a blocking slot item. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	float BlockingItemOpacity = 0.5f;
	
	/** Opacity to set when item is being used (right-clicked). */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	float UsingItemOpacity = 0.6f;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	float SwapSlotSizeMultiplier = 0.5f;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Highlight_Image;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Item_Image;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> StackCount_TextBlock;

private:
	int32 InternalStacks = 0;
};
