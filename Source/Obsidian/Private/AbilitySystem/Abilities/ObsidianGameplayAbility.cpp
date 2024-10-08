// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/Abilities/ObsidianGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "CharacterComponents/ObsidianCharacterMovementComponent.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Obsidian/Obsidian.h"
#include "Obsidian/ObsidianGameplayTags.h"

UObsidianGameplayAbility::UObsidianGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

APlayerController* UObsidianGameplayAbility::GetPlayerControllerFromActorInfo() const
{
	return (CurrentActorInfo ? CurrentActorInfo->PlayerController.Get() : nullptr);
}

UAbilitySystemComponent* UObsidianGameplayAbility::GetAbilitySystemCompFromActorInfo() const
{
	return (CurrentActorInfo ? CurrentActorInfo->AbilitySystemComponent.Get() : nullptr);
}

USkeletalMeshComponent* UObsidianGameplayAbility::GetSkeletalMeshCompFromActorInfo() const
{
	return (CurrentActorInfo ? CurrentActorInfo->SkeletalMeshComponent.Get() : nullptr);
}

UMovementComponent* UObsidianGameplayAbility::GetMovementCompFromActorInfo() const
{
	return (CurrentActorInfo ? CurrentActorInfo->MovementComponent.Get() : nullptr);
}

UCharacterMovementComponent* UObsidianGameplayAbility::GetCharacterMovementCompFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<UCharacterMovementComponent>(CurrentActorInfo->MovementComponent.Get()) : nullptr);
}

UObsidianCharacterMovementComponent* UObsidianGameplayAbility::GetObsidianCharacterMovementCompFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<UObsidianCharacterMovementComponent>(CurrentActorInfo->MovementComponent.Get()) : nullptr);
}

AObsidianPlayerController* UObsidianGameplayAbility::GetObsidianPlayerControllerFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<AObsidianPlayerController>(CurrentActorInfo->PlayerController.Get()) : nullptr);
}

UObsidianAbilitySystemComponent* UObsidianGameplayAbility::GetObsidianAbilitySystemCompFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<UObsidianAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get()) : nullptr);
}

FVector UObsidianGameplayAbility::GetOwnerLocationFromActorInfo() const
{
	return (CurrentActorInfo ? GetOwningActorFromActorInfo()->GetActorLocation() : FVector::ZeroVector);
}

bool UObsidianGameplayAbility::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,
                                                                 const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
		// Specialized version to handle death exclusion and AbilityTags expansion via ASC

	bool bBlocked = false;
	bool bMissing = false;

	UAbilitySystemGlobals& AbilitySystemGlobals = UAbilitySystemGlobals::Get();
	const FGameplayTag& BlockedTag = AbilitySystemGlobals.ActivateFailTagsBlockedTag;
	const FGameplayTag& MissingTag = AbilitySystemGlobals.ActivateFailTagsMissingTag;

	// Check if any of this ability's tags are currently blocked
	if (AbilitySystemComponent.AreAbilityTagsBlocked(AbilityTags))
	{
		bBlocked = true;
	}

	const UObsidianAbilitySystemComponent* ObsidianASC = Cast<UObsidianAbilitySystemComponent>(&AbilitySystemComponent);
	static FGameplayTagContainer AllRequiredTags;
	static FGameplayTagContainer AllBlockedTags;

	AllRequiredTags = ActivationRequiredTags;
	AllBlockedTags = ActivationBlockedTags;

	// Expand our ability tags to add additional required/blocked tags
	if (ObsidianASC)
	{
		ObsidianASC->GetAdditionalActivationTagRequirements(AbilityTags, AllRequiredTags, AllBlockedTags);
	}

	// Check to see the required/blocked tags for this ability
	if (AllBlockedTags.Num() || AllRequiredTags.Num())
	{
		static FGameplayTagContainer AbilitySystemComponentTags;
		
		AbilitySystemComponentTags.Reset();
		AbilitySystemComponent.GetOwnedGameplayTags(AbilitySystemComponentTags);

		if (AbilitySystemComponentTags.HasAny(AllBlockedTags))
		{
			if (OptionalRelevantTags && AbilitySystemComponentTags.HasTag(ObsidianGameplayTags::Status_Death))
			{
				// If player is dead and was rejected due to blocking tags, give that feedback
				OptionalRelevantTags->AddTag(ObsidianGameplayTags::Ability_ActivationFail_IsDead);
			}

			bBlocked = true;
		}

		if (!AbilitySystemComponentTags.HasAll(AllRequiredTags))
		{
			bMissing = true;
		}
	}

	if (SourceTags != nullptr)
	{
		if (SourceBlockedTags.Num() || SourceRequiredTags.Num())
		{
			if (SourceTags->HasAny(SourceBlockedTags))
			{
				bBlocked = true;
			}

			if (!SourceTags->HasAll(SourceRequiredTags))
			{
				bMissing = true;
			}
		}
	}

	if (TargetTags != nullptr)
	{
		if (TargetBlockedTags.Num() || TargetRequiredTags.Num())
		{
			if (TargetTags->HasAny(TargetBlockedTags))
			{
				bBlocked = true;
			}

			if (!TargetTags->HasAll(TargetRequiredTags))
			{
				bMissing = true;
			}
		}
	}

	if (bBlocked)
	{
		if (OptionalRelevantTags && BlockedTag.IsValid())
		{
			OptionalRelevantTags->AddTag(BlockedTag);
		}
		return false;
	}
	if (bMissing)
	{
		if (OptionalRelevantTags && MissingTag.IsValid())
		{
			OptionalRelevantTags->AddTag(MissingTag);
		}
		return false;
	}

	return true;
}

FObsidianTaggedMontage UObsidianGameplayAbility::GetRandomAnimMontageToPlay()
{
	const uint16 ArrCount = AbilityMontages.Num();
	if(ArrCount == 0)
	{
		if(GetAvatarActorFromActorInfo())
		{
			UE_LOG(LogObsidian, Error, TEXT("Attack Montages are empty on [%s] for [%s]."), *GetNameSafe(this), *GetNameSafe(GetAvatarActorFromActorInfo()));
		}
		else
		{
			UE_LOG(LogObsidian, Error, TEXT("Attack Montages are empty on [%s]."), *GetNameSafe(this));
		}
		return FObsidianTaggedMontage();
	}

	// Handling this case exclusively as this is very probable case
	if(ArrCount == 1)
	{
		return AbilityMontages[0];
	}

	const uint16 MontageNumber = FMath::RandRange(0, ArrCount - 1);
	return AbilityMontages[MontageNumber];
}

UAnimMontage* UObsidianGameplayAbility::GetAnimMontage()
{
	const uint16 ArrCount = AbilityMontages.Num();
	if(ArrCount == 0)
	{
		if(GetAvatarActorFromActorInfo())
		{
			UE_LOG(LogObsidian, Error, TEXT("Attack Montages are empty on [%s] for [%s]."), *GetNameSafe(this), *GetNameSafe(GetAvatarActorFromActorInfo()));
		}
		else
		{
			UE_LOG(LogObsidian, Error, TEXT("Attack Montages are empty on [%s]."), *GetNameSafe(this));
		}
		return nullptr;
	}

	return AbilityMontages[0].AbilityMontage;
}
