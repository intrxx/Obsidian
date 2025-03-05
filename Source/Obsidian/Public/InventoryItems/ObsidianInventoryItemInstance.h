// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "ObsidianItemAffixStack.h"
#include "Core/ObsidianGameplayTagStack.h"
#include "Fragments/OInventoryItemFragment_Affixes.h"
#include "ObsidianTypes/ObsidianItemTypes.h"
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

	static UObsidianInventoryItemInstance* DuplicateItem(const UObsidianInventoryItemInstance* OriginalItem, UObject* Outer);

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	FGameplayTag GetItemRarity() const
	{
		return ItemRarity;
	}
	
	void SetItemRarity(const FGameplayTag& InItemRarityTag)
	{
		ItemRarity = InItemRarityTag;
	}
	
	/**
	 * Item Affixes.
	 */

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	void SetIdentified(const bool InIdentified);

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	bool IsItemIdentified() const;
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	void AddAffix(const FObsidianItemAffix& AffixToAdd);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	void RemoveAffix(const FGameplayTag& AffixTag);

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	TArray<FObsidianAffixDescriptionRow> GetAffixesAsUIDescription() const;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	int32 GetItemCombinedAffixLimit() const;
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	int32 GetItemAddedSuffixCount() const;
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	int32 GetItemAddedPrefixCount() const;
	
	void SetItemAffixesCountLimit(const int32 InAffixesLimit);
	void SetItemAddedSuffixCount(const int32 InAddedSuffixes);
	void SetItemAddedPrefixCount(const int32 InAddedPrefixes);

	/**
	 * Tag Stacks.
	 */

	/** Adds a specified number of stacks to the tag (does nothing if StackCount is below 1). */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	void AddItemStackCount(const FGameplayTag ToTag, const int32 StackCount);

	/** Removes a specified number of stacks to the tag (does nothing if StackCount is below 1). */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	void RemoveItemStackCount(const FGameplayTag FromTag, const int32 StackCount);

	/** Overrides stacks on provided tag (does nothing if StackCount is below 1). */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	void OverrideItemStackCount(const FGameplayTag Tag, const int32 NewStackCount);
	
	/** Returns the stack count of the specified tag (or 0 is there is no stack for this tag). */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	int32 GetItemStackCount(const FGameplayTag Tag) const;

	/** Return true if there is at least one stack of the specified tag. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	bool HasStackCountForTag(const FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	bool HasAnyStacks() const;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	bool IsStackable() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	void SetStackable(const bool InStackable);

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
	 * Item Current Grid Location.
	 */

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	FVector2D GetItemCurrentGridLocation() const;
	
	void SetItemCurrentGridLocation(const FVector2D CurrentGridLocationToSet);

	/** Should be called when removing item from inventory. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	void ResetItemCurrentGridLocation();
	
	/**
	 * Item Image.
	 */

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	UTexture2D* GetItemImage() const;
	
	void SetItemImage(UTexture2D* ItemImageToSet);
	
	/**
	 * Item Skeletal Mesh.
	 */
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	USkeletalMesh* GetItemSkeletalMesh() const;
	
	void SetItemSkeletalMesh(USkeletalMesh* InItemSkeletalMesh);

	/**
	 * Item Static Mesh.
	 */

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	UStaticMesh* GetItemDroppedMesh() const;
	
	void SetItemDroppedMesh(UStaticMesh* InItemDroppedMesh);

	/**
	 * Item Display Name.
	 */

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	FText GetItemDisplayName() const;
	
	void SetItemDisplayName(const FText& InItemDisplayName);

	/**
	 * Item Description.
	 */

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	FText GetItemDescription() const;
	
	void SetItemDescription(const FText& InItemDescription);

	/**
	 * Item Additional Description.
	 */

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	FText GetItemAdditionalDescription() const;
	
	void SetItemAdditionalDescription(const FText& InItemAdditionalDescription);

	/**
	 * Debug Name
	 */

	/** Gets the debug name of the item, will be valid only on the server. */	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	FString GetItemDebugName() const;

	void SetItemDebugName(const FString& InItemDebugName);

private:
	UPROPERTY(Replicated)
	TSubclassOf<UObsidianInventoryItemDefinition> ItemDef;

	UPROPERTY(Replicated)
	FGameplayTag ItemRarity = ObsidianGameplayTags::Item_Rarity_Normal;
	
	/**
	 * Item Affixes.
	 */

	UPROPERTY(Replicated)
	bool bIdentified = true;
	
	UPROPERTY(Replicated)
	FObsidianItemAffixStack ItemAffixes;
	
	/**
	 * Item Stack Count.
	 */
	
	UPROPERTY(Replicated)
	FGameplayTagStackContainer ItemStackTags;

	UPROPERTY(Replicated)
	bool bStackable = false;

	UPROPERTY(Replicated)
	TArray<FVector2D> ItemGridSize;

	UPROPERTY(Replicated)
	FVector2D ItemGridSpan = FVector2D::ZeroVector;

	/** Current Item Location in the inventory grid, should be valid only if the item is already placed in the inventory. */
	UPROPERTY(Replicated)
	FVector2D ItemCurrentGridLocation  = FVector2D::ZeroVector;

	UPROPERTY(Replicated)
	TObjectPtr<UTexture2D> ItemImage = nullptr;

	UPROPERTY(Replicated)
	TObjectPtr<USkeletalMesh> ItemSkeletalMesh = nullptr;

	UPROPERTY(Replicated)
	TObjectPtr<UStaticMesh> ItemDroppedMesh = nullptr;
	
	UPROPERTY(Replicated)
	FText ItemDisplayName = FText::GetEmpty();

	UPROPERTY(Replicated)
	FText ItemDescription = FText::GetEmpty();
	
	UPROPERTY(Replicated)
	FText ItemAdditionalDescription = FText::GetEmpty();
	
	FString DebugName = FString("Not Set");
};
