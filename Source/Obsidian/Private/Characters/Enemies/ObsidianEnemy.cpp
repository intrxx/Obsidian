// Copyright 2024 Michał Ogiński

#include "Characters/Enemies/ObsidianEnemy.h"

#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "AbilitySystem/Attributes/ObsidianEnemyAttributeSet.h"
#include "CharacterComponents/ObsidianPawnExtensionComponent.h"
#include "CharacterComponents/Attributes/ObsidianEnemyAttributesComponent.h"
#include "ObsidianTypes/ObsidianChannels.h"
#include "ObsidianTypes/ObsidianStencilValues.h"

AObsidianEnemy::AObsidianEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(Obsidian_TraceChannel_Highlight, ECR_Block);
	GetMesh()->SetCustomDepthStencilValue(ObsidianHighlight::Red);
	GetMesh()->SetRenderCustomDepth(false);
	
	ObsidianAbilitySystemComponent = CreateDefaultSubobject<UObsidianAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	ObsidianAbilitySystemComponent->SetIsReplicated(true);
	ObsidianAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	CommonAttributeSet = CreateDefaultSubobject<UObsidianCommonAttributeSet>(TEXT("CommonAttributeSet"));
	EnemyAttributeSet = CreateDefaultSubobject<UObsidianEnemyAttributeSet>(TEXT("EnemyAttributeSet"));
	
	EnemyAttributesComponent = CreateDefaultSubobject<UObsidianEnemyAttributesComponent>(TEXT("EnemyAttributesComponent"));
}

void AObsidianEnemy::StartHighlight()
{
	GetMesh()->SetRenderCustomDepth(true);
}

void AObsidianEnemy::StopHighlight()
{
	GetMesh()->SetRenderCustomDepth(false);
}

void AObsidianEnemy::BeginPlay()
{
	Super::BeginPlay();

	check(PawnExtComp);
	PawnExtComp->InitializeAbilitySystem(ObsidianAbilitySystemComponent, this);
}
