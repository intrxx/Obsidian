// Copyright 2024 out of sCope team - Michał Ogiński

#include "Characters/Enemies/Specified/ObsidianBoss_TreeOrc.h"
#include "AbilitySystemComponent.h"
#include "CharacterComponents/ObsidianBossComponent.h"
#include "Obsidian/ObsidianGameplayTags.h"

AObsidianBoss_TreeOrc::AObsidianBoss_TreeOrc(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RightHandEquipmentMesh->SetupAttachment(GetMesh(), ObsidianMeshSocketNames::BackWeaponSocket);
}

void AObsidianBoss_TreeOrc::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	BossComponent->OnBossThresholdReached_50Delegate.AddUObject(this, &ThisClass::HandleThreshold_50);
}

void AObsidianBoss_TreeOrc::EquipWeapon()
{
	RightHandEquipmentMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		ObsidianMeshSocketNames::RightHandWeaponSocket);
}

void AObsidianBoss_TreeOrc::HandleThreshold_50()
{
	if(UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		FGameplayTagContainer ActivationTags;
		ActivationTags.AddTag(ObsidianGameplayTags::AbilityActivation_TreeOrc_Equip);
		ASC->TryActivateAbilitiesByTag(ActivationTags);
	}
}
