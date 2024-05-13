// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/Data/OAbilityTagRelationshipMapping.h"

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
