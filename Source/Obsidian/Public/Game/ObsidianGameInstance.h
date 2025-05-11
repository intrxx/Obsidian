// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "CommonGameInstance.h"
#include "ObsidianGameInstance.generated.h"

class AObsidianHero;
/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianGameInstance : public UCommonGameInstance
{
	GENERATED_BODY()

public:
	UObsidianGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY()
	TSoftClassPtr<AObsidianHero> ChosenHero;

	UPROPERTY()
	FText TempHeroName = FText();
};
