// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "UObject/Interface.h"

// ~ Project


#include "ObsidianInteractionInterface.generated.h"

UINTERFACE(MinimalAPI)
class UObsidianInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface that adds the functionality to Actor to fire interaction logic.
 */
class OBSIDIAN_API IObsidianInteractionInterface
{
	GENERATED_BODY()

public:
	virtual AActor* GetInteractionActor() = 0;
	virtual bool CanInteract() = 0;
	virtual float GetInteractionRadius() = 0;
	virtual void Interact() = 0;
};
