// Copyright 2026 out of sCope team - intrxx

#include "ObsidianCombatTestActors.h"

#include "AbilitySystem/Attributes/ObsidianEnemyAttributeSet.h"
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"


AObsidianTestCombatant::AObsidianTestCombatant(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
	CombatantAbilitySystemComponent = CreateDefaultSubobject<UObsidianAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

UAbilitySystemComponent* AObsidianTestCombatant::GetAbilitySystemComponent() const
{
	return CombatantAbilitySystemComponent;
}

void AObsidianTestCombatant::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CombatantAbilitySystemComponent->InitAbilityActorInfo(this, this);
}

AObsidianTestCombatant_Enemy::AObsidianTestCombatant_Enemy(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
	EnemyAttributeSet = CreateDefaultSubobject<UObsidianEnemyAttributeSet>(TEXT("EnemyAttributeSet"));
	CommonAttributeSet = EnemyAttributeSet;

	Tags.Emplace(ObsidianActorTags::Enemy);
}

AObsidianTestCombatant_Hero::AObsidianTestCombatant_Hero(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
	HeroAttributeSet = CreateDefaultSubobject<UObsidianHeroAttributeSet>(TEXT("HeroAttributeSet"));
	CommonAttributeSet = HeroAttributeSet;

	Tags.Emplace(ObsidianActorTags::Player);
}
