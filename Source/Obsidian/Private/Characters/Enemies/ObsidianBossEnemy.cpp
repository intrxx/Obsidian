// Copyright 2026 out of sCope team - intrxx

#include "Characters/Enemies/ObsidianBossEnemy.h"

#include "BrainComponent.h"

#include "AI/ObsidianBossAIController.h"
#include "CharacterComponents/Attributes/ObsidianEnemyAttributesComponent.h"
#include "CharacterComponents/ObsidianAdvancedCombatComponent.h"
#include "CharacterComponents/ObsidianBossComponent.h"
#include "Characters/Heroes/ObsidianHero.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "Obsidian/ObsidianLogCategories.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"


AObsidianBossEnemy::AObsidianBossEnemy(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
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

void AObsidianBossEnemy::PossessedBy(AController* InNewController)
{
	Super::PossessedBy(InNewController);

	if(!HasAuthority())
	{
		return;
	}
	
	ObsidianBossAIController = Cast<AObsidianBossAIController>(InNewController);
	if(ObsidianBossAIController == nullptr)
	{
		UE_LOG(ObLogAI, Error, TEXT("Boss [%s] was possessed by [%s] which is not an Obsidian Boss AI Controller, it will have no AI."),
			*GetNameSafe(this), *GetNameSafe(InNewController));
		return;
	}

	// The AI Logic is started by the Controller right after the Boss gets possessed.
	ObsidianBossAIController->SetStateTree(DefaultStateTree);
}

void AObsidianBossEnemy::OnAbilitySystemInitialized()
{
	Super::OnAbilitySystemInitialized();

	check(EnemyAttributesComponent);
	EnemyAttributesComponent->OnDeathStarted.AddUniqueDynamic(this, &ThisClass::OnDeathStarted);
	EnemyAttributesComponent->OnDeathFinished.AddUniqueDynamic(this, &ThisClass::OnDeathFinished);
}

void AObsidianBossEnemy::OnDeathStarted(AActor* InOwningActor)
{
	Super::OnDeathStarted(InOwningActor);

	if(ObsidianBossAIController && ObsidianBossAIController->GetBrainComponent())
	{
		ObsidianBossAIController->GetBrainComponent()->StopLogic(TEXT("Death"));
	}
}

void AObsidianBossEnemy::OnDeathFinished(AActor* InOwningActor)
{
	Super::OnDeathFinished(InOwningActor);
}

void AObsidianBossEnemy::HandleAdvancedCombatHit(const FHitResult& InHitResult)
{
	//TODO(intrxx) Verify if I actually use it in the future.
	BP_HandleAdvancedCombatHit(InHitResult);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if(ASC == nullptr)
	{
	 	return;
	}

	FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit();
	TargetData->HitResult = InHitResult;

	FGameplayEventData Payload;
	Payload.TargetData = TargetData; 
	
	ASC->HandleGameplayEvent(ObsidianGameplayTags::Data::AdvancedCombat::Hit, &Payload);
}
