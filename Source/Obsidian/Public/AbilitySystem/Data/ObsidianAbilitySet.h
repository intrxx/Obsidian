// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "ActiveGameplayEffectHandle.h"
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"

#include "ObsidianAbilitySet.generated.h"

class UObsidianAbilitySystemComponent;
class UObsidianGameplayAbility;
class UGameplayEffect;
class UAttributeSet;

/**
 *  Data used by the Ability Set to grant gameplay abilities.
 */
USTRUCT(BlueprintType)
struct FObsidianAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:
	/** Gameplay Ability to grant. */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UObsidianGameplayAbility> Ability = nullptr;

	/** Level of the granted ability. */
	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

	/** Gameplay Tag used to process input for the ability. */
	UPROPERTY(EditDefaultsOnly, meta=(Categories = "Input"))
	FGameplayTag InputTag;
	
#if WITH_EDITOR
	EDataValidationResult ValidateData(FDataValidationContext& InContext, const int InIndex) const;
#endif
};

/**
 *  Data used by the Ability Set to grant additional gameplay effects.
 */
USTRUCT(BlueprintType)
struct FObsidianAbilitySet_GameplayEffect
{
	GENERATED_BODY()

public:
	/** Gameplay Effect to grant. */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;
	
	/** Level of the granted Gameplay Effect. */
	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.0f;

	/** If this is set to true, this Gameplay effect will be given as the last one (Backing attributes should have this set to true). */
	UPROPERTY(EditDefaultsOnly)
	bool bIsDependentOnOtherAttributes = false;

#if WITH_EDITOR
	EDataValidationResult ValidateData(FDataValidationContext& InContext, const int InIndex) const;
#endif
};

/**
 *  Data used by the Ability Set to grant Attribute Sets.
 */
USTRUCT(BlueprintType)
struct FObsidianAbilitySet_AttributeSet
{
	GENERATED_BODY()

public:
	/** Attribute Set to grant. */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAttributeSet> AttributeSet;
	
#if WITH_EDITOR
	EDataValidationResult ValidateData(FDataValidationContext& InContext, const int InIndex) const;
#endif
};

/**
 *  Data used by the Ability Set to store handles to what has been granted.
 */
USTRUCT()
struct FObsidianAbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:
	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& InHandle);
	void AddActiveGameplayEffectSpecHandle(const FActiveGameplayEffectHandle& InHandle);
	void AddAttributeSet(UAttributeSet* InAttributeSet);

	void TakeFromAbilitySystem(UObsidianAbilitySystemComponent* InObsidianASC);

protected:
	/** Handles to granted Gameplay Abilities. */
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> GameplayAbilitySpecHandles;

	/** Handles to granted Gameplay Effects. */
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	/** Pointers to granted Attribute Sets. */
	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};

/**
 * Non-mutable Data Asset to grant Gameplay Abilities, Gameplay Effects and Attribute Sets.
 */
UCLASS(BlueprintType, Const)
class OBSIDIAN_API UObsidianAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UObsidianAbilitySet(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	/**
	 * Grants the Ability Set.
	 * @param ObsidianASC Specified Ability System Component to grant the Ability Set.
	 * @param GrantedHandles Handles that can be later used to take away anything that was granted.
	 * @param SourceObject Used for Gameplay Ability Spec Handle to specify its Source Object
	 */
	void GiveToAbilitySystem(UObsidianAbilitySystemComponent* InObsidianASC, FObsidianAbilitySet_GrantedHandles* OutGrantedHandles, UObject* InSourceObject = nullptr) const;
	void GiveToAbilitySystem(UObsidianAbilitySystemComponent* InObsidianASC, FObsidianAbilitySet_GrantedHandles* OutGrantedHandles, const float InLevelOverride, UObject* InSourceObject = nullptr) const;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& InContext) const override;
#endif

protected:
	/** Gameplay Abilities to grant when this Ability Set is granted. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities", meta = (TitleProperty = Ability))
	TArray<FObsidianAbilitySet_GameplayAbility> GrantedGameplayAbilities;

	/** Gameplay Effects to grant when this Ability Set is granted. */
	UPROPERTY(EditDefaultsOnly, Category = "Effects", meta = (TitleProperty = GameplayEffect))
	TArray<FObsidianAbilitySet_GameplayEffect> GrantedGameplayEffects;

	/** Attribute Sets to grant when this Ability Set is granted. */
	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets")
	TArray<FObsidianAbilitySet_AttributeSet> GrantedAttributeSets;
};
