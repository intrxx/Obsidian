// Copyright 2026 out of sCope team - intrxx

#include "CoreMinimal.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "CQTest.h"

#include "AbilitySystem/Attributes/ObsidianEnemyAttributeSet.h"
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "ObsidianCombatTestActors.h"
#include "ObsidianCombatTestFixture.h"


/**
 * Tests of how UObsidianCommonAttributeSet, UObsidianEnemyAttributeSet and UObsidianHeroAttributeSet react to
 * Gameplay Effects, run them from Session Frontend -> Automation -> Obsidian.Combat.AttributeSets.
 */
TEST_CLASS(ObsidianAttributeSetTests, "Obsidian.Combat.AttributeSets")
{
	TUniquePtr<FObsidianCombatTestEnvironment> Env;

	FGameplayTag Physical = ObsidianGameplayTags::DamageType::Physical;

	static constexpr float Tolerance = 0.01f;

	BEFORE_EACH()
	{
		Env = MakeUnique<FObsidianCombatTestEnvironment>();
	}

	AFTER_EACH()
	{
		Env.Reset();
	}

	UObsidianEnemyAttributeSet& EnemyAttributes() const
	{
		return *Env->Enemy->GetEnemyAttributeSet();
	}

	UObsidianHeroAttributeSet& HeroAttributes() const
	{
		return *Env->Hero->GetHeroAttributeSet();
	}

	void HeroDamagesEnemy(const float InDamage) const
	{
		Env->ApplyDamage(*Env->Hero, *Env->Enemy, {{Physical, InDamage}});
	}

	void EnemyDamagesHero(const float InDamage) const
	{
		Env->ApplyDamage(*Env->Enemy, *Env->Hero, {{Physical, InDamage}});
	}

	/** Adds the magnitude to the attribute of the Enemy with an instant Gameplay Effect applied by the Enemy itself. */
	void ModifyEnemy(const FGameplayAttribute& InAttribute, const float InMagnitude) const
	{
		Env->ApplyInstantModifier(*Env->Enemy, *Env->Enemy, InAttribute, InMagnitude);
	}

	void ModifyHero(const FGameplayAttribute& InAttribute, const float InMagnitude) const
	{
		Env->ApplyInstantModifier(*Env->Hero, *Env->Hero, InAttribute, InMagnitude);
	}

	void SetEnemyHealthPool(const float InHealth, const float InEnergyShield) const
	{
		EnemyAttributes().SetHealth(InHealth);
		EnemyAttributes().SetMaxEnergyShield(InEnergyShield);
		EnemyAttributes().SetEnergyShield(InEnergyShield);
	}

	/**
	 * Incoming Damage.
	 */

	TEST_METHOD(IncomingDamage_NoEnergyShield_LowersHealth)
	{
		SetEnemyHealthPool(100.0f, 0.0f);

		HeroDamagesEnemy(30.0f);

		ASSERT_THAT(IsNear(70.0f, EnemyAttributes().GetHealth(), Tolerance));
	}

	TEST_METHOD(IncomingDamage_EnergyShield_IsDepletedBeforeHealth)
	{
		SetEnemyHealthPool(100.0f, 50.0f);

		HeroDamagesEnemy(30.0f);

		ASSERT_THAT(IsNear(20.0f, EnemyAttributes().GetEnergyShield(), Tolerance));
		ASSERT_THAT(IsNear(100.0f, EnemyAttributes().GetHealth(), Tolerance));
	}

	TEST_METHOD(IncomingDamage_MoreThanEnergyShield_RestLowersHealth)
	{
		SetEnemyHealthPool(100.0f, 50.0f);

		HeroDamagesEnemy(80.0f);

		ASSERT_THAT(IsNear(0.0f, EnemyAttributes().GetEnergyShield(), Tolerance));
		ASSERT_THAT(IsNear(70.0f, EnemyAttributes().GetHealth(), Tolerance));
	}

	TEST_METHOD(IncomingDamage_MoreThanHealthPool_HealthStopsAtZero)
	{
		SetEnemyHealthPool(100.0f, 50.0f);

		HeroDamagesEnemy(5000.0f);

		ASSERT_THAT(IsNear(0.0f, EnemyAttributes().GetEnergyShield(), Tolerance));
		ASSERT_THAT(IsNear(0.0f, EnemyAttributes().GetHealth(), Tolerance));
	}

	TEST_METHOD(IncomingDamage_IsResetAfterItIsApplied)
	{
		HeroDamagesEnemy(30.0f);
		EnemyDamagesHero(30.0f);

		ASSERT_THAT(IsNear(0.0f, EnemyAttributes().GetIncomingDamage(), Tolerance));
		ASSERT_THAT(IsNear(0.0f, EnemyAttributes().GetIncomingStaggerMagnitude(), Tolerance));
		ASSERT_THAT(IsNear(0.0f, HeroAttributes().GetIncomingDamage(), Tolerance));
		ASSERT_THAT(IsNear(0.0f, HeroAttributes().GetIncomingStaggerMagnitude(), Tolerance));
	}

	/**
	 * Running out of Health.
	 */

	TEST_METHOD(OutOfHealth_LethalDamage_IsBroadcastOnce)
	{
		SetEnemyHealthPool(100.0f, 0.0f);

		int32 BroadcastCount = 0;
		AActor* Instigator = nullptr;
		float OldHealth = 0.0f;
		EnemyAttributes().OnOutOfHealth.AddLambda([&BroadcastCount, &Instigator, &OldHealth](AActor* InEffectInstigator, AActor* InEffectCauser,
			const FGameplayEffectSpec* InEffectSpec, float InEffectMagnitude, float InOldValue, float InNewValue)
			{
				BroadcastCount++;
				Instigator = InEffectInstigator;
				OldHealth = InOldValue;
			});

		HeroDamagesEnemy(150.0f);

		ASSERT_THAT(AreEqual(1, BroadcastCount));
		ASSERT_THAT(IsTrue(Instigator == Env->Hero));
		ASSERT_THAT(IsNear(100.0f, OldHealth, Tolerance));
	}

	TEST_METHOD(OutOfHealth_DamageToDeadTarget_IsNotBroadcastAgain)
	{
		SetEnemyHealthPool(100.0f, 0.0f);

		int32 BroadcastCount = 0;
		EnemyAttributes().OnOutOfHealth.AddLambda([&BroadcastCount](AActor* InEffectInstigator, AActor* InEffectCauser,
			const FGameplayEffectSpec* InEffectSpec, float InEffectMagnitude, float InOldValue, float InNewValue)
			{
				BroadcastCount++;
			});

		HeroDamagesEnemy(150.0f);
		HeroDamagesEnemy(150.0f);

		ASSERT_THAT(AreEqual(1, BroadcastCount));
	}

	TEST_METHOD(OutOfHealth_DamageAbsorbedByHealthPool_IsNotBroadcast)
	{
		SetEnemyHealthPool(10.0f, 50.0f);

		int32 BroadcastCount = 0;
		EnemyAttributes().OnOutOfHealth.AddLambda([&BroadcastCount](AActor* InEffectInstigator, AActor* InEffectCauser,
			const FGameplayEffectSpec* InEffectSpec, float InEffectMagnitude, float InOldValue, float InNewValue)
			{
				BroadcastCount++;
			});

		HeroDamagesEnemy(55.0f);

		ASSERT_THAT(AreEqual(0, BroadcastCount));
		ASSERT_THAT(IsNear(5.0f, EnemyAttributes().GetHealth(), Tolerance));
	}

	TEST_METHOD(OutOfHealth_PlayerKilledByEnemy_IsBroadcast)
	{
		HeroAttributes().SetHealth(100.0f);

		int32 BroadcastCount = 0;
		HeroAttributes().OnOutOfHealth.AddLambda([&BroadcastCount](AActor* InEffectInstigator, AActor* InEffectCauser,
			const FGameplayEffectSpec* InEffectSpec, float InEffectMagnitude, float InOldValue, float InNewValue)
			{
				BroadcastCount++;
			});

		EnemyDamagesHero(150.0f);

		ASSERT_THAT(AreEqual(1, BroadcastCount));
		ASSERT_THAT(IsNear(0.0f, HeroAttributes().GetHealth(), Tolerance));
	}

	/**
	 * Experience for killing.
	 */

	TEST_METHOD(Experience_PlayerKillsEnemy_PlayerGainsExperience)
	{
		SetEnemyHealthPool(100.0f, 0.0f);

		HeroDamagesEnemy(150.0f);

		ASSERT_THAT(IsTrue(HeroAttributes().GetExperience() > 0.0f));
	}

	TEST_METHOD(Experience_PlayerDamagesEnemyWithoutKilling_NoExperienceGained)
	{
		SetEnemyHealthPool(100.0f, 0.0f);

		HeroDamagesEnemy(50.0f);

		ASSERT_THAT(IsNear(0.0f, HeroAttributes().GetExperience(), Tolerance));
	}

	TEST_METHOD(Experience_PlayerDamagesDeadEnemy_NoMoreExperienceGained)
	{
		SetEnemyHealthPool(100.0f, 0.0f);

		HeroDamagesEnemy(150.0f);
		const float ExperienceForKill = HeroAttributes().GetExperience();
		HeroDamagesEnemy(150.0f);

		ASSERT_THAT(IsNear(ExperienceForKill, HeroAttributes().GetExperience(), Tolerance));
	}

	TEST_METHOD(Experience_MaxExperienceForLevel_GrowsWithEveryLevel)
	{
		float PreviousMaxExperience = 0.0f;
		for (uint8 Level = 1; Level <= 100; ++Level)
		{
			const float MaxExperience = UObsidianHeroAttributeSet::GetMaxExperienceForLevel(Level);

			ASSERT_THAT(IsTrue(MaxExperience > PreviousMaxExperience, FString::Printf(TEXT("Level %d needs less experience than the level before it."), Level)));
			PreviousMaxExperience = MaxExperience;
		}
	}

	/**
	 * Health and Energy Shield.
	 */

	TEST_METHOD(Health_ModifiedDirectly_StaysBetweenZeroAndMaxHealth)
	{
		SetEnemyHealthPool(100.0f, 0.0f);
		EnemyAttributes().SetMaxHealth(150.0f);

		ModifyEnemy(UObsidianCommonAttributeSet::GetHealthAttribute(), 500.0f);
		ASSERT_THAT(IsNear(150.0f, EnemyAttributes().GetHealth(), Tolerance));

		// The clamped value needs to be the real one, not only the one that is displayed.
		ModifyEnemy(UObsidianCommonAttributeSet::GetHealthAttribute(), -50.0f);
		ASSERT_THAT(IsNear(100.0f, EnemyAttributes().GetHealth(), Tolerance));

		ModifyEnemy(UObsidianCommonAttributeSet::GetHealthAttribute(), -500.0f);
		ASSERT_THAT(IsNear(0.0f, EnemyAttributes().GetHealth(), Tolerance));

		ModifyEnemy(UObsidianCommonAttributeSet::GetHealthAttribute(), 50.0f);
		ASSERT_THAT(IsNear(50.0f, EnemyAttributes().GetHealth(), Tolerance));
	}

	TEST_METHOD(EnergyShield_ModifiedDirectly_StaysBetweenZeroAndMaxEnergyShield)
	{
		SetEnemyHealthPool(100.0f, 80.0f);

		ModifyEnemy(UObsidianCommonAttributeSet::GetEnergyShieldAttribute(), 500.0f);
		ASSERT_THAT(IsNear(80.0f, EnemyAttributes().GetEnergyShield(), Tolerance));

		ModifyEnemy(UObsidianCommonAttributeSet::GetEnergyShieldAttribute(), -30.0f);
		ASSERT_THAT(IsNear(50.0f, EnemyAttributes().GetEnergyShield(), Tolerance));

		ModifyEnemy(UObsidianCommonAttributeSet::GetEnergyShieldAttribute(), -500.0f);
		ASSERT_THAT(IsNear(0.0f, EnemyAttributes().GetEnergyShield(), Tolerance));

		ModifyEnemy(UObsidianCommonAttributeSet::GetEnergyShieldAttribute(), 30.0f);
		ASSERT_THAT(IsNear(30.0f, EnemyAttributes().GetEnergyShield(), Tolerance));
	}

	TEST_METHOD(IncomingHealthHealing_RestoresHealthUpToMaxHealth)
	{
		SetEnemyHealthPool(100.0f, 0.0f);
		EnemyAttributes().SetMaxHealth(150.0f);

		ModifyEnemy(UObsidianCommonAttributeSet::GetIncomingHealthHealingAttribute(), 30.0f);
		ASSERT_THAT(IsNear(130.0f, EnemyAttributes().GetHealth(), Tolerance));

		ModifyEnemy(UObsidianCommonAttributeSet::GetIncomingHealthHealingAttribute(), 30.0f);
		ASSERT_THAT(IsNear(150.0f, EnemyAttributes().GetHealth(), Tolerance));
		ASSERT_THAT(IsNear(0.0f, EnemyAttributes().GetIncomingHealthHealing(), Tolerance));
	}

	TEST_METHOD(IncomingHealthHealing_DoesNotRestoreEnergyShield)
	{
		SetEnemyHealthPool(100.0f, 50.0f);
		HeroDamagesEnemy(30.0f);

		ModifyEnemy(UObsidianCommonAttributeSet::GetIncomingHealthHealingAttribute(), 30.0f);

		ASSERT_THAT(IsNear(20.0f, EnemyAttributes().GetEnergyShield(), Tolerance));
	}

	TEST_METHOD(IncomingEnergyShieldHealing_RestoresEnergyShieldUpToMaxEnergyShield)
	{
		SetEnemyHealthPool(100.0f, 50.0f);
		HeroDamagesEnemy(80.0f);

		ModifyEnemy(UObsidianCommonAttributeSet::GetIncomingEnergyShieldHealingAttribute(), 20.0f);
		ASSERT_THAT(IsNear(20.0f, EnemyAttributes().GetEnergyShield(), Tolerance));
		ASSERT_THAT(IsNear(70.0f, EnemyAttributes().GetHealth(), Tolerance));

		ModifyEnemy(UObsidianCommonAttributeSet::GetIncomingEnergyShieldHealingAttribute(), 500.0f);
		ASSERT_THAT(IsNear(50.0f, EnemyAttributes().GetEnergyShield(), Tolerance));
		ASSERT_THAT(IsNear(0.0f, EnemyAttributes().GetIncomingEnergyShieldHealing(), Tolerance));
	}

	/**
	 * Stagger Meter.
	 */

	TEST_METHOD(IncomingStagger_IsAddedToStaggerMeter)
	{
		ModifyEnemy(UObsidianCommonAttributeSet::GetIncomingStaggerMagnitudeAttribute(), 100.0f);
		ModifyEnemy(UObsidianCommonAttributeSet::GetIncomingStaggerMagnitudeAttribute(), 50.0f);

		ASSERT_THAT(IsNear(150.0f, EnemyAttributes().GetStaggerMeter(), Tolerance));
		ASSERT_THAT(IsNear(0.0f, EnemyAttributes().GetIncomingStaggerMagnitude(), Tolerance));
	}

	TEST_METHOD(IncomingStagger_IsScaledByStaggerMultiplier)
	{
		EnemyAttributes().SetStaggerMultiplier(0.5f);

		ModifyEnemy(UObsidianCommonAttributeSet::GetIncomingStaggerMagnitudeAttribute(), 100.0f);

		ASSERT_THAT(IsNear(50.0f, EnemyAttributes().GetStaggerMeter(), Tolerance));
	}

	TEST_METHOD(IncomingStagger_OverMaxStaggerMeter_ResetsStaggerMeter)
	{
		EnemyAttributes().SetMaxStaggerMeter(120.0f);

		ModifyEnemy(UObsidianCommonAttributeSet::GetIncomingStaggerMagnitudeAttribute(), 100.0f);
		ASSERT_THAT(IsNear(100.0f, EnemyAttributes().GetStaggerMeter(), Tolerance));

		ModifyEnemy(UObsidianCommonAttributeSet::GetIncomingStaggerMagnitudeAttribute(), 100.0f);
		ASSERT_THAT(IsNear(0.0f, EnemyAttributes().GetStaggerMeter(), Tolerance));
	}

	/**
	 * Hero resources.
	 */

	TEST_METHOD(Mana_ModifiedDirectly_StaysBetweenZeroAndMaxMana)
	{
		HeroAttributes().SetMaxMana(100.0f);
		HeroAttributes().SetMana(100.0f);

		ModifyHero(UObsidianHeroAttributeSet::GetManaAttribute(), 500.0f);
		ASSERT_THAT(IsNear(100.0f, HeroAttributes().GetMana(), Tolerance));

		ModifyHero(UObsidianHeroAttributeSet::GetManaAttribute(), -30.0f);
		ASSERT_THAT(IsNear(70.0f, HeroAttributes().GetMana(), Tolerance));

		ModifyHero(UObsidianHeroAttributeSet::GetManaAttribute(), -500.0f);
		ASSERT_THAT(IsNear(0.0f, HeroAttributes().GetMana(), Tolerance));

		ModifyHero(UObsidianHeroAttributeSet::GetManaAttribute(), 30.0f);
		ASSERT_THAT(IsNear(30.0f, HeroAttributes().GetMana(), Tolerance));
	}

	TEST_METHOD(IncomingManaReplenishing_RestoresManaUpToMaxMana)
	{
		HeroAttributes().SetMaxMana(100.0f);
		HeroAttributes().SetMana(50.0f);

		ModifyHero(UObsidianHeroAttributeSet::GetIncomingManaReplenishingAttribute(), 30.0f);
		ASSERT_THAT(IsNear(80.0f, HeroAttributes().GetMana(), Tolerance));

		ModifyHero(UObsidianHeroAttributeSet::GetIncomingManaReplenishingAttribute(), 30.0f);
		ASSERT_THAT(IsNear(100.0f, HeroAttributes().GetMana(), Tolerance));
		ASSERT_THAT(IsNear(0.0f, HeroAttributes().GetIncomingManaReplenishing(), Tolerance));
	}

	TEST_METHOD(Stamina_ModifiedDirectly_StaysBetweenZeroAndMaxStamina)
	{
		HeroAttributes().SetMaxStamina(100.0f);
		HeroAttributes().SetStamina(100.0f);

		ModifyHero(UObsidianHeroAttributeSet::GetStaminaAttribute(), 500.0f);
		ASSERT_THAT(IsNear(100.0f, HeroAttributes().GetStamina(), Tolerance));

		// The clamped value needs to be the real one, not only the one that is displayed.
		ModifyHero(UObsidianHeroAttributeSet::GetStaminaAttribute(), -30.0f);
		ASSERT_THAT(IsNear(70.0f, HeroAttributes().GetStamina(), Tolerance));

		ModifyHero(UObsidianHeroAttributeSet::GetStaminaAttribute(), -500.0f);
		ASSERT_THAT(IsNear(0.0f, HeroAttributes().GetStamina(), Tolerance));

		ModifyHero(UObsidianHeroAttributeSet::GetStaminaAttribute(), 30.0f);
		ASSERT_THAT(IsNear(30.0f, HeroAttributes().GetStamina(), Tolerance));
	}

	TEST_METHOD(OutOfStamina_StaminaDrained_IsBroadcastOnce)
	{
		HeroAttributes().SetMaxStamina(100.0f);
		HeroAttributes().SetStamina(100.0f);

		int32 BroadcastCount = 0;
		HeroAttributes().OnOutOfStamina.AddLambda([&BroadcastCount](AActor* InEffectInstigator, AActor* InEffectCauser,
			const FGameplayEffectSpec* InEffectSpec, float InEffectMagnitude, float InOldValue, float InNewValue)
			{
				BroadcastCount++;
			});

		ModifyHero(UObsidianHeroAttributeSet::GetStaminaAttribute(), -60.0f);
		ASSERT_THAT(AreEqual(0, BroadcastCount));

		ModifyHero(UObsidianHeroAttributeSet::GetStaminaAttribute(), -40.0f);
		ASSERT_THAT(AreEqual(1, BroadcastCount));

		ModifyHero(UObsidianHeroAttributeSet::GetStaminaAttribute(), -40.0f);
		ASSERT_THAT(AreEqual(1, BroadcastCount));
	}

	TEST_METHOD(OutOfStamina_Broadcast_ReportsStaminaFromBeforeTheDrain)
	{
		HeroAttributes().SetMaxStamina(100.0f);
		HeroAttributes().SetStamina(40.0f);

		float OldStamina = -1.0f;
		float NewStamina = -1.0f;
		HeroAttributes().OnOutOfStamina.AddLambda([&OldStamina, &NewStamina](AActor* InEffectInstigator, AActor* InEffectCauser,
			const FGameplayEffectSpec* InEffectSpec, float InEffectMagnitude, float InOldValue, float InNewValue)
			{
				OldStamina = InOldValue;
				NewStamina = InNewValue;
			});

		ModifyHero(UObsidianHeroAttributeSet::GetStaminaAttribute(), -40.0f);

		ASSERT_THAT(IsNear(40.0f, OldStamina, Tolerance));
		ASSERT_THAT(IsNear(0.0f, NewStamina, Tolerance));
	}

	/**
	 * Changing the max value of Hero resources.
	 */

	TEST_METHOD(MaxHealth_Increased_HeroGainsTheSameAmountOfHealth)
	{
		EnemyDamagesHero(200.0f);
		const float HealthBefore = HeroAttributes().GetHealth();

		ModifyHero(UObsidianCommonAttributeSet::GetMaxHealthAttribute(), 500.0f);

		ASSERT_THAT(IsNear(FObsidianCombatTestEnvironment::DefaultHealth + 500.0f, HeroAttributes().GetMaxHealth(), Tolerance));
		ASSERT_THAT(IsNear(HealthBefore + 500.0f, HeroAttributes().GetHealth(), Tolerance));
	}

	TEST_METHOD(MaxHealth_Decreased_HealthDoesNotStayAboveMaxHealth)
	{
		ModifyHero(UObsidianCommonAttributeSet::GetMaxHealthAttribute(), -500.0f);

		ASSERT_THAT(IsNear(FObsidianCombatTestEnvironment::DefaultHealth - 500.0f, HeroAttributes().GetMaxHealth(), Tolerance));
		ASSERT_THAT(IsNear(HeroAttributes().GetMaxHealth(), HeroAttributes().GetHealth(), Tolerance));

		// The lowered value needs to be the real one, not only the one that is displayed.
		ModifyHero(UObsidianCommonAttributeSet::GetHealthAttribute(), -100.0f);
		ASSERT_THAT(IsNear(HeroAttributes().GetMaxHealth() - 100.0f, HeroAttributes().GetHealth(), Tolerance));
	}

	TEST_METHOD(MaxEnergyShield_Increased_HeroGainsTheSameAmountOfEnergyShield)
	{
		ModifyHero(UObsidianCommonAttributeSet::GetMaxEnergyShieldAttribute(), 200.0f);
		ASSERT_THAT(IsNear(200.0f, HeroAttributes().GetEnergyShield(), Tolerance));

		EnemyDamagesHero(150.0f);
		ModifyHero(UObsidianCommonAttributeSet::GetMaxEnergyShieldAttribute(), 100.0f);

		ASSERT_THAT(IsNear(300.0f, HeroAttributes().GetMaxEnergyShield(), Tolerance));
		ASSERT_THAT(IsNear(150.0f, HeroAttributes().GetEnergyShield(), Tolerance));
	}

	TEST_METHOD(MaxMana_Increased_HeroGainsTheSameAmountOfMana)
	{
		ModifyHero(UObsidianHeroAttributeSet::GetMaxManaAttribute(), 100.0f);
		ASSERT_THAT(IsNear(100.0f, HeroAttributes().GetMana(), Tolerance));

		ModifyHero(UObsidianHeroAttributeSet::GetManaAttribute(), -60.0f);
		ModifyHero(UObsidianHeroAttributeSet::GetMaxManaAttribute(), 50.0f);

		ASSERT_THAT(IsNear(150.0f, HeroAttributes().GetMaxMana(), Tolerance));
		ASSERT_THAT(IsNear(90.0f, HeroAttributes().GetMana(), Tolerance));
	}

	TEST_METHOD(MaxMana_Decreased_ManaDoesNotStayAboveMaxMana)
	{
		ModifyHero(UObsidianHeroAttributeSet::GetMaxManaAttribute(), 100.0f);

		ModifyHero(UObsidianHeroAttributeSet::GetMaxManaAttribute(), -40.0f);

		ASSERT_THAT(IsNear(60.0f, HeroAttributes().GetMaxMana(), Tolerance));
		ASSERT_THAT(IsNear(60.0f, HeroAttributes().GetMana(), Tolerance));
	}

	TEST_METHOD(MaxStamina_Increased_HeroGainsTheSameAmountOfStamina)
	{
		ModifyHero(UObsidianHeroAttributeSet::GetMaxStaminaAttribute(), 100.0f);

		ASSERT_THAT(IsNear(100.0f, HeroAttributes().GetMaxStamina(), Tolerance));
		ASSERT_THAT(IsNear(100.0f, HeroAttributes().GetStamina(), Tolerance));
	}
};

#endif // WITH_DEV_AUTOMATION_TESTS
