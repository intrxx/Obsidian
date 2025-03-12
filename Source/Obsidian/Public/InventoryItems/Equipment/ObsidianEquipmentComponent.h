// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "ObsidianEquipmentList.h"
#include "Components/ActorComponent.h"
#include "ObsidianTypes/ObsidianItemTypes.h"
#include "ObsidianEquipmentComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogEquipment, Log, All);

USTRUCT(BlueprintType)
struct FObsidianEquipmentSlotDefinition
{
	GENERATED_BODY()

public:
	FObsidianEquipmentSlotDefinition(){};
	FObsidianEquipmentSlotDefinition(const FGameplayTag& InSlotTag, const FGameplayTagContainer& InAcceptedEquipmentCategories)
		: SlotTag(InSlotTag)
		, AcceptedEquipmentCategories(InAcceptedEquipmentCategories)
	{};

	bool IsValid() const;
	
	bool CanEquipToSlot(const FGameplayTag& EquipmentCategory) const;

	void AddBannedEquipmentCategory(const FGameplayTag& InBannedCategory);
	void AddBannedEquipmentCategories(const FGameplayTagContainer& InBannedCategories);
	
	void RemoveBannedEquipmentCategory(const FGameplayTag& BannedCategoryToRemove);
	void RemoveBannedEquipmentCategories(const FGameplayTagContainer& BannedCategoriesToRemove);
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	FGameplayTag SlotTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	FGameplayTagContainer AcceptedEquipmentCategories = FGameplayTagContainer::EmptyContainer;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	FGameplayTagContainer BannedEquipmentCategories = FGameplayTagContainer::EmptyContainer;
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

	UObsidianInventoryItemInstance* GetEquippedInstanceAtSlot(const FGameplayTag& SlotTag);

	TArray<UObsidianInventoryItemInstance*> GetAllEquippedItems() const;

	/** Finds Equipment Slot if one exists in the Equipment, might return invalid slot when nothing was found, check IsValid for safety. */
	FObsidianEquipmentSlotDefinition FindEquipmentSlotByTag(const FGameplayTag& SlotTag);

	TArray<FObsidianEquipmentSlotDefinition> FindMatchingEquipmentSlotsByItemCategory(const FGameplayTag& ItemCategory);

	bool AutomaticallyEquipItem(UObsidianInventoryItemInstance* InstanceToEquip);
	
	bool EquipItemToSpecificSlot(UObsidianInventoryItemInstance* InstanceToEquip, const FGameplayTag& SlotTag);

	EObsidianEquipResult CanEquipInstance(UObsidianInventoryItemInstance* Instance, const FGameplayTag& SlotTag);

	UObsidianInventoryItemInstance* AutomaticallyEquipItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef);
	
	UObsidianInventoryItemInstance* EquipItemToSpecificSlot(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FGameplayTag& SlotTag);

	EObsidianEquipResult CanEquipTemplate(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FGameplayTag& SlotTag);

	void UnequipItem(UObsidianInventoryItemInstance* InstanceToUnequip);

	//~ Start of UObject interface
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	//~ End of UObject interface

protected:
	bool DoesItemFitEquipmentSlot(const FGameplayTag& SlotTag, const FGameplayTag& ItemCategory);
	
	void AddBannedEquipmentCategoryToSlot(const FGameplayTag& SlotTag, const FGameplayTag& InItemCategory);
	void AddBannedEquipmentCategoriesToSlot(const FGameplayTag& SlotTag, const FGameplayTagContainer& InItemCategories);
	
	void RemoveBannedEquipmentCategoryToSlot(const FGameplayTag& SlotTag, const FGameplayTag& ItemCategoryToRemove);
	void RemoveBannedEquipmentCategoriesToSlot(const FGameplayTag& SlotTag, const FGameplayTagContainer& ItemCategoriesToRemove);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TArray<FObsidianEquipmentSlotDefinition> EquipmentSlots;

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
