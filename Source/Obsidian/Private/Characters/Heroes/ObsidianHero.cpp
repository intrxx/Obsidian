// Copyright 2024 out of sCope team - Michał Ogiński

#include "Characters/Heroes/ObsidianHero.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "AbilitySystem/Data/ObsidianAbilitySet.h"
#include "Camera/CameraComponent.h"
#include "CharacterComponents/ObsidianCharacterMovementComponent.h"
#include "CharacterComponents/ObsidianHeroComponent.h"
#include "CharacterComponents/ObsidianPawnExtensionComponent.h"
#include "CharacterComponents/Attributes/ObsidianHeroAttributesComponent.h"
#include "Characters/ObsidianPawnData.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Characters/Player/ObsidianPlayerState.h"
#include "Components/WidgetComponent.h"
#include "UI/ProgressBars/ObsidianHeroHealthBar_Simple.h"
#include "UI/ProgressBars/ObsidianHeroHealthBar.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"
#include "UI/ObsidianHUD.h"
#include "UI/ObsidianWidgetBase.h"

AObsidianHero::AObsidianHero(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->SetRelativeRotation(FRotator(-43.0f, 0.0f, 0.0f));
	SpringArmComponent->TargetArmLength = 1100.0f;
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
	HeroAttributesComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	HeroAttributesComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathFinished);

	HealthBarWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarComp"));
	HealthBarWidgetComp->SetupAttachment(GetRootComponent());
	HealthBarWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidgetComp->SetDrawAtDesiredSize(true);
	
	UObsidianCharacterMovementComponent* ObsidianMovementComp = CastChecked<UObsidianCharacterMovementComponent>(GetCharacterMovement());
	ObsidianMovementComp->bOrientRotationToMovement = true;
	ObsidianMovementComp->RotationRate = FRotator(0.f, 800.f, 0.f);
	ObsidianMovementComp->bConstrainToPlane = true;
	ObsidianMovementComp->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	
	// Identifies this class as a Player character
	Tags.Emplace(ObsidianActorTags::Player);
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

	InitializeHealthBar();
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

	InitializeHealthBar();
}

AObsidianPlayerState* AObsidianHero::GetObsidianPlayerState() const
{
	return CastChecked<AObsidianPlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}

AObsidianPlayerController* AObsidianHero::GetObsidianPlayerController() const
{
	return CastChecked<AObsidianPlayerController>(GetController(), ECastCheckedType::NullAllowed);
}

UObsidianWidgetBase* AObsidianHero::GetHealthBarWidget() const
{
	if(HealthBarWidgetComp)
	{
		UObsidianWidgetBase* WidgetBase = Cast<UObsidianWidgetBase>(HealthBarWidgetComp->GetUserWidgetObject());
		if(WidgetBase == nullptr)
		{
			HealthBarWidgetComp->InitWidget();
			WidgetBase = Cast<UObsidianWidgetBase>(HealthBarWidgetComp->GetUserWidgetObject());
		}
		return WidgetBase;
	}
	return nullptr;
}

int32 AObsidianHero::GetCharacterLevel()
{
	return GetObsidianPlayerState() == nullptr ? GetObsidianPlayerState()->GetHeroLevel() : 1;
}

bool AObsidianHero::IsDeadOrDying_Implementation() const
{
	if(HeroAttributesComponent)
	{
		return HeroAttributesComponent->IsDeadOrDying();
	}
	return false;
}

AActor* AObsidianHero::GetAvatarActor_Implementation()
{
	return this;
}

void AObsidianHero::ClientUpdateBossDetectingPlayer_Implementation(AActor* BossActor, const bool bSeenPlayer)
{
	if(const AObsidianPlayerController* ObsidianPC = GetObsidianPlayerController())
	{
		ObsidianPC->OnBossDetectedPlayerDelegate.ExecuteIfBound(BossActor, bSeenPlayer);
	}
}

void AObsidianHero::OnAbilitySystemInitialized()
{
	Super::OnAbilitySystemInitialized();

	UObsidianAbilitySystemComponent* ObsidianASC = GetObsidianAbilitySystemComponent();
	check(ObsidianASC);

	HeroAttributesComponent->InitializeWithAbilitySystem(ObsidianASC, this);
	
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

void AObsidianHero::OnDeathStarted(AActor* OwningActor)
{
	if(IsValid(Controller))
	{
		Controller->SetIgnoreMoveInput(true);
	}
	
	Super::OnDeathStarted(OwningActor);

	if(!DeathMontages.IsEmpty())
	{
		const float DeathMontagesLength = DeathMontages.Num();
		const float RandomIndex = FMath::RandRange(0.f, DeathMontagesLength - 1);
		
		PlayAnimMontage(DeathMontages[RandomIndex]);
	}
}

void AObsidianHero::OnDeathFinished(AActor* OwningActor)
{
	Super::OnDeathFinished(OwningActor);
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

void AObsidianHero::InitializeHealthBar() const
{
	if(HealthBarWidgetComp)
	{
		if(IsLocallyControlled())
		{
			HealthBarWidgetComp->SetWidgetClass(AutonomousHealthBarClass);
		}
		else
		{
			HealthBarWidgetComp->SetWidgetClass(SimulatedHealthBarClass);
			if(UObsidianWidgetBase* Widget = GetHealthBarWidget())
			{
				Widget->SetWidgetController(HeroAttributesComponent);
			}
		}
	}
}
