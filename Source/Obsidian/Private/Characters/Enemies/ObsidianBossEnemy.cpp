// Copyright 2024 out of sCope team - Michał Ogiński


#include "Characters/Enemies/ObsidianBossEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/ObsidianBossAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "CharacterComponents/Attributes/ObsidianEnemyAttributesComponent.h"
#include "Characters/Heroes/ObsidianHero.h"
#include "ObsidianTypes/ObsidianActorTags.h"

AObsidianBossEnemy::AObsidianBossEnemy(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
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
	ObsidianBossAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	ObsidianBossAIController->RunBehaviorTree(BehaviorTree);
}

void AObsidianBossEnemy::HandleIntroduction(UBlackboardComponent* OwningBlackboard, AObsidianHero* TargetActor)
{
	float IntroductionLength = 0.0f;
	if(IntroductionMontage)
	{
		IntroductionLength = PlayAnimMontage(IntroductionMontage);
	}

	UWorld* World = GetWorld();
	if(World)
	{
		World->GetTimerManager().SetTimer(IntroductionFinishedTimerHandle, FTimerDelegate::CreateWeakLambda(this, [OwningBlackboard]()
		{
			OwningBlackboard->SetValueAsBool(FName(TEXT("bIntroduced")), true);
			
		}), IntroductionLength, false);
	}
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
}

void AObsidianBossEnemy::OnDeathFinished(AActor* OwningActor)
{
	Super::OnDeathFinished(OwningActor);
}
