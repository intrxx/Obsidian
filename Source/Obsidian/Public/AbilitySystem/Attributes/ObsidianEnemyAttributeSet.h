// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "AbilitySystemComponent.h"
#include "CoreMinimal.h"

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

	ATTRIBUTE_ACCESSORS(UObsidianEnemyAttributeSet, HitReactThreshold);

	//~ Begin UObject Interface
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End UObject Interface

	//~ Begin UAttributeSet Interface
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& InData) override;
	//~ End UAttributeSet Interface

protected:
	UFUNCTION()
	void OnRep_HitReactThreshold(const FGameplayAttributeData& InOldValue);

private:
	/** The current Hit React Threshold Attribute. Defines a percent threshold for which attacks greater than it will cause hit react. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HitReactThreshold, Category = "Obsidian|EAttributes|HitReactThreshold", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData HitReactThreshold;
};
