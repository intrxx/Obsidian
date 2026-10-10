// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CommonGameInstance.h"
#include "CoreMinimal.h"

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
	UObsidianGameInstance(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	
};
