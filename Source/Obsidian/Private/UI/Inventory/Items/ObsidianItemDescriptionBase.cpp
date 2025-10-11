// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/Items/ObsidianItemDescriptionBase.h"

// ~ Core
#include "CommonTextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/Overlay.h"

// ~ Project
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

void UObsidianItemDescriptionBase::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::HitTestInvisible);
	// Unidentified_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
	//
	// Implicit_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
	// Prefix1_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
	// Prefix2_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
	// Prefix3_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
	// Suffix1_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
	// Suffix2_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
	// Suffix3_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
}

UCommonTextBlock* UObsidianItemDescriptionBase::GetFreePrefixBlock()
{
	if(!Prefix1_TextBlock->IsVisible() && Prefix1_TextBlock->GetText().IsEmpty())
	{
		return Prefix1_TextBlock;
	}
	if(!Prefix2_TextBlock->IsVisible() && Prefix2_TextBlock->GetText().IsEmpty())
	{
		return Prefix2_TextBlock;
	}
	if(!Prefix3_TextBlock->IsVisible() && Prefix3_TextBlock->GetText().IsEmpty())
	{
		return Prefix3_TextBlock;
	}
	ensureMsgf(false, TEXT("Not a single prefix box is free, this should never happen!"));
	return nullptr;
}

UCommonTextBlock* UObsidianItemDescriptionBase::GetFreeSuffixBlock()
{
	if(!Suffix1_TextBlock->IsVisible() && Suffix1_TextBlock->GetText().IsEmpty())
	{
		return Suffix1_TextBlock;
	}
	if(!Suffix2_TextBlock->IsVisible() && Suffix2_TextBlock->GetText().IsEmpty())
	{
		return Suffix2_TextBlock;
	}
	if(!Suffix3_TextBlock->IsVisible() && Suffix3_TextBlock->GetText().IsEmpty())
	{
		return Suffix3_TextBlock;
	}
	ensureMsgf(false, TEXT("Not a single suffix box is free, this should never happen!"));
	return nullptr;
}

void UObsidianItemDescriptionBase::InitializeWidgetWithItemStats(const FObsidianItemStats& ItemStats, const bool bDisplayItemImage)
{
	if(bDisplayItemImage && ItemStats.ContainsItemImage())
	{
		if(UTexture2D* ItemTexture = ItemStats.GetItemImage())
		{
			const FIntPoint GridSpan = ItemStats.GetItemGridSpan();
			const FVector2D ImageDesiredSize = FVector2D(
				GridSpan.X * ObsidianInventoryItemsStatics::InventorySlotSize.X,
				GridSpan.Y * ObsidianInventoryItemsStatics::InventorySlotSize.Y);
			
			FSlateBrush ItemImageBrush;
			ItemImageBrush.SetImageSize(ImageDesiredSize);
			ItemImageBrush.SetResourceObject(ItemTexture);
			Item_Image->SetBrush(ItemImageBrush);
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
	else
	{
		StacksContainer_HorizontalBox->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	if(ItemStats.ContainsDescription())
	{
		SetItemDescription(ItemStats.GetDescription());
	}
	else
	{
		ItemDescription_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	if(ItemStats.ContainsAdditionalDescription())
	{
		SetAdditionalItemDescription(ItemStats.GetAdditionalDescription());
	}
	else
	{
		AdditionalItemDescription_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
	}

	FText ItemDisplayName = ItemStats.GetDisplayName();
	if(ItemStats.ContainsAffixes() && ItemStats.IsIdentified())
	{
		TArray<FObsidianAffixDescriptionRow> AffixDescriptionRows = ItemStats.GetAffixDescriptions();
		
		FString ItemNameString = ItemDisplayName.ToString();
		if (ItemStats.ItemRarity == EObsidianItemRarity::Magic)
		{
			for(const FObsidianAffixDescriptionRow& Row : AffixDescriptionRows)
			{
				if (Row.AffixType == EObsidianAffixType::Prefix)
				{
					ItemNameString = FString::Printf(TEXT("%s "), *Row.AffixItemNameAddition) + ItemNameString;
				}
				else if (Row.AffixType == EObsidianAffixType::Suffix)
				{
					ItemNameString += FString::Printf(TEXT(" of %s"), *Row.AffixItemNameAddition);
				}
			}
		}
		else if (ItemStats.ItemRarity == EObsidianItemRarity::Rare)
		{
			ensure(ItemStats.ContainsDisplayNameAddition());
			ItemNameString = FString::Printf(TEXT("%s "), *ItemStats.GetItemDisplayNameAddition()) + ItemNameString;
		}
		
		ItemDisplayName = FText::FromString(ItemNameString);
		
		for(const FObsidianAffixDescriptionRow& Row : AffixDescriptionRows)
		{
			if(Row.AffixType == EObsidianAffixType::Implicit)
			{
				Implicit_TextBlock->SetText(Row.AffixRowDescription);
				Implicit_TextBlock->SetVisibility(ESlateVisibility::Visible);
			}

			if(Row.AffixType == EObsidianAffixType::Prefix)
			{
				if(UCommonTextBlock* FreePrefixBlock = GetFreePrefixBlock())
				{
					FreePrefixBlock->SetText(Row.AffixRowDescription);
					FreePrefixBlock->SetVisibility(ESlateVisibility::Visible);
				}
			}

			if(Row.AffixType == EObsidianAffixType::Suffix)
			{
				if(UCommonTextBlock* FreeSuffixBlock = GetFreeSuffixBlock())
				{
					FreeSuffixBlock->SetText(Row.AffixRowDescription);
					FreeSuffixBlock->SetVisibility(ESlateVisibility::Visible);
				}
			}
		}
	}
	else if(ItemStats.SupportsIdentification() && !ItemStats.IsIdentified())
	{
		Unidentified_TextBlock->SetVisibility(ESlateVisibility::Visible);
		IdentificationHint_TextBlock->SetVisibility(ESlateVisibility::Visible);
	}

	SetItemDisplayName(ItemDisplayName, ItemStats.ItemRarity);
}

void UObsidianItemDescriptionBase::SetItemDisplayName(const FText& DisplayName, const EObsidianItemRarity Rarity)
{
	if(!DisplayName.IsEmpty())
	{
		ItemName_TextBlock->SetText(DisplayName);

		switch (Rarity)
		{
			case EObsidianItemRarity::Normal:
				{
					check(NormalItemName_TextStyle);
					ItemName_TextBlock->SetStyle(NormalItemName_TextStyle);
				} break;
			case EObsidianItemRarity::Magic:
				{
					check(MagicItemName_TextStyle);
					ItemName_TextBlock->SetStyle(MagicItemName_TextStyle);
				} break;
			case EObsidianItemRarity::Rare:
				{
					check(RareItemName_TextStyle)
					ItemName_TextBlock->SetStyle(RareItemName_TextStyle);
				} break;
			case EObsidianItemRarity::Unique:
				{
					check(UniqueItemName_TextStyle);
					ItemName_TextBlock->SetStyle(UniqueItemName_TextStyle);
				} break;
			case EObsidianItemRarity::Set:
				{
					UE_LOG(LogTemp, Warning, TEXT("Add SetItemName_TextStyle!"));
				} break;
			case EObsidianItemRarity::Quest:
				{
					UE_LOG(LogTemp, Warning, TEXT("Add QuestItemName_TextStyle!"));
				} break;
				default:
					{
						check(NormalItemName_TextStyle);
						ItemName_TextBlock->SetStyle(NormalItemName_TextStyle);
					} break;
		}
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

bool UObsidianItemDescriptionBase::IsEquipmentDescription() const
{
	return AssociatedItemPosition.IsPositionedAtSlot() && AssociatedItemPosition.GetOwningStashTabTag() == FGameplayTag::EmptyTag;
}

bool UObsidianItemDescriptionBase::IsInventoryItemDescription() const
{
	return AssociatedItemPosition.IsPositionedOnGrid() && AssociatedItemPosition.GetOwningStashTabTag() == FGameplayTag::EmptyTag;
}

bool UObsidianItemDescriptionBase::IsPlayerStashItemDescription() const
{
	return AssociatedItemPosition.GetOwningStashTabTag() != FGameplayTag::EmptyTag;
}



