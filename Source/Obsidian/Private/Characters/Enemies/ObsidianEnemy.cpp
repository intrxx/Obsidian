// Copyright 2024 out of sCope team - Michał Ogiński

#include "Characters/Enemies/ObsidianEnemy.h"

#include "AbilitySystem/Data/ObsidianAbilitySet.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "AbilitySystem/Attributes/ObsidianEnemyAttributeSet.h"
#include "CharacterComponents/ObsidianPawnExtensionComponent.h"
#include "CharacterComponents/Attributes/ObsidianEnemyAttributesComponent.h"
#include "ObsidianTypes/ObsidianChannels.h"
#include "Characters/ObsidianPawnData.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "ObsidianTypes/ObsidianStencilValues.h"
#include "UI/ObsidianWidgetBase.h"

AObsidianEnemy::AObsidianEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(Obsidian_TraceChannel_PlayerCursorTrace, ECR_Block);
	GetMesh()->SetCustomDepthStencilValue(ObsidianHighlight::Red);
	GetMesh()->SetRenderCustomDepth(false);
	
	ObsidianAbilitySystemComponent = CreateDefaultSubobject<UObsidianAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	ObsidianAbilitySystemComponent->SetIsReplicated(true);
	ObsidianAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	CommonAttributeSet = CreateDefaultSubobject<UObsidianCommonAttributeSet>(TEXT("CommonAttributeSet"));
	EnemyAttributeSet = CreateDefaultSubobject<UObsidianEnemyAttributeSet>(TEXT("EnemyAttributeSet"));
	
	EnemyAttributesComponent = CreateDefaultSubobject<UObsidianEnemyAttributesComponent>(TEXT("EnemyAttributesComponent"));

	HealthBarWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	HealthBarWidgetComp->SetupAttachment(GetRootComponent());

	USkeletalMeshComponent* MeshComp = GetMesh();
	MeshComp->SetCollisionResponseToChannel(Obsidian_ObjectChannel_Projectile, ECR_Overlap);
	
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	CapsuleComp->SetCollisionResponseToChannel(Obsidian_ObjectChannel_Projectile, ECR_Overlap);
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

void AObsidianEnemy::OnAbilitySystemInitialized()
{
	Super::OnAbilitySystemInitialized();

	UObsidianAbilitySystemComponent* ObsidianASC = GetObsidianAbilitySystemComponent();
	check(ObsidianASC);
	
	if(UObsidianWidgetBase* HealthBarWidget = Cast<UObsidianWidgetBase>(HealthBarWidgetComp->GetUserWidgetObject()))
	{
		HealthBarWidget->SetWidgetController(EnemyAttributesComponent);
	}
	else
	{
		HealthBarWidgetComp->InitWidget();
		HealthBarWidget = Cast<UObsidianWidgetBase>(HealthBarWidgetComp->GetUserWidgetObject());
		HealthBarWidget->SetWidgetController(EnemyAttributesComponent);
	}
	
	EnemyAttributesComponent->InitializeWithAbilitySystem(ObsidianASC);

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
}

void AObsidianEnemy::OnAbilitySystemUninitialized()
{
	Super::OnAbilitySystemUninitialized();

	EnemyAttributesComponent->UninitializeFromAbilitySystem();
}

int32 AObsidianEnemy::GetCharacterLevel()
{
	return EnemyLevel;
}
