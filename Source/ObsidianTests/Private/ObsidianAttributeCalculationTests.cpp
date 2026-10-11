// Copyright 2026 out of sCope team - intrxx

#include "CoreMinimal.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "CQTest.h"

#include "AbilitySystem/Attributes/ObsidianEnemyAttributeSet.h"
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "AbilitySystem/ModMagCalculations/ObsidianMMC_Accuracy.h"
#include "AbilitySystem/ModMagCalculations/ObsidianMMC_Armor.h"
#include "AbilitySystem/ModMagCalculations/ObsidianMMC_ChaosResistance.h"
#include "AbilitySystem/ModMagCalculations/ObsidianMMC_EnemyAccuracy.h"
#include "AbilitySystem/ModMagCalculations/ObsidianMMC_Evasion.h"
#include "AbilitySystem/ModMagCalculations/ObsidianMMC_MaxEnergyShield.h"
#include "AbilitySystem/ModMagCalculations/ObsidianMMC_MaxHealth.h"
#include "AbilitySystem/ModMagCalculations/ObsidianMMC_MaxMana.h"
#include "AbilitySystem/ModMagCalculations/ObsidianMMC_MaxStamina.h"
#include "AbilitySystem/ModMagCalculations/SpecialResource/ObsidianMMC_MaxSpecialResource_Assassin.h"
#include "AbilitySystem/ModMagCalculations/SpecialResource/ObsidianMMC_MaxSpecialResource_Barbarian.h"
#include "AbilitySystem/ModMagCalculations/SpecialResource/ObsidianMMC_MaxSpecialResource_Paladin.h"
#include "AbilitySystem/ModMagCalculations/SpecialResource/ObsidianMMC_MaxSpecialResource_Witch.h"
#include "ObsidianCombatTestActors.h"
#include "ObsidianCombatTestFixture.h"


/**
 * Tests of the Modifier Magnitude Calculations that derive attributes from other attributes, run them from
 * Session Frontend -> Automation -> Obsidian.Combat.AttributeCalculations.
 *
 * Every calculation returns the bonus that gets added to the attribute, not the final value of the attribute.
 */
TEST_CLASS(ObsidianAttributeCalculationTests, "Obsidian.Combat.AttributeCalculations")
{
	TUniquePtr<FObsidianCombatTestEnvironment> Env;

	static constexpr float Tolerance = 0.01f;

	BEFORE_EACH()
	{
		Env = MakeUnique<FObsidianCombatTestEnvironment>();
	}

	AFTER_EACH()
	{
		Env.Reset();
	}

	UObsidianHeroAttributeSet& HeroAttributes() const
	{
		return *Env->Hero->GetHeroAttributeSet();
	}

	template<typename CalculationClass>
	float CalculateForHero() const
	{
		return Env->EvaluateMagnitudeCalculation(CalculationClass::StaticClass(), *Env->Hero);
	}

	/**
	 * Defences.
	 */

	TEST_METHOD(Armor_IsIncreasedByStrengthAndIncreasedArmorPercent)
	{
		HeroAttributes().SetArmor(100.0f);
		HeroAttributes().SetIncreasedArmorPercent(50.0f);
		HeroAttributes().SetStrength(40.0f);

		// 1 for every 4 Strength + 50% of 100 Armor.
		ASSERT_THAT(IsNear(60.0f, CalculateForHero<UObsidianMMC_Armor>(), Tolerance));
	}

	TEST_METHOD(Armor_StrengthBonus_IsRoundedDown)
	{
		HeroAttributes().SetStrength(43.0f);

		ASSERT_THAT(IsNear(10.0f, CalculateForHero<UObsidianMMC_Armor>(), Tolerance));
	}

	TEST_METHOD(Armor_NegativeAttributes_GiveNoBonus)
	{
		HeroAttributes().SetArmor(100.0f);
		HeroAttributes().SetIncreasedArmorPercent(-50.0f);
		HeroAttributes().SetStrength(-40.0f);

		ASSERT_THAT(IsNear(0.0f, CalculateForHero<UObsidianMMC_Armor>(), Tolerance));
	}

	TEST_METHOD(Evasion_IsIncreasedByDexterityAndIncreasedEvasionPercent)
	{
		HeroAttributes().SetEvasion(200.0f);
		HeroAttributes().SetIncreasedEvasionPercent(25.0f);
		HeroAttributes().SetDexterity(11.0f);

		// 3 for every 2 Dexterity + 25% of 200 Evasion.
		ASSERT_THAT(IsNear(65.0f, CalculateForHero<UObsidianMMC_Evasion>(), Tolerance));
	}

	TEST_METHOD(MaxEnergyShield_IsIncreasedByIntelligenceAndIncreasedEnergyShieldPercentage)
	{
		HeroAttributes().SetMaxEnergyShield(100.0f);
		HeroAttributes().SetIncreasedEnergyShieldPercentage(50.0f);
		HeroAttributes().SetIntelligence(10.0f);

		// 3 for every Intelligence + 50% of 100 Max Energy Shield.
		ASSERT_THAT(IsNear(80.0f, CalculateForHero<UObsidianMMC_MaxEnergyShield>(), Tolerance));
	}

	TEST_METHOD(MaxEnergyShield_IsNotIncreasedByHealthAttributes)
	{
		HeroAttributes().SetIntelligence(10.0f);
		HeroAttributes().SetIncreasedHealthPercentage(50.0f);

		// The Hero has 10000 Max Health, none of it should turn into Energy Shield.
		ASSERT_THAT(IsNear(30.0f, CalculateForHero<UObsidianMMC_MaxEnergyShield>(), Tolerance));
	}

	TEST_METHOD(ChaosResistance_IsIncreasedByFaith)
	{
		HeroAttributes().SetFaith(10.0f);

		// 1 for every 3 Faith.
		ASSERT_THAT(IsNear(3.0f, CalculateForHero<UObsidianMMC_ChaosResistance>(), Tolerance));
	}

	/**
	 * Accuracy.
	 */

	TEST_METHOD(Accuracy_IsIncreasedByDexterity)
	{
		HeroAttributes().SetDexterity(11.0f);

		// 5 for every 2 Dexterity.
		ASSERT_THAT(IsNear(25.0f, CalculateForHero<UObsidianMMC_Accuracy>(), Tolerance));
	}

	TEST_METHOD(EnemyAccuracy_GrowsWithEnemyLevel)
	{
		Env->Enemy->SetCombatantLevel(1);
		const float AccuracyAtFirstLevel = Env->EvaluateMagnitudeCalculation(UObsidianMMC_EnemyAccuracy::StaticClass(), *Env->Enemy);

		Env->Enemy->SetCombatantLevel(10);
		const float AccuracyAtTenthLevel = Env->EvaluateMagnitudeCalculation(UObsidianMMC_EnemyAccuracy::StaticClass(), *Env->Enemy);

		ASSERT_THAT(IsNear(31.0f, AccuracyAtFirstLevel, Tolerance));
		ASSERT_THAT(IsNear(85.0f, AccuracyAtTenthLevel, Tolerance));
	}

	/**
	 * Resources.
	 */

	TEST_METHOD(MaxHealth_IsIncreasedByStrengthDexterityAndIncreasedHealthPercentage)
	{
		HeroAttributes().SetMaxHealth(200.0f);
		HeroAttributes().SetIncreasedHealthPercentage(10.0f);
		HeroAttributes().SetStrength(10.0f);
		HeroAttributes().SetDexterity(5.0f);

		// 2 for every Strength + 1 for every Dexterity + 10% of 200 Max Health.
		ASSERT_THAT(IsNear(45.0f, CalculateForHero<UObsidianMMC_MaxHealth>(), Tolerance));
	}

	TEST_METHOD(MaxMana_IsIncreasedByFaithDexterityAndIncreasedManaPercentage)
	{
		HeroAttributes().SetMaxMana(100.0f);
		HeroAttributes().SetIncreasedManaPercentage(20.0f);
		HeroAttributes().SetFaith(10.0f);
		HeroAttributes().SetDexterity(5.0f);

		// 2 for every Faith + 1 for every Dexterity + 20% of 100 Max Mana.
		ASSERT_THAT(IsNear(45.0f, CalculateForHero<UObsidianMMC_MaxMana>(), Tolerance));
	}

	TEST_METHOD(MaxStamina_IsIncreasedByStrengthAndDexterity)
	{
		HeroAttributes().SetStrength(10.0f);
		HeroAttributes().SetDexterity(5.0f);

		// 1 for every Strength + 2 for every Dexterity, the bonus for the level comes from a Curve Table set in content.
		ASSERT_THAT(IsNear(20.0f, CalculateForHero<UObsidianMMC_MaxStamina>(), Tolerance));
	}

	/**
	 * Special Resources of Hero classes.
	 */

	TEST_METHOD(MaxSpecialResource_Witch_IsIncreasedByIntelligence)
	{
		HeroAttributes().SetIntelligence(13.0f);

		// 1 for every 6 Intelligence.
		ASSERT_THAT(IsNear(2.0f, CalculateForHero<UObsidianMMC_MaxSpecialResource_Witch>(), Tolerance));
	}

	TEST_METHOD(MaxSpecialResource_Barbarian_IsIncreasedByStrength)
	{
		HeroAttributes().SetStrength(13.0f);

		// 1 for every 6 Strength.
		ASSERT_THAT(IsNear(2.0f, CalculateForHero<UObsidianMMC_MaxSpecialResource_Barbarian>(), Tolerance));
	}

	TEST_METHOD(MaxSpecialResource_Paladin_IsIncreasedByFaith)
	{
		HeroAttributes().SetFaith(13.0f);

		// 1 for every 5 Faith.
		ASSERT_THAT(IsNear(2.0f, CalculateForHero<UObsidianMMC_MaxSpecialResource_Paladin>(), Tolerance));
	}

	TEST_METHOD(MaxSpecialResource_Assassin_IsIncreasedByIntelligenceAndDexterity)
	{
		HeroAttributes().SetIntelligence(25.0f);
		HeroAttributes().SetDexterity(19.0f);

		// 1 for every 10 Intelligence + 1 for every 10 Dexterity, both rounded down on their own.
		ASSERT_THAT(IsNear(3.0f, CalculateForHero<UObsidianMMC_MaxSpecialResource_Assassin>(), Tolerance));
	}
};

#endif // WITH_DEV_AUTOMATION_TESTS
