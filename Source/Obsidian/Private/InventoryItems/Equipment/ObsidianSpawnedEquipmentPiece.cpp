// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/Equipment/ObsidianSpawnedEquipmentPiece.h"

// ~ Core

// ~ Project

AObsidianSpawnedEquipmentPiece::AObsidianSpawnedEquipmentPiece(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bReplicates = true;

	EquipmentPieceMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Equipment Piece Mesh"));
	SetRootComponent(EquipmentPieceMesh);
}

USkeletalMeshSocket const* AObsidianSpawnedEquipmentPiece::GetEquipmentSocketByName(const FName SocketName) const
{
	if(EquipmentPieceMesh)
	{
		return EquipmentPieceMesh->GetSocketByName(SocketName);
	}
	return nullptr;
}

