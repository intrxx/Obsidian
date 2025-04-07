// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
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
};
