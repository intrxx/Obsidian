// Copyright 2026 out of sCope team - intrxx

#include "AbilitySystem/Data/OAbilityTagRelationshipMapping.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif // ~ With Editor


void UOAbilityTagRelationshipMapping::GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& InAbilityTags,
                                                                     FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagToCancel) const
{
	for(int32 i = 0; i < AbilityTagRelationships.Num(); i++)
	{
		const FObsidianAbilityTagRelationship& Tags = AbilityTagRelationships[i];
		
		if(InAbilityTags.HasTag(Tags.AbilityTag))
		{
			if(OutTagsToBlock)
			{
				OutTagsToBlock->AppendTags(Tags.AbilityTagsToBlock);
			}
			if(OutTagToCancel)
			{
				OutTagToCancel->AppendTags(Tags.AbilityTagsToCancel);
			}
		}
	}
}

void UOAbilityTagRelationshipMapping::GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& InAbilityTags,
	FGameplayTagContainer* OutActivationRequiredTags, FGameplayTagContainer* OutActivationBlockedTags) const
{
	for(int32 i = 0; i < AbilityTagRelationships.Num(); i++)
	{
		const FObsidianAbilityTagRelationship& Tags = AbilityTagRelationships[i];
		
		if(InAbilityTags.HasTag(Tags.AbilityTag))
		{
			if(OutActivationRequiredTags)
			{
				OutActivationRequiredTags->AppendTags(Tags.ActivationRequiredTags);
			}
			if(OutActivationBlockedTags)
			{
				OutActivationBlockedTags->AppendTags(Tags.ActivationBlockedTags);
			}
		}
	}
}

bool UOAbilityTagRelationshipMapping::IsAbilityCanceledByTag(const FGameplayTagContainer& InAbilityTags,
	const FGameplayTag& InActionTag) const
{
	for(int32 i = 0; i < AbilityTagRelationships.Num(); i++)
	{
		const FObsidianAbilityTagRelationship& Tags = AbilityTagRelationships[i];

		if(Tags.AbilityTag == InActionTag && Tags.AbilityTagsToCancel.HasAny(InAbilityTags))
		{
			return true;
		}
	}
	return false;
}

#if WITH_EDITOR
EDataValidationResult FObsidianAbilityTagRelationship::ValidateData(FDataValidationContext& InContext, const int InIndex) const
{
	EDataValidationResult Result = EDataValidationResult::Valid;

	if(!AbilityTag.IsValid())
	{
		Result = EDataValidationResult::Invalid;

		const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("Ability Tag at index [%i] is invalid! \n"
			"Please set a valid Ability Tag or delete this index entry in the Ability Tag Relationships array"), InIndex));

		InContext.AddError(ErrorMessage);
	}

	return Result;
}

EDataValidationResult UOAbilityTagRelationshipMapping::IsDataValid(FDataValidationContext& InContext) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(InContext), EDataValidationResult::Valid);

	unsigned int TagRelationshipIndex = 0;
	for(const FObsidianAbilityTagRelationship& TagRelationship : AbilityTagRelationships)
	{
		Result =  CombineDataValidationResults(Result, TagRelationship.ValidateData(InContext, TagRelationshipIndex));
		TagRelationshipIndex++;
	}
	
	return Result;
}
#endif // ~ With Editor
