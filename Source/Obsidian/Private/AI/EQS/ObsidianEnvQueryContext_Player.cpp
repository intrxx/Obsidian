// Copyright 2024 out of sCope team - Michał Ogiński

#include "AI/EQS/ObsidianEnvQueryContext_Player.h"

// ~ Core
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "Kismet/GameplayStatics.h"

// ~ Project
#include "AI/AObsidianAIControllerBase.h"
#include "Characters/Enemies/ObsidianRegularEnemy.h"
#include "Characters/Heroes/ObsidianHero.h"
#include "Obsidian/ObsidianGameModule.h"

void UObsidianEnvQueryContext_Player::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	TArray<AActor*> Heroes;
	bool bWasSuccessful = false;
	
	if(const AObsidianRegularEnemy* EnemyOwner = Cast<AObsidianRegularEnemy>(QueryInstance.Owner.Get()))
	{
		if(AObsidianAIControllerBase* ObsidianAIController = EnemyOwner->GetObsidianAIController())
		{
			if(const UBlackboardComponent* BlackboardComponent = ObsidianAIController->GetBlackboardComponent())
			{
				if(AObsidianHero* HeroTarget = Cast<AObsidianHero>(BlackboardComponent->GetValueAsObject(TEXT("TargetToFollow"))))
				{
					Heroes.Add(HeroTarget);
					bWasSuccessful = true;
				}
			}
		}
	}

	// Fallback to getting all actors.
	if(!bWasSuccessful)
	{
		UGameplayStatics::GetAllActorsOfClass(this, AObsidianHero::StaticClass(), Heroes);
	}
	
	if(!Heroes.IsEmpty())
	{
		UEnvQueryItemType_Actor::SetContextHelper(ContextData, Heroes);
		return;
	}

#if !UE_BUILD_SHIPPING
	UE_LOG(LogObsidian, Error, TEXT("Context [%hs] failed to provide Player Context for [%s]."),
		__FUNCTION__, *GetNameSafe(QueryInstance.Owner.Get()));
#endif
}
