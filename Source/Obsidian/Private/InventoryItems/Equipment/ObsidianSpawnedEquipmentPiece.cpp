// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/Equipment/ObsidianSpawnedEquipmentPiece.h"


AObsidianSpawnedEquipmentPiece::AObsidianSpawnedEquipmentPiece(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bReplicates = true;

	EquipmentPieceMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Equipment Piece Mesh"));
	SetRootComponent(EquipmentPieceMesh);
}

USkeletalMeshSocket const* AObsidianSpawnedEquipmentPiece::GetEquipmentSocketByName(const FName InSocketName) const
{
	if(EquipmentPieceMesh)
	{
		return EquipmentPieceMesh->GetSocketByName(InSocketName);
	}
	return nullptr;
}

