// Copyright 2026 out of sCope team - intrxx

#include "AI/StateTree/ObsidianStateTreeCondition_CanActivateAbility.h"

#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "StateTreeExecutionContext.h"

#include "Characters/ObsidianCharacterBase.h"
#include "Core/FunctionLibraries/ObsidianAbilitySystemFunctionLibrary.h"
#include "Obsidian/ObsidianLogCategories.h"


#define LOCTEXT_NAMESPACE "ObsidianStateTree"

bool FObsidianStateTreeCondition_CanActivateAbility::TestCondition(FStateTreeExecutionContext& InContext) const
{
	const FInstanceDataType& InstanceData = InContext.GetInstanceData(*this);

	const AObsidianCharacterBase* ObsidianCharacter = Cast<AObsidianCharacterBase>(InstanceData.Actor);
	if(ObsidianCharacter == nullptr)
	{
		UE_LOG(ObLogAI, Error, TEXT("Actor is invalid on [%hs], it needs to be an Obsidian Character."), __FUNCTION__);
		return false;
	}

	const UAbilitySystemComponent* OwnerASC = ObsidianCharacter->GetAbilitySystemComponent();
	const FGameplayAbilitySpec* ActivatableAbilitySpec = ObsidianCharacter->GetFirstAbilitySpecForTag(ActivateAbilityWithTag);
	if(OwnerASC == nullptr || ActivatableAbilitySpec == nullptr)
	{
		return false;
	}

	const UGameplayAbility* Ability = ActivatableAbilitySpec->GetPrimaryInstance();
	if(Ability == nullptr)
	{
		return false;
	}

	const FGameplayTagContainer SourceTags = OwnerASC->GetOwnedGameplayTags();
	FGameplayTagContainer TargetTags;
	UObsidianAbilitySystemFunctionLibrary::GetAllOwnedTagsFromActor(InstanceData.TargetActor, TargetTags);

	const bool bCanActivate = Ability->CanActivateAbility(ActivatableAbilitySpec->Handle, OwnerASC->AbilityActorInfo.Get(), &SourceTags, &TargetTags);

#if !UE_BUILD_SHIPPING
	if(bDebugEnabled)
	{
		UE_LOG(ObLogAI, Log, TEXT("Owner: [%s] is %s to activate Ability: [%s]"), *GetNameSafe(ObsidianCharacter),
			bCanActivate ? TEXT("able") : TEXT("NOT able"), *GetNameSafe(Ability));
	}
#endif

	return bCanActivate;
}

#if WITH_EDITOR
FText FObsidianStateTreeCondition_CanActivateAbility::GetDescription(const FGuid& InID, FStateTreeDataView InInstanceDataView,
	const IStateTreeBindingLookup& InBindingLookup, EStateTreeNodeFormatting InFormatting) const
{
	return FText::Format(LOCTEXT("CanActivateAbilityDescription", "Can Activate Ability {0}"), FText::FromName(ActivateAbilityWithTag.GetTagName()));
}
#endif

#undef LOCTEXT_NAMESPACE
