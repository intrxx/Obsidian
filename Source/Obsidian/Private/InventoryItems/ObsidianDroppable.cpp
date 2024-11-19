// Copyright 2024 out of sCope team - Michał Ogiński


#include "InventoryItems/ObsidianDroppable.h"

AObsidianDroppable::AObsidianDroppable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

FPickupContent AObsidianDroppable::GetPickupContent() const
{
	return StaticContent;
}


