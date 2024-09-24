// Copyright 2024 out of sCope team - Michał Ogiński

#include "Characters/Enemies/Specified/ObsidianBoss_TreeOrc.h"
#include "AI/AObsidianAIControllerBase.h"
#include "CharacterComponents/ObsidianBossComponent.h"

AObsidianBoss_TreeOrc::AObsidianBoss_TreeOrc(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RightHandEquipmentMesh->SetupAttachment(GetMesh(), ObsidianMeshSocketNames::BackWeaponSocket);
}

void AObsidianBoss_TreeOrc::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if(HasAuthority())
	{
		OnThresholdReached_50DelegateHandle = BossComponent->OnBossThresholdReached_50Delegate.AddUObject(this, &ThisClass::HandleThreshold_50);
	}
}

void AObsidianBoss_TreeOrc::EquipWeapon()
{
	if(OnThresholdReached_50DelegateHandle.IsValid())
	{
		OnThresholdReached_50DelegateHandle.Reset();
	}

	RightHandEquipmentMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		ObsidianMeshSocketNames::RightHandWeaponSocket);
}

void AObsidianBoss_TreeOrc::HandleThreshold_50()
{
	if(ObsidianBossAIController)
	{
		ObsidianBossAIController->RunBehaviorTree(ArmedBehaviorTree);
	}
}
