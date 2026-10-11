// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/ActorTestSpawner.h"
#include "GameplayTagContainer.h"
#include "Templates/SubclassOf.h"

#include "ObsidianTestWorldCleanup.h"

class AObsidianTestCombatant;
class AObsidianTestCombatant_Enemy;
class AObsidianTestCombatant_Hero;
class UGameplayEffectExecutionCalculation;
class UGameplayModMagnitudeCalculation;
struct FGameplayAttribute;

/** What happened to the target of FObsidianCombatTestEnvironment::ApplyDamage. */
struct FObsidianTestDamageResult
{
	/** Energy Shield and Health the target lost. */
	float DamageTaken = 0.0f;

	bool bIsBlockedAttack = false;
	bool bIsCriticalAttack = false;
	bool bIsEvadedHit = false;
	bool bIsSuppressedSpell = false;
	bool bIsTargetImmune = false;
};

/**
 * A Hero and an Enemy that can damage each other, created in a transient game world.
 *
 * Both start with attributes that take the randomness out of the Damage Execution, the rolls that can't be forced
 * to one result (a hit can always be evaded with at least 5% chance to hit) are left out of the tests:
 *  - 10000 Health and Max Health, so the damage is never cut short by the death of the target,
 *  - 10000 Accuracy against 0 Evasion, so every hit lands,
 *  - 0 Critical Strike, Spell Suppression and Block chance, so none of them happen until a test sets them to 100,
 *  - Max Stagger Meter that can't be filled, the Hero also can't reach the Max Experience.
 *
 * A new environment should be created for every test, so the tests never share state.
 */
struct FObsidianCombatTestEnvironment
{
	FObsidianCombatTestEnvironment();

	static constexpr float DefaultHealth = 10000.0f;

	/**
	 * Result of a Modifier Magnitude Calculation evaluated for the given Combatant, the same way it happens when
	 * the Combatant applies the Gameplay Effect with the calculation to itself.
	 */
	static float EvaluateMagnitudeCalculation(const TSubclassOf<UGameplayModMagnitudeCalculation>& InCalculationClass,
		const AObsidianTestCombatant& InCombatant);

	/**
	 * Applies an instant Gameplay Effect with the Damage Execution, the damage is passed the way Damage Abilities do it,
	 * as Set By Caller magnitudes of Damage Type tags. Uses UObsidianDamageExecution when the execution is not provided.
	 */
	static FObsidianTestDamageResult ApplyDamage(const AObsidianTestCombatant& InSource, const AObsidianTestCombatant& InTarget,
		const TMap<FGameplayTag, float>& InDamageTypes, const TSubclassOf<UGameplayEffectExecutionCalculation>& InExecutionClass = nullptr);

	/** Applies an instant Gameplay Effect that adds the magnitude to the attribute of the target. */
	static void ApplyInstantModifier(const AObsidianTestCombatant& InSource, const AObsidianTestCombatant& InTarget,
		const FGameplayAttribute& InAttribute, const float InMagnitude);

	static void AddTag(const AObsidianTestCombatant& InCombatant, const FGameplayTag& InTag);

	/** Declared before the Spawner, so it is destroyed after the world of the Spawner. */
	FObsidianTestWorldCleanup WorldCleanup;
	FActorTestSpawner Spawner;
	AObsidianTestCombatant_Hero* Hero = nullptr;
	AObsidianTestCombatant_Enemy* Enemy = nullptr;
};

#endif // WITH_DEV_AUTOMATION_TESTS
