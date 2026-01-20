// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "UObject/Interface.h"

// ~ Project


#include "ObsidianInteractionInterface.generated.h"

class AObsidianPlayerController;

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
	virtual void Interact(AObsidianPlayerController* InteractingPlayerController) = 0;

	virtual bool RequiresOngoingInteraction();
	virtual void StopInteraction(AObsidianPlayerController* InteractingPlayerController);
	
};
