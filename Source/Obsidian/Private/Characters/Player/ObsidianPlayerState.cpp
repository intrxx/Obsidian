// Copyright 2024 out of sCope team - Michał Ogiński

#include "Characters/Player/ObsidianPlayerState.h"

// ~ Core
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// ~ Project
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Game/ObsidianGameInstance.h"

AObsidianPlayerState::AObsidianPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// GAS needs higher NetUpdateFrequency
	SetNetUpdateFrequency(100.f);

	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UObsidianAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	HeroAttributeSet = CreateDefaultSubobject<UObsidianHeroAttributeSet>(TEXT("HeroAttributeSet"));
}

void AObsidianPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, HeroLevel);
}

AObsidianPlayerController* AObsidianPlayerState::GetObsidianPlayerController() const
{
	return CastChecked<AObsidianPlayerController>(GetPlayerController(), ECastCheckedType::NullAllowed);
}

AObsidianHero* AObsidianPlayerState::GetObsidianHero() const
{
	if (const AObsidianPlayerController* ObsidianPC = CastChecked<AObsidianPlayerController>(GetPlayerController()))
	{
		return ObsidianPC->GetObsidianHero();
	}
	return nullptr;
}

UAbilitySystemComponent* AObsidianPlayerState::GetAbilitySystemComponent() const
{
	return GetObsidianAbilitySystemComponent();
}

FText AObsidianPlayerState::GetObsidianPlayerName()
{
	// Temp
	if(PlayerName.IsEmpty())
	{
		if(const UObsidianGameInstance* GameInstance = Cast<UObsidianGameInstance>(UGameplayStatics::GetGameInstance(this)))
		{
			SetObsidianPlayerName(GameInstance->TempHeroName);
		}
	}
	// Temp
	return PlayerName;
}

void AObsidianPlayerState::IncreaseHeroLevel()
{
	HeroLevel++;
	
	OnHeroLevelUp.Broadcast(HeroLevel);
}

void AObsidianPlayerState::OnRep_HeroLevel()
{
	OnHeroLevelUp.Broadcast(HeroLevel);
}

void AObsidianPlayerState::SetObsidianPlayerName(const FText& InName)
{
	PlayerName = InName;
}






