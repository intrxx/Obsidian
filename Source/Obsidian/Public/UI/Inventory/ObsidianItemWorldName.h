// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetBase.h"
#include "ObsidianItemWorldName.generated.h"

class UImage;
class UCommonTextBlock;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemDescMouseButtonDownSignature, const bool bLeftControlDown);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemDescMouseHoverSignature, const bool bEnter);

/**
 * Widget class that represents the Item's name above the Item Actor in the world.
 */
UCLASS()
class OBSIDIAN_API UObsidianItemWorldName : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetItemName(const FText& ItemName);
	
public:
	/** Delegate that fires when the Player presses the left mouse button onto the item desc. */
	FOnItemDescMouseButtonDownSignature OnItemDescMouseButtonDownDelegate;

	/** Delegate that fires when the Player hovers over the item Desc Widget, will fire with true if enters, false if leaves. */
	FOnItemDescMouseHoverSignature OnItemDescMouseHoverDelegate;
	
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
