// Copyright 2026 out of sCope team - intrxx

#include "AI/Tasks/ObsidianBTTask_SetGameplayAttribute.h"

// ~ Core
#include "AIController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

// ~ Project

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

FString UObsidianBTTask_SetGameplayAttribute::GetStaticDescription() const
{
	FString BasicMessage = FString("");
	
	switch(DurationPolicy)
	{
	case EGameplayEffectDurationType::Instant:
		BasicMessage = FString::Printf(TEXT("Duration Policy is [Instant] \n"));
		break;
	case EGameplayEffectDurationType::Infinite:
		BasicMessage = FString::Printf(TEXT("Duration Policy is [Infinite] \n"));
		break;
	case EGameplayEffectDurationType::HasDuration:
		BasicMessage = FString::Printf(TEXT("Duration Policy is [Has Duration] \n")) +=
			FString::Printf(TEXT("Duration: [%f] \n"), Duration);
		break;
	}

	FString FinalMessage = BasicMessage;
	
	for(const FGameplayAttributeSet& Modifier : Modifiers)
	{
		FinalMessage += FString("\n") +=
			FString::Printf(TEXT("Gameplay Attribute: [%s] \n"), *Modifier.GameplayAttribute.GetName()) +=
			FString::Printf(TEXT("Magnitude: [%f] \n"), Modifier.Magnitude.Value);

		if(Modifier.ModifierOp == EGameplayModOp::Additive)
		{
			FinalMessage += FString::Printf(TEXT("ModifierOp: [Additive] \n"));
		}
		else if(Modifier.ModifierOp == EGameplayModOp::Override)
		{
			FinalMessage += FString::Printf(TEXT("ModifierOp: [Override] \n"));
		}
		else if(Modifier.ModifierOp == EGameplayModOp::Multiplicitive)
		{
			FinalMessage += FString::Printf(TEXT("ModifierOp: [Multiplicitive] \n"));
		}
		else if(Modifier.ModifierOp == EGameplayModOp::Division)
		{
			FinalMessage += FString::Printf(TEXT("ModifierOp: [Division] \n"));
		}
	}

	return FinalMessage;
}
