// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "ObsidianEquipmentList.h"
#include "ObsidianTypes/ObsidianItemTypes.h"

#include "Components/ActorComponent.h"
#include "ObsidianEquipmentComponent.generated.h"

class UObsidianInventoryComponent;
DECLARE_LOG_CATEGORY_EXTERN(LogEquipment, Log, All);

/**
 * Component that manages equipping items on the Heroes.
 */
UCLASS( ClassGroup=(InventoryItems), meta=(BlueprintSpawnableComponent) )
class OBSIDIAN_API UObsidianEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UObsidianEquipmentComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UObsidianInventoryComponent* GetInventoryComponentFromOwner() const;

	UObsidianInventoryItemInstance* GetEquippedInstanceAtSlot(const FGameplayTag& SlotTag);
	UObsidianInventoryItemInstance* GetEquippedInstanceAtSlot(const FObsidianEquipmentSlotDefinition& Slot);
	TArray<UObsidianInventoryItemInstance*> GetAllEquippedItems() const;
	UObsidianInventoryItemInstance* GetEquipmentPieceByTag(const FGameplayTag& SlotTag) const;
	USkeletalMeshComponent* GetMainEquippedMeshFromSlot(const FGameplayTag& SlotTag) const;

	/** Finds Equipment Slot if one exists in the Equipment, might return invalid slot when nothing was found, check IsValid for safety. */
	FObsidianEquipmentSlotDefinition FindEquipmentSlotByTag(const FGameplayTag& SlotTag);

	TArray<FObsidianEquipmentSlotDefinition> FindMatchingEquipmentSlotsByItemCategory(const FGameplayTag& ItemCategory);

	FObsidianItemStats GetItemStatsBySlotTag(const FGameplayTag& SlotTag);
	
	EObsidianEquipResult CanEquipInstance(const UObsidianInventoryItemInstance* Instance, const FGameplayTag& SlotTag);
	EObsidianEquipResult CanEquipTemplate(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FGameplayTag& SlotTag);
	
	EObsidianEquipResult CanReplaceInstance(const UObsidianInventoryItemInstance* Instance, const FGameplayTag& SlotTag);
	EObsidianEquipResult CanReplaceTemplate(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FGameplayTag& SlotTag);
	
	UObsidianInventoryItemInstance* AutomaticallyEquipItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef);
	bool AutomaticallyEquipItem(UObsidianInventoryItemInstance* InstanceToEquip);
	
	UObsidianInventoryItemInstance* EquipItemToSpecificSlot(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FGameplayTag& SlotTag);
	bool EquipItemToSpecificSlot(UObsidianInventoryItemInstance* InstanceToEquip, const FGameplayTag& SlotTag);

	bool ReplaceItemAtSpecificSlot(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FGameplayTag& SlotTag, const FGameplayTag& EquipSlotTagOverride = FGameplayTag::EmptyTag);
	bool ReplaceItemAtSpecificSlot(UObsidianInventoryItemInstance* InstanceToEquip, const FGameplayTag& SlotTag, const FGameplayTag& EquipSlotTagOverride = FGameplayTag::EmptyTag);
	
	void WeaponSwap();

	void UnequipItem(UObsidianInventoryItemInstance* InstanceToUnequip);

	//~ Start of UObject interface
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	//~ End of UObject interface

protected:
	EObsidianEquipResult CanPlaceItemAtEquipmentSlot(const FGameplayTag& SlotTag, const FGameplayTag& ItemCategory);
	
	void AddBannedEquipmentCategoryToSlot(const FGameplayTag& SlotTag, const FGameplayTag& InItemCategory);
	void AddBannedEquipmentCategoriesToSlot(const FGameplayTag& SlotTag, const FGameplayTagContainer& InItemCategories);
	
	void RemoveBannedEquipmentCategoryToSlot(const FGameplayTag& SlotTag, const FGameplayTag& ItemCategoryToRemove);
	void RemoveBannedEquipmentCategoriesToSlot(const FGameplayTag& SlotTag, const FGameplayTagContainer& ItemCategoriesToRemove);

private:
	void CreateDefaultEquipmentSlots();

private:
	/** Actual array of equipped items, also hold Map for Slot at which item instance is equipped. */
	UPROPERTY(Replicated)
	FObsidianEquipmentList EquipmentList;
};

#if !UE_BUILD_SHIPPING

namespace ObsidianEquipmentDebugHelpers
{
	const inline TMap<EObsidianEquipResult, FString> EquipResultToStringMap =
	{
		{EObsidianEquipResult::None, TEXT("None")},
		{EObsidianEquipResult::ItemUnfitForCategory, TEXT("Item Unfit For Category")},
		{EObsidianEquipResult::ItemUnequippable, TEXT("Item Unequippable")},
		{EObsidianEquipResult::UnableToEquip_BannedCategory, TEXT("Unable To Equip - Banned Category")},
		{EObsidianEquipResult::UnableToEquip_NoSufficientInventorySpace, TEXT("Unable To Equip - No Sufficient Inventory Space")},
		{EObsidianEquipResult::UnableToEquip_DoesNotFitWithOtherWeaponType, TEXT("Unable To Equip - Does Not Fit With Other Weapon Type")},
		{EObsidianEquipResult::ItemUnientified, TEXT("Item Unientified")},
		{EObsidianEquipResult::NotEnoughHeroLevel, TEXT("Not Enough Hero Level")},
		{EObsidianEquipResult::NotEnoughDexterity, TEXT("Not Enough Dexterity")},
		{EObsidianEquipResult::NotEnoughIntelligence, TEXT("Not Enough Intelligence")},
		{EObsidianEquipResult::NotEnoughStrength, TEXT("Not Enough Strength")},
		{EObsidianEquipResult::NotEnoughFaith, TEXT("Not Enough Faith")},
		{EObsidianEquipResult::CanEquip, TEXT("Can Equip")}
	};

	inline FString GetEquipResultString(const EObsidianEquipResult Result)
	{
		return EquipResultToStringMap[Result];
	}
}

#endif
