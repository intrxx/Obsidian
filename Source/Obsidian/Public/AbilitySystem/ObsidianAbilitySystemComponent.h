// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
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
	UObsidianAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ProcessAbilityInput(float DeltaTime, bool bPauseGame);
	void ClearAbilityInput();

	void SetTagRelationshipMapping(UOAbilityTagRelationshipMapping* MappingToSet);
	void AbilityActorInfoSet();
	void BindToOnEffectAppliedDelegate();

	void GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const;

	UFUNCTION(BlueprintCallable, Category = "GameplayCue", meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
	void ExecuteGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);

	UFUNCTION(BlueprintCallable, Category = "GameplayCue", meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
	void AddGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);

	UFUNCTION(BlueprintCallable, Category = "GameplayCue", meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
	void RemoveGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);

public:
	FEffectAppliedAssetTags OnEffectAppliedAssetTags;
	FOnAuraDisabled OnAuraDisabledDelegate;
	
protected:
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

	UFUNCTION(Client, Reliable)
	void ClientOnEffectApplied(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle EffectHandle);

	float CalculateFullEffectMagnitude(const FGameplayEffectSpec& EffectSpec);
	
protected:
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	UPROPERTY()
	TObjectPtr<UOAbilityTagRelationshipMapping> TagRelationshipMapping;
	
};
