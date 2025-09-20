// Copyright 2024 out of sCope team - Michał Ogiński

#include "Characters/Enemies/ObsidianBossEnemy.h"

// ~ Core
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

// ~ Project
#include "AI/ObsidianBossAIController.h"
#include "CharacterComponents/ObsidianAdvancedCombatComponent.h"
#include "CharacterComponents/ObsidianBossComponent.h"
#include "CharacterComponents/Attributes/ObsidianEnemyAttributesComponent.h"
#include "Characters/Heroes/ObsidianHero.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"

AObsidianBossEnemy::AObsidianBossEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AdvancedCombatComponent = CreateDefaultSubobject<UObsidianAdvancedCombatComponent>(TEXT("AdvancedCombatComponent"));

	UMeshComponent* CharacterMesh = GetMesh();
	check(CharacterMesh);
	
	const TMap<EObsidianTracedMeshType, UPrimitiveComponent*> TracedMeshesMap
		{
			{EObsidianTracedMeshType::CharacterMesh, CharacterMesh},
			{EObsidianTracedMeshType::CharacterMesh_LeftHand, CharacterMesh},
			{EObsidianTracedMeshType::CharacterMesh_RightHand, CharacterMesh}
		};
	AdvancedCombatComponent->AddTracedMeshes(TracedMeshesMap);
	AdvancedCombatComponent->AddIgnoredActor(this);
	AdvancedCombatComponent->OnAttackHitDelegate.AddDynamic(this, &ThisClass::HandleAdvancedCombatHit);

	BossComponent = CreateDefaultSubobject<UObsidianBossComponent>(TEXT("BossComponent"));
	
	Tags.Emplace(ObsidianActorTags::BossEnemy);
}

void AObsidianBossEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if(!HasAuthority())
	{
		return;
	}
	
	ObsidianBossAIController = Cast<AObsidianBossAIController>(NewController);
	ObsidianBossAIController->GetBlackboardComponent()->InitializeBlackboard(*DefaultBehaviorTree->BlackboardAsset);
	ObsidianBossAIController->RunBehaviorTree(DefaultBehaviorTree);
}

void AObsidianBossEnemy::OnAbilitySystemInitialized()
{
	Super::OnAbilitySystemInitialized();

	check(EnemyAttributesComponent);
	EnemyAttributesComponent->OnDeathStarted.AddUniqueDynamic(this, &ThisClass::OnDeathStarted);
	EnemyAttributesComponent->OnDeathFinished.AddUniqueDynamic(this, &ThisClass::OnDeathFinished);
}

void AObsidianBossEnemy::OnDeathStarted(AActor* OwningActor)
{
	Super::OnDeathStarted(OwningActor);

	if(ObsidianBossAIController)
	{
		ObsidianBossAIController->GetBrainComponent()->StopLogic(TEXT("Death"));
	}
}

void AObsidianBossEnemy::OnDeathFinished(AActor* OwningActor)
{
	Super::OnDeathFinished(OwningActor);
}

void AObsidianBossEnemy::HandleAdvancedCombatHit(const FHitResult& HitResult)
{
	//TODO(intrxx) Verify if I actually use it in the future.
	BP_HandleAdvancedCombatHit(HitResult);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if(ASC == nullptr)
	{
	 	return;
	}

	FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit();
	TargetData->HitResult = HitResult;

	FGameplayEventData Payload;
	Payload.TargetData = TargetData; 
	
	ASC->HandleGameplayEvent(ObsidianGameplayTags::Data_AdvancedCombat_Hit, &Payload);
}
