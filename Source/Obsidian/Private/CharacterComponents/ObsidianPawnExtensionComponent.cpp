// Copyright 2024 Michał Ogiński


#include "CharacterComponents/ObsidianPawnExtensionComponent.h"
#include "Net/UnrealNetwork.h"

UObsidianPawnExtensionComponent::UObsidianPawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UObsidianPawnExtensionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UObsidianPawnExtensionComponent, PawnData);
}
