// Copyright 2026 out of sCope team - intrxx

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
	static FText GetHeroClassText(const EObsidianHeroClass InHeroClass);

	static bool DoesTagMatchesAnySubTag(const FGameplayTag InTagToCheck, const FGameplayTag& InSubTagToCheck);
	
	static FGameplayTag GetOpposedEquipmentTagForTag(const FGameplayTag InMainTag);

	static EObsidianGameNetworkType GetCurrentNetworkType(const UObject* InWorldContextObject);
	static bool IsOfflineNetworkType(const EObsidianGameNetworkType InNetworkType);
};
