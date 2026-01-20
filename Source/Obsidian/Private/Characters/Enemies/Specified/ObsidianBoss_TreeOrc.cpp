// Copyright 2026 out of sCope team - intrxx

#include "Characters/Enemies/Specified/ObsidianBoss_TreeOrc.h"

// ~ Core
#include "Net/UnrealNetwork.h"

// ~ Project
#include "AI/AObsidianAIControllerBase.h"
#include "CharacterComponents/ObsidianAdvancedCombatComponent.h"
#include "CharacterComponents/ObsidianBossComponent.h"


AObsidianBoss_TreeOrc::AObsidianBoss_TreeOrc(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RightHandEquipmentMesh = CreateDefaultSubobject<USkeletalMeshComponent>("RightHandEquipmentMesh");
	RightHandEquipmentMesh->SetupAttachment(GetMesh(), ObsidianMeshSocketNames::BackWeaponSocket);
	RightHandEquipmentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandEquipmentMesh->SetIsReplicated(true);

	AdvancedCombatComponent->AddTracedMesh(RightHandEquipmentMesh, EObsidianTracedMeshType::RightHandWeaponMesh);

	EnemyRarity = EObsidianEntityRarity::SpecialBoss;
}

void AObsidianBoss_TreeOrc::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if(HasAuthority())
	{
		OnThresholdReached_50DelegateHandle = BossComponent->OnBossThresholdReached_50Delegate.AddUObject(this, &ThisClass::HandleThreshold_50);
	}
}

void AObsidianBoss_TreeOrc::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, RightHandEquipmentMesh);
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

FVector AObsidianBoss_TreeOrc::GetAbilitySocketLocationFromRHWeapon_Implementation()
{
	if(RightHandEquipmentMesh)
	{
		return RightHandEquipmentMesh->GetSocketLocation(WeaponSocketName);
	}
	return FVector::ZeroVector;
}

void AObsidianBoss_TreeOrc::HandleThreshold_50()
{
	if(ObsidianBossAIController)
	{
		ObsidianBossAIController->RunBehaviorTree(ArmedBehaviorTree);
	}
}
