// Copyright 2026 out of sCope team - intrxx

#include "CoreMinimal.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "CQTest.h"

#include "AbilitySystem/Attributes/ObsidianEnemyAttributeSet.h"
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "AbilitySystem/Executions/EnemyObsidianDamageExecution.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "ObsidianCombatTestActors.h"
#include "ObsidianCombatTestFixture.h"


/**
 * Tests of UObsidianDamageExecution and UEnemyObsidianDamageExecution, run them from
 * Session Frontend -> Automation -> Obsidian.Combat.DamageExecution.
 *
 * The Hero is the one dealing damage to the Enemy, unless the test is about damaging the Player.
 */
TEST_CLASS(ObsidianDamageExecutionTests, "Obsidian.Combat.DamageExecution")
{
	TUniquePtr<FObsidianCombatTestEnvironment> Env;

	FGameplayTag Physical = ObsidianGameplayTags::DamageType::Physical;
	FGameplayTag Fire = ObsidianGameplayTags::DamageType::Elemental::Fire;
	FGameplayTag Cold = ObsidianGameplayTags::DamageType::Elemental::Cold;
	FGameplayTag Lightning = ObsidianGameplayTags::DamageType::Elemental::Lightning;
	FGameplayTag Chaos = ObsidianGameplayTags::DamageType::Chaos;

	static constexpr float Tolerance = 0.01f;

	BEFORE_EACH()
	{
		Env = MakeUnique<FObsidianCombatTestEnvironment>();
	}

	AFTER_EACH()
	{
		Env.Reset();
	}

	FObsidianTestDamageResult HeroDamagesEnemy(const TMap<FGameplayTag, float>& InDamageTypes) const
	{
		return Env->ApplyDamage(*Env->Hero, *Env->Enemy, InDamageTypes);
	}

	FObsidianTestDamageResult EnemyDamagesHero(const TMap<FGameplayTag, float>& InDamageTypes) const
	{
		return Env->ApplyDamage(*Env->Enemy, *Env->Hero, InDamageTypes, UEnemyObsidianDamageExecution::StaticClass());
	}

	/**
	 * Damage gathering.
	 */

	TEST_METHOD(Damage_NoMitigation_DealsFullDamage)
	{
		const FObsidianTestDamageResult Result = HeroDamagesEnemy({{Physical, 100.0f}});

		ASSERT_THAT(IsNear(100.0f, Result.DamageTaken, Tolerance));
		ASSERT_THAT(IsNear(FObsidianCombatTestEnvironment::DefaultHealth - 100.0f, Env->Enemy->GetCommonAttributeSet()->GetHealth(), Tolerance));
		ASSERT_THAT(IsFalse(Result.bIsCriticalAttack));
		ASSERT_THAT(IsFalse(Result.bIsEvadedHit));
		ASSERT_THAT(IsFalse(Result.bIsSuppressedSpell));
		ASSERT_THAT(IsFalse(Result.bIsBlockedAttack));
		ASSERT_THAT(IsFalse(Result.bIsTargetImmune));
	}

	TEST_METHOD(Damage_EveryDamageType_IsSummed)
	{
		const FObsidianTestDamageResult Result = HeroDamagesEnemy({{Physical, 10.0f}, {Fire, 20.0f}, {Cold, 30.0f},
			{Lightning, 40.0f}, {Chaos, 50.0f}});

		ASSERT_THAT(IsNear(150.0f, Result.DamageTaken, Tolerance));
	}

	TEST_METHOD(Damage_NoDamage_LeavesTargetUntouched)
	{
		const FObsidianTestDamageResult Result = HeroDamagesEnemy({});

		ASSERT_THAT(IsNear(0.0f, Result.DamageTaken, Tolerance));
		ASSERT_THAT(IsNear(0.0f, Env->Enemy->GetCommonAttributeSet()->GetStaggerMeter(), Tolerance));
	}

	TEST_METHOD(FlatDamage_OfSource_IsAddedToDamageOfTheSameType)
	{
		UObsidianCommonAttributeSet* HeroAttributes = Env->Hero->GetCommonAttributeSet();
		HeroAttributes->SetMinFlatFireDamage(20.0f);
		HeroAttributes->SetMaxFlatFireDamage(20.0f);
		Env->Enemy->GetCommonAttributeSet()->SetFireResistance(50.0f);

		const FObsidianTestDamageResult Result = HeroDamagesEnemy({{Fire, 30.0f}});

		// (30 + 20) Fire Damage, halved by the Fire Resistance.
		ASSERT_THAT(IsNear(25.0f, Result.DamageTaken, Tolerance));
	}

	TEST_METHOD(FlatDamage_OfSource_IsDealtWithoutDamageFromAbility)
	{
		UObsidianCommonAttributeSet* HeroAttributes = Env->Hero->GetCommonAttributeSet();
		HeroAttributes->SetMinFlatPhysicalDamage(1.0f);
		HeroAttributes->SetMaxFlatPhysicalDamage(1.0f);
		HeroAttributes->SetMinFlatFireDamage(2.0f);
		HeroAttributes->SetMaxFlatFireDamage(2.0f);
		HeroAttributes->SetMinFlatColdDamage(3.0f);
		HeroAttributes->SetMaxFlatColdDamage(3.0f);
		HeroAttributes->SetMinFlatLightningDamage(4.0f);
		HeroAttributes->SetMaxFlatLightningDamage(4.0f);
		HeroAttributes->SetMinFlatChaosDamage(5.0f);
		HeroAttributes->SetMaxFlatChaosDamage(5.0f);

		const FObsidianTestDamageResult Result = HeroDamagesEnemy({});

		ASSERT_THAT(IsNear(15.0f, Result.DamageTaken, Tolerance));
	}

	TEST_METHOD(FlatDamage_IsRolledBetweenMinAndMax)
	{
		UObsidianCommonAttributeSet* HeroAttributes = Env->Hero->GetCommonAttributeSet();
		HeroAttributes->SetMinFlatColdDamage(10.0f);
		HeroAttributes->SetMaxFlatColdDamage(20.0f);

		for (int32 i = 0; i < 50; ++i)
		{
			const FObsidianTestDamageResult Result = HeroDamagesEnemy({});

			ASSERT_THAT(IsTrue(Result.DamageTaken >= 10.0f - Tolerance && Result.DamageTaken <= 20.0f + Tolerance));
		}
	}

	/**
	 * Resistances.
	 */

	TEST_METHOD(FireResistance_ReducesFireDamage)
	{
		Env->Enemy->GetCommonAttributeSet()->SetFireResistance(75.0f);

		ASSERT_THAT(IsNear(25.0f, HeroDamagesEnemy({{Fire, 100.0f}}).DamageTaken, Tolerance));
	}

	TEST_METHOD(ColdResistance_ReducesColdDamage)
	{
		Env->Enemy->GetCommonAttributeSet()->SetColdResistance(75.0f);

		ASSERT_THAT(IsNear(25.0f, HeroDamagesEnemy({{Cold, 100.0f}}).DamageTaken, Tolerance));
	}

	TEST_METHOD(LightningResistance_ReducesLightningDamage)
	{
		Env->Enemy->GetCommonAttributeSet()->SetLightningResistance(75.0f);

		ASSERT_THAT(IsNear(25.0f, HeroDamagesEnemy({{Lightning, 100.0f}}).DamageTaken, Tolerance));
	}

	TEST_METHOD(ChaosResistance_ReducesChaosDamage)
	{
		Env->Enemy->GetCommonAttributeSet()->SetChaosResistance(75.0f);

		ASSERT_THAT(IsNear(25.0f, HeroDamagesEnemy({{Chaos, 100.0f}}).DamageTaken, Tolerance));
	}

	TEST_METHOD(Resistance_DoesNotReduceOtherDamageTypes)
	{
		UObsidianCommonAttributeSet* EnemyAttributes = Env->Enemy->GetCommonAttributeSet();
		EnemyAttributes->SetFireResistance(75.0f);

		ASSERT_THAT(IsNear(100.0f, HeroDamagesEnemy({{Physical, 100.0f}}).DamageTaken, Tolerance));
		ASSERT_THAT(IsNear(100.0f, HeroDamagesEnemy({{Cold, 100.0f}}).DamageTaken, Tolerance));
		ASSERT_THAT(IsNear(100.0f, HeroDamagesEnemy({{Lightning, 100.0f}}).DamageTaken, Tolerance));
		ASSERT_THAT(IsNear(100.0f, HeroDamagesEnemy({{Chaos, 100.0f}}).DamageTaken, Tolerance));
	}

	TEST_METHOD(ColdResistance_DoesNotReduceChaosDamage)
	{
		Env->Enemy->GetCommonAttributeSet()->SetColdResistance(75.0f);

		ASSERT_THAT(IsNear(100.0f, HeroDamagesEnemy({{Chaos, 100.0f}}).DamageTaken, Tolerance));
	}

	TEST_METHOD(Resistance_Negative_IncreasesDamage)
	{
		Env->Enemy->GetCommonAttributeSet()->SetFireResistance(-50.0f);

		ASSERT_THAT(IsNear(150.0f, HeroDamagesEnemy({{Fire, 100.0f}}).DamageTaken, Tolerance));
	}

	TEST_METHOD(Resistance_OfSource_DoesNotReduceDamage)
	{
		Env->Hero->GetCommonAttributeSet()->SetFireResistance(75.0f);

		ASSERT_THAT(IsNear(100.0f, HeroDamagesEnemy({{Fire, 100.0f}}).DamageTaken, Tolerance));
	}

	/**
	 * Armor.
	 */

	TEST_METHOD(Armor_ReducesPhysicalDamage)
	{
		Env->Enemy->GetCommonAttributeSet()->SetArmor(500.0f);

		// Mitigation is Armor / (Armor + 5 * Damage), so 500 Armor takes away half of the 100 damage hit.
		ASSERT_THAT(IsNear(50.0f, HeroDamagesEnemy({{Physical, 100.0f}}).DamageTaken, Tolerance));
	}

	TEST_METHOD(Armor_IsLessEffectiveAgainstBiggerHits)
	{
		Env->Enemy->GetCommonAttributeSet()->SetArmor(500.0f);

		const float SmallHit = HeroDamagesEnemy({{Physical, 100.0f}}).DamageTaken;
		const float BigHit = HeroDamagesEnemy({{Physical, 1000.0f}}).DamageTaken;

		ASSERT_THAT(IsTrue(SmallHit < 100.0f - 1.0f, TEXT("Armor should mitigate a meaningful part of the small hit.")));
		ASSERT_THAT(IsTrue(BigHit < 1000.0f - 1.0f, TEXT("Armor should mitigate a meaningful part of the big hit.")));
		ASSERT_THAT(IsTrue((SmallHit / 100.0f) < (BigHit / 1000.0f), TEXT("The bigger hit should get more of its damage through the Armor.")));
	}

	TEST_METHOD(Armor_DoesNotReduceNonPhysicalDamage)
	{
		Env->Enemy->GetCommonAttributeSet()->SetArmor(500.0f);

		ASSERT_THAT(IsNear(100.0f, HeroDamagesEnemy({{Fire, 100.0f}}).DamageTaken, Tolerance));
		ASSERT_THAT(IsNear(100.0f, HeroDamagesEnemy({{Chaos, 100.0f}}).DamageTaken, Tolerance));
	}

	/**
	 * Critical Strikes.
	 */

	TEST_METHOD(CriticalStrike_FullChance_MultipliesDamage)
	{
		UObsidianCommonAttributeSet* HeroAttributes = Env->Hero->GetCommonAttributeSet();
		HeroAttributes->SetCriticalStrikeChance(100.0f);
		HeroAttributes->SetCriticalStrikeDamageMultiplier(150.0f);

		const FObsidianTestDamageResult Result = HeroDamagesEnemy({{Physical, 100.0f}});

		ASSERT_THAT(IsTrue(Result.bIsCriticalAttack));
		ASSERT_THAT(IsNear(150.0f, Result.DamageTaken, Tolerance));
	}

	TEST_METHOD(CriticalStrike_NoChance_NeverHappens)
	{
		UObsidianCommonAttributeSet* HeroAttributes = Env->Hero->GetCommonAttributeSet();
		HeroAttributes->SetCriticalStrikeChance(0.0f);
		HeroAttributes->SetCriticalStrikeDamageMultiplier(150.0f);

		for (int32 i = 0; i < 50; ++i)
		{
			const FObsidianTestDamageResult Result = HeroDamagesEnemy({{Physical, 100.0f}});

			ASSERT_THAT(IsFalse(Result.bIsCriticalAttack));
			ASSERT_THAT(IsNear(100.0f, Result.DamageTaken, Tolerance));
		}
	}

	TEST_METHOD(CriticalStrike_ChanceOfTarget_DoesNotMatter)
	{
		UObsidianCommonAttributeSet* EnemyAttributes = Env->Enemy->GetCommonAttributeSet();
		EnemyAttributes->SetCriticalStrikeChance(100.0f);
		EnemyAttributes->SetCriticalStrikeDamageMultiplier(150.0f);

		const FObsidianTestDamageResult Result = HeroDamagesEnemy({{Physical, 100.0f}});

		ASSERT_THAT(IsFalse(Result.bIsCriticalAttack));
		ASSERT_THAT(IsNear(100.0f, Result.DamageTaken, Tolerance));
	}

	/**
	 * Spell Suppression.
	 */

	TEST_METHOD(SpellSuppression_FullChance_ReducesDamageByMagnitude)
	{
		UObsidianCommonAttributeSet* EnemyAttributes = Env->Enemy->GetCommonAttributeSet();
		EnemyAttributes->SetSpellSuppressionChance(100.0f);
		EnemyAttributes->SetSpellSuppressionMagnitude(40.0f);

		const FObsidianTestDamageResult Result = HeroDamagesEnemy({{Fire, 100.0f}});

		ASSERT_THAT(IsTrue(Result.bIsSuppressedSpell));
		ASSERT_THAT(IsNear(60.0f, Result.DamageTaken, Tolerance));
	}

	TEST_METHOD(SpellSuppression_NoChance_NeverHappens)
	{
		UObsidianCommonAttributeSet* EnemyAttributes = Env->Enemy->GetCommonAttributeSet();
		EnemyAttributes->SetSpellSuppressionChance(0.0f);
		EnemyAttributes->SetSpellSuppressionMagnitude(40.0f);

		for (int32 i = 0; i < 50; ++i)
		{
			const FObsidianTestDamageResult Result = HeroDamagesEnemy({{Fire, 100.0f}});

			ASSERT_THAT(IsFalse(Result.bIsSuppressedSpell));
			ASSERT_THAT(IsNear(100.0f, Result.DamageTaken, Tolerance));
		}
	}

	/**
	 * Hit chance.
	 */

	TEST_METHOD(Evasion_AccuracyFarAboveEvasion_HitAlwaysLands)
	{
		Env->Enemy->GetCommonAttributeSet()->SetEvasion(100.0f);

		for (int32 i = 0; i < 50; ++i)
		{
			const FObsidianTestDamageResult Result = HeroDamagesEnemy({{Physical, 100.0f}});

			ASSERT_THAT(IsFalse(Result.bIsEvadedHit));
			ASSERT_THAT(IsNear(100.0f, Result.DamageTaken, Tolerance));
		}
	}

	TEST_METHOD(Evasion_Hit_EitherLandsFullyOrIsEvaded)
	{
		Env->Hero->GetCommonAttributeSet()->SetAccuracy(100.0f);
		Env->Enemy->GetCommonAttributeSet()->SetEvasion(5000.0f);

		for (int32 i = 0; i < 100; ++i)
		{
			const FObsidianTestDamageResult Result = HeroDamagesEnemy({{Physical, 100.0f}});

			ASSERT_THAT(IsNear(Result.bIsEvadedHit ? 0.0f : 100.0f, Result.DamageTaken, Tolerance));
		}
	}

	/**
	 * Immunity.
	 */

	TEST_METHOD(Immunity_TargetWithImmunityTag_TakesNoDamage)
	{
		Env->AddTag(*Env->Enemy, ObsidianGameplayTags::Status::Immunity);

		const FObsidianTestDamageResult Result = HeroDamagesEnemy({{Physical, 100.0f}, {Fire, 100.0f}});

		ASSERT_THAT(IsTrue(Result.bIsTargetImmune));
		ASSERT_THAT(IsNear(0.0f, Result.DamageTaken, Tolerance));
		ASSERT_THAT(IsNear(0.0f, Env->Enemy->GetCommonAttributeSet()->GetStaggerMeter(), Tolerance));
	}

	TEST_METHOD(Immunity_SourceWithImmunityTag_StillDealsDamage)
	{
		Env->AddTag(*Env->Hero, ObsidianGameplayTags::Status::Immunity);

		const FObsidianTestDamageResult Result = HeroDamagesEnemy({{Physical, 100.0f}});

		ASSERT_THAT(IsFalse(Result.bIsTargetImmune));
		ASSERT_THAT(IsNear(100.0f, Result.DamageTaken, Tolerance));
	}

	/**
	 * Damage taken multipliers.
	 */

	TEST_METHOD(AllDamageMultiplier_OfTarget_ScalesDamageTaken)
	{
		Env->Enemy->GetCommonAttributeSet()->SetAllDamageMultiplier(0.5f);

		ASSERT_THAT(IsNear(50.0f, HeroDamagesEnemy({{Physical, 60.0f}, {Fire, 40.0f}}).DamageTaken, Tolerance));
	}

	TEST_METHOD(StaggerDamageTakenMultiplier_OfTarget_IncreasesDamageTaken)
	{
		Env->Enemy->GetCommonAttributeSet()->SetStaggerDamageTakenMultiplier(50.0f);

		ASSERT_THAT(IsNear(150.0f, HeroDamagesEnemy({{Physical, 100.0f}}).DamageTaken, Tolerance));
	}

	/**
	 * Stagger.
	 */

	TEST_METHOD(Stagger_Damage_FillsStaggerMeterOfTarget)
	{
		const UObsidianCommonAttributeSet* EnemyAttributes = Env->Enemy->GetCommonAttributeSet();

		HeroDamagesEnemy({{Physical, 100.0f}});
		const float StaggerAfterFirstHit = EnemyAttributes->GetStaggerMeter();
		HeroDamagesEnemy({{Physical, 200.0f}});
		const float StaggerAfterSecondHit = EnemyAttributes->GetStaggerMeter();

		ASSERT_THAT(IsTrue(StaggerAfterFirstHit > 0.0f));
		// The stagger is proportional to the damage, the second hit was twice as strong.
		ASSERT_THAT(IsNear(StaggerAfterFirstHit * 3.0f, StaggerAfterSecondHit, 2.0f));
	}

	/**
	 * Blocking, only Enemies damaging the Player can be blocked.
	 */

	TEST_METHOD(Block_FullHitBlockChance_BlocksAllDamage)
	{
		Env->Hero->GetHeroAttributeSet()->SetHitBlockChance(100.0f);

		const FObsidianTestDamageResult Result = EnemyDamagesHero({{Physical, 100.0f}});

		ASSERT_THAT(IsTrue(Result.bIsBlockedAttack));
		ASSERT_THAT(IsNear(0.0f, Result.DamageTaken, Tolerance));
	}

	TEST_METHOD(Block_FullSpellBlockChance_BlocksAllDamage)
	{
		Env->Hero->GetHeroAttributeSet()->SetSpellBlockChance(100.0f);

		const FObsidianTestDamageResult Result = EnemyDamagesHero({{Fire, 100.0f}});

		ASSERT_THAT(IsTrue(Result.bIsBlockedAttack));
		ASSERT_THAT(IsNear(0.0f, Result.DamageTaken, Tolerance));
	}

	TEST_METHOD(Block_NoChance_DamageIsCalculatedAsUsual)
	{
		UObsidianHeroAttributeSet* HeroAttributes = Env->Hero->GetHeroAttributeSet();
		HeroAttributes->SetHitBlockChance(0.0f);
		HeroAttributes->SetSpellBlockChance(0.0f);
		HeroAttributes->SetFireResistance(75.0f);

		for (int32 i = 0; i < 50; ++i)
		{
			const FObsidianTestDamageResult Result = EnemyDamagesHero({{Fire, 100.0f}});

			ASSERT_THAT(IsFalse(Result.bIsBlockedAttack));
			ASSERT_THAT(IsNear(25.0f, Result.DamageTaken, Tolerance));
		}
	}
};

#endif // WITH_DEV_AUTOMATION_TESTS
