// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "ObsidianEquipmentList.h"
#include "Components/ActorComponent.h"
#include "ObsidianTypes/ObsidianItemTypes.h"
#include "ObsidianEquipmentComponent.generated.h"

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

	void EquipItem(UObsidianInventoryItemInstance* InstanceToEquip);
	
	void EquipItemToSpecificSlot(UObsidianInventoryItemInstance* InstanceToEquip, const FGameplayTag& SlotTag);

	EObsidianEquipResult CanEquipInstance(UObsidianInventoryItemInstance* Instance, const FGameplayTag& SlotTag);

	UObsidianInventoryItemInstance* EquipItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef);
	
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
