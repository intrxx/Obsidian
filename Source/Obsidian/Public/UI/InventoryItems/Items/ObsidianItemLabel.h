// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"
#include "UI/ObsidianWidgetBase.h"

#include "ObsidianItemLabel.generated.h"

class UBorder;
class UCommonTextBlock;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnItemLabelMouseButtonDownSignature, const int32 PlayerIndex,
	const FObsidianItemInteractionFlags& InteractionFlags, const FGuid& LabelID);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemLabelMouseHoverSignature, const bool bEnter, const FGuid& LabelID);

/**
 * Widget class that represents the Item's name above the Item Actor in the world.
 */
UCLASS(meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API UObsidianItemLabel : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	UObsidianItemLabel();

	FGuid GetLabelID() const { return LabelID; }
	bool IsInUse() const { return bLabelInUse; }
	void MarkInUse(const bool bInUse, const FGuid& InWithGuid = FGuid());

	void SetItemName(const FText& InItemName);

	void HandleItemLabelHighlightBegin() const;
	void HandleItemLabelHighlightEnd() const;

	/** Delegate that fires when the Player presses the left mouse button onto the item desc. */
	FOnItemLabelMouseButtonDownSignature OnItemLabelMouseButtonDownDelegate;

	/** Delegate that fires when the Player hovers over the item Desc Widget, will fire with true if enters, false if leaves. */
	FOnItemLabelMouseHoverSignature OnItemLabelMouseHoverDelegate;

protected:
	//~ Begin UUserWidget Interface
	virtual void NativeConstruct() override;
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	//~ End UUserWidget Interface

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> ItemName_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UBorder> Main_Border;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	FLinearColor HoveredBrushColor;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	FLinearColor RegularBrushColor;

	bool bLabelInUse = false;
	FGuid LabelID = FGuid();
};
