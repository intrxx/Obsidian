// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "UI/ObsidianWidgetBase.h"
#include "ObsidianItemDescriptionBase.generated.h"

class UObsidianItemDescRequirementsBlock;
struct FGameplayTag;
struct FObsidianItemStats;

class UObsidianAffixRow;
class UCommonTextStyle;
class UVerticalBox;
class UHorizontalBox;
class UCommonTextBlock;
class UImage;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianItemDescriptionBase : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	void InitializeWidgetWithItemStats(const FObsidianItemStats& ItemStats, const bool bDisplayItemImage = false);
	
	void SetItemDisplayName(const FText& DisplayName, const EObsidianItemRarity Rarity);
	void SetItemDescription(const FText& ItemDescription);
	void SetAdditionalItemDescription(const FText& AdditionalItemDescription);

	void SetStackCount(const int32 CurrentStacks, const int32 MaxStacks);
	void UpdateCurrentStackCount(const int32 CurrentStacks);

	void DestroyDescriptionWidget();

	bool IsEquipmentDescription() const;
	bool IsInventoryItemDescription() const;
	bool IsPlayerStashItemDescription() const;
	
	void SetAssociatedItemPosition(const FObsidianItemPosition& InPosition)
	{
		AssociatedItemPosition = InPosition;
	};
	
	FObsidianItemPosition GetAssociatedItemPosition() const
	{
		return AssociatedItemPosition;
	}

protected:
	virtual void NativeConstruct() override;

	UObsidianAffixRow* GetFreePrimaryItemAffixBlock();
	UObsidianAffixRow* GetFreePrefixBlock();
	UObsidianAffixRow* GetFreeSuffixBlock();
	UObsidianAffixRow* GetFreeBlockForUniqueItem();

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UVerticalBox> Description_VerticalBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Item_Image;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ItemName_Image;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> ItemName_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianAffixRow> PrimaryItemAffix1_AffixRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianAffixRow> PrimaryItemAffix2_AffixRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianAffixRow> PrimaryItemAffix3_AffixRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> StacksContainer_HorizontalBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianItemDescRequirementsBlock> ItemRequirements_RequirementsBlock;
	
	UPROPERTY(meta=(BindWidget))	
	TObjectPtr<UCommonTextBlock> StackCount_TextBlock;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> ItemDescription_TextBlock;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> AdditionalItemDescription_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> Unidentified_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianAffixRow> SkillImplicit_AffixRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ImplicitSeparator_Image;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianAffixRow> Implicit_AffixRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> SkillImplicitSeparator_Image;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianAffixRow> Prefix1_AffixRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianAffixRow> Prefix2_AffixRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianAffixRow> Prefix3_AffixRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianAffixRow> Suffix1_AffixRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianAffixRow> Suffix2_AffixRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianAffixRow> Suffix3_AffixRow;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> IdentificationHint_TextBlock;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	TSubclassOf<UCommonTextStyle> NormalItemName_TextStyle;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	TSubclassOf<UCommonTextStyle> MagicItemName_TextStyle;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	TSubclassOf<UCommonTextStyle> RareItemName_TextStyle;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	TSubclassOf<UCommonTextStyle> UniqueItemName_TextStyle;

private:
	void CollapseStatBlocks();

private:
	int32 CurrentStackCount = 0;
	int32 MaxStackCount = 0;
	
	FObsidianItemPosition AssociatedItemPosition = FObsidianItemPosition();
};

