// Copyright 2026 out of sCope team - intrxx

#include "UI/InventoryItems/Items/ObsidianItemLabel.h"

#include <CommonTextBlock.h>
#include <Components/Border.h>

UObsidianItemLabel::UObsidianItemLabel()
{
	RegularBrushColor = FLinearColor(1.0f, 1.0f, 1.0f, 0.8f);
	HoveredBrushColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void UObsidianItemLabel::NativeConstruct()
{
	Super::NativeConstruct();

	if(Main_Border)
	{
		Main_Border->SetBrushColor(RegularBrushColor);
	}
}

FGuid UObsidianItemLabel::GetLabelID() const
{
	return LabelID;
}

bool UObsidianItemLabel::IsInUse() const
{
	return bLabelInUse;
}

void UObsidianItemLabel::MarkInUse(const bool bInUse, const FGuid& WithGuid)
{
#if !UE_BUILD_SHIPPING
	if (bInUse)
	{
		checkf(WithGuid.IsValid(), TEXT("Please provide a valid Guid when marking Label in use."));
	}
	else
	{
		checkf(WithGuid.IsValid() == false, TEXT("Please do not provide a valid Guid when marking Label"
										   " not longer in use"));
	}
#endif
	
	LabelID = WithGuid;
	bLabelInUse = bInUse;
}

void UObsidianItemLabel::SetItemName(const FText& ItemName)
{
	if(ItemName_TextBlock)
	{
		ItemName_TextBlock->SetText(ItemName);
	}
}

void UObsidianItemLabel::HandleItemLabelHighlightBegin() const
{
	if(Main_Border)
	{
		Main_Border->SetBrushColor(HoveredBrushColor);
	}
}

void UObsidianItemLabel::HandleItemLabelHighlightEnd() const
{
	if(Main_Border)
	{
		Main_Border->SetBrushColor(RegularBrushColor);
	}
}

FReply UObsidianItemLabel::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	//TODO(intrxx) Maybe some widget animation
	if(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		const int32 Index = InMouseEvent.GetUserIndex();
		FObsidianItemInteractionFlags InteractionFlags;
		InteractionFlags.bAutomaticallyAddToWindow = InMouseEvent.IsLeftControlDown();

		check(LabelID.IsValid())
		OnItemLabelMouseButtonDownDelegate.Broadcast(Index, InteractionFlags, LabelID);
	}
	return FReply::Handled();
}

void UObsidianItemLabel::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	
	HandleItemLabelHighlightBegin();
	if (ensure(LabelID.IsValid()))
	{
		OnItemLabelMouseHoverDelegate.Broadcast(true, LabelID);
	}
}

void UObsidianItemLabel::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	
	HandleItemLabelHighlightEnd();
	if (LabelID.IsValid())
	{
		OnItemLabelMouseHoverDelegate.Broadcast(false, LabelID);
	}
}
