#include "ObsidianGameplayTags.h"
#include "Engine/EngineTypes.h"
#include "GameplayTagsManager.h"

namespace ObsidianGameplayTags
{
	// Abilities
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivationFail_IsDead, "Ability.ActivationFail.IsDead", "Ability failed to activate because its owner is dead.");

	// Statuses
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Death, "Status.Death", "Target has the death status.");
	
	// Native Input
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Move, "Input.Native.Move", "Move input.");

	// Ability Input
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability1, "Input.Ability.Ability1", "Ability1 input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability2, "Input.Ability.Ability2", "Ability2 input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability3, "Input.Ability.Ability3", "Ability3 input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability4, "Input.Ability.Ability4", "Ability4 input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability5, "Input.Ability.Ability5", "Ability5 input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability6, "Input.Ability.Ability6", "Ability6 input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability7, "Input.Ability.Ability7", "Ability7 input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability8, "Input.Ability.Ability8", "Ability8 input.");

	// Data
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Data_DisplayValue, "Data.DisplayValue", "Tag used for abilities to pass info to UI");
	
	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString)
	{
		const UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();
		FGameplayTag ReturnTag = TagsManager.RequestGameplayTag(FName(*TagString), false);

		if(bMatchPartialString && !ReturnTag.IsValid())
		{
			FGameplayTagContainer AllTags;
			TagsManager.RequestAllGameplayTags(AllTags, true);

			for(const FGameplayTag& Tag : AllTags)
			{
				if(Tag.ToString().Contains(TagString))
				{
					UE_LOG(LogTemp, Display, TEXT("Did not find exact match for [%s] but found partial match on tag [%s]."), *TagString, *Tag.ToString());
					ReturnTag = Tag;
					break;
				}	
			}
		}
		return ReturnTag;
	}
}