// Copyright 2026 out of sCope team - intrxx

#include "AI/ObsidianBossAIController.h"

#include "AbilitySystemGlobals.h"
#include "Components/StateTreeAIComponent.h"
#include "Perception/AIPerceptionComponent.h"

#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "CharacterComponents/Attributes/ObsidianHeroAttributesComponent.h"
#include "Characters/Enemies/ObsidianBossEnemy.h"
#include "Characters/Heroes/ObsidianHero.h"


AObsidianBossAIController::AObsidianBossAIController()
{
	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component"));

	StateTreeComp = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeComponent"));
	// The State Tree is given by the Boss when it gets possessed, the AI Logic is started right after.
	StateTreeComp->SetStartLogicAutomatically(false);
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

void AObsidianBossAIController::SetStateTree(UStateTree* InStateTree)
{
	StateTreeComp->SetStateTree(InStateTree);
}

void AObsidianBossAIController::SendStateTreeEvent(const FGameplayTag& InEventTag)
{
	StateTreeComp->SendStateTreeEvent(InEventTag);
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

void AObsidianBossAIController::OnUnPossess()
{
	// The State Tree needs the Pawn to be stopped, it would be left running its Tasks if this was done after the Pawn is gone.
	if(StateTreeComp)
	{
		StateTreeComp->StopLogic(TEXT("UnPossess"));
	}

	Super::OnUnPossess();
}

void AObsidianBossAIController::OnStimulusChanged(AActor* InUpdatedActor, FAIStimulus InStimulus)
{
	AObsidianHero* PerceivedHero = Cast<AObsidianHero>(InUpdatedActor);
	if(PerceivedHero == nullptr)
	{
		return;
	}
	
	if(InStimulus.IsActive())
	{
		PerceivedHero->ClientUpdateBossDetectingPlayer(EnemyOwner, true);

		bIsPlayerSeen = true;
		HeroTarget = PerceivedHero;
	}
	else
	{
		bIsPlayerSeen = false;
		HeroTarget = nullptr;

		PerceivedHero->ClientUpdateBossDetectingPlayer(nullptr, false);
	}
}


