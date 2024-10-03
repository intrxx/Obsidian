// Copyright 2024 out of sCope team - Michał Ogiński

#include "Characters/ObsidianCharacterBase.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "CharacterComponents/ObsidianPawnExtensionComponent.h"
#include "MotionWarpingComponent.h"
#include "CharacterComponents/ObsidianCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"

AObsidianCharacterBase::AObsidianCharacterBase(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UObsidianCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	USkeletalMeshComponent* MeshComp = GetMesh();
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	MeshComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	MeshComp->SetCollisionResponseToChannel(Obsidian_ObjectChannel_Projectile, ECR_Overlap);
	MeshComp->SetGenerateOverlapEvents(true);
	
	// This seems like a hack now - this fixes incorrect spawning transforms for projectiles which is taken from sockets. //TODO Validate later
	MeshComp->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickMontagesAndRefreshBonesWhenPlayingMontages;

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	CapsuleComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	CapsuleComp->SetCollisionResponseToChannel(Obsidian_ObjectChannel_Projectile, ECR_Overlap);
	CapsuleComp->SetGenerateOverlapEvents(false);

	RightHandEquipmentMesh = CreateDefaultSubobject<USkeletalMeshComponent>("RightHandEquipmentMesh");
	RightHandEquipmentMesh->SetupAttachment(MeshComp, ObsidianMeshSocketNames::RightHandWeaponSocket);
	RightHandEquipmentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandEquipmentMesh->SetIsReplicated(true);
	
	LeftHandEquipmentMesh = CreateDefaultSubobject<USkeletalMeshComponent>("LeftHandEquipmentMesh");
	LeftHandEquipmentMesh->SetupAttachment(MeshComp, ObsidianMeshSocketNames::LeftHandWeaponSocket);
	LeftHandEquipmentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandEquipmentMesh->SetIsReplicated(true);

	MotionWarpingComp = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
	
	PawnExtComp = CreateDefaultSubobject<UObsidianPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	PawnExtComp->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	PawnExtComp->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));

	//** These should be the default names for sockets on any Mesh that wish to call Combat Interface's functions for ability socket locations. */
	WeaponSocketName = ObsidianAbilitySocketNames::Weapon;
	LeftHandSocketName = ObsidianAbilitySocketNames::LeftHand;
	RightHandSocketName = ObsidianAbilitySocketNames::RightHand;
}

UCapsuleComponent* AObsidianCharacterBase::GetCapsuleComp() const
{
	if(UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
	{
		return CapsuleComp;
	}
	return nullptr; 
}

UObsidianAbilitySystemComponent* AObsidianCharacterBase::GetObsidianAbilitySystemComponent() const
{
	return Cast<UObsidianAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* AObsidianCharacterBase::GetAbilitySystemComponent() const
{
	if(PawnExtComp == nullptr)
	{
		return nullptr;
	}
	return  PawnExtComp->GetObsidianAbilitySystemComponent();
}

void AObsidianCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AObsidianCharacterBase, LeftHandEquipmentMesh);
	DOREPLIFETIME(AObsidianCharacterBase, RightHandEquipmentMesh);
}

void AObsidianCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AObsidianCharacterBase::OnAbilitySystemInitialized()
{
}

void AObsidianCharacterBase::OnAbilitySystemUninitialized()
{
}

void AObsidianCharacterBase::OnDeathStarted(AActor* OwningActor)
{
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	UCharacterMovementComponent* CharacterMoveComp = GetCharacterMovement();
	check(CharacterMoveComp);
	CharacterMoveComp->StopMovementImmediately();
	CharacterMoveComp->DisableMovement();

	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
	check(AbilitySystemComponent);
	AbilitySystemComponent->CancelAbilities();
}

void AObsidianCharacterBase::OnDeathFinished(AActor* OwningActor)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
}

void AObsidianCharacterBase::DestroyDueToDeath()
{
	BP_OnDeathFinished();
	UninitAndDestroy();
}

void AObsidianCharacterBase::UninitAndDestroy()
{
	if(GetLocalRole() == ROLE_Authority)
	{
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}

	if(UObsidianAbilitySystemComponent* ObsidianASC = GetObsidianAbilitySystemComponent())
	{
		if(ObsidianASC->GetAvatarActor() == this)
		{
			PawnExtComp->UninitializeAbilitySystem();
		}
	}

	SetActorHiddenInGame(true);
}

FVector AObsidianCharacterBase::GetAbilitySocketLocationForTag_Implementation(const FGameplayTag& Tag)
{
	if(Tag == ObsidianGameplayTags::GameplayEvent_AbilityMontage_Socket_RightHandWeapon)
	{
		return GetAbilitySocketLocationFromRHWeapon_Implementation();
	}
	if(Tag == ObsidianGameplayTags::GameplayEvent_AbilityMontage_Socket_LeftHandWeapon)
	{
		return GetAbilitySocketLocationFromLHWeapon_Implementation();
	}
	if(Tag == ObsidianGameplayTags::GameplayEvent_AbilityMontage_Socket_BetweenHands)
	{
		return GetAbilityBetweenHandsSocketLocation_Implementation();
	}
	if(Tag == ObsidianGameplayTags::GameplayEvent_AbilityMontage_Socket_LeftHand)
	{
		return GetAbilitySocketLocationFromLeftHand_Implementation();
	}
	if(Tag == ObsidianGameplayTags::GameplayEvent_AbilityMontage_Socket_RightHand)
	{
		return GetAbilitySocketLocationFromRightHand_Implementation();
	}
	
	return FVector::ZeroVector;
}

FVector AObsidianCharacterBase::GetAbilitySocketLocationFromLHWeapon_Implementation()
{
	if(LeftHandEquipmentMesh)
	{
		return LeftHandEquipmentMesh->GetSocketLocation(WeaponSocketName);
	}
	return FVector::ZeroVector;
}

FVector AObsidianCharacterBase::GetAbilitySocketLocationFromRHWeapon_Implementation()
{
	if(RightHandEquipmentMesh)
	{
		return RightHandEquipmentMesh->GetSocketLocation(WeaponSocketName);
	}
	return FVector::ZeroVector;
}

FVector AObsidianCharacterBase::GetAbilitySocketLocationFromLeftHand_Implementation()
{
	if(const USkeletalMeshComponent* MeshComp = GetMesh())
	{
		return MeshComp->GetSocketLocation(LeftHandSocketName);
	}
	return FVector::ZeroVector;
}

FVector AObsidianCharacterBase::GetAbilitySocketLocationFromRightHand_Implementation()
{
	if(const USkeletalMeshComponent* MeshComp = GetMesh())
	{
		return MeshComp->GetSocketLocation(RightHandSocketName);
	}
	return FVector::ZeroVector;
}

FVector AObsidianCharacterBase::GetAbilityBetweenHandsSocketLocation_Implementation()
{
	if(const USkeletalMeshComponent* MeshComp = GetMesh())
	{
		const FVector RightHandSocketLocation = MeshComp->GetSocketLocation(RightHandSocketName);
		const FVector LeftHandSocketLocation = MeshComp->GetSocketLocation(LeftHandSocketName);

		const FVector MiddleVector = ((RightHandSocketLocation + LeftHandSocketLocation) / 2.0f);
		
		return MiddleVector;
	}
	return FVector::ZeroVector;
}

void AObsidianCharacterBase::SetMotionWarpingFacingTarget_Implementation(const FName MotionWarpName, const FVector& FacingTarget)
{
	if(MotionWarpingComp)
	{
		MotionWarpingComp->AddOrUpdateWarpTargetFromLocation(MotionWarpName, FacingTarget);
	}
}



