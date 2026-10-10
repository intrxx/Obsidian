// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/Fragments/OInventoryItemFragment_Equippable.h"

#include "Core/ObsidianGameplayStatics.h"
#include "Game/Save/ObsidianHeroSaveGame.h"
#include "InventoryItems/Equipment/ObsidianSpawnedEquipmentPiece.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"


//
// Equipment Actor
//

// ~ Start of FObsidianEquipmentActor
FObsidianEquipmentActor::FObsidianEquipmentActor(const FObsidianSavedEquipmentPiece& InSavedEquipmentActor)
	: ActorToSpawn(InSavedEquipmentActor.SoftActorToSpawn.LoadSynchronous())
	, bOverrideAttachSocket(InSavedEquipmentActor.bOverrideAttachSocket)
	, AttachSocket(InSavedEquipmentActor.AttachSocketName)
	, AttachTransform(InSavedEquipmentActor.AttachTransform)
{
}
// ~ End of FObsidianEquipmentActor

void FObsidianEquipmentActor::OverrideAttachSocket(const FGameplayTag& InSlotTag)
{
	if(ObsidianGameplayTags::GetSlotToAttachSocketMap().Contains(InSlotTag))
	{
		AttachSocket = ObsidianGameplayTags::GetSlotToAttachSocketMap()[InSlotTag];
	}
}

//
// Inventory Item Fragment - Equippable
//

void UOInventoryItemFragment_Equippable::OnInstancedCreated(UObsidianInventoryItemInstance* InInstance) const
{
	InInstance->SetEquipmentActors(ActorsToSpawn);
	InInstance->SetEquippable(true);
}

FObsidianItemRequirements UOInventoryItemFragment_Equippable::GetItemDefaultEquippingRequirements() const
{
	return DefaultEquippingRequirements;
}

