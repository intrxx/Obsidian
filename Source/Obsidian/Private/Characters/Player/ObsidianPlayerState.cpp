// Copyright 2024 Michał Ogiński


#include "Characters/Player/ObsidianPlayerState.h"

AObsidianPlayerState::AObsidianPlayerState()
{
	// GAS needs higher NetUpdateFrequency
	NetUpdateFrequency = 100.f;
}
