// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Inventory/ObsidianItem.h"
#include "CommonTextBlock.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"

FReply UObsidianItem::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) - I don't know what is the difference, leaving it here for now
	if(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		OnItemLeftMouseButtonPressedDelegate.Broadcast(ItemDesiredPosition, this);
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UObsidianItem::InitializeItemWidget(const FVector2D& DesiredPosition, const FVector2D& ItemGridSpan, UTexture2D* ItemImage, const int32 CurrentStack)
{
	Root_SizeBox->SetWidthOverride(ItemGridSpan.X * WidthConstant);
	Root_SizeBox->SetHeightOverride(ItemGridSpan.Y * HeightConstant);
	Item_Image->SetBrushFromTexture(ItemImage);
	ItemDesiredPosition = DesiredPosition;
	InternalStacks = CurrentStack;
	
	if(CurrentStack == 0)
	{
		StackCount_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	const FText StackCountText = FText::FromString(FString::Printf(TEXT("%d"), CurrentStack));
	StackCount_TextBlock->SetText(StackCountText);
	StackCount_TextBlock->SetVisibility(ESlateVisibility::Visible);
}

void UObsidianItem::AddCurrentStackCount(const int32 StackCountToAdd)
{
	if(StackCountToAdd <= 0)
	{
		return;
	}
	
	const int32 NewStack = InternalStacks + StackCountToAdd;
	if(NewStack <= 0)
	{
		StackCount_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	const FText StackCountText = FText::FromString(FString::Printf(TEXT("%d"), NewStack));
	StackCount_TextBlock->SetText(StackCountText);
	StackCount_TextBlock->SetVisibility(ESlateVisibility::Visible);
}
