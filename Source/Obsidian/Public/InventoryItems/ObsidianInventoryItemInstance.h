// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Core/ObsidianGameplayTagStack.h"
#include "ObsidianInventoryItemInstance.generated.h"

class UObsidianInventoryItemFragment;
class UObsidianInventoryItemDefinition;
/**
 * Instance of and Item Definition. When a player obtain an item, this is the actual thing that they receive.
 */
UCLASS(BlueprintType)
class OBSIDIAN_API UObsidianInventoryItemInstance : public UObject
{
	GENERATED_BODY()

public:
	UObsidianInventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~ Start of UObject interface
	virtual bool IsSupportedForNetworking() const override {return true;}
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End of UObject interface

	TSubclassOf<UObsidianInventoryItemDefinition> GetItemDef() const
	{
		return ItemDef;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, meta=(DeterminesOutputType = FragmentClass))
	const UObsidianInventoryItemFragment* FindFragmentByClass(TSubclassOf<UObsidianInventoryItemFragment> FragmentClass) const;

	template <typename T>
	const T* FindFragmentByClass() const
	{
		return (T*)FindFragmentByClass(T::StaticClass());
	}

	void SetItemDef(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef)
	{
		ItemDef = InItemDef;
	}

	/**
	 * Tag Stacks.
	 */

	/** Adds a specified number of stacks to the tag (does nothing if StackCount is below 1). */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	void AddItemStackCount(const FGameplayTag ToTag, const int32 StackCount);

	/** Removes a specified number of stacks to the tag (does nothing if StackCount is below 1). */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	void RemoveItemStackCount(const FGameplayTag FromTag, const int32 StackCount);
	
	/** Returns the stack count of the specified tag (or 0 is there is no stack for this tag). */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	int32 GetItemStackCount(const FGameplayTag Tag) const;

	/** Return true if there is at least one stack of the specified tag. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	bool HasStackCountForTag(const FGameplayTag Tag) const;

	/**
	 * Item Grid Size.
	 */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	TArray<FVector2D> GetItemGridSize() const;
	
	void SetItemGridSize(const TArray<FVector2D>& GridSizeToSet);

	/**
	 * Item Grid Span.
	 */

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	FVector2D GetItemGridSpan() const;
	
	void SetItemGridSpan(const FVector2D GridSpanToSet);
	
	/**
	 * Item Image.
	 */

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	UTexture2D* GetItemImage() const;
	
	void SetItemImage(UTexture2D* ItemImageToSet);

private:
	UPROPERTY(Replicated)
	TSubclassOf<UObsidianInventoryItemDefinition> ItemDef;
	
	UPROPERTY(Replicated)
	FGameplayTagStackContainer ItemStackTags;

	UPROPERTY(Replicated)
	TArray<FVector2D> ItemGridSize;

	UPROPERTY(Replicated)
	FVector2D ItemGridSpan;

	UPROPERTY(Replicated)
	TObjectPtr<UTexture2D> ItemImage;
};
