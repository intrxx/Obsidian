// Copyright 2024 out of sCope team - Michał Ogiński

#include "Characters/Heroes/ObsidianHero.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "AbilitySystem/Data/ObsidianAbilitySet.h"
#include "Camera/CameraComponent.h"
#include "CharacterComponents/ObsidianHeroComponent.h"
#include "CharacterComponents/ObsidianPawnExtensionComponent.h"
#include "CharacterComponents/Attributes/ObsidianHeroAttributesComponent.h"
#include "Characters/ObsidianPawnData.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Characters/Player/ObsidianPlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "UI/ObsidianHUD.h"

AObsidianHero::AObsidianHero()
{
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

	HeroComponent = CreateDefaultSubobject<UObsidianHeroComponent>(TEXT("HeroComponent"));

	HeroAttributesComponent = CreateDefaultSubobject<UObsidianHeroAttributesComponent>(TEXT("HeroAttributesComponent"));

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 600.f, 0.f);
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

void AObsidianHero::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init for the server
	if(AObsidianPlayerState* ObsidianPS = GetObsidianPlayerState())
	{
		check(PawnExtComp);
		PawnExtComp->InitializeAbilitySystem(ObsidianPS->GetObsidianAbilitySystemComponent(), ObsidianPS);
	}
}

void AObsidianHero::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init for the client
	if(AObsidianPlayerState* ObsidianPS = GetObsidianPlayerState())
	{
		check(PawnExtComp);
		PawnExtComp->InitializeAbilitySystem(ObsidianPS->GetObsidianAbilitySystemComponent(), ObsidianPS);
	}
}

AObsidianPlayerState* AObsidianHero::GetObsidianPlayerState() const
{
	return CastChecked<AObsidianPlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}

AObsidianPlayerController* AObsidianHero::GetObsidianPlayerController() const
{
	return CastChecked<AObsidianPlayerController>(GetController(), ECastCheckedType::NullAllowed);
}

int32 AObsidianHero::GetCharacterLevel()
{
	return GetObsidianPlayerState() == nullptr ? GetObsidianPlayerState()->GetHeroLevel() : 1;
}

void AObsidianHero::OnAbilitySystemInitialized()
{
	Super::OnAbilitySystemInitialized();

	UObsidianAbilitySystemComponent* ObsidianASC = GetObsidianAbilitySystemComponent();
	check(ObsidianASC);

	HeroAttributesComponent->InitializeWithAbilitySystem(ObsidianASC);
	
	if(const UObsidianPawnExtensionComponent* PawnExt = UObsidianPawnExtensionComponent::FindPawnExtComponent(this))
	{
		if(const UObsidianPawnData* PawnData = PawnExt->GetPawnData())
		{
			for(const UObsidianAbilitySet* AbilitySet : PawnData->AbilitySets)
			{
				AbilitySet->GiveToAbilitySystem(ObsidianASC, nullptr, this);
			}
		}
	}

	InitializeUI(ObsidianASC);
	ObsidianASC->AbilityActorInfoSet();
}

void AObsidianHero::OnAbilitySystemUninitialized()
{
	Super::OnAbilitySystemUninitialized();

	HeroAttributesComponent->UninitializeFromAbilitySystem();
}

void AObsidianHero::InitializeUI(UObsidianAbilitySystemComponent* ObsidianASC)
{
	AObsidianPlayerState* ObsidianPS = GetObsidianPlayerState();
	AObsidianPlayerController* ObsidianPC = GetObsidianPlayerController();
	if(ObsidianPS && ObsidianPC)
	{
		if(AObsidianHUD* ObsidianHUD = ObsidianPC->GetObsidianHUD())
		{
			ObsidianHUD->InitOverlay(ObsidianPC, ObsidianPS, ObsidianASC, HeroAttributesComponent);
		}
	}
}
