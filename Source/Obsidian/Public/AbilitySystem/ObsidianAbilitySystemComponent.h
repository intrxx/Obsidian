// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "AbilitySystemComponent.h"
#include "CoreMinimal.h"

#include "ObsidianTypes/ObsidianUITypes.h"

#include "ObsidianAbilitySystemComponent.generated.h"

USTRUCT()
struct FObsidianEffectUIData
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTagContainer AssetTags;

	UPROPERTY()
	EGameplayEffectDurationType EffectDurationPolicy = EGameplayEffectDurationType();

	UPROPERTY()
	float EffectMagnitude = 0.f;

	UPROPERTY()
	float EffectDuration = 0.f;
	
	UPROPERTY()
	FObsidianEffectUIStackingData StackingData;
	
	UPROPERTY()
	bool bStackingEffect = false;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAppliedAssetTags, const FObsidianEffectUIData& /** Asset Tags */);

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnAuraDisabled, const FGameplayTag, EffectUIInfoTag);

class UOAbilityTagRelationshipMapping;

/**
 * The base Ability System Component class used in this project.
 */
UCLASS()
class OBSIDIAN_API UObsidianAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UObsidianAbilitySystemComponent(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	void AbilityInputTagPressed(const FGameplayTag& InInputTag);
	void AbilityInputTagReleased(const FGameplayTag& InInputTag);

	void ProcessAbilityInput(float InDeltaTime, bool bInPauseGame);
	void ClearAbilityInput();

	void SetTagRelationshipMapping(UOAbilityTagRelationshipMapping* InMappingToSet);
	void AbilityActorInfoSet();
	void BindToOnEffectAppliedDelegate();

	void GetAdditionalActivationTagRequirements(const FGameplayTagContainer& InAbilityTags, FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const;

	UFUNCTION(BlueprintCallable, Category = "GameplayCue", meta = (AutoCreateRefTerm = "InGameplayCueParameters", GameplayTagFilter = "GameplayCue"))
	void ExecuteGameplayCueLocal(const FGameplayTag InGameplayCueTag, const FGameplayCueParameters& InGameplayCueParameters);

	UFUNCTION(BlueprintCallable, Category = "GameplayCue", meta = (AutoCreateRefTerm = "InGameplayCueParameters", GameplayTagFilter = "GameplayCue"))
	void AddGameplayCueLocal(const FGameplayTag InGameplayCueTag, const FGameplayCueParameters& InGameplayCueParameters);

	UFUNCTION(BlueprintCallable, Category = "GameplayCue", meta = (AutoCreateRefTerm = "InGameplayCueParameters", GameplayTagFilter = "GameplayCue"))
	void RemoveGameplayCueLocal(const FGameplayTag InGameplayCueTag, const FGameplayCueParameters& InGameplayCueParameters);

public:
	FEffectAppliedAssetTags OnEffectAppliedAssetTags;
	FOnAuraDisabled OnAuraDisabledDelegate;
	
protected:
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& InSpec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& InSpec) override;

	UFUNCTION(Client, Reliable)
	void ClientOnEffectApplied(UAbilitySystemComponent* InASC, const FGameplayEffectSpec& InEffectSpec, FActiveGameplayEffectHandle InEffectHandle);

	float CalculateFullEffectMagnitude(const FGameplayEffectSpec& InEffectSpec);
	
protected:
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	UPROPERTY()
	TObjectPtr<UOAbilityTagRelationshipMapping> TagRelationshipMapping;
	
};
