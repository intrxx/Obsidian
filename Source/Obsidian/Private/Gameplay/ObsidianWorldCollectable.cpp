// Copyright 2024 out of sCope team - Michał Ogiński


#include "Gameplay//ObsidianWorldCollectable.h"

AObsidianWorldCollectable::AObsidianWorldCollectable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

FPickupContent AObsidianWorldCollectable::GetPickupContent() const
{
	return StaticContent;
}


