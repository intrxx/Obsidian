// Copyright 2026 out of sCope team - intrxx

#include "CharacterComponents/ObsidianBossComponent.h"


UObsidianBossComponent::UObsidianBossComponent(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UObsidianBossComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


