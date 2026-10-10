// Copyright 2026 out of sCope team - intrxx

#include "AI/Decorators/ObsidianBTDecorator_CanActivateAbility.h"

#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameplayAbilitySpec.h"

#include "Characters/Enemies/ObsidianEnemy.h"
#include "Core/FunctionLibraries/ObsidianAbilitySystemFunctionLibrary.h"
#include "Obsidian/ObsidianLogCategories.h"


UObsidianBTDecorator_CanActivateAbility::UObsidianBTDecorator_CanActivateAbility()
{
	INIT_DECORATOR_NODE_NOTIFY_FLAGS();
	
	NodeName = "Can Activate Ability?";

	// Can't abort, the decorator is not watching anything.
	FlowAbortMode = EBTFlowAbortMode::None;
	bAllowAbortNone = false;
	bAllowAbortChildNodes = false;
	bAllowAbortLowerPri = false;
}

bool UObsidianBTDecorator_CanActivateAbility::CalculateRawConditionValue(UBehaviorTreeComponent& InOwnerComp,
	uint8* InNodeMemory) const
{
	const AAIController* AIController = InOwnerComp.GetAIOwner();
	if(AIController == nullptr)
	{
		UE_LOG(ObLogAI, Error, TEXT("AI Controller is invalid on [%hs]."), __FUNCTION__);
		return false;
	}

	const UBlackboardComponent* BlackboardComponent = InOwnerComp.GetBlackboardComponent();
	if(BlackboardComponent == nullptr)
	{
		UE_LOG(ObLogAI, Error, TEXT("Blackboard Component is invalid on [%hs]."), __FUNCTION__);
		return false;
	}

	if(const AObsidianEnemy* ObsidianEnemy = Cast<AObsidianEnemy>(AIController->GetPawn()))
	{
		const FGameplayAbilitySpec* ActivatableAbilitySpec = ObsidianEnemy->GetFirstAbilitySpecForTag(ActivateAbilityWithTag);
		
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
#if !UE_BUILD_SHIPPING
				if(bDebugEnabled)
				{
					UE_LOG(ObLogAI, Log, TEXT("Owner: [%s] is able to activate Ability: [%s]"), *GetNameSafe(ObsidianEnemy), *GetNameSafe(Ability));
				}
#endif
				
				return true;
			}
#if !UE_BUILD_SHIPPING
			if(bDebugEnabled)
			{
				UE_LOG(ObLogAI, Log, TEXT("Owner: [%s] is NOT able to activate Ability: [%s]"), *GetNameSafe(ObsidianEnemy), *GetNameSafe(Ability));
			}
#endif
		}
	}
	return false;
}

FString UObsidianBTDecorator_CanActivateAbility::GetStaticDescription() const
{
	return FString::Printf(TEXT("Gameplay Tag: [%s] \n"), *ActivateAbilityWithTag.GetTagName().ToString()) +=
		FString::Printf(TEXT("Target Actor Key: [%s] \n"), *TargetActor_Selector.SelectedKeyName.ToString()) +=
		bDebugEnabled ? FString::Printf(TEXT("Debug Enabled.")) : FString::Printf(TEXT("Debug Disabled.")); 
}


