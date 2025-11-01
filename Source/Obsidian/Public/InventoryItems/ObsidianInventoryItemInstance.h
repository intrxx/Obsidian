// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>

#include "InventoryItems/ItemAffixes/ObsidianItemAffixStack.h"
#include "InventoryItems/ObsidianGameplayTagStack.h"
#include "Fragments/OInventoryItemFragment_Equippable.h"
#include "Fragments/OInventoryItemFragment_Usable.h"
#include "Fragments/Shards/ObsidianUsableShard.h"
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "ObsidianInventoryItemInstance.generated.h"

class AObsidianPlayerController;
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
	
	/**
	 * Item. 
	 */

	FGuid GetUniqueItemID() const;

	void GenerateUniqueItemID();

	int32 GetItemLevel() const;

	void SetItemLevel(const int32 InItemLevel);
	
	TSubclassOf<UObsidianInventoryItemDefinition> GetItemDef() const;

	void SetItemDef(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef);
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Item")
	EObsidianItemRarity GetItemRarity() const;
	void SetItemRarity(const EObsidianItemRarity InItemRarity);

	FGameplayTag GetItemCategoryTag() const;
	void SetItemCategory(const FGameplayTag& InItemCategoryTag);

	FGameplayTag GetItemBaseTypeTag() const;
	void SetItemBaseType(const FGameplayTag& InItemBaseTypeTag);

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Item")
	FObsidianItemPosition GetItemCurrentPosition() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Item")
	void SetItemCurrentPosition(const FObsidianItemPosition& CurrentPositionToSet);

	/** Should be called when removing item from inventory. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Item")
	void ResetItemCurrentPosition();

	/**
	 * Usability
	 */

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Item")
	void SetUsable(const bool IsUsable);

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Item")
	bool IsItemUsable() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Item")
	void SetUsableShard(UObsidianUsableShard* InUsableShard);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Item")
	bool UseItem(AObsidianPlayerController* ItemOwner, UObsidianInventoryItemInstance* UsingOntoInstance);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Item")
	void SetUsableItemType(const EObsidianUsableItemType InUsableItemTyp);

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Item")
	EObsidianUsableItemType GetUsableItemType() const;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Item")
	FObsidianItemsMatchingUsableContext FireItemUseUIContext(const TArray<UObsidianInventoryItemInstance*>& AllItems) const;

	/**
	 * Equipping.
	 */

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Item")
	void SetEquippable(const bool InEquippable);
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Item")
	bool IsItemEquippable() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Item")
	void SetItemNeedTwoSlots(const bool InNeedsTwoSlots);
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Item")
	bool DoesItemNeedTwoSlots() const;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Item")
	TArray<AObsidianSpawnedEquipmentPiece*> GetSpawnedActors() const;

	void SetEquipmentActors(const TArray<FObsidianEquipmentActor>& EquipmentActors);
	void SpawnEquipmentActors(const FGameplayTag& SlotTag);
	void DestroyEquipmentActors();

	bool HasEquippingRequirements() const;
	FObsidianItemRequirements GetEquippingRequirements() const;
	void InitializeEquippingRequirements(const FObsidianItemRequirements& InRequirements);
	
	/**
	 * Affixes.
	 */
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Item")
	void SetStartsIdentified(const bool InStartsIdentified);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Item")
	void SetIdentified(const bool InIdentified);
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Item")
	bool IsItemIdentified() const;

	TArray<FObsidianActiveItemAffix> GetAllItemAffixes() const;
	
	void InitializeAffixes(const TArray<FObsidianActiveItemAffix>& AffixesToInitialize);
	void AddAffix(const FObsidianActiveItemAffix& AffixToAdd);
	void RemoveAffix(const FGameplayTag& AffixTag);
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Item")
	int32 GetItemCombinedAffixLimit() const;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Item")
	int32 GetItemAddedAffixCount() const;
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Item")
	int32 GetItemAddedSuffixCount() const;
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Item")
	int32 GetItemAddedPrefixCount() const;
	
	void SetItemAffixesCountLimit(const int32 InAffixesLimit);
	void SetItemAddedSuffixCount(const int32 InAddedSuffixes);
	void SetItemAddedPrefixCount(const int32 InAddedPrefixes);

	TArray<UObsidianAffixAbilitySet*> GetAffixAbilitySetsFromItem() const;
	
	/**
	 * Stacks.
	 */

	/** Adds a specified number of stacks to the tag (does nothing if AvailableStackCount is below 1). */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Item")
	void AddItemStackCount(const FGameplayTag ToTag, const int32 StackCount);

	/** Removes a specified number of stacks to the tag (does nothing if AvailableStackCount is below 1). */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Item")
	void RemoveItemStackCount(const FGameplayTag FromTag, const int32 StackCount);

	/** Overrides stacks on provided tag (does nothing if AvailableStackCount is below 1). */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Item")
	void OverrideItemStackCount(const FGameplayTag Tag, const int32 NewStackCount);
	
	/** Returns the stack count of the specified tag (or 0 is there is no stack for this tag). */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Item")
	int32 GetItemStackCount(const FGameplayTag Tag) const;

	/** Return true if there is at least one stack of the specified tag. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Item")
	bool HasStackCountForTag(const FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Item")
	bool HasAnyStacks() const;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Item")
	bool IsStackable() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Item")
	void SetStackable(const bool InStackable);

	/**
	 * Appearance.
	 */
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Item")
	FIntPoint GetItemGridSpan() const;
	
	void SetItemGridSpan(const FIntPoint& GridSpanToSet);
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Item")
	UTexture2D* GetItemImage() const;
	
	void SetItemImage(UTexture2D* ItemImageToSet);
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Item")
	UStaticMesh* GetItemDroppedMesh() const;
	
	void SetItemDroppedMesh(UStaticMesh* InItemDroppedMesh);
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Item")
	FText GetItemDisplayName() const;
	
	void SetItemDisplayName(const FText& InItemDisplayName);

	void SetGeneratedNameAdditions(const FObsidianItemGeneratedNameData& InNameData);
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Item")
	FString GetRareItemDisplayNameAddition() const;
	
	void SetRareItemDisplayNameAddition(const FString& InItemNameAddition);

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Item")
	FString GetMagicAffixMultiplierItemDisplayNameAddition() const;
	
	void SetMagicAffixMultiplierItemDisplayNameAddition(const FString& InItemNameAddition);

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Item")
	FText GetItemDescription() const;
	
	void SetItemDescription(const FText& InItemDescription);

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Item")
	FText GetItemAdditionalDescription() const;
	
	void SetItemAdditionalDescription(const FText& InItemAdditionalDescription);

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Item")
	float GetItemSlotPadding() const;
	
	void SetItemSlotPadding(const float InItemSlotPadding);
	
	/**
	 * Debug.
	 */

	/** Gets the debug name of the item, will be valid only on the server. */	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Item")
	FString GetItemDebugName() const;

	void SetItemDebugName(const FString& InItemDebugName);

private:
	/**
	 * Item.
	 */

	UPROPERTY(Replicated)
	FGuid ItemUniqueID;

	UPROPERTY(Replicated)
	int32 ItemLevel = INDEX_NONE;
	
	UPROPERTY(Replicated)
	TSubclassOf<UObsidianInventoryItemDefinition> ItemDef;

	UPROPERTY(Replicated)
	FGameplayTag ItemCategory = FGameplayTag::EmptyTag;

	UPROPERTY(Replicated)
	FGameplayTag ItemBaseType = FGameplayTag::EmptyTag;

	UPROPERTY(Replicated)
	EObsidianItemRarity ItemRarity = EObsidianItemRarity::None;

	/** Current Item Location on the Grid or in the Slot, will be valid only if the item is placed somewhere. */
	UPROPERTY(Replicated)
	FObsidianItemPosition ItemCurrentPosition = FObsidianItemPosition();

	/**
	 * Usability
	 */

	UPROPERTY(Replicated)
	bool bUsable = false;
	
	UPROPERTY(Replicated)
	EObsidianUsableItemType UsableItemType = EObsidianUsableItemType::UIT_None;

	UPROPERTY(Replicated)
	TObjectPtr<UObsidianUsableShard> UsableShard = nullptr;

	/**
	 * Equipping.
	 */

	UPROPERTY(Replicated)
	bool bEquippable = false;

	/** Whether or not the item needs two slots to be equipped, e.g. Two-Handed weapons. */
	UPROPERTY(Replicated)
	bool bNeedsTwoSlots = false;

	UPROPERTY(Replicated)
	TArray<FObsidianEquipmentActor> ActorsToSpawn;

	UPROPERTY(Replicated)
	TArray<TObjectPtr<AObsidianSpawnedEquipmentPiece>> SpawnedActors;

	UPROPERTY(Replicated)
	FObsidianItemRequirements EquippingRequirements = FObsidianItemRequirements();
	
	/**
	 * Affixes.
	 */

	UPROPERTY(Replicated)
	bool bStartsIdentified = false;

	UPROPERTY(Replicated)
	bool bIdentified = false;
	
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
	FIntPoint ItemGridSpan = FIntPoint::NoneValue;
	
	UPROPERTY(Replicated)
	TObjectPtr<UTexture2D> ItemImage = nullptr;
	
	UPROPERTY(Replicated)
	TObjectPtr<UStaticMesh> ItemDroppedMesh = nullptr;
	
	UPROPERTY(Replicated)
	FText ItemDisplayName = FText::GetEmpty();

	//TODO(intrxx) This item addition is only generated at drop as of now, if the item will be upgraded to Rare with some gameplay mechanic this will need to be generated too.
	UPROPERTY(Replicated)
	FObsidianItemGeneratedNameData ItemNameAdditionsData = FObsidianItemGeneratedNameData();

	UPROPERTY(Replicated)
	FText ItemDescription = FText::GetEmpty();
	
	UPROPERTY(Replicated)
	FText ItemAdditionalDescription = FText::GetEmpty();

	UPROPERTY(Replicated)
	float ItemSlotPadding = 4.0f;

	/**
	 * Debug.
	 */
	
	FString DebugName = FString("Not Set");
};
