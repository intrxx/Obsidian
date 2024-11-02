// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "NativeGameplayTags.h"

namespace ObsidianGameplayTags
{
	/**
	 * ---- Damage Types ----
	 */
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_DamageType_Physical);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_DamageType_Elemental_Fire);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_DamageType_Elemental_Cold);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_DamageType_Elemental_Lightning);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_DamageType_Chaos);
	
	/** All damage Types stored for convenience */
	OBSIDIAN_API extern const TArray<FGameplayTag> DamageTypes;
	
	/**
	 *  ---- Effects ----
	 */
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_HitReact)
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Stagger)
	
	/**
	 * ---- Statuses ----
	 */
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dying);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dead);

	/**
	 * ---- Input ----
	 */
	
	/**
	 * Native
	 */
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Move_Keyboard);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Move_Mouse);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_CharacterStatus);

	/**
	 * Ability
	 */
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Ability_Move_Mouse);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Ability_Move_Roll);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Ability1);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Ability2);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Ability3);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Ability4);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Ability5);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Ability6);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Ability7);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Ability8);

	/*
	 * ---- Data ----
	 */

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_AdvancedCombat_Hit);
	
	/**
	 * UI Data
	 */
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_DataSpecifierTag)
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_EffectData_Flask_HealthHealing);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_EffectData_Flask_ManaHealing);

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_EffectData_Aura_Defiance);

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_EffectData_Effect_Poison);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_EffectData_Effect_Chill);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_EffectData_Effect_Ignite);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_EffectData_Effect_Shock);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_EffectData_Effect_Curse_ElementalWeakness);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_GlobeData_HealingHealth);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_GlobeData_RepleanishingMana);

	/**
	 * ---- Gameplay Events ----
	 */

	/**
	 * Shared
	 */
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Death);

	/**
	 * Hero
	 */
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_AbilityMontage_Player_Firebolt)
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_AbilityMontage_Player_Aura)

	/**
	 * Tree Orc
	 */
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_AbilityMontage_TreeOrc_EquipWeapon);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_AbilityMontage_TreeOrc_SpawnComboProjectile);

	/**
	 * Skeletal Mage
	 */
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_AbilityMontage_SkeletalMage_SpawnFireNova);
	
	/**
	 * Sockets
	 */
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_AbilityMontage_Socket_RightHandWeapon)
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_AbilityMontage_Socket_LeftHandWeapon)
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_AbilityMontage_Socket_RightHand)
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_AbilityMontage_Socket_LeftHand)
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_AbilityMontage_Socket_BetweenHands)

	/**
	 * ---- Gameplay Cues ----
	 */

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_MeleeImpact)

	/**
	 * ---- Ability Activation Tags ----
	 */
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivationFail_IsDead);

	/**
	 * Shared
	 */
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityActivation_HitReact);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityActivation_Stagger);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityActivation_Introduction);

	/**
	 * Zombie
	 */
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityActivation_Zombie_MeleeAttack);

	/**
	 * Ranged Goblin
	 */
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityActivation_RangedGoblin_BowAttack);

	/**
	 * Skeletal Mage
	 */
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityActivation_SkeletalMage_FireBall);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityActivation_SkeletalMage_FireNova);

	/**
	 * Tree Orc
	 */
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityActivation_TreeOrc_UnarmedSwing);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityActivation_TreeOrc_LeapAttack);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityActivation_TreeOrc_Equip);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityActivation_TreeOrc_ComboSwing);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityActivation_TreeOrc_ArmedSwing);
	
	/**
	 * ---- Cooldowns ----
	 */

	/**
	 * Shared
	 */
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Cooldown_HitReact)

	/**
	 * Tree Orc
	 */
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Cooldown_TreeOrc_LeapAttack)
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Cooldown_TreeOrc_ComboSwing)
	
	/**
	 * Skeletal Mage
	 */
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Cooldown_SkeletalMage_FireNova)
	
	/**
	 * ---- End of Cooldowns ----
	 */
	
	OBSIDIAN_API FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);
}


