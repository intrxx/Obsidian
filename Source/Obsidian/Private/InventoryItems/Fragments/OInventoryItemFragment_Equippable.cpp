// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Fragments/OInventoryItemFragment_Equippable.h"

// ~ Core

// ~ Project
#include "Core/ObsidianGameplayStatics.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianItemsFunctionLibrary.h"

//
// Equipment Actor
//

void FObsidianEquipmentActor::OverrideAttachSocket(const FGameplayTag& SlotTag)
{
	if(ObsidianGameplayTags::GetSlotToAttachSocketMap().Contains(SlotTag))
	{
		AttachSocket = ObsidianGameplayTags::GetSlotToAttachSocketMap()[SlotTag];
	}
}

//
// Inventory Item Fragment - Equippable
//

void UOInventoryItemFragment_Equippable::OnInstancedCreated(UObsidianInventoryItemInstance* Instance) const
{
	Instance->SetEquipmentActors(ActorsToSpawn);
	Instance->SetEquippable(true);
}

FObsidianItemRequirements UOInventoryItemFragment_Equippable::GetItemDefaultEquippingRequirements() const
{
	return DefaultEquippingRequirements;
}

