// Copyright 2026 out of sCope team - intrxx

#include "UI/InventoryItems/Items/ObsidianDraggedItem.h"

#include "CommonTextBlock.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"

#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Stacks.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"


void UObsidianDraggedItem::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetAlignmentInViewport(FVector2D(0.51f));
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UObsidianDraggedItem::InitializeItemWidgetWithItemDef(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef, const FObsidianItemGeneratedData& InGeneratedData)
{
	StackCount_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
	
	if(InItemDef == nullptr)
	{
		FFrame::KismetExecutionMessage(TEXT("Provided ItemDef is invalid in UObsidianDraggedItem::InitializeItemWidgetWithItemDef."), ELogVerbosity::Error);
		return;
	}
	
	InternalStacks = InGeneratedData.GetStackCount();
	
	UObsidianInventoryItemDefinition* DefaultObject = InItemDef.GetDefaultObject();
	if(DefaultObject == nullptr)
	{
		return;
	}
	
	const UOInventoryItemFragment_Appearance* AppearanceFragment = Cast<UOInventoryItemFragment_Appearance>(
			DefaultObject->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass()));
	if(AppearanceFragment)
	{
		const FIntPoint ItemGridSpan = AppearanceFragment->GetItemGridSpanFromDesc();
		Root_SizeBox->SetWidthOverride(ItemGridSpan.X * ObsidianInventoryItemsStatics::InventorySlotSize.X);
		Root_SizeBox->SetHeightOverride(ItemGridSpan.Y * ObsidianInventoryItemsStatics::InventorySlotSize.Y);

		SetDesiredSizeInViewport(ItemGridSpan * ObsidianInventoryItemsStatics::InventorySlotSize.X);
		
		UTexture2D* ItemImage = AppearanceFragment->GetItemImage();
		Item_Image->SetBrushFromTexture(ItemImage);
	}

	const UOInventoryItemFragment_Stacks* StacksFragment = Cast<UOInventoryItemFragment_Stacks>(
		DefaultObject->FindFragmentByClass(UOInventoryItemFragment_Stacks::StaticClass()));
	if(StacksFragment)
	{
		bStackableItem = StacksFragment->IsStackable();
		if(!bStackableItem || InternalStacks == 0)
		{
			StackCount_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			const FText StackCountText = FText::AsNumber(InternalStacks);
			StackCount_TextBlock->SetText(StackCountText);
			StackCount_TextBlock->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UObsidianDraggedItem::InitializeItemWidgetWithItemInstance(const UObsidianInventoryItemInstance* InItemInstance)
{
	StackCount_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
	
	if(InItemInstance == nullptr)
	{
		FFrame::KismetExecutionMessage(TEXT("Provided ItemInstance is invalid in UObsidianDraggedItem::InitializeItemWidgetWithItemInstance."), ELogVerbosity::Error);
		return;
	}
	
	const FIntPoint ItemGridSpan = InItemInstance->GetItemGridSpan();
	Root_SizeBox->SetWidthOverride(ItemGridSpan.X * ObsidianInventoryItemsStatics::InventorySlotSize.X);
	Root_SizeBox->SetHeightOverride(ItemGridSpan.Y * ObsidianInventoryItemsStatics::InventorySlotSize.Y);

	SetDesiredSizeInViewport(ItemGridSpan * ObsidianInventoryItemsStatics::InventorySlotSize.X);

	UTexture2D* ItemImage = InItemInstance->GetItemImage();
	Item_Image->SetBrushFromTexture(ItemImage);

	bStackableItem = InItemInstance->IsStackable();
	if(bStackableItem == false)
	{
		StackCount_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	const int32 CurrentStack = InItemInstance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	const FText StackCountText = FText::AsNumber(CurrentStack);
	StackCount_TextBlock->SetText(StackCountText);
	StackCount_TextBlock->SetVisibility(ESlateVisibility::Visible);
}

void UObsidianDraggedItem::UpdateStackCount(const int32 InNewStackCount)
{
	if(bStackableItem == false)
	{
		return;
	}

	if(InNewStackCount == InternalStacks)
	{
		return;
	}
	
	InternalStacks = InNewStackCount;
	const FText StackCountText = FText::AsNumber(InternalStacks);
	StackCount_TextBlock->SetText(StackCountText);
	StackCount_TextBlock->SetVisibility(ESlateVisibility::Visible);
}

