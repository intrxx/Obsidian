// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "ObsidianAbilitySystemComponent.generated.h"

USTRUCT()
struct FObsidianEffectUIData
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTagContainer AssetTags;

	UPROPERTY()
	EGameplayEffectDurationType EffectDurationPolicy;

	UPROPERTY()
	float EffectMagnitude = 0.f;

	UPROPERTY()
	float EffectDuration = 0.f;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAppliedAssetTags, const FObsidianEffectUIData& /** Asset Tags */);

class UOAbilityTagRelationshipMapping;

/**
 * The base Ability System Component class used in this project.
 */
UCLASS()
class OBSIDIAN_API UObsidianAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UObsidianAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ProcessAbilityInput(float DeltaTime, bool bPauseGame);
	void ClearAbilityInput();

	void SetTagRelationshipMapping(UOAbilityTagRelationshipMapping* MappingToSet);
	void AbilityActorInfoSet();

	void GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const;

public:
	FEffectAppliedAssetTags EffectAppliedAssetTags;
	
protected:
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

	void OnEffectApplied(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle EffectHandle);

	float CalculateFullEffectMagnitude(const FGameplayEffectSpec& EffectSpec);
	

protected:
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	UPROPERTY()
	TObjectPtr<UOAbilityTagRelationshipMapping> TagRelationshipMapping;
	
};
