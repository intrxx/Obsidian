// Copyright 2024 out of sCope team - Michał Ogiński


#include "Characters/Enemies/Specified/ObsidianBoss_TreeOrc.h"

AObsidianBoss_TreeOrc::AObsidianBoss_TreeOrc(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RightHandEquipmentMesh->SetupAttachment(GetMesh(), ObsidianMeshSocketNames::BackWeaponSocket);
}
