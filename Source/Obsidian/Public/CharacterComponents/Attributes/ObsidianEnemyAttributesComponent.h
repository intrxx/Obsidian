// Copyright 2024 Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "CharacterComponents/Attributes/ObsidianAttributesComponent.h"
#include "ObsidianEnemyAttributesComponent.generated.h"

class UObsidianEnemyAttributeSet;
/**
 * Class that handles all attributes that are given to Enemy classes.
 */
UCLASS()
class OBSIDIAN_API UObsidianEnemyAttributesComponent : public UObsidianAttributesComponent
{
	GENERATED_BODY()

public:
	UObsidianEnemyAttributesComponent(const FObjectInitializer& ObjectInitializer);
	
	/** Returns the ENEMY Attributes Component if one exists on the specified actor. */
	UFUNCTION(BlueprintPure, Category = "Obsidian|EnemyAttributes")
	static UObsidianEnemyAttributesComponent* FindAttributesComponent(const AActor* Actor) {return (Actor ? Actor->FindComponentByClass<UObsidianEnemyAttributesComponent>() : nullptr);}

	//~ Start of ObsidianAttributesComponent
	virtual void InitializeWithAbilitySystem(UObsidianAbilitySystemComponent* InASC, AActor* Owner = nullptr) override;
	virtual void UninitializeFromAbilitySystem() override;
	//~ End of ObsidianAttributesComponent
	
	/**
	 * Getters for Gameplay Attributes.
	 */
	
	/** Getters for EnemySpecificAttribute Value and Attribute from UObsidianEnemyAttributeSet. */
	float GetEnemySpecificAttribute() const;
	FGameplayAttribute GetEnemySpecificAttributeAttribute() const;

	/**
	 * 
	 */

protected:
	/**
	 * Callbacks for Attribute change delegates.
	 */
	
	virtual void EnemySpecificAttributeChanged(const FOnAttributeChangeData& Data);

	/**
	 * 
	 */

protected:
	/**
	 * Sets used by this component.
	 */
	
	UPROPERTY()
	TObjectPtr<const UObsidianEnemyAttributeSet> EnemyAttributeSet;

	/**
	 * Attribute change delegate handles.
	 */

	FDelegateHandle EnemySpecificAttributeChangedDelegateHandle;

	/**
	 * 
	 */
};
