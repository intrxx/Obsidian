// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "ObsidianEquipmentList.h"
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "Components/ActorComponent.h"
#include "ObsidianEquipmentComponent.generated.h"

class AObsidianPlayerController;
class UObsidianInventoryComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogEquipment, Log, All);

/**
 * 
 */
USTRUCT(BlueprintType)
struct FObsidianDefaultEquipmentTemplate
{
	GENERATED_BODY()

public:
	/** Item Definition to equip. */
	UPROPERTY(EditAnywhere)
	TSubclassOf<UObsidianInventoryItemDefinition> DefaultItemDef = nullptr;

	/** Stack Count of the item to equip. For Obsidian, it should always be 1 but exposing it for future sake. */
	UPROPERTY(EditAnywhere)
	int32 StackCount = 1;

	/** Tag of the slot to equip this default equipment to. If left empty will abort equipping. */
	UPROPERTY(EditAnywhere, meta=(Categories = "Item.Slot.Equipment"))
	FGameplayTag EquipmentSlotTag = FGameplayTag::EmptyTag;
};

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

	UObsidianInventoryItemInstance* GetEquippedInstanceAtSlot(const FGameplayTag& SlotTag) const;
	UObsidianInventoryItemInstance* GetEquippedInstanceAtSlot(const FObsidianEquipmentSlotDefinition& Slot) const;
	TArray<UObsidianInventoryItemInstance*> GetAllEquippedItems() const;
	USkeletalMeshComponent* GetMainEquippedMeshFromSlot(const FGameplayTag& SlotTag) const;

	/** Finds Equipment Slot if one exists in the Equipment, might return invalid slot when nothing was found, check IsValid for safety. */
	FObsidianEquipmentSlotDefinition FindEquipmentSlotByTag(const FGameplayTag& SlotTag);

	TArray<FObsidianEquipmentSlotDefinition> FindMatchingSlotsForItemCategory(const FGameplayTag& ItemCategory);
	TArray<FObsidianEquipmentSlotDefinition> FindPossibleSlotsForEquipping_WithInstance(const UObsidianInventoryItemInstance* ForInstance);
	TArray<FObsidianEquipmentSlotDefinition> FindPossibleSlotsForEquipping_WithItemDef(const TSubclassOf<UObsidianInventoryItemDefinition>& ForItemDef);
	
	bool IsItemEquippedAtSlot(const FGameplayTag& SlotTag);

	bool CanOwnerModifyEquipmentState();
	
	EObsidianEquipCheckResult CanEquipInstance(const UObsidianInventoryItemInstance* Instance, const FGameplayTag& SlotTag);
	EObsidianEquipCheckResult CanEquipTemplate(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FGameplayTag& SlotTag);
	
	EObsidianEquipCheckResult CanReplaceInstance(const UObsidianInventoryItemInstance* Instance, const FGameplayTag& SlotTag);
	EObsidianEquipCheckResult CanReplaceTemplate(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FGameplayTag& SlotTag);
	
	FObsidianEquipmentResult AutomaticallyEquipItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FObsidianItemGeneratedData& ItemGeneratedData);
	FObsidianEquipmentResult AutomaticallyEquipItem(UObsidianInventoryItemInstance* InstanceToEquip);
	
	FObsidianEquipmentResult EquipItemToSpecificSlot(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FGameplayTag& SlotTag, const FObsidianItemGeneratedData& ItemGeneratedData);
	FObsidianEquipmentResult EquipItemToSpecificSlot(UObsidianInventoryItemInstance* InstanceToEquip, const FGameplayTag& SlotTag);

	FObsidianEquipmentResult ReplaceItemAtSpecificSlot(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FGameplayTag& SlotTag, const FObsidianItemGeneratedData& ItemGeneratedData, const FGameplayTag& EquipSlotTagOverride = FGameplayTag::EmptyTag);
	FObsidianEquipmentResult ReplaceItemAtSpecificSlot(UObsidianInventoryItemInstance* InstanceToEquip, const FGameplayTag& SlotTag, const FGameplayTag& EquipSlotTagOverride = FGameplayTag::EmptyTag);
	
	void WeaponSwap();

	FObsidianEquipmentResult UnequipItem(UObsidianInventoryItemInstance* InstanceToUnequip);

	//~ Start of UObject interface
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	//~ End of UObject interface

protected:
	virtual void BeginPlay() override;
	
	EObsidianEquipCheckResult CanPlaceItemAtEquipmentSlot(const FGameplayTag& SlotTag, const FGameplayTag& ItemCategory);
	void CheckEquipmentPairAcceptance(const FObsidianEquipmentSlotDefinition& PrimarySlot, const FGameplayTag& PrimaryWeaponCategory, EObsidianEquipCheckResult& OutResult);
	
	void AddBannedEquipmentCategoryToSlot(const FGameplayTag& SlotTag, const FGameplayTag& InItemCategory);
	void AddBannedEquipmentCategoriesToSlot(const FGameplayTag& SlotTag, const FGameplayTagContainer& InItemCategories);
	
	void RemoveBannedEquipmentCategoryToSlot(const FGameplayTag& SlotTag, const FGameplayTag& ItemCategoryToRemove);
	void RemoveBannedEquipmentCategoriesToSlot(const FGameplayTag& SlotTag, const FGameplayTagContainer& ItemCategoriesToRemove);

protected:
	UPROPERTY(EditAnywhere, Category = "Obsidian|Default")
	TArray<FObsidianDefaultEquipmentTemplate> DefaultEquipmentItems;
	
private:
	void CreateDefaultEquipmentSlots();

	/** Equip default items specified in DefaultEquipmentItems. */
	void EquipDefaultItems();

	TArray<FObsidianEquipmentSlotDefinition> Internal_GetEquipmentSlots() const;

private:
#if WITH_GAMEPLAY_DEBUGGER
	friend class FGameplayDebuggerCategory_Equipment;
#endif
	
	/** Actual array of equipped items, also hold Map for Slot at which item instance is equipped. */
	UPROPERTY(Replicated)
	FObsidianEquipmentList EquipmentList;

	UPROPERTY()
	TObjectPtr<AObsidianPlayerController> CachedOwnerPlayerController;
};

#if !UE_BUILD_SHIPPING

namespace ObsidianEquipmentDebugHelpers
{
	const inline TMap<EObsidianEquipCheckResult, FString> EquipResultToStringMap =
	{
		{EObsidianEquipCheckResult::None, TEXT("None")},
		{EObsidianEquipCheckResult::ItemUnfitForCategory, TEXT("Item Unfit For Category")},
		{EObsidianEquipCheckResult::ItemUnequippable, TEXT("Item Unequippable")},
		{EObsidianEquipCheckResult::EquipmentActionsBlocked, TEXT("Equipment Actions Blocked")},
		{EObsidianEquipCheckResult::UnableToEquip_BannedCategory, TEXT("Unable To Equip - Banned Category")},
		{EObsidianEquipCheckResult::UnableToEquip_NoSufficientInventorySpace, TEXT("Unable To Equip - No Sufficient Inventory Space")},
		{EObsidianEquipCheckResult::UnableToEquip_DoesNotFitWithOtherWeaponType, TEXT("Unable To Equip - Does Not Fit With Other Weapon Type")},
		{EObsidianEquipCheckResult::ItemUnientified, TEXT("Item Unientified")},
		{EObsidianEquipCheckResult::NotEnoughHeroLevel, TEXT("Not Enough Hero Level")},
		{EObsidianEquipCheckResult::NotEnoughDexterity, TEXT("Not Enough Dexterity")},
		{EObsidianEquipCheckResult::NotEnoughIntelligence, TEXT("Not Enough Intelligence")},
		{EObsidianEquipCheckResult::NotEnoughStrength, TEXT("Not Enough Strength")},
		{EObsidianEquipCheckResult::NotEnoughFaith, TEXT("Not Enough Faith")},
		{EObsidianEquipCheckResult::CanEquip, TEXT("Can Equip")}
	};

	inline FString GetEquipResultString(const EObsidianEquipCheckResult Result)
	{
		return EquipResultToStringMap[Result];
	}
}

#endif
