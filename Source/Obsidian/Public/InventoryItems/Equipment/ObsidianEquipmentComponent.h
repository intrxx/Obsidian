// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "InventoryItems/ObsidianItemContainerComponent.h"
#include "ObsidianEquipmentList.h"
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "ObsidianEquipmentComponent.generated.h"

struct FObsidianSavedItem;

class AObsidianPlayerState;
class AObsidianPlayerController;
class UObsidianInventoryComponent;

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
class OBSIDIAN_API UObsidianEquipmentComponent : public UObsidianItemContainerComponent
{
	GENERATED_BODY()

public:	
	UObsidianEquipmentComponent(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void InitSaveData(const bool bInReceivedInitialItems);
	
	bool DidReceiveInitialEquipmentItems() const;


	
	UObsidianInventoryItemInstance* GetEquippedInstanceAtSlot(const FGameplayTag& InSlotTag) const;
	UObsidianInventoryItemInstance* GetEquippedInstanceAtSlot(const FObsidianEquipmentSlotDefinition& InSlot) const;
	TArray<UObsidianInventoryItemInstance*> GetAllEquippedItems() const;
	USkeletalMeshComponent* GetMainEquippedMeshFromSlot(const FGameplayTag& InSlotTag) const;

	/** Finds Equipment Slot if one exists in the Equipment, might return invalid slot when nothing was found, check IsValid for safety. */
	FObsidianEquipmentSlotDefinition FindEquipmentSlotByTag(const FGameplayTag& InSlotTag);

	TArray<FObsidianEquipmentSlotDefinition> FindMatchingSlotsForItemCategory(const FGameplayTag& InItemCategory);
	TArray<FObsidianEquipmentSlotDefinition> FindPossibleSlotsForEquipping_WithInstance(const UObsidianInventoryItemInstance* InForInstance);
	TArray<FObsidianEquipmentSlotDefinition> FindPossibleSlotsForEquipping_WithItemDef(const TSubclassOf<UObsidianInventoryItemDefinition>& InForItemDef,
		const FObsidianItemGeneratedData& InItemGeneratedData);
	
	bool IsItemEquippedAtSlot(const FGameplayTag& InSlotTag);

	bool CanOwnerModifyEquipmentState();
	
	EObsidianEquipCheckResult CanEquipInstance(const UObsidianInventoryItemInstance* InInstance, const FGameplayTag& InSlotTag);
	EObsidianEquipCheckResult CanEquipTemplate(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef, const FGameplayTag& InSlotTag, const FObsidianItemGeneratedData& InItemGeneratedData);
	
	EObsidianEquipCheckResult CanReplaceInstance(const UObsidianInventoryItemInstance* InInstance, const FGameplayTag& InSlotTag);
	EObsidianEquipCheckResult CanReplaceTemplate(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef, const FGameplayTag& InSlotTag, const FObsidianItemGeneratedData& InItemGeneratedData);
	
	FObsidianEquipmentResult AutomaticallyEquipItem(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef, const FObsidianItemGeneratedData& InItemGeneratedData);
	FObsidianEquipmentResult AutomaticallyEquipItem(UObsidianInventoryItemInstance* InInstanceToEquip);
	
	FObsidianEquipmentResult EquipItemToSpecificSlot(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef, const FGameplayTag& InSlotTag, const FObsidianItemGeneratedData& InItemGeneratedData);
	FObsidianEquipmentResult EquipItemToSpecificSlot(UObsidianInventoryItemInstance* InInstanceToEquip, const FGameplayTag& InSlotTag);

	FObsidianEquipmentResult ReplaceItemAtSpecificSlot(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef, const FGameplayTag& InSlotTag, const FObsidianItemGeneratedData& InItemGeneratedData, const FGameplayTag& InEquipSlotTagOverride = FGameplayTag::EmptyTag);
	FObsidianEquipmentResult ReplaceItemAtSpecificSlot(UObsidianInventoryItemInstance* InInstanceToEquip, const FGameplayTag& InSlotTag, const FGameplayTag& InEquipSlotTagOverride = FGameplayTag::EmptyTag);
	
	void WeaponSwap();

	FObsidianEquipmentResult UnequipItem(UObsidianInventoryItemInstance* InInstanceToUnequip);

	void LoadEquippedItem(const FObsidianSavedItem& InEquippedSavedItem);
	
	//~ Start of UObsidianItemContainerComponent interface
	virtual TArray<UObsidianInventoryItemInstance*> GetContainedItems() const override;
	//~ End of UObsidianItemContainerComponent interface

protected:
	virtual void BeginPlay() override;
	
	//~ Start of UObsidianItemContainerComponent interface
	virtual FGameplayTag GetBlockActionsTag() const override;
	virtual void AddItemInstanceToList(UObsidianInventoryItemInstance* InInstance, const FObsidianItemPosition& InToPosition) override;
	virtual UObsidianInventoryItemInstance* AddItemDefinitionToList(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef,
		const FObsidianItemGeneratedData& InItemGeneratedData, const int32 InStackCount, const FObsidianItemPosition& InToPosition) override;
	virtual void RemoveItemInstanceFromList(UObsidianInventoryItemInstance* InInstance) override;
	//~ End of UObsidianItemContainerComponent interface

	EObsidianEquipCheckResult CanPlaceItemAtEquipmentSlot(const FGameplayTag& InSlotTag, const FGameplayTag& InItemCategory);

	/** Shared part of CanReplaceInstance and CanReplaceTemplate, checks the slot itself once the item is known to be equippable. */
	EObsidianEquipCheckResult CanReplaceItemAtEquipmentSlot(const FGameplayTag& InSlotTag, const FGameplayTag& InItemCategory,
		const bool bInItemNeedsTwoSlots);

	/**
	 * Item that needs two slots can't share them, moves the item from the sister slot of provided slot to the Inventory.
	 * Returns false if it could not be done, true otherwise (also when there is nothing in the sister slot).
	 */
	bool MoveSisterSlotItemToInventory(const FGameplayTag& InSlotTag);
	
	/** Checks weather the item can be equipped with other weapon type already equipped in other hand. */
	bool CanEquipWithOtherWeaponType(const FObsidianEquipmentSlotDefinition& InPrimarySlot, const FGameplayTag& InPrimaryWeaponCategory);
	
	void AddBannedEquipmentCategoryToSlot(const FGameplayTag& InSlotTag, const FGameplayTag& InItemCategory);
	void AddBannedEquipmentCategoriesToSlot(const FGameplayTag& InSlotTag, const FGameplayTagContainer& InItemCategories);
	
	void RemoveBannedEquipmentCategoryToSlot(const FGameplayTag& InSlotTag, const FGameplayTag& InItemCategoryToRemove);
	void RemoveBannedEquipmentCategoriesToSlot(const FGameplayTag& InSlotTag, const FGameplayTagContainer& InItemCategoriesToRemove);

protected:
	/** THIS IS NOT SUPPORTED WITH NEW DYNAMIC GENERATED ITEM TYPES. */
	UPROPERTY(EditAnywhere, meta=(DeprecatedProperty), Category = "Obsidian|Default")
	TArray<FObsidianDefaultEquipmentTemplate> DefaultEquipmentItems;
	
private:
	void CreateDefaultEquipmentSlots();

	/** Equip default items specified in DefaultEquipmentItems. */
	void EquipDefaultItems();

	TArray<FObsidianEquipmentSlotDefinition> Internal_GetEquipmentSlots() const;

	EObsidianEquipCheckResult IsItemEquippingPossible(const UObsidianInventoryItemInstance* InInstance);
	EObsidianEquipCheckResult IsItemEquippingPossible(const UObsidianInventoryItemDefinition* InDefinition, const FObsidianItemGeneratedData& InItemGeneratedData);
	EObsidianEquipCheckResult CheckItemRequirements(const FObsidianItemRequirements& InItemRequirements) const;
	
private:
#if WITH_GAMEPLAY_DEBUGGER
	friend class FGameplayDebuggerCategory_Equipment;
#endif
	
	/** Actual array of equipped items, also hold Map for Slot at which item instance is equipped. */
	UPROPERTY(Replicated)
	FObsidianEquipmentList EquipmentList;



	UPROPERTY(Replicated)
	bool bReceivedInitialEquipmentItems = false;
	FDelegateHandle AddDefaultItemsDelegateHandle;
};

#if !UE_BUILD_SHIPPING

namespace ObsidianEquipmentDebugHelpers
{
	const inline TMap<EObsidianEquipCheckResult, FString> EquipResultToStringMap =
	{
		{EObsidianEquipCheckResult::None, TEXT("None")},
		{EObsidianEquipCheckResult::CannotEquipToSlot, TEXT("Cannot Equip to Slot - Either Banned or not Accepted.")},
		{EObsidianEquipCheckResult::ItemUnequippable, TEXT("Item Unequippable")},
		{EObsidianEquipCheckResult::EquipmentActionsBlocked, TEXT("Equipment Actions Blocked")},
		{EObsidianEquipCheckResult::UnableToEquip_NoSufficientInventorySpace, TEXT("Unable To Equip - No Sufficient Inventory Space")},
		{EObsidianEquipCheckResult::UnableToEquip_DoesNotFitWithOtherWeaponType, TEXT("Unable To Equip - Does Not Fit With Other Weapon Type")},
		{EObsidianEquipCheckResult::ItemUnientified, TEXT("Item Unientified")},
		{EObsidianEquipCheckResult::WrongHeroClass, TEXT("Wrong Hero Class")},
		{EObsidianEquipCheckResult::HeroLevelTooLow, TEXT("Hero Level Too Low")},
		{EObsidianEquipCheckResult::NotEnoughDexterity, TEXT("Not Enough Dexterity")},
		{EObsidianEquipCheckResult::NotEnoughIntelligence, TEXT("Not Enough Intelligence")},
		{EObsidianEquipCheckResult::NotEnoughStrength, TEXT("Not Enough Strength")},
		{EObsidianEquipCheckResult::NotEnoughFaith, TEXT("Not Enough Faith")},
		{EObsidianEquipCheckResult::CanEquip, TEXT("Can Equip")}
	};

	inline FString GetEquipResultString(const EObsidianEquipCheckResult InResult)
	{
		return EquipResultToStringMap[InResult];
	}
}

#endif
