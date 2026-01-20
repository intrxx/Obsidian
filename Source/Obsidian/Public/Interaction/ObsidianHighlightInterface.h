// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "UObject/Interface.h"
#include "ObsidianHighlightInterface.generated.h"

/**
 * This class does not need to be modified.
 */
UINTERFACE(MinimalAPI)
class UObsidianHighlightInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OBSIDIAN_API IObsidianHighlightInterface
{
	GENERATED_BODY()

public:
	virtual AActor* GetHighlightAvatarActor() = 0;
	
	virtual void StartHighlight() = 0;
	virtual void StopHighlight() = 0;
};
