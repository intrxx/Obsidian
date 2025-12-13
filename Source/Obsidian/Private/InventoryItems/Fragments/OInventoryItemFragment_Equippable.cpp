// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Fragments/OInventoryItemFragment_Equippable.h"

#include "InventoryItems/Equipment/ObsidianSpawnedEquipmentPiece.h"
#include "Core/ObsidianGameplayStatics.h"
#include "Game/Save/ObsidianHeroSaveGame.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"

//
// Equipment Actor
//

// ~ Start of FObsidianEquipmentActor
FObsidianEquipmentActor::FObsidianEquipmentActor(const FObsidianSavedEquipmentPiece& SavedEquipmentActor)
	: ActorToSpawn(SavedEquipmentActor.SoftActorToSpawn.LoadSynchronous())
	, bOverrideAttachSocket(SavedEquipmentActor.bOverrideAttachSocket)
	, AttachSocket(SavedEquipmentActor.AttachSocketName)
	, AttachTransform(SavedEquipmentActor.AttachTransform)
{
}
// ~ End of FObsidianEquipmentActor

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

