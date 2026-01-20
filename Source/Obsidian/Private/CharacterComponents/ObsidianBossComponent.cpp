// Copyright 2026 out of sCope team - intrxx

#include "CharacterComponents/ObsidianBossComponent.h"

// ~ Core

// ~ Project

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


