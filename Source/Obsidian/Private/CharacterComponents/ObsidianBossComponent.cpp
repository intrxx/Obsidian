// Copyright 2024 out of sCope team - Michał Ogiński


#include "CharacterComponents/ObsidianBossComponent.h"

UObsidianBossComponent::UObsidianBossComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UObsidianBossComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


