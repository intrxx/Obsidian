// Copyright 2024 out of sCope team - Michał Ogiński


#include "Characters/Player/ObsidianPlayerState.h"

#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "Net/UnrealNetwork.h"

AObsidianPlayerState::AObsidianPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// GAS needs higher NetUpdateFrequency
	NetUpdateFrequency = 100.f;

	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UObsidianAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	HeroAttributeSet = CreateDefaultSubobject<UObsidianHeroAttributeSet>(TEXT("HeroAttributeSet"));
}

void AObsidianPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AObsidianPlayerState, HeroLevel);
}

UAbilitySystemComponent* AObsidianPlayerState::GetAbilitySystemComponent() const
{
	return GetObsidianAbilitySystemComponent();
}

void AObsidianPlayerState::OnRep_HeroLevel()
{
}




