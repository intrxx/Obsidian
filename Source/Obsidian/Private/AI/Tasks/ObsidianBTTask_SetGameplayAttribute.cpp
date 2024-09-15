// Copyright 2024 out of sCope team - Michał Ogiński

#include "AIController.h"
#include "AI/Tasks/ObsidianBTTask_SetGameplayAttribute.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

UObsidianBTTask_SetGameplayAttribute::UObsidianBTTask_SetGameplayAttribute()
{
	NodeName = FString("Set Gameplay Attribute");
	INIT_TASK_NODE_NOTIFY_FLAGS();
}

EBTNodeResult::Type UObsidianBTTask_SetGameplayAttribute::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
 	AAIController* AIController = OwnerComp.GetAIOwner();
	if(AIController == nullptr)
	{
		UE_VLOG(OwnerComp.GetOwner(), LogBehaviorTree, Error, TEXT("UObsidianBTTask_SetGameplayAttribute::ExecuteTask failed since AIController is missing."));
		return EBTNodeResult::Failed;
	}
	AActor* Actor = AIController->GetPawn();
	if(Actor == nullptr)
	{
		UE_VLOG(OwnerComp.GetOwner(), LogBehaviorTree, Error, TEXT("UObsidianBTTask_SetGameplayAttribute::ExecuteTask failed since Actor is missing."));
		return EBTNodeResult::Failed;
	}

	if(UAbilitySystemComponent* OwningASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
	{
		for(FGameplayAttributeSet Modifier : Modifiers)
		{
			UE_LOG(LogTemp, Warning, TEXT("Attribute: [%s], Magnitude: [%f]"), *Modifier.GameplayAttribute.GetName(), Modifier.Magnitude.Value);
			UGameplayEffect* GameplayEffect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("Set Gameplay Attributes")));
			GameplayEffect->DurationPolicy = DurationPolicy;
			if(DurationPolicy == EGameplayEffectDurationType::HasDuration)
			{
				GameplayEffect->DurationMagnitude = FGameplayEffectModifierMagnitude(Duration);
			}
			GameplayEffect->Modifiers.SetNum(1);

			FGameplayModifierInfo& ModifierInfo = GameplayEffect->Modifiers[0];
			ModifierInfo.ModifierMagnitude = Modifier.Magnitude;
			ModifierInfo.ModifierOp = Modifier.ModifierOp;
			ModifierInfo.Attribute = Modifier.GameplayAttribute;
			
			OwningASC->ApplyGameplayEffectToSelf(GameplayEffect, 1.0f, OwningASC->MakeEffectContext());
		}
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
