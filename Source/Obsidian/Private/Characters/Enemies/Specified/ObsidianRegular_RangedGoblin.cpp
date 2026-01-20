// Copyright 2026 out of sCope team - intrxx

#include "Characters/Enemies/Specified/ObsidianRegular_RangedGoblin.h"

// ~ Core
#include "Net/UnrealNetwork.h"

// ~ Project


AObsidianRegular_RangedGoblin::AObsidianRegular_RangedGoblin(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	LeftHandEquipmentMesh = CreateDefaultSubobject<USkeletalMeshComponent>("LeftHandEquipmentMesh");
	LeftHandEquipmentMesh->SetupAttachment(GetMesh(), ObsidianMeshSocketNames::LeftHandWeaponSocket);
	LeftHandEquipmentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandEquipmentMesh->SetIsReplicated(true);
}

void AObsidianRegular_RangedGoblin::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, LeftHandEquipmentMesh);
}

FVector AObsidianRegular_RangedGoblin::GetAbilitySocketLocationFromLHWeapon_Implementation()
{
	if(LeftHandEquipmentMesh)
	{
		return LeftHandEquipmentMesh->GetSocketLocation(WeaponSocketName);
	}
	return FVector::ZeroVector;
}
