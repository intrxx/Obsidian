// Copyright 2024 out of sCope team - Michał Ogiński


#include "Characters/Enemies/Specified/ObsidianBoss_TreeOrc.h"

AObsidianBoss_TreeOrc::AObsidianBoss_TreeOrc(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BackWeaponSkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BackWeaponSkeletalMeshComp"));
	BackWeaponSkeletalMeshComp->SetupAttachment(GetMesh(), ObsidianMeshSocketNames::BackWeaponSocket);
	BackWeaponSkeletalMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
