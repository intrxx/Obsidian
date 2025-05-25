// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"

// ~ Project


#include "OAbilityTagRelationshipMapping.generated.h"

class UObject;

USTRUCT()
struct FObsidianAbilityTagRelationship
{
	GENERATED_BODY()

	/** The tag that this container relationship is about. Single tag, but abilities can have multiple of these */
	UPROPERTY(EditAnywhere, Category = "Ability", meta = (Categories = "Gameplay.Action"))
	FGameplayTag AbilityTag;
	
	/** Cancel any Abilities that has this Ability Tag. */
	UPROPERTY(EditAnywhere, Category = "Ability")
	FGameplayTagContainer AbilityTagsToCancel;
	
	/** Blocks any Abilities that has this Ability Tag. */
	UPROPERTY(EditAnywhere, Category = "Ability")
	FGameplayTagContainer AbilityTagsToBlock;
	
	/** Tags required to use this Ability. */
	UPROPERTY(EditAnywhere, Category = "Ability")
	FGameplayTagContainer ActivationRequiredTags;

	/** Tags that will prevent this Ability from being used. */
	UPROPERTY(EditAnywhere, Category = "Ability")
	FGameplayTagContainer ActivationBlockedTags;

#if WITH_EDITOR
	EDataValidationResult ValidateData(FDataValidationContext& Context, const int Index) const;
#endif
};

/**
 * Mapping representing how ability tags block or cancel other abilities
 */
UCLASS()
class OBSIDIAN_API UOAbilityTagRelationshipMapping : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Given a set of ability tags, parse the tag relationship and fill out tags to block and cancel */
	void GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagToCancel) const;

	/** Given a set of ability tags, add additional required and blocking tags */
	void GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutActivationRequiredTags, FGameplayTagContainer* OutActivationBlockedTags) const;

	/** Returns true if the specified ability tags are canceled by the passed in action tag */
	bool IsAbilityCanceledByTag(const FGameplayTagContainer& AbilityTags, const FGameplayTag& ActionTag) const;

#if WITH_EDITOR
	EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif

private:
	/** The list of relationships between different gameplay tags (which ones block or cancel others) */
	UPROPERTY(EditAnywhere, Category = "Ability", meta = (TitleProperty="AbilityTag"))
	TArray<FObsidianAbilityTagRelationship> AbilityTagRelationships;
	
};
