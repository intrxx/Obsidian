// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "ObsidianItemAffixStack.h"
#include "InventoryItems/ObsidianGameplayTagStack.h"
#include "Fragments/OInventoryItemFragment_Affixes.h"
#include "Fragments/OInventoryItemFragment_Equippable.h"
#include "Fragments/Shards/ObsidianUsableShard.h"
#include "ObsidianTypes/ObsidianItemTypes.h"
#include "ObsidianInventoryItemInstance.generated.h"

class UObsidianUsableShard;
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

	/** Called when Instance is created for the first time and is initialized with all necessary variables. */
	void OnInstanceCreatedAndInitialized();

	UFUNCTION(BlueprintPure, Category=Equipment)
	APawn* GetPawn() const;

	UWorld* GetWorld() const override final;
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, meta=(DeterminesOutputType = FragmentClass))
	const UObsidianInventoryItemFragment* FindFragmentByClass(TSubclassOf<UObsidianInventoryItemFragment> FragmentClass) const;

	template <typename T>
	const T* FindFragmentByClass() const
	{
		return (T*)FindFragmentByClass(T::StaticClass());
	}

	static UObsidianInventoryItemInstance* DuplicateItem(const UObsidianInventoryItemInstance* OriginalItem, UObject* Outer);

	/**
	 * Item. 
	 */
	
	TSubclassOf<UObsidianInventoryItemDefinition> GetItemDef() const;
	
	void SetItemDef(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef);
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	FGameplayTag GetItemRarity() const;
	
	void SetItemRarity(const FGameplayTag& InItemRarityTag);

	FGameplayTag GetItemCategoryTag() const;

	void SetItemCategory(const FGameplayTag& InItemCategoryTag);

	/**
	 * Usability
	 */

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	void SetUsable(const bool IsUsable);
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	bool IsItemUsable() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	void SetUsableShard(UObsidianUsableShard* InUsableShard);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	bool UseItem(UObsidianInventoryItemInstance* UsingOntoInstance);

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	FObsidianItemsMatchingUsableContext FireItemUseUIContext(const TArray<UObsidianInventoryItemInstance*>& AllItems) const;

	/**
	 * Equipping.
	 */

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	void SetEquippable(const bool InEquippable);
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	bool IsItemEquippable() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	void SetShouldBlockOtherSlot(const bool InShouldBlock);
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	bool ShouldBlockOtherSlot() const;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	TArray<AObsidianSpawnedEquipmentPiece*> GetSpawnedActors() const;

	void SetEquipmentActors(const TArray<FObsidianEquipmentActor>& EquipmentActors);
	void SpawnEquipmentActors(const FGameplayTag& SlotTag);
	void DestroyEquipmentActors();

	void SetAbilitySets(const TArray<UObsidianAbilitySet*>& InAbilitySets);
	TArray<UObsidianAbilitySet*> GetOwningAbilitySets() const;
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	FGameplayTag GetItemCurrentEquipmentSlot() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	void SetItemCurrentEquipmentSlot(const FGameplayTag& CurrentEquipmentSlotToSet);

	/** Should be called when item unequipped. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	void ResetItemCurrentEquipmentSlot();
	
	/**
	 * Affixes.
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
	 * Stacks.
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
	 * Appearance.
	 */
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	TArray<FVector2D> GetItemGridSize() const;
	
	void SetItemGridSize(const TArray<FVector2D>& GridSizeToSet);
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	FVector2D GetItemGridSpan() const;
	
	void SetItemGridSpan(const FVector2D GridSpanToSet);
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	UTexture2D* GetItemImage() const;
	
	void SetItemImage(UTexture2D* ItemImageToSet);
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	UStaticMesh* GetItemDroppedMesh() const;
	
	void SetItemDroppedMesh(UStaticMesh* InItemDroppedMesh);
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	FText GetItemDisplayName() const;
	
	void SetItemDisplayName(const FText& InItemDisplayName);

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	FText GetItemDescription() const;
	
	void SetItemDescription(const FText& InItemDescription);

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	FText GetItemAdditionalDescription() const;
	
	void SetItemAdditionalDescription(const FText& InItemAdditionalDescription);

	/**
	 * Inventory.
	 */

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	FVector2D GetItemCurrentGridLocation() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	void SetItemCurrentGridLocation(const FVector2D CurrentGridLocationToSet);

	/** Should be called when removing item from inventory. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	void ResetItemCurrentGridLocation();
	
	/**
	 * Debug.
	 */

	/** Gets the debug name of the item, will be valid only on the server. */	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	FString GetItemDebugName() const;

	void SetItemDebugName(const FString& InItemDebugName);

private:
	/**
	 * Item.
	 */
	
	UPROPERTY(Replicated)
	TSubclassOf<UObsidianInventoryItemDefinition> ItemDef;

	UPROPERTY(Replicated)
	FGameplayTag ItemCategory = FGameplayTag::EmptyTag;

	UPROPERTY(Replicated)
	FGameplayTag ItemRarity = ObsidianGameplayTags::Item_Rarity_Normal;

	/**
	 * Usability
	 */

	UPROPERTY(Replicated)
	bool bUsable = false;

	UPROPERTY(Replicated)
	TObjectPtr<UObsidianUsableShard> UsableShard = nullptr;

	/**
	 * Equipping.
	 */

	UPROPERTY(Replicated)
	bool bEquippable = false;

	UPROPERTY(Replicated)
	bool bShouldBlockOtherSlot = false;

	UPROPERTY(Replicated)
	TArray<FObsidianEquipmentActor> ActorsToSpawn;

	UPROPERTY(Replicated)
	TArray<TObjectPtr<AObsidianSpawnedEquipmentPiece>> SpawnedActors;

	/** Does not need to be replicate since the attributes will replicate, idk about abilities, will need to check. */
	TArray<TObjectPtr<UObsidianAbilitySet>> AbilitySetsToGive;

	/** Current Item Equipment Slot, should be valid only if the item is equipped. */
	UPROPERTY(Replicated)
	FGameplayTag ItemCurrentEquipmentSlot = FGameplayTag::EmptyTag;
	
	/**
	 * Affixes.
	 */

	UPROPERTY(Replicated)
	bool bIdentified = true;
	
	UPROPERTY(Replicated)
	FObsidianItemAffixStack ItemAffixes;
	
	/**
	 * Stacks.
	 */
	
	UPROPERTY(Replicated)
	FGameplayTagStackContainer ItemStackTags;

	UPROPERTY(Replicated)
	bool bStackable = false;

	/**
	 * Appearance.
	 */
	
	UPROPERTY(Replicated)
	TArray<FVector2D> ItemGridSize;

	UPROPERTY(Replicated)
	FVector2D ItemGridSpan = FVector2D::ZeroVector;
	
	UPROPERTY(Replicated)
	TObjectPtr<UTexture2D> ItemImage = nullptr;
	
	UPROPERTY(Replicated)
	TObjectPtr<UStaticMesh> ItemDroppedMesh = nullptr;
	
	UPROPERTY(Replicated)
	FText ItemDisplayName = FText::GetEmpty();

	UPROPERTY(Replicated)
	FText ItemDescription = FText::GetEmpty();
	
	UPROPERTY(Replicated)
	FText ItemAdditionalDescription = FText::GetEmpty();

	/**
	 * Inventory.
	 */

	/** Current Item Location in the inventory grid, should be valid only if the item is already placed in the inventory. */
	UPROPERTY(Replicated)
	FVector2D ItemCurrentGridLocation = FVector2D::ZeroVector;

	/**
	 * Debug.
	 */
	
	FString DebugName = FString("Not Set");
};
