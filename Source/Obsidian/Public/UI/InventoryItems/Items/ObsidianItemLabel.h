// Copyright 2026 out of sCope team - intrxx

#pragma once

#include <CoreMinimal.h>

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
UCLASS()
class OBSIDIAN_API UObsidianItemLabel : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	UObsidianItemLabel();
	
	virtual void NativeConstruct() override;

	FGuid GetLabelID() const;
	bool IsInUse() const;
	void MarkInUse(const bool bInUse, const FGuid& WithGuid = FGuid());
	
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
	TObjectPtr<UBorder> Main_Border;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	FLinearColor HoveredBrushColor;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	FLinearColor RegularBrushColor;

	bool bLabelInUse = false;
	FGuid LabelID = FGuid();
};
