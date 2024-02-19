// Copyright 2024 Michał Ogiński

#include "Characters/ObsidianCharacterBase.h"
#include "CharacterComponents/ObsidianPawnExtensionComponent.h"

AObsidianCharacterBase::AObsidianCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	RightHandEquipmentMesh = CreateDefaultSubobject<USkeletalMeshComponent>("RightHandEquipmentMesh");
	RightHandEquipmentMesh->SetupAttachment(GetMesh(), FName("EquipmentRightHandSocket"));
	RightHandEquipmentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	LeftHandEquipmentMesh = CreateDefaultSubobject<USkeletalMeshComponent>("LeftHandEquipmentMesh");
	LeftHandEquipmentMesh->SetupAttachment(GetMesh(), FName("EquipmentLeftHandSocket"));
	LeftHandEquipmentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	PawnExtComp = CreateDefaultSubobject<UObsidianPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
}

void AObsidianCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}


