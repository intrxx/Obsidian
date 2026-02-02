// Copyright 2026 out of sCope team - intrxx

#pragma once

#include <CoreMinimal.h>

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "UI/ObsidianWidgetBase.h"
#include "ObsidianItemLabel.generated.h"

class UImage;
class UCommonTextBlock;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemLabelMouseButtonDownSignature, const int32 PlayerIndex,
	const FObsidianItemInteractionFlags& InteractionFlags);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemLabelMouseHoverSignature, const bool bEnter);

/**
 * Widget class that represents the Item's name above the Item Actor in the world.
 */
UCLASS()
class OBSIDIAN_API UObsidianItemLabel : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetItemName(const FText& ItemName);

	void HandleItemLabelHighlightBegin() const;
	void HandleItemLabelHighlightEnd() const;
	
public:
	/** Delegate that fires when the Player presses the left mouse button onto the item desc. */
	FOnItemLabelMouseButtonDownSignature OnItemLabelMouseButtonDownDelegate;

	/** Delegate that fires when the Player hovers over the item Desc Widget, will fire with true if enters, false if leaves. */
	FOnItemLabelMouseHoverSignature OnItemLabelMouseHoverDelegate;
	
protected:
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> ItemName_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Background_Image;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	FSlateColor HoveredBackgroundColor;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	FSlateColor RegularBackgroundColor;
};
