// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Items/ItemSpecific/ObsidianTownPortal.h"

// ~ Core

// ~ Project

AObsidianTownPortal::AObsidianTownPortal(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

}


