// Copyright 2024 Michał Ogiński

#include "Characters/Heroes/ObsidianHero.h"
#include "Camera/CameraComponent.h"
#include "CharacterComponents/ObsidianHeroComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

AObsidianHero::AObsidianHero()
{
	HeroComponent = CreateDefaultSubobject<UObsidianHeroComponent>(TEXT("HeroComponent"));
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->SetRelativeRotation(FRotator(-52.5f, 0.0f, 0.0f));
	SpringArmComponent->TargetArmLength = 920.f;
	SpringArmComponent->bInheritPitch = false;
	SpringArmComponent->bInheritPitch = false;
	SpringArmComponent->bInheritYaw = false;

	//TODO Need to decide if i want to use it later on
	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->CameraLagSpeed = 15.f;
	SpringArmComponent->CameraLagMaxDistance = 20.f;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
}

void AObsidianHero::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	HeroComponent->InitializePlayerInput(PlayerInputComponent);
}
