// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Inventory/ObsidianDraggedItem.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "Components/Image.h"
#include "CommonTextBlock.h"
#include "Components/SizeBox.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Stacks.h"
#include "Obsidian/ObsidianGameplayTags.h"

void UObsidianDraggedItem::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetAlignmentInViewport(FVector2D(0.51f));
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UObsidianDraggedItem::InitializeItemWidgetWithItemDef(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const int32 Stacks)
{
	StackCount_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
	
	if(ItemDef == nullptr)
	{
		FFrame::KismetExecutionMessage(TEXT("Provided ItemDef is invalid in UObsidianDraggedItem::InitializeItemWidgetWithItemDef."), ELogVerbosity::Error);
		return;
	}

	InternalItemDef = ItemDef;
	InternalStacks = Stacks;
	
	UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject();
	if(DefaultObject == nullptr)
	{
		return;
	}
	
	const UOInventoryItemFragment_Appearance* AppearanceFragment = Cast<UOInventoryItemFragment_Appearance>(
			DefaultObject->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass()));
	if(AppearanceFragment)
	{
		const FVector2D ItemGridSpan = AppearanceFragment->GetItemGridSpanFromDesc();
		Root_SizeBox->SetWidthOverride(ItemGridSpan.X * WidthConstant);
		Root_SizeBox->SetHeightOverride(ItemGridSpan.Y * HeightConstant);

		SetDesiredSizeInViewport(ItemGridSpan * WidthConstant);
		
		UTexture2D* ItemImage = AppearanceFragment->GetItemImage();
		Item_Image->SetBrushFromTexture(ItemImage);
	}

	const UOInventoryItemFragment_Stacks* StacksFragment = Cast<UOInventoryItemFragment_Stacks>(
		DefaultObject->FindFragmentByClass(UOInventoryItemFragment_Stacks::StaticClass()));
	if(StacksFragment)
	{
		if(!StacksFragment->IsStackable() || InternalStacks == 0)
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

void UObsidianDraggedItem::InitializeItemWidgetWithItemInstance(UObsidianInventoryItemInstance* ItemInstance)
{
	StackCount_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
	
	if(ItemInstance == nullptr)
	{
		FFrame::KismetExecutionMessage(TEXT("Provided ItemInstance is invalid in UObsidianDraggedItem::InitializeItemWidgetWithItemInstance."), ELogVerbosity::Error);
		return;
	}

	InternalItemInstance = ItemInstance;

	const FVector2D ItemGridSpan = InternalItemInstance->GetItemGridSpan();
	Root_SizeBox->SetWidthOverride(ItemGridSpan.X * WidthConstant);
	Root_SizeBox->SetHeightOverride(ItemGridSpan.Y * HeightConstant);

	SetDesiredSizeInViewport(ItemGridSpan * WidthConstant);

	UTexture2D* ItemImage = InternalItemInstance->GetItemImage();
	Item_Image->SetBrushFromTexture(ItemImage);
	
	if(ItemInstance->IsStackable() == false)
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
	InternalStacks = NewStackCount;
	
	const FText StackCountText = FText::FromString(FString::Printf(TEXT("%d"), InternalStacks));
	StackCount_TextBlock->SetText(StackCountText);
	StackCount_TextBlock->SetVisibility(ESlateVisibility::Visible);
}

void UObsidianDraggedItem::SyncStackCountWithInstance() const
{
	if(InternalItemInstance->IsStackable() == false)
	{
		return;
	}
	
	const int32 StackCount = InternalItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	const FText StackCountText = FText::FromString(FString::Printf(TEXT("%d"), StackCount));
	StackCount_TextBlock->SetText(StackCountText);
	StackCount_TextBlock->SetVisibility(ESlateVisibility::Visible);
}
