// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/Items/ObsidianDraggedItem.h"

// ~ Core
#include "Components/Image.h"
#include "CommonTextBlock.h"
#include "Components/SizeBox.h"

// ~ Project
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Stacks.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

void UObsidianDraggedItem::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetAlignmentInViewport(FVector2D(0.51f));
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UObsidianDraggedItem::InitializeItemWidgetWithItemDef(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FObsidianItemGeneratedData& GeneratedData)
{
	StackCount_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
	
	if(ItemDef == nullptr)
	{
		FFrame::KismetExecutionMessage(TEXT("Provided ItemDef is invalid in UObsidianDraggedItem::InitializeItemWidgetWithItemDef."), ELogVerbosity::Error);
		return;
	}
	
	InternalStacks = GeneratedData.AvailableStackCount;
	
	UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject();
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
			const FText StackCountText = FText::FromString(FString::Printf(TEXT("%d"), InternalStacks));
			StackCount_TextBlock->SetText(StackCountText);
			StackCount_TextBlock->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UObsidianDraggedItem::InitializeItemWidgetWithItemInstance(const UObsidianInventoryItemInstance* ItemInstance)
{
	StackCount_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
	
	if(ItemInstance == nullptr)
	{
		FFrame::KismetExecutionMessage(TEXT("Provided ItemInstance is invalid in UObsidianDraggedItem::InitializeItemWidgetWithItemInstance."), ELogVerbosity::Error);
		return;
	}
	
	const FIntPoint ItemGridSpan = ItemInstance->GetItemGridSpan();
	Root_SizeBox->SetWidthOverride(ItemGridSpan.X * ObsidianInventoryItemsStatics::InventorySlotSize.X);
	Root_SizeBox->SetHeightOverride(ItemGridSpan.Y * ObsidianInventoryItemsStatics::InventorySlotSize.Y);

	SetDesiredSizeInViewport(ItemGridSpan * ObsidianInventoryItemsStatics::InventorySlotSize.X);

	UTexture2D* ItemImage = ItemInstance->GetItemImage();
	Item_Image->SetBrushFromTexture(ItemImage);

	bStackableItem = ItemInstance->IsStackable();
	if(bStackableItem == false)
	{
		StackCount_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	const int32 CurrentStack = ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	const FText StackCountText = FText::FromString(FString::Printf(TEXT("%d"), CurrentStack));
	StackCount_TextBlock->SetText(StackCountText);
	StackCount_TextBlock->SetVisibility(ESlateVisibility::Visible);
}

void UObsidianDraggedItem::UpdateStackCount(const int32 NewStackCount)
{
	if(bStackableItem == false)
	{
		return;
	}

	if(NewStackCount == InternalStacks)
	{
		return;
	}
	
	InternalStacks = NewStackCount;
	const FText StackCountText = FText::FromString(FString::Printf(TEXT("%d"), InternalStacks));
	StackCount_TextBlock->SetText(StackCountText);
	StackCount_TextBlock->SetVisibility(ESlateVisibility::Visible);
}

