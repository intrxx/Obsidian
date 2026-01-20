// Copyright 2026 out of sCope team - intrxx

#pragma once

#include <CoreMinimal.h>
#include <ActiveGameplayEffectHandle.h>
#include <GameplayAbilitySpecHandle.h>
#include <GameplayTagContainer.h>

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include <Engine/DataAsset.h>
#include "ObsidianAffixAbilitySet.generated.h"

class UObsidianAbilitySystemComponent;
class UObsidianGameplayAbility;
class UGameplayEffect;
class UAttributeSet;

/**
 *  Data used by the Ability Set to grant gameplay abilities.
 */
USTRUCT(BlueprintType)
struct FObsidianAffixAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:
	/** Gameplay Ability to grant. */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UObsidianGameplayAbility> Ability = nullptr;
	
	/** Gameplay Tag used to process input for the ability for abilities that require Input for activation. */
	UPROPERTY(EditDefaultsOnly, meta=(Categories="Input"))
	FGameplayTag OptionalInputTag;
	
#if WITH_EDITOR
	EDataValidationResult ValidateData(FDataValidationContext& Context, const int Index) const;
#endif
};

/**
 *  Data used by the Ability Set to grant additional gameplay effects.
 */
USTRUCT(BlueprintType)
struct FObsidianAffixAbilitySet_GameplayEffect
{
	GENERATED_BODY()

public:
	/** Gameplay Effect to grant. */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

#if WITH_EDITOR
	EDataValidationResult ValidateData(FDataValidationContext& Context, const int Index) const;
#endif
};

/**
 *  Data used by the Ability Set to store handles to what has been granted.
 */
USTRUCT()
struct FObsidianAffixAbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:
	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void AddActiveGameplayEffectSpecHandle(const FActiveGameplayEffectHandle& Handle);

	void TakeFromAbilitySystem(UObsidianAbilitySystemComponent* ObsidianASC);

protected:
	/** Handles to granted Gameplay Abilities. */
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> GameplayAbilitySpecHandles;

	/** Handles to granted Gameplay Effects. */
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;
};

/**
 * Dynamic Ability Set that generated and give Gameplay Effect/Gameplay Abilities based on Item Affix modifiers.
 */
UCLASS(BlueprintType)
class OBSIDIAN_API UObsidianAffixAbilitySet : public UDataAsset
{
	GENERATED_BODY()

public:
	UObsidianAffixAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	 * Grants the Ability Set.
	 * @param ObsidianASC Specified Ability System Component to grant the Ability Set.
	 * @param GrantedHandles Handles that can be later used to take away anything that was granted.
	 * @param AffixTag Tag identifier of particular Affix.
	 * @param AffixValue Value/Values of Affix that are applied to the owner.
	 * @param SourceObject Used for Gameplay Ability Spec Handle to specify its Source Object
	 */
	void GiveToAbilitySystem(UObsidianAbilitySystemComponent* ObsidianASC, const FGameplayTag& AffixTag, const FObsidianActiveAffixValue& AffixValue,
		FObsidianAffixAbilitySet_GrantedHandles* GrantedHandles, UObject* SourceObject = nullptr) const;
	void GiveItemAffixesToAbilitySystem(UObsidianAbilitySystemComponent* ObsidianASC, const TArray<FObsidianActiveItemAffix>& ItemAffixes,
			FObsidianAffixAbilitySet_GrantedHandles* GrantedHandles, UObject* SourceObject = nullptr) const;
	
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif

protected:
	/** Gameplay Abilities to grant when this Ability Set is granted. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities", meta = (TitleProperty = Ability))
	TArray<FObsidianAffixAbilitySet_GameplayAbility> GrantedGameplayAbilities;

	/** Gameplay Effects to grant when this Ability Set is granted. */
	UPROPERTY(EditDefaultsOnly, Category = "Effects", meta = (TitleProperty = GameplayEffect))
	TArray<FObsidianAffixAbilitySet_GameplayEffect> GrantedGameplayEffects;
};
