// Copyright 2026 out of sCope team - intrxx


#include "AbilitySystem/Abilities/ObsidianGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "CharacterComponents/Movement/ObsidianCharacterMovementComponent.h"
#include "CharacterComponents/Movement/ObsidianEnemyMovementComponent.h"
#include "CharacterComponents/Movement/ObsidianHeroMovementComponent.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "Obsidian/ObsidianLogCategories.h"


UObsidianGameplayAbility::UObsidianGameplayAbility(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
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

UObsidianHeroMovementComponent* UObsidianGameplayAbility::GetObsidianHeroMovementCompFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<UObsidianHeroMovementComponent>(CurrentActorInfo->MovementComponent.Get()) : nullptr);
}

UObsidianEnemyMovementComponent* UObsidianGameplayAbility::GetObsidianEnemyMovementCompFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<UObsidianEnemyMovementComponent>(CurrentActorInfo->MovementComponent.Get()) : nullptr);
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
	return (CurrentActorInfo ? GetAvatarActorFromActorInfo()->GetActorLocation() : FVector::ZeroVector);
}

bool UObsidianGameplayAbility::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& InAbilitySystemComponent,
                                                                 const FGameplayTagContainer* InSourceTags, const FGameplayTagContainer* InTargetTags, FGameplayTagContainer* OutOptionalRelevantTags) const
{
		// Specialized version to handle death exclusion and AbilityTags expansion via ASC

	bool bBlocked = false;
	bool bMissing = false;

	UAbilitySystemGlobals& AbilitySystemGlobals = UAbilitySystemGlobals::Get();
	const FGameplayTag& BlockedTag = AbilitySystemGlobals.ActivateFailTagsBlockedTag;
	const FGameplayTag& MissingTag = AbilitySystemGlobals.ActivateFailTagsMissingTag;

	const FGameplayTagContainer& AssetTags = GetAssetTags();
	
	// Check if any of this ability's tags are currently blocked
	if (InAbilitySystemComponent.AreAbilityTagsBlocked(AssetTags))
	{
		bBlocked = true;
	}

	const UObsidianAbilitySystemComponent* ObsidianASC = Cast<UObsidianAbilitySystemComponent>(&InAbilitySystemComponent);
	static FGameplayTagContainer AllRequiredTags;
	static FGameplayTagContainer AllBlockedTags;

	AllRequiredTags = ActivationRequiredTags;
	AllBlockedTags = ActivationBlockedTags;

	// Expand our ability tags to add additional required/blocked tags
	if (ObsidianASC)
	{
		ObsidianASC->GetAdditionalActivationTagRequirements(AssetTags, AllRequiredTags, AllBlockedTags);
	}

	// Check to see the required/blocked tags for this ability
	if (AllBlockedTags.Num() || AllRequiredTags.Num())
	{
		static FGameplayTagContainer AbilitySystemComponentTags;
		
		AbilitySystemComponentTags.Reset();
		InAbilitySystemComponent.GetOwnedGameplayTags(AbilitySystemComponentTags);

		if (AbilitySystemComponentTags.HasAny(AllBlockedTags))
		{
			if (OutOptionalRelevantTags && AbilitySystemComponentTags.HasTag(ObsidianGameplayTags::Status::Death::Death))
			{
				// If player is dead and was rejected due to blocking tags, give that feedback
				OutOptionalRelevantTags->AddTag(ObsidianGameplayTags::Ability::ActivationFail::IsDead);
			}

			bBlocked = true;
		}

		if (!AbilitySystemComponentTags.HasAll(AllRequiredTags))
		{
			bMissing = true;
		}
	}

	if (InSourceTags != nullptr)
	{
		if (SourceBlockedTags.Num() || SourceRequiredTags.Num())
		{
			if (InSourceTags->HasAny(SourceBlockedTags))
			{
				bBlocked = true;
			}

			if (!InSourceTags->HasAll(SourceRequiredTags))
			{
				bMissing = true;
			}
		}
	}

	if (InTargetTags != nullptr)
	{
		if (TargetBlockedTags.Num() || TargetRequiredTags.Num())
		{
			if (InTargetTags->HasAny(TargetBlockedTags))
			{
				bBlocked = true;
			}

			if (!InTargetTags->HasAll(TargetRequiredTags))
			{
				bMissing = true;
			}
		}
	}

	if (bBlocked)
	{
		if (OutOptionalRelevantTags && BlockedTag.IsValid())
		{
			OutOptionalRelevantTags->AddTag(BlockedTag);
		}
		return false;
	}
	if (bMissing)
	{
		if (OutOptionalRelevantTags && MissingTag.IsValid())
		{
			OutOptionalRelevantTags->AddTag(MissingTag);
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
			UE_LOG(ObLogAbilitySystem, Error, TEXT("Attack Montages are empty on [%s] for [%s]."), *GetNameSafe(this), *GetNameSafe(GetAvatarActorFromActorInfo()));
		}
		else
		{
			UE_LOG(ObLogAbilitySystem, Error, TEXT("Attack Montages are empty on [%s]."), *GetNameSafe(this));
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
			UE_LOG(ObLogAbilitySystem, Error, TEXT("Attack Montages are empty on [%s] for [%s]."), *GetNameSafe(this), *GetNameSafe(GetAvatarActorFromActorInfo()));
		}
		else
		{
			UE_LOG(ObLogAbilitySystem, Error, TEXT("Attack Montages are empty on [%s]."), *GetNameSafe(this));
		}
		return nullptr;
	}

	return AbilityMontages[0].AbilityMontage;
}

FVector UObsidianGameplayAbility::GetRandomPointInCircleAroundOrigin(const FVector& InOrigin, const float InRadius, const float InFixedHeight)
{
	const float Angle = FMath::RandRange(0.0f, 2.0f * PI);

	const float X = FMath::Cos(Angle) * InRadius;
	const float Y = FMath::Sin(Angle) * InRadius;

	const FVector OriginZeroZ = FVector(InOrigin.X, InOrigin.Y, 0.0f);
	return OriginZeroZ + FVector(X, Y, InFixedHeight);
}

TArray<FVector> UObsidianGameplayAbility::GetPointsOnCircleAroundOriginNormalized(const FVector& InOrigin, const float InNumberOfPoints,
	const float InRadius, const float InFixedHeight)
{
	TArray<FVector> Points;
	Points.Reserve(InNumberOfPoints);
	
	if (InNumberOfPoints == 0)
	{
		return Points;
	}

	const FVector OriginZeroZ = FVector(InOrigin.X, InOrigin.Y, 0.0f);
	const float AngleStep = (2.0f * PI) / InNumberOfPoints;

	for (uint16 i = 0; i < InNumberOfPoints; ++i)
	{
		const float Angle = AngleStep * static_cast<float>(i);

		const float X = FMath::Cos(Angle) * InRadius;
		const float Y = FMath::Sin(Angle) * InRadius;

		Points.Add(OriginZeroZ + FVector(X, Y, InFixedHeight));
	}

	return Points;
}
