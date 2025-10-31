// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/Items/ObsidianItemDescriptionBase.h"

// ~ Core
#include "CommonTextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/Overlay.h"

// ~ Project
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"
#include "UI/Components/ObsidianAffixRow.h"

void UObsidianItemDescriptionBase::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::HitTestInvisible);
}

UObsidianAffixRow* UObsidianItemDescriptionBase::GetFreePrefixBlock()
{
	for (UObsidianAffixRow* AffixRow : {Prefix1_AffixRow, Prefix2_AffixRow, Prefix3_AffixRow})
	{
		if (AffixRow && AffixRow->IsFree())
		{
			return AffixRow;
		}
	}
	ensureMsgf(false, TEXT("Not a single prefix box is free, this should never happen!"));
	return nullptr;
}

UObsidianAffixRow* UObsidianItemDescriptionBase::GetFreeSuffixBlock()
{
	for (UObsidianAffixRow* AffixRow : {Suffix1_AffixRow, Suffix2_AffixRow, Suffix3_AffixRow})
	{
		if (AffixRow && AffixRow->IsFree())
		{
			return AffixRow;
		}
	}
	ensureMsgf(false, TEXT("Not a single suffix box is free, this should never happen!"));
	return nullptr;
}

UObsidianAffixRow* UObsidianItemDescriptionBase::GetFreeBlockForUniqueItem()
{
	const TArray<UObsidianAffixRow*> AffixRows = {
		Prefix1_AffixRow, Prefix2_AffixRow, Prefix3_AffixRow,
		Suffix1_AffixRow, Suffix2_AffixRow, Suffix3_AffixRow
	};
	for (UObsidianAffixRow* AffixRow : AffixRows)
	{
		if (AffixRow && AffixRow->IsFree())
		{
			return AffixRow;
		}
	}
	ensureMsgf(false, TEXT("Not a single affix block is free, this should never happen!"));
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

	//TODO(intrxx) maybe the creation of items name should be done inside the item logic? This might potentially be expensive if spammed.
	FText ItemDisplayName = ItemStats.GetDisplayName();
	if(ItemStats.SupportsIdentification() && !ItemStats.IsIdentified())
	{
		Unidentified_TextBlock->SetVisibility(ESlateVisibility::Visible);
		IdentificationHint_TextBlock->SetVisibility(ESlateVisibility::Visible);

		if (ItemStats.ContainsAffixes()) // We always want to show Skill Implicit if we got one.
		{
			for(const FObsidianAffixDescriptionRow& Row : ItemStats.GetAffixDescriptions()) 
			{
				if (Row.AffixType == EObsidianAffixType::SkillImplicit)
				{
					SkillImplicit_AffixRow->InitializeAffixRow(Row.AffixRowDescription);
					break;
				}
			}
		}
	}
	else if(ItemStats.ContainsAffixes())
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

			if (ItemStats.ContainsMagicDisplayNameAddition())
			{
				ItemNameString = FString::Printf(TEXT("%s "), *ItemStats.GetMagicItemDisplayNameAddition()) + ItemNameString;
			}
		}
		else if (ItemStats.ItemRarity == EObsidianItemRarity::Rare)
		{
			ensure(ItemStats.ContainsRareDisplayNameAddition());
			ItemNameString = FString::Printf(TEXT("%s "), *ItemStats.GetRareItemDisplayNameAddition()) + ItemNameString;
		}
		
		ItemDisplayName = FText::FromString(ItemNameString);
		
		for(const FObsidianAffixDescriptionRow& Row : AffixDescriptionRows)
		{
			switch (Row.AffixType)
			{
				case EObsidianAffixType::SkillImplicit:
					{
						SkillImplicit_AffixRow->InitializeAffixRow(Row.AffixRowDescription);
					} break;
				case EObsidianAffixType::Implicit:
					{
						Implicit_AffixRow->InitializeAffixRow(Row.AffixRowDescription);
					} break;
				case EObsidianAffixType::Prefix:
					{
						if(UObsidianAffixRow* FreePrefixRow = GetFreePrefixBlock())
						{
							FreePrefixRow->InitializeAffixRow(Row.AffixRowDescription);
						}
					} break;
				case EObsidianAffixType::Suffix:
					{
						if(UObsidianAffixRow* FreeSuffixRow = GetFreeSuffixBlock())
						{
							FreeSuffixRow->InitializeAffixRow(Row.AffixRowDescription);
						}
					} break;
				case EObsidianAffixType::Unique:
					{
						if(UObsidianAffixRow* FreeAffixRow = GetFreeBlockForUniqueItem())
						{
							FreeAffixRow->InitializeAffixRow(Row.AffixRowDescription);
						}
					} break;
				default:
				{} break;
			}
		}
	}
	
	SetItemDisplayName(ItemDisplayName, ItemStats.ItemRarity);

	if (ItemStats.HasHeroClassRequirement())
	{
		
	}
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



