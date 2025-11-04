// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "Abilities/GameplayAbility.h"
#include "ObsidianGameplayAbility.generated.h"

class UObsidianCharacterMovementComponent;
class UCharacterMovementComponent;
class AObsidianPlayerController;
class UObsidianAbilitySystemComponent;

/**
 * Defines how the Ability is meant to activate.
 */
UENUM(BlueprintType)
enum class EObsidianGameplayAbility_ActivationPolicy : uint8
{
	// Try to activate the Ability when the input is triggered.
	EAP_OnInputTriggered UMETA(DisplayName = "On Input Triggered"),

	// Continually try to activate the Ability while the input is active.
	EAP_WhileInputActive UMETA(DisplayName = "While input Active"),

	// Try to activate the ability when an avatar is assigned.
	EAP_OnSpawn UMETA(DisplayName = "On Spawn")
};

//TODO(intrxx) Think about Lyra like AbilityActivationGroup

USTRUCT(BlueprintType)
struct FObsidianTaggedMontage
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AbilityMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, meta=(Categories = "GameplayEvent.AbilityMontage"), BlueprintReadOnly)
	FGameplayTagContainer MontageTags = FGameplayTagContainer();
};

/**
 * The base Gameplay Ability class used in this project.
 */
UCLASS()
class OBSIDIAN_API UObsidianGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	friend UObsidianAbilitySystemComponent;

public:
	UObsidianGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	EObsidianGameplayAbility_ActivationPolicy GetAbilityActivationPolicy() const
	{
		return ActivationPolicy;
	}

protected:
	template<typename T>
	T* GetCharacterFromActorInfo() const;
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Ability")
	APlayerController* GetPlayerControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Ability")
	UAbilitySystemComponent* GetAbilitySystemCompFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Ability")
	USkeletalMeshComponent* GetSkeletalMeshCompFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Ability")
	UMovementComponent* GetMovementCompFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Ability")
	UCharacterMovementComponent* GetCharacterMovementCompFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Ability")
	UObsidianCharacterMovementComponent* GetObsidianCharacterMovementCompFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Ability")
	AObsidianPlayerController* GetObsidianPlayerControllerFromActorInfo() const;
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Ability")
	UObsidianAbilitySystemComponent* GetObsidianAbilitySystemCompFromActorInfo() const;
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Ability")
	FVector GetOwnerLocationFromActorInfo() const;
	
	//~UGameplayAbility interface
	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	//~End of UGameplayAbility interface

	/** Randomly chooses the Anim Montage to play. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|AbilityAnims")
	FObsidianTaggedMontage GetRandomAnimMontageToPlay();
	
	/** Grabs the first Anim Montage from ability, useful when there is only one. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|AbilityAnims")
	UAnimMontage* GetAnimMontage();

	FVector GetRandomPointInCircleAroundOrigin(const FVector& Origin, const float Radius, const float FixedHeight);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Ability Activation")
	EObsidianGameplayAbility_ActivationPolicy ActivationPolicy = EObsidianGameplayAbility_ActivationPolicy::EAP_OnInputTriggered;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TArray<FObsidianTaggedMontage> AbilityMontages;
};

template <typename T>
T* UObsidianGameplayAbility::GetCharacterFromActorInfo() const
{
	return CurrentActorInfo ? Cast<T>(CurrentActorInfo->AvatarActor.Get()) : nullptr;
}
