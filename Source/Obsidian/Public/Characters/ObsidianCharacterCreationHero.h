// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ModularCharacter.h"
#include "ObsidianCharacterCreationHero.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API AObsidianCharacterCreationHero : public AModularCharacter
{
	GENERATED_BODY()

public:
	AObsidianCharacterCreationHero(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Obsidian|CharacterCreationHero")
	void StartHighlight();

	UFUNCTION(BlueprintCallable, Category = "Obsidian|CharacterCreationHero")
	void StopHighlight();

	UFUNCTION(BlueprintCallable, Category = "Obsidian|CharacterCreationHero")
	void PlayChooseMeAnimMontage();

	/** This function is temporary, this is not the right solution to the problem. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|CharacterCreationHero")
	void StopPlayingChooseMeAnimMontage();
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	FGameplayTag HeroTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TObjectPtr<UAnimMontage> ChooseMeAnimMontage;
	
};
