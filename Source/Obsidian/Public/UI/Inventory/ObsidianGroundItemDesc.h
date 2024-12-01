// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetBase.h"
#include "ObsidianGroundItemDesc.generated.h"

class UImage;
DECLARE_MULTICAST_DELEGATE(FOnItemDescMouseButtonDownSignature);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemDescMouseHover, const bool bEnter);

class UCommonTextBlock;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianGroundItemDesc : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
public:
	/** Delegate that fires when the Player presses the left mouse button onto the item desc. */
	FOnItemDescMouseButtonDownSignature OnItemDescMouseButtonDownDelegate;

	/** Delegate that fires when the Player hovers over the item Desc Widget, will fire with true if enters, false if leaves. */
	FOnItemDescMouseHover OnItemDescMouseHoverDelegate;
	
protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
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
