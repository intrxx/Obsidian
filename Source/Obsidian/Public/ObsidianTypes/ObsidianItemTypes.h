// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "ObsidianItemTypes.generated.h"

class UObsidianInventoryItemInstance;

namespace ObsidianInventoryItemsStatics
{
	static const FVector2D InventorySlotSize(64.0f, 64.0f); 
}

namespace ObsidianDefaultStackCounts 
{
	static constexpr int32 CurrentStackCount = 1;
	static constexpr int32 MaxStackCount = 1;
	static constexpr int32 LimitCount = 0;

	/** Returns the unified project default for given Stack Tag. */
	inline int32 GetUnifiedDefaultForTag(const FGameplayTag Tag)
	{
		if(Tag == ObsidianGameplayTags::Item_StackCount_Current)
		{
			return CurrentStackCount;
		}
		if(Tag == ObsidianGameplayTags::Item_StackCount_Max)
		{
			return MaxStackCount;
		}
		if(Tag == ObsidianGameplayTags::Item_StackCount_Limit)
		{
			return LimitCount;
		}
		return 0;
	}
}

USTRUCT(BlueprintType)
struct FObsidianAddingStacksResult
{
	GENERATED_BODY()

public:
	/** The amount of stacks that was added from provided Item to other Item/Items. */
	UPROPERTY(BlueprintReadOnly)
	int32 AddedStacks = 0;

	/** The amount of stacks that is left on the provided Item. */
	UPROPERTY(BlueprintReadOnly)
	int32 StacksLeft = -1;
	
	/** Whole Item was added as stacks to other Item/Items. */
	UPROPERTY(BlueprintReadOnly)
	bool bAddedWholeItemAsStacks = false;

	/** Added at least 1 stack from provided item to some other item. */
	UPROPERTY(BlueprintReadOnly)
	bool bAddedSomeOfTheStacks = false;

	/** The last Item Instance that we added some stacks to. */
	UPROPERTY(BlueprintReadOnly)
	UObsidianInventoryItemInstance* LastAddedToInstance = nullptr;
};

USTRUCT(BlueprintType)
struct FObsidianStacksUIData
{
public:
	GENERATED_BODY();
	
	FObsidianStacksUIData()
	{}
	FObsidianStacksUIData(const int32 CurrentStacks, const int32 MaxStacks)
		: CurrentItemStackCount(CurrentStacks)
		, MaxItemStackCount(MaxStacks)
	{}

	void SetCurrentStacks(const int32 InCurrentStacks)
	{
		CurrentItemStackCount = InCurrentStacks;
	}

	void SetMaxStacks(const int32 InMaxStacks)
	{
		MaxItemStackCount = InMaxStacks;
	}
	
public:
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentItemStackCount = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 MaxItemStackCount = 0;
};

USTRUCT()
struct FDraggedItem
{
public:
	GENERATED_BODY()
	
	FDraggedItem(){};
	FDraggedItem(UObsidianInventoryItemInstance* InInstance)
		: Instance(InInstance)
		, Stacks(InInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current))
	{}
	FDraggedItem(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef, const int32 InStacks)
		: ItemDef(InItemDef)
		, Stacks(InStacks)
	{}

	bool IsEmpty() const
	{
		return !Instance && !ItemDef;
	}

	void Clear()
	{
		Instance = nullptr;
		ItemDef = nullptr;
		Stacks = 0;
	}

public:
	UPROPERTY()
	TObjectPtr<UObsidianInventoryItemInstance> Instance = nullptr;

	UPROPERTY()
	TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = nullptr;
	
	UPROPERTY()
	int32 Stacks = 0;
};

USTRUCT(BlueprintType)
struct FObsidianItemStats
{
	GENERATED_BODY()

public:
	bool ContainsItemImage() const
	{
		return bContainsItemImage;
	}
	
	/** Checks if Item Stats contain Item Display Name. This should technically be present on every Item Stats. */
	bool ContainsDisplayName() const
	{
		return bContainsDisplayName;
	}

	/** Checks if the Item Stats contain Description. */
	bool ContainsDescription() const
	{
		return bContainsDescription;
	}

	/** Checks if the Item Stats contain Additional Description. */
	bool ContainsAdditionalDescription() const
	{
		return bContainsAdditionalDescription;
	}

	/** Checks if the Item Stats contain Stacks info. */
	bool ContainsStacks() const
	{
		return bContainsStacks;
	}

	UTexture2D* GetItemImage() const
	{
		return ItemImage;
	}

	FVector2D GetItemGridSpan() const
	{
		return ItemGridSpan;
	}

	FText GetDisplayName() const
	{
		return DisplayName;
	}

	FText GetDescription() const
	{
		return Description;
	}

	FText GetAdditionalDescription() const
	{
		return AdditionalDescription;
	}

	FObsidianStacksUIData GetItemStacks() const
	{
		return StacksData;
	}

	/** It takes an additional ItemGridSpan argument as UI needs it to rescale the image for now. */
	void SetItemImage(UTexture2D* InItemImage, const FVector2D& InItemGridSpan)
	{
		bContainsItemImage = true;
		ItemImage = InItemImage;
		ItemGridSpan = InItemGridSpan;
	}
	
	void SetDisplayName(const FText& InDisplayName)
	{
		bContainsDisplayName = true;
		DisplayName = InDisplayName;
	}

	void SetDescription(const FText& InDescription)
	{
		bContainsDescription = true;
		Description = InDescription;
	}

	void SetAdditionalDescription(const FText& InAdditionalDescription)
	{
		bContainsAdditionalDescription = true;
		AdditionalDescription = InAdditionalDescription;
	}

	void SetStacks(const int32 InCurrentStack, const int32 InMaxStacks)
	{
		bContainsStacks = true;
		StacksData = FObsidianStacksUIData(InCurrentStack, InMaxStacks);
	}

	void SetCurrentStacks(const int32 InCurrentStack)
	{
		bContainsStacks = true;
		StacksData.SetCurrentStacks(InCurrentStack);
	}

	void SetMaxStacks(const int32 InMaxStacks)
	{
		bContainsStacks = true;
		StacksData.SetMaxStacks(InMaxStacks);
	}
	
private:
	UPROPERTY()
	TObjectPtr<UTexture2D> ItemImage;

	UPROPERTY()
	FVector2D ItemGridSpan = FVector2D::Zero();
	
	/**
	 * Item Descriptors.
	 */

	UPROPERTY()
	FText DisplayName = FText::GetEmpty();

	UPROPERTY()
	FText Description = FText::GetEmpty();

	UPROPERTY()
	FText AdditionalDescription = FText::GetEmpty();
	
	/**
	 * Stacks.
	 */

	UPROPERTY()
	FObsidianStacksUIData StacksData = FObsidianStacksUIData();

	/**
	 * Contains booleans.
	 */

	bool bContainsItemImage = false;
	bool bContainsDisplayName = false;
	bool bContainsDescription = false;
	bool bContainsAdditionalDescription = false;
	bool bContainsStacks = false;
};


