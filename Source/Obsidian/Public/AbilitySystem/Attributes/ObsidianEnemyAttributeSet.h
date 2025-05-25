// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"

// ~ Project


#include "ObsidianCommonAttributeSet.h"
#include "ObsidianEnemyAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianEnemyAttributeSet : public UObsidianCommonAttributeSet
{
	GENERATED_BODY()

public:
	UObsidianEnemyAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	ATTRIBUTE_ACCESSORS(UObsidianEnemyAttributeSet, HitReactThreshold);

protected:
	UFUNCTION()
	void OnRep_HitReactThreshold(const FGameplayAttributeData& OldValue);

private:
	/** The current Hit React Threshold Attribute. Defines a percent threshold for which attacks greater than it will cause hit react. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitReactThreshold, Category = "Obsidian|EAttributes|HitReactThreshold", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData HitReactThreshold;
};
