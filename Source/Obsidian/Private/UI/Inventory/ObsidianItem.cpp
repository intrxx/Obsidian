// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Inventory/ObsidianItem.h"
#include "CommonTextBlock.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "ObsidianTypes/ObsidianItemTypes.h"

FReply UObsidianItem::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		const bool bShiftPressed = InMouseEvent.IsShiftDown();
		OnItemLeftMouseButtonPressedDelegate.Broadcast(this, bShiftPressed);
	}
	if(InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		OnItemRightMouseButtonPressedDelegate.Broadcast(this);
	}

	return FReply::Handled();
}

void UObsidianItem::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnItemMouseEnterDelegate.Broadcast(this);
}

void UObsidianItem::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	OnItemMouseLeaveDelegate.Broadcast();
}

void UObsidianItem::InitializeItemWidget(const FVector2D& DesiredPosition, const FVector2D& ItemGridSpan, UTexture2D* ItemImage, const int32 CurrentStack)
{
	Root_SizeBox->SetWidthOverride(ItemGridSpan.X * ObsidianInventoryItemsStatics::InventorySlotSize.X);
	Root_SizeBox->SetHeightOverride(ItemGridSpan.Y * ObsidianInventoryItemsStatics::InventorySlotSize.Y);
	Item_Image->SetBrushFromTexture(ItemImage);

	ItemDesiredGridSpan = ItemGridSpan;
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

void UObsidianItem::InitializeItemWidget(const FGameplayTag& EquipmentSlot, const FVector2D& ItemGridSpan, UTexture2D* ItemImage)
{
	Root_SizeBox->SetWidthOverride(ItemGridSpan.X * ObsidianInventoryItemsStatics::InventorySlotSize.X);
	Root_SizeBox->SetHeightOverride(ItemGridSpan.Y * ObsidianInventoryItemsStatics::InventorySlotSize.Y);
	Item_Image->SetBrushFromTexture(ItemImage);
	ItemEquipmentSlot = EquipmentSlot;
	
	StackCount_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
}

void UObsidianItem::AddCurrentStackCount(const int32 StackCountToAdd)
{
	if(StackCountToAdd <= 0)
	{
		return;
	}
	
	InternalStacks += StackCountToAdd;
	if(InternalStacks <= 0)
	{
		StackCount_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	const FText StackCountText = FText::FromString(FString::Printf(TEXT("%d"), InternalStacks));
	StackCount_TextBlock->SetText(StackCountText);
	StackCount_TextBlock->SetVisibility(ESlateVisibility::Visible);
}

void UObsidianItem::OverrideCurrentStackCount(const int32 NewStackCount)
{
	if(NewStackCount <= 0)
	{
		return;
	}
	InternalStacks = NewStackCount;
	
	const FText StackCountText = FText::FromString(FString::Printf(TEXT("%d"), NewStackCount));
	StackCount_TextBlock->SetText(StackCountText);
	StackCount_TextBlock->SetVisibility(ESlateVisibility::Visible);
}

FSlateBrush UObsidianItem::GetItemImage() const
{
	if(Item_Image)
	{
		return Item_Image->GetBrush();
	}
	return FSlateBrush();
}

FVector2D UObsidianItem::GetItemSize() const
{
	return FVector2D(Root_SizeBox->GetWidthOverride(), Root_SizeBox->GetHeightOverride());
}

void UObsidianItem::SetSize(const FVector2D& ItemGridSpan)
{
	Root_SizeBox->SetWidthOverride(ItemGridSpan.X * ObsidianInventoryItemsStatics::InventorySlotSize.X);
	Root_SizeBox->SetHeightOverride(ItemGridSpan.Y * ObsidianInventoryItemsStatics::InventorySlotSize.Y);
}

void UObsidianItem::SetUsingItemProperties()
{
	SetRenderOpacity(UsingItemOpacity);
}

void UObsidianItem::ResetUsingItemProperties()
{
	SetRenderOpacity(1.0f);
}
