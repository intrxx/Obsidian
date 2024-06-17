// Copyright 2024 out of sCope team - Michał Ogiński

#include "Characters/ObsidianCharacterBase.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "CharacterComponents/ObsidianPawnExtensionComponent.h"
#include "MotionWarpingComponent.h"
#include "CharacterComponents/ObsidianCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AObsidianCharacterBase::AObsidianCharacterBase(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UObsidianCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	USkeletalMeshComponent* MeshComp = GetMesh();
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	MeshComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	MeshComp->SetGenerateOverlapEvents(true);

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	CapsuleComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	CapsuleComp->SetGenerateOverlapEvents(false);

	RightHandEquipmentMesh = CreateDefaultSubobject<USkeletalMeshComponent>("RightHandEquipmentMesh");
	RightHandEquipmentMesh->SetupAttachment(GetMesh(), FName("EquipmentRightHandSocket"));
	RightHandEquipmentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	LeftHandEquipmentMesh = CreateDefaultSubobject<USkeletalMeshComponent>("LeftHandEquipmentMesh");
	LeftHandEquipmentMesh->SetupAttachment(GetMesh(), FName("EquipmentLeftHandSocket"));
	LeftHandEquipmentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MotionWarpingComp = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
	
	PawnExtComp = CreateDefaultSubobject<UObsidianPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	PawnExtComp->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	PawnExtComp->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));
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

void AObsidianCharacterBase::Ragdoll() const
{
	USkeletalMeshComponent* CharacterMesh = GetMesh();
	check(CharacterMesh);

	CharacterMesh->SetCollisionProfileName(FName("Ragdoll"), true);
	CharacterMesh->SetAllBodiesBelowSimulatePhysics(RagdollImpulseBone, true, true);

	if(bRagdollWithImpulse)
	{
		const UCharacterMovementComponent* CharacterMoveComp = GetCharacterMovement();
		check(CharacterMoveComp);

		const FVector LastUpdateVelocity = CharacterMoveComp->GetLastUpdateVelocity();
		const FVector Impulse = LastUpdateVelocity.GetSafeNormal() * RagdollImpulseStrength + LastUpdateVelocity;
		CharacterMesh->AddImpulse(Impulse, RagdollImpulseBone, true);
	}
}

FVector AObsidianCharacterBase::GetAbilitySocketLocationFromLHWeapon()
{
	if(LeftHandEquipmentMesh)
	{
		return LeftHandEquipmentMesh->GetSocketLocation(WeaponSocketName);
	}
	return FVector::ZeroVector;
}

FVector AObsidianCharacterBase::GetAbilitySocketLocationFromRHWeapon()
{
	if(RightHandEquipmentMesh)
	{
		return RightHandEquipmentMesh->GetSocketLocation(WeaponSocketName);
	}
	return FVector::ZeroVector;
}

FVector AObsidianCharacterBase::GetAbilitySocketLocationFromLeftHand()
{
	if(const USkeletalMeshComponent* MeshComp = GetMesh())
	{
		return MeshComp->GetSocketLocation(LeftHandSocketName);
	}
	return FVector::ZeroVector;
}

FVector AObsidianCharacterBase::GetAbilitySocketLocationFromRightHand()
{
	if(const USkeletalMeshComponent* MeshComp = GetMesh())
	{
		return MeshComp->GetSocketLocation(RightHandSocketName);
	}
	return FVector::ZeroVector;
}

FVector AObsidianCharacterBase::GetAbilityDefaultLocation()
{
	if(const USkeletalMeshComponent* MeshComp = GetMesh())
	{
		FVector SocketLocation = MeshComp->GetSocketLocation(DefaultSocketName);
		SocketLocation = SocketLocation + (GetActorForwardVector() * DefaultAbilitySocketLocationOffset);

		return SocketLocation;
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



