// Copyright 2024 out of sCope team - Michał Ogiński


#include "AI/Decorators/ObsidianBTDecorator_CanActivateAbility.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "GameplayAbilitySpec.h"
#include "Abilities/GameplayAbility.h"
#include "Characters/Enemies/ObsidianEnemy.h"
#include "Core/ObsidianAbilitySystemFunctionLibrary.h"
#include "Obsidian/Obsidian.h"

UObsidianBTDecorator_CanActivateAbility::UObsidianBTDecorator_CanActivateAbility()
{
	INIT_DECORATOR_NODE_NOTIFY_FLAGS();
	
	FlowAbortMode = EBTFlowAbortMode::Self;
	NodeName = "Can Activate Ability?";
}

bool UObsidianBTDecorator_CanActivateAbility::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if(AIController == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("AI Controller is invalid on [UObsidianBTDecorator_CanActivateAbility]."));
		return false;
	}

	UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
	if(BlackboardComponent)
	{
		UE_LOG(LogObsidian, Error, TEXT("Blackboard Component Controller is invalid on [UObsidianBTDecorator_CanActivateAbility]."));
		return false;
	}

	if(AObsidianEnemy* ObsidianEnemy = Cast<AObsidianEnemy>(AIController->GetPawn()))
	{
		FGameplayAbilitySpec* ActivatableAbilitySpec = ObsidianEnemy->GetFirstAbilitySpecForTag(ActivateAbilityWithTag);
		
		if(const UGameplayAbility* Ability = ActivatableAbilitySpec->GetPrimaryInstance())
		{
			const UAbilitySystemComponent* EnemyASC = ObsidianEnemy->GetAbilitySystemComponent();
			const FGameplayAbilityActorInfo* ActorInfo = EnemyASC->AbilityActorInfo.Get();

			AActor* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetActor_Selector.SelectedKeyName));
			
			const FGameplayTagContainer SourceTags = EnemyASC->GetOwnedGameplayTags();
			FGameplayTagContainer TargetTags;
			UObsidianAbilitySystemFunctionLibrary::GetAllOwnedTagsFromActor(TargetActor, TargetTags);
			
			if(Ability->CanActivateAbility(ActivatableAbilitySpec->Handle, ActorInfo, &SourceTags, &TargetTags))
			{
				return true;
			}
		}
	}
	return false;
}
