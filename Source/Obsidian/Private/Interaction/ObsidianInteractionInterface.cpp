// Copyright 2024 out of sCope team - Michał Ogiński

#include "Interaction/ObsidianInteractionInterface.h"

// ~ Core

// ~ Project

bool IObsidianInteractionInterface::RequiresOngoingInteraction()
{
	return false;
}

void IObsidianInteractionInterface::StopInteraction(AObsidianPlayerController* InteractingPlayerController)
{
	
}
