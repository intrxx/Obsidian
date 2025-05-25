// Copyright 2024 out of sCope team - Michał Ogiński

#include "AbilitySystem/Data/OAbilityTagRelationshipMapping.h"

// ~ Core
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif // ~ With Editor

// ~ Project

void UOAbilityTagRelationshipMapping::GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags,
                                                                     FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagToCancel) const
{
	for(int32 i = 0; i < AbilityTagRelationships.Num(); i++)
	{
		const FObsidianAbilityTagRelationship& Tags = AbilityTagRelationships[i];
		
		if(AbilityTags.HasTag(Tags.AbilityTag))
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

void UOAbilityTagRelationshipMapping::GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags,
	FGameplayTagContainer* OutActivationRequiredTags, FGameplayTagContainer* OutActivationBlockedTags) const
{
	for(int32 i = 0; i < AbilityTagRelationships.Num(); i++)
	{
		const FObsidianAbilityTagRelationship& Tags = AbilityTagRelationships[i];
		
		if(AbilityTags.HasTag(Tags.AbilityTag))
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

bool UOAbilityTagRelationshipMapping::IsAbilityCanceledByTag(const FGameplayTagContainer& AbilityTags,
	const FGameplayTag& ActionTag) const
{
	for(int32 i = 0; i < AbilityTagRelationships.Num(); i++)
	{
		const FObsidianAbilityTagRelationship& Tags = AbilityTagRelationships[i];

		if(Tags.AbilityTag == ActionTag && Tags.AbilityTagsToCancel.HasAny(AbilityTags))
		{
			return true;
		}
	}
	return false;
}

#if WITH_EDITOR
EDataValidationResult FObsidianAbilityTagRelationship::ValidateData(FDataValidationContext& Context, const int Index) const
{
	EDataValidationResult Result = EDataValidationResult::Valid;

	if(!AbilityTag.IsValid())
	{
		Result = EDataValidationResult::Invalid;

		const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("Ability Tag at index [%i] is invalid! \n"
			"Please set a valid Ability Tag or delete this index entry in the Ability Tag Relationships array"), Index));

		Context.AddError(ErrorMessage);
	}

	return Result;
}

EDataValidationResult UOAbilityTagRelationshipMapping::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	unsigned int TagRelationshipIndex = 0;
	for(const FObsidianAbilityTagRelationship& TagRelationship : AbilityTagRelationships)
	{
		Result =  CombineDataValidationResults(Result, TagRelationship.ValidateData(Context, TagRelationshipIndex));
		TagRelationshipIndex++;
	}
	
	return Result;
}
#endif // ~ With Editor
