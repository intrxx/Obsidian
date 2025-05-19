// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Fragments/OInventoryItemFragment_Equippable.h"
#include "Core/ObsidianGameplayStatics.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"

//
// Equipment Actor
//

void FObsidianEquipmentActor::OverrideAttachSocket(const FGameplayTag& SlotTag)
{
	if(ObsidianInventoryItemsStatics::SlotToAttachSocketMap.Contains(SlotTag))
	{
		AttachSocket = ObsidianInventoryItemsStatics::SlotToAttachSocketMap[SlotTag];
	}
}

//
// Inventory Item Fragment - Equippable
//

void UOInventoryItemFragment_Equippable::OnInstancedCreated(UObsidianInventoryItemInstance* Instance) const
{
	Instance->SetEquipmentActors(ActorsToSpawn);
	Instance->SetAbilitySets(AbilitySetsToGrant);
	Instance->SetEquippable(true);
}
