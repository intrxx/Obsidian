// Copyright 2024 out of sCope team - Michał Ogiński


#include "CharacterComponents/ObsidianBossComponent.h"

UObsidianBossComponent::UObsidianBossComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UObsidianBossComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


