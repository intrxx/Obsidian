// Copyright 2026 out of sCope team - intrxx

#pragma once

#include <CoreMinimal.h>
#include <GameplayTagContainer.h>

#include "ObsidianTypes/ObsidianCoreTypes.h"

#include <Kismet/GameplayStatics.h>
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
	
	static FGameplayTag GetOpposedEquipmentTagForTag(const FGameplayTag MainTag);

	static EObsidianGameNetworkType GetCurrentNetworkType(const UObject* WorldContextObject);
	static bool IsOfflineNetworkType(const EObsidianGameNetworkType NetworkType);
};
