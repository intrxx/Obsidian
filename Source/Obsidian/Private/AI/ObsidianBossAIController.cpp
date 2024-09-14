// Copyright 2024 out of sCope team - Michał Ogiński


#include "AI/ObsidianBossAIController.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Characters/Heroes/ObsidianHero.h"
#include "CharacterComponents/Attributes/ObsidianHeroAttributesComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/Enemies/ObsidianBossEnemy.h"

AObsidianBossAIController::AObsidianBossAIController()
{
	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component"));
}

UObsidianAbilitySystemComponent* AObsidianBossAIController::GetObsidianAbilitySystemComponent() const
{
	if(EnemyOwner)
	{
		return CastChecked<UObsidianAbilitySystemComponent>(EnemyOwner->GetAbilitySystemComponent());
	}

	if(UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawn()))
	{
		return CastChecked<UObsidianAbilitySystemComponent>(ASC);
	}
	
	return nullptr;
}

void AObsidianBossAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	EnemyOwner = Cast<AObsidianBossEnemy>(InPawn);
	
	if(PerceptionComp)
	{
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &ThisClass::OnStimulusChanged);
	}
}

void AObsidianBossAIController::OnStimulusChanged(AActor* UpdatedActor, FAIStimulus Stimulus)
{
	AObsidianHero* HeroTarget = Cast<AObsidianHero>(UpdatedActor);
	if(HeroTarget == nullptr)
	{
		return;
	}
	
	if(Stimulus.IsActive())
	{
		HeroTarget->UpdateBossDetectingPlayer(EnemyOwner, true);
		
		Blackboard->SetValueAsBool(FName(TEXT("bIsPlayerSeen")), true);
		Blackboard->SetValueAsObject(FName(TEXT("HeroTarget")), HeroTarget);
	}
	else
	{
		Blackboard->SetValueAsBool(FName(TEXT("bIsPlayerSeen")), false);
		Blackboard->ClearValue(FName(TEXT("HeroTarget")));

		HeroTarget->UpdateBossDetectingPlayer(nullptr, false);
	}
}


