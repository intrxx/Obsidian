// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "AbilitySystemComponent.h"
#include "CoreMinimal.h"
#include "AbilitySystem/Attributes/ObsidianAttributeSetBase.h"
#include "ObsidianEnemyAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianEnemyAttributeSet : public UObsidianAttributeSetBase
{
	GENERATED_BODY()

public:
	UObsidianEnemyAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	ATTRIBUTE_ACCESSORS(UObsidianEnemyAttributeSet, EnemySpecificAttribute);

protected:
	UFUNCTION()
	void OnRep_EnemySpecificAttribute(const FGameplayAttributeData& OldValue);

private:
	/** The current EnemySpecificAttribute attribute. TODO This is just a setup thing for now as I don't have any meaningful Enemy specific Attribute in mind right now */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_EnemySpecificAttribute, Category = "Obsidian|EAttributes|EnemySpecificAttribute", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData EnemySpecificAttribute;
};
