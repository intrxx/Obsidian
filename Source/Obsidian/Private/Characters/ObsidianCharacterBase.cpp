// Copyright 2024 out of sCope team - Michał Ogiński

#include "Characters/ObsidianCharacterBase.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "CharacterComponents/ObsidianPawnExtensionComponent.h"
#include "Components/CapsuleComponent.h"

AObsidianCharacterBase::AObsidianCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	MeshComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	CapsuleComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	RightHandEquipmentMesh = CreateDefaultSubobject<USkeletalMeshComponent>("RightHandEquipmentMesh");
	RightHandEquipmentMesh->SetupAttachment(GetMesh(), FName("EquipmentRightHandSocket"));
	RightHandEquipmentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	LeftHandEquipmentMesh = CreateDefaultSubobject<USkeletalMeshComponent>("LeftHandEquipmentMesh");
	LeftHandEquipmentMesh->SetupAttachment(GetMesh(), FName("EquipmentLeftHandSocket"));
	LeftHandEquipmentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
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



