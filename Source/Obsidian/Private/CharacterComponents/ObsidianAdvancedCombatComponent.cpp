// Copyright 2024 out of sCope team - Michał Ogiński


#include "CharacterComponents/ObsidianAdvancedCombatComponent.h"

UObsidianAdvancedCombatComponent::UObsidianAdvancedCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UObsidianAdvancedCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UObsidianAdvancedCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(bStartTrace)
	{
		UE_LOG(LogTemp, Warning, TEXT("Tracing"));
	}
}

void UObsidianAdvancedCombatComponent::StartTrace(EObsidianTraceType TraceType)
{
	bStartTrace = true;
}

void UObsidianAdvancedCombatComponent::StopTrace()
{
	bStartTrace = false;
}

