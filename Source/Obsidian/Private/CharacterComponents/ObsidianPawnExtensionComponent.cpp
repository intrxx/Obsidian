// Copyright 2026 out of sCope team - intrxx

#include "CharacterComponents/ObsidianPawnExtensionComponent.h"

#include <Net/UnrealNetwork.h>

#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Characters/ObsidianPawnData.h"

UObsidianPawnExtensionComponent::UObsidianPawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	
	SetIsReplicatedByDefault(true);

	AbilitySystemComponent = nullptr;
}

void UObsidianPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UObsidianPawnExtensionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PawnData);
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
		// but this most likely will not happen in Obsidian
	}

	AbilitySystemComponent = InASC;
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, Pawn);

	if(ensure(PawnData))
	{
		InASC->SetTagRelationshipMapping(PawnData->TagRelationshipMapping);
	}

	OnAbilitySystemInitialized.Broadcast();
}

void UObsidianPawnExtensionComponent::UninitializeAbilitySystem()
{
	if(AbilitySystemComponent == nullptr)
	{
		return;
	}

	// We need to check if we are still the owner, otherwise other pawn already uninitialized us when the become the avatar actor
	if(AbilitySystemComponent->GetAvatarActor() == GetOwner())
	{
		AbilitySystemComponent->CancelAbilities();
		AbilitySystemComponent->ClearAbilityInput();
		AbilitySystemComponent->RemoveAllGameplayCues();

		if(AbilitySystemComponent->GetOwnerActor() != nullptr)
		{
			AbilitySystemComponent->SetAvatarActor(nullptr);
		}
		else
		{
			// If the ASC doesn't have a valid owner, we need to clear *all* actor info, not just the avatar pairing ~ LYRA
			AbilitySystemComponent->ClearActorInfo();
		}

		OnAbilitySystemUninitialized.Broadcast();
	}
	AbilitySystemComponent = nullptr;
}

void UObsidianPawnExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if(!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemInitialized.Add(Delegate);
	}

	// If we already have valid ASC we should fire the delegate
	if(AbilitySystemComponent)
	{
		Delegate.Execute();
	}
}

void UObsidianPawnExtensionComponent::OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if(!OnAbilitySystemUninitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemUninitialized.Add(Delegate);
	}
}

