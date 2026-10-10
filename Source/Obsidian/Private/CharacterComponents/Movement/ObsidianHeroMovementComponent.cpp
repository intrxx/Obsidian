// Copyright 2026 out of sCope - intrxx

#include "CharacterComponents/Movement/ObsidianHeroMovementComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Net/UnrealNetwork.h"

#include "CharacterComponents/Attributes/ObsidianHeroAttributesComponent.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "Obsidian/ObsidianLogCategories.h"


UObsidianHeroMovementComponent::UObsidianHeroMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UObsidianHeroMovementComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentWalkState);
	DOREPLIFETIME(ThisClass, bUserTurnOnWalkingState);
	DOREPLIFETIME(ThisClass, bWentOutOfStamina);
}

float UObsidianHeroMovementComponent::GetMaxSpeed() const
{
	const AActor* Owner = GetOwner();
	if(!Owner)
	{
		return Super::GetMaxSpeed();
	}

	const UObsidianHeroAttributesComponent* HeroAttributes = UObsidianHeroAttributesComponent::FindHeroAttributesComponent(Owner);
	if(!HeroAttributes)
	{
		return Super::GetMaxSpeed();
	}

	if (CurrentWalkState == ObsidianGameplayTags::Movement::State::Running)
	{
		return HeroAttributes->GetSprintSpeed();
	}

	return HeroAttributes->GetMovementSpeed();
}

FGameplayTag UObsidianHeroMovementComponent::GetCurrentWalkState() const
{
	return CurrentWalkState;
}

void UObsidianHeroMovementComponent::ToggleWalkingState()
{
	bUserTurnOnWalkingState = !bUserTurnOnWalkingState;
}

bool UObsidianHeroMovementComponent::IsInWalkingState() const
{
	return bUserTurnOnWalkingState;
}

void UObsidianHeroMovementComponent::HandleOutOfStamina()
{
	if (CurrentWalkState == ObsidianGameplayTags::Movement::State::Running)
	{
		const bool bSuccess = HandleWalkingStateChanged(ObsidianGameplayTags::Movement::State::Walking);
		if (bSuccess && bWentOutOfStamina == false)
		{
			bWentOutOfStamina = true;
		}
	}
}

void UObsidianHeroMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (((CharacterMovementCVars::AsyncCharacterMovement == 1) && IsAsyncCallbackRegistered()) == false)
	{
		const AActor* Owner = GetOwner();
		if(!Owner)
		{
			return Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
		}

		const UObsidianHeroAttributesComponent* HeroAttributes = UObsidianHeroAttributesComponent::FindHeroAttributesComponent(Owner);
		if(!HeroAttributes)
		{
			return Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
		}

		const bool bHasStamina = HeroAttributes->GetStamina() > 0.0f;
		if (GetVelocityForNavMovement().SizeSquared() < KINDA_SMALL_NUMBER)
		{
			const bool bSuccess = HandleWalkingStateChanged(ObsidianGameplayTags::Movement::State::Standing);
			if (bSuccess && bWentOutOfStamina && bHasStamina)
			{
				bWentOutOfStamina = false;
			}
		}
		else if (bUserTurnOnWalkingState == false && bWentOutOfStamina == false && bHasStamina)
		{
			HandleWalkingStateChanged(ObsidianGameplayTags::Movement::State::Running);
		}
		else
		{
			HandleWalkingStateChanged(ObsidianGameplayTags::Movement::State::Walking);
		}
	}
}

bool UObsidianHeroMovementComponent::HandleWalkingStateChanged(const FGameplayTag& NewWalkingState)
{
	if (CurrentWalkState != NewWalkingState)
	{
		if (AActor* Owner = GetOwner())
		{
			if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner))
			{
				UE_LOG(ObLogCharacter, Verbose, TEXT("CurrentWalkingState [%s] changing to NewWalkingState [%s]"),
					*CurrentWalkState.ToString(), *NewWalkingState.ToString())
				ASC->RemoveLooseGameplayTag(CurrentWalkState);
				ASC->AddLooseGameplayTag(NewWalkingState);
				CurrentWalkState = NewWalkingState;
				return true;
			}
		}
	}
	return false;
}


