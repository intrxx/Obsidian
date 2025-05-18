// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/GameplayStatics.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"
#include "ObsidianGameplayStatics.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianGameplayStatics : public UGameplayStatics
{
	GENERATED_BODY()
	
public:
	static FText GetHeroClassText(const EObsidianHeroClass HeroClass);

	static bool DoesTagMatchesAnySubTag(const FGameplayTag TagToCheck, const FGameplayTag& SubTagToCheck);
	
	static FGameplayTag GetOpposedEuipmentTagForTag(const FGameplayTag MainTag);
};
