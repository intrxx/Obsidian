// Copyright 2024 Michał Ogiński


#include "CharacterComponents/ObsidianPawnExtensionComponent.h"

#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

UObsidianPawnExtensionComponent::UObsidianPawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	
	SetIsReplicatedByDefault(true);

	AbilitySystemComponent = nullptr;
}

void UObsidianPawnExtensionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UObsidianPawnExtensionComponent, PawnData);
}

void UObsidianPawnExtensionComponent::InitializeAbilitySystem(UObsidianAbilitySystemComponent* InASC,
	AActor* InOwnerActor)
{
	check(InASC);
	check(InOwnerActor);

	if(AbilitySystemComponent == InASC)
	{
		// The Ability System Comp hasn't changed - do nothing.
		return;
	}

	APawn* Pawn = GetPawnChecked<APawn>();
	AActor* ExistingAvatar = InASC->GetAvatarActor();
	
	if(AbilitySystemComponent)
	{
		// TODO if the ASC is valid there is a pawn acting as the ASC's avatar and it needs to be kicked
	}

	AbilitySystemComponent = InASC;
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, Pawn);

	// TODO Create Tag Relationship Mapping and set it here

	// TODO Broadcast the OnAbilitySystemInitialized delegate here
}
