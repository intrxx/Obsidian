// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Inventory/ObsidianItemDescriptionBase.h"

#include "CommonTextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "ObsidianTypes/ObsidianItemTypes.h"

void UObsidianItemDescriptionBase::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UObsidianItemDescriptionBase::InitializeWidgetWithItemStats(const FObsidianItemStats& ItemStats, const bool bDisplayItemImage)
{
	SetItemDisplayName(ItemStats.GetDisplayName());

	if(bDisplayItemImage && ItemStats.ContainsItemImage())
	{
		if(UTexture2D* ItemTexture = ItemStats.GetItemImage())
		{
			Item_Image->SetBrushFromTexture(ItemTexture);
		}
	}
	else
	{
		Item_Image->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	if(ItemStats.ContainsStacks())
	{
		const FObsidianStacksUIData StacksData = ItemStats.GetItemStacks();
		SetStackCount(StacksData.CurrentItemStackCount, StacksData.MaxItemStackCount);
	}
	if(ItemStats.ContainsDescription())
	{
		SetItemDescription(ItemStats.GetDescription());
	}
	if(ItemStats.ContainsAdditionalDescription())
	{
		SetAdditionalItemDescription(ItemStats.GetAdditionalDescription());
	}
}

void UObsidianItemDescriptionBase::SetItemDisplayName(const FText& DisplayName)
{
	if(!DisplayName.IsEmpty())
	{
		ItemName_TextBlock->SetText(DisplayName);
	}
}

void UObsidianItemDescriptionBase::SetStackCount(const int32 CurrentStacks, const int32 MaxStacks)
{
	CurrentStackCount = CurrentStacks;
	MaxStackCount = MaxStacks;
	
	StacksContainer_HorizontalBox->SetVisibility(ESlateVisibility::Visible);
	const FText StackCountText = FText::FromString(FString::Printf(TEXT("%d/%d"), CurrentStackCount, MaxStackCount));
	StackCount_TextBlock->SetText(StackCountText);
}

void UObsidianItemDescriptionBase::UpdateCurrentStackCount(const int32 CurrentStacks)
{
	CurrentStackCount = CurrentStacks;
	
	StacksContainer_HorizontalBox->SetVisibility(ESlateVisibility::Visible);
	const FText StackCountText = FText::FromString(FString::Printf(TEXT("%d/%d"), CurrentStackCount, MaxStackCount));
	StackCount_TextBlock->SetText(StackCountText);
}

void UObsidianItemDescriptionBase::SetItemDescription(const FText& ItemDescription)
{
	if(!ItemDescription.IsEmpty())
	{
		ItemDescription_TextBlock->SetVisibility(ESlateVisibility::Visible);
		ItemDescription_TextBlock->SetText(ItemDescription);
	}
}

void UObsidianItemDescriptionBase::SetAdditionalItemDescription(const FText& AdditionalItemDescription)
{
	if(!AdditionalItemDescription.IsEmpty())
	{
		AdditionalItemDescription_TextBlock->SetVisibility(ESlateVisibility::Visible);
		AdditionalItemDescription_TextBlock->SetText(AdditionalItemDescription);
	}
}

void UObsidianItemDescriptionBase::CollapseStatBlocks()
{
	StacksContainer_HorizontalBox->SetVisibility(ESlateVisibility::Collapsed);
	ItemDescription_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
	AdditionalItemDescription_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
}

void UObsidianItemDescriptionBase::DestroyDescriptionWidget()
{
	RemoveFromParent();
}


