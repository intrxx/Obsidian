// Copyright 2024 Michał Ogiński

#include "Characters/Enemies/ObsidianEnemy.h"

#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "CharacterComponents/ObsidianPawnExtensionComponent.h"
#include "CharacterComponents/Attributes/ObsidianEnemyAttributesComponent.h"
#include "ObsidianTypes/ObsidianChannels.h"
#include "ObsidianTypes/ObsidianStencilValues.h"

AObsidianEnemy::AObsidianEnemy()
{
	ObsidianAbilitySystemComponent = CreateDefaultSubobject<UObsidianAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	ObsidianAbilitySystemComponent->SetIsReplicated(true);
	ObsidianAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	CommonAttributeSet = CreateDefaultSubobject<UObsidianCommonAttributeSet>(TEXT("AttributeSet"));
	
	EnemyAttributesComponent = CreateDefaultSubobject<UObsidianEnemyAttributesComponent>(TEXT("EnemyAttributesComponent"));
	
	GetMesh()->SetCollisionResponseToChannel(Obsidian_TraceChannel_Highlight, ECR_Block);
	GetMesh()->SetCustomDepthStencilValue(ObsidianHighlight::Red);
	GetMesh()->SetRenderCustomDepth(false);
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
