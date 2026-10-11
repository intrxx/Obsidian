// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "Characters/ObsidianCharacterBase.h"

#include "ObsidianCombatTestActors.generated.h"

class UObsidianAbilitySystemComponent;
class UObsidianCommonAttributeSet;
class UObsidianEnemyAttributeSet;
class UObsidianHeroAttributeSet;

/**
 * Characters used by the combat automation tests (see ObsidianDamageExecutionTests.cpp, ObsidianAttributeSetTests.cpp and
 * ObsidianAttributeCalculationTests.cpp).
 *
 * The real Enemy and Hero need Pawn Data, Ability Sets and UI to initialize their Ability System, tests only need
 * a Character that owns an Ability System Component with the right Attribute Set, so these are built here in code.
 * They live in the ObsidianTests editor module, so they are never part of the game.
 */

/** Character that owns its Ability System Component and is its own Avatar. */
UCLASS(Abstract, HideDropdown, NotBlueprintable)
class AObsidianTestCombatant : public AObsidianCharacterBase
{
	GENERATED_BODY()

public:
	AObsidianTestCombatant(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	UObsidianAbilitySystemComponent* GetCombatantAbilitySystemComponent() const { return CombatantAbilitySystemComponent; }
	UObsidianCommonAttributeSet* GetCommonAttributeSet() const { return CommonAttributeSet; }

	void SetCombatantLevel(const uint8 InLevel) { CombatantLevel = InLevel; }

	//~ Begin IAbilitySystemInterface Interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End IAbilitySystemInterface Interface

	//~ Begin AActor Interface
	virtual void PostInitializeComponents() override;
	//~ End AActor Interface

	//~ Begin IObsidianCombatInterface Interface
	virtual uint8 GetCharacterLevel() override { return CombatantLevel; }
	//~ End IObsidianCombatInterface Interface

protected:
	UPROPERTY()
	TObjectPtr<UObsidianAbilitySystemComponent> CombatantAbilitySystemComponent;

	/** Set by the child classes to the Attribute Set they create. */
	UPROPERTY()
	TObjectPtr<UObsidianCommonAttributeSet> CommonAttributeSet;

	uint8 CombatantLevel = 1;
};

/** Combatant with the Enemy Attribute Set. */
UCLASS(HideDropdown, NotBlueprintable)
class AObsidianTestCombatant_Enemy : public AObsidianTestCombatant
{
	GENERATED_BODY()

public:
	AObsidianTestCombatant_Enemy(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	UObsidianEnemyAttributeSet* GetEnemyAttributeSet() const { return EnemyAttributeSet; }

protected:
	UPROPERTY()
	TObjectPtr<UObsidianEnemyAttributeSet> EnemyAttributeSet;
};

/** Combatant with the Hero Attribute Set, identified as a Player the same way the Hero is. */
UCLASS(HideDropdown, NotBlueprintable)
class AObsidianTestCombatant_Hero : public AObsidianTestCombatant
{
	GENERATED_BODY()

public:
	AObsidianTestCombatant_Hero(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	UObsidianHeroAttributeSet* GetHeroAttributeSet() const { return HeroAttributeSet; }

protected:
	UPROPERTY()
	TObjectPtr<UObsidianHeroAttributeSet> HeroAttributeSet;
};
