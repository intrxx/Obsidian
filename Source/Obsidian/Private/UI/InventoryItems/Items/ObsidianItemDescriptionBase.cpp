// Copyright 2026 out of sCope team - intrxx

#include "UI/InventoryItems/Items/ObsidianItemDescriptionBase.h"

#include "CommonTextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/Overlay.h"

#include "Obsidian/ObsidianLogCategories.h"
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"
#include "UI/InventoryItems/Items/ObsidianAffixRow.h"
#include "UI/InventoryItems/Items/ObsidianItemDescRequirementsBlock.h"


void UObsidianItemDescriptionBase::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::HitTestInvisible);
}

UObsidianAffixRow* UObsidianItemDescriptionBase::GetFreePrimaryItemAffixBlock()
{
	for (UObsidianAffixRow* AffixRow : {PrimaryItemAffix1_AffixRow, PrimaryItemAffix2_AffixRow, PrimaryItemAffix3_AffixRow})
	{
		if (AffixRow && AffixRow->IsFree())
		{
			return AffixRow;
		}
	}
	ensureMsgf(false, TEXT("Not a single primary affix box is free, this should never happen!"));
	return nullptr;
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

void UObsidianItemDescriptionBase::InitializeWidgetWithItemStats(const FObsidianItemStats& InItemStats, const bool bInDisplayItemImage)
{
	if(bInDisplayItemImage && InItemStats.ContainsItemImage())
	{
		if(UTexture2D* ItemTexture = InItemStats.GetItemImage())
		{
			const FIntPoint GridSpan = InItemStats.GetItemGridSpan();
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
	
	if(InItemStats.ContainsStacks())
	{
		const FObsidianStacksUIData StacksData = InItemStats.GetItemStacks();
		SetStackCount(StacksData.CurrentItemStackCount, StacksData.MaxItemStackCount);
	}
	else
	{
		StacksContainer_HorizontalBox->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	if(InItemStats.ContainsDescription())
	{
		SetItemDescription(InItemStats.GetDescription());
	}
	else
	{
		ItemDescription_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	if(InItemStats.ContainsAdditionalDescription())
	{
		SetAdditionalItemDescription(InItemStats.GetAdditionalDescription());
	}
	else
	{
		AdditionalItemDescription_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
	}

	//TODO(intrxx) maybe the creation of items name should be done inside the item logic? This might potentially be expensive if spammed.
	FText ItemDisplayName = InItemStats.GetDisplayName();
	if(InItemStats.SupportsIdentification() && !InItemStats.IsIdentified())
	{
		Unidentified_TextBlock->SetVisibility(ESlateVisibility::Visible);
		IdentificationHint_TextBlock->SetVisibility(ESlateVisibility::Visible);

		if (InItemStats.ContainsAffixes()) // We always want to show Skill Implicit and Primary Item Affix if we got them.
		{
			for(const FObsidianAffixDescriptionRow& Row : InItemStats.GetAffixDescriptions()) 
			{
				if (Row.AffixType == EObsidianAffixType::SkillImplicit)
				{
					SkillImplicit_AffixRow->InitializeAffixRow(Row.AffixRowDescription);
				}
				else if (Row.AffixType == EObsidianAffixType::PrimaryItemAffix)
				{
					if(UObsidianAffixRow* PrimarySkillImplicitRow = GetFreePrimaryItemAffixBlock())
					{
						PrimarySkillImplicitRow->InitializeAffixRow(Row.AffixRowDescription);
					}
				}
			}
		}
	}
	else if(InItemStats.ContainsAffixes())
	{
		TArray<FObsidianAffixDescriptionRow> AffixDescriptionRows = InItemStats.GetAffixDescriptions();
		
		FString ItemNameString = ItemDisplayName.ToString();
		if (InItemStats.ItemRarity == EObsidianItemRarity::Magic)
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

			if (InItemStats.ContainsMagicDisplayNameAddition())
			{
				ItemNameString = FString::Printf(TEXT("%s "), *InItemStats.GetMagicItemDisplayNameAddition()) + ItemNameString;
			}
		}
		else if (InItemStats.ItemRarity == EObsidianItemRarity::Rare)
		{
			ensure(InItemStats.ContainsRareDisplayNameAddition());
			ItemNameString = FString::Printf(TEXT("%s "), *InItemStats.GetRareItemDisplayNameAddition()) + ItemNameString;
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
				case EObsidianAffixType::PrimaryItemAffix:
					{
						SkillImplicitSeparator_Image->SetVisibility(ESlateVisibility::Visible);
						if(UObsidianAffixRow* PrimarySkillImplicitRow = GetFreePrimaryItemAffixBlock())
						{
							PrimarySkillImplicitRow->InitializeAffixRow(Row.AffixRowDescription);
						}
					} break;
				case EObsidianAffixType::Implicit:
					{
						ImplicitSeparator_Image->SetVisibility(ESlateVisibility::Visible);
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
	
	SetItemDisplayName(ItemDisplayName, InItemStats.ItemRarity);

	if (InItemStats.HasItemEquippingRequirements())
	{
		ItemRequirements_RequirementsBlock->InitializeRequirementsBlock(InItemStats.GetItemEquippingRequirements());
	}
	else
	{
		ItemRequirements_RequirementsBlock->SetVisibility(ESlateVisibility::Collapsed);
	}

	// NOT SHOWN STATS, WILL BE SHOWN AFTER ALT
}

void UObsidianItemDescriptionBase::SetItemDisplayName(const FText& InDisplayName, const EObsidianItemRarity InRarity)
{
	if(!InDisplayName.IsEmpty())
	{
		ItemName_TextBlock->SetText(InDisplayName);

		switch (InRarity)
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
					UE_LOG(ObLogItems, Warning, TEXT("Add SetItemName_TextStyle!"));
				} break;
			case EObsidianItemRarity::Quest:
				{
					UE_LOG(ObLogItems, Warning, TEXT("Add QuestItemName_TextStyle!"));
				} break;
				default:
					{
						check(NormalItemName_TextStyle);
						ItemName_TextBlock->SetStyle(NormalItemName_TextStyle);
					} break;
		}
	}
}

void UObsidianItemDescriptionBase::SetStackCount(const int32 InCurrentStacks, const int32 InMaxStacks)
{
	CurrentStackCount = InCurrentStacks;
	MaxStackCount = InMaxStacks;
	
	StacksContainer_HorizontalBox->SetVisibility(ESlateVisibility::Visible);
	const FText StackCountText = FText::FromString(FString::Printf(TEXT("%d/%d"), CurrentStackCount, MaxStackCount));
	StackCount_TextBlock->SetText(StackCountText);
}

void UObsidianItemDescriptionBase::UpdateCurrentStackCount(const int32 InCurrentStacks)
{
	CurrentStackCount = InCurrentStacks;
	
	StacksContainer_HorizontalBox->SetVisibility(ESlateVisibility::Visible);
	const FText StackCountText = FText::FromString(FString::Printf(TEXT("%d/%d"), CurrentStackCount, MaxStackCount));
	StackCount_TextBlock->SetText(StackCountText);
}

void UObsidianItemDescriptionBase::SetItemDescription(const FText& InItemDescription)
{
	if(!InItemDescription.IsEmpty())
	{
		ItemDescription_TextBlock->SetVisibility(ESlateVisibility::Visible);
		ItemDescription_TextBlock->SetText(InItemDescription);
	}
}

void UObsidianItemDescriptionBase::SetAdditionalItemDescription(const FText& InAdditionalItemDescription)
{
	if(!InAdditionalItemDescription.IsEmpty())
	{
		AdditionalItemDescription_TextBlock->SetVisibility(ESlateVisibility::Visible);
		AdditionalItemDescription_TextBlock->SetText(InAdditionalItemDescription);
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
	return AssociatedItemPosition.IsOnEquipmentSlot();
}

bool UObsidianItemDescriptionBase::IsInventoryItemDescription() const
{
	return AssociatedItemPosition.IsOnInventoryGrid();
}

bool UObsidianItemDescriptionBase::IsPlayerStashItemDescription() const
{
	return AssociatedItemPosition.IsOnStash();
}



