// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "ObsidianEquipmentList.h"
#include "Components/ActorComponent.h"
#include "ObsidianTypes/ObsidianItemTypes.h"
#include "ObsidianEquipmentComponent.generated.h"

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

	UObsidianInventoryItemInstance* GetEquippedInstanceAtSlot(const FGameplayTag& SlotTag);

	TArray<UObsidianInventoryItemInstance*> GetAllEquippedItems() const;

	void AutomaticallyEquipItem(UObsidianInventoryItemInstance* InstanceToEquip);
	
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
