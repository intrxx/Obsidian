// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ObsidianHighlightInterface.generated.h"

// This class does not need to be modified.
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

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void StartHighlight() = 0;
	virtual void StopHighlight() = 0;
};
