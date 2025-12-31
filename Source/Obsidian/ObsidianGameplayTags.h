// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "NativeGameplayTags.h"

// ~ Project

namespace ObsidianGameplayTags
{
	OBSIDIAN_API FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);
	
	/**
	 * ---- User Interface ----
	 */

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_MainMenu);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_GameplayMenu);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_Gameplay);
	
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
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Immunity);

	/**
	 * ---- Input ----
	 */
	
	/**
	 * Native
	 */
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Move_Keyboard);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Move_Mouse);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_ReleaseUsingItem);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_ReleaseContinouslyUsingItem);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_DropItem);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Interact);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_CharacterStatus);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Inventory);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_PassiveSkillTree);

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_WeaponSwap);

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_UI_MainMenu);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_UI_Action_Backwards);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_UI_OpenGameplayMenu);
	
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
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_EffectData_Aura_Health);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_EffectData_Aura_Mana);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_EffectData_Aura_Energy);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_EffectData_Aura_Evasion);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_EffectData_Aura_Armor);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_EffectData_Aura_CriticalDamage);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_EffectData_Aura_CriticalChance);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_EffectData_Aura_ChaosDamage);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_EffectData_Aura_PhysicalDamage);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_EffectData_Aura_CirclingElementalDamage);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_EffectData_Aura_HealthRegeneration);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_EffectData_Aura_ManaRegeneration);

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_EffectData_Effect_Poison);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_EffectData_Effect_Chill);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_EffectData_Effect_Ignite);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_EffectData_Effect_Shock);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_EffectData_Effect_Special_Immunity);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_EffectData_Effect_Special_DamageReduction);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_EffectData_Effect_Curse_ElementalWeakness);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_GlobeData_HealingHealth);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_GlobeData_RepleanishingMana);

	/**
	 * ---- Gameplay Messages ----
	 */

	/**
	 * Inventory
	 */
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_Inventory_Changed);

	/**
	 * Equipment
	 */
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_Equipment_Changed);

	/**
	 * Player Stash
	 */
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_PlayerStash_Changed);
	
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
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_AbilityMontage_Player_Firebolt);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_AbilityMontage_Player_FlyingKnifes);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_AbilityMontage_Player_MagneticHammer);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_AbilityMontage_Player_Slash);

	/**
	 * Tree Orc
	 */
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_AbilityMontage_TreeOrc_EquipWeapon);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_AbilityMontage_TreeOrc_SpawnComboProjectile);

	/**
	 * Ranged Goblin
	 */
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_AbilityMontage_RangedGoblin_SpawnSlingShotProj);
	
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
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Cooldown_HitReact);

	/**
	 * Tree Orc
	 */
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Cooldown_TreeOrc_LeapAttack);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Cooldown_TreeOrc_ComboSwing);
	
	/**
	 * Skeletal Mage
	 */
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Cooldown_SkeletalMage_FireNova);
	
	/**
	 * ---- Items ----
	 */

	/**
	 * Inventory
	 */

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Inventory_BlockActions);

	/**
	 * Equipment
	 */

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equipment_BlockActions);

	/**
	 * Player Stash
	 */
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(PlayerStash_BlockActions);

	/**
	 * Item Category.
	 */
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Currency_Resource);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Currency_Functional);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Armor_Helmet);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Armor_BodyArmor);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Armor_Belt);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Armor_Gloves);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Armor_Boots);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Offhand_Shield);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Offhand_Quiver);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Jewellery_Amulet);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Jewellery_Ring);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Weapon_Melee_OneHand_Dagger);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Weapon_Melee_OneHand_Flail);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Weapon_Melee_OneHand_Mace);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Weapon_Melee_OneHand_Sword);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Weapon_Melee_OneHand_Axe);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Weapon_Melee_TwoHand_Mace);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Weapon_Melee_TwoHand_Sword);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Weapon_Melee_TwoHand_Axe);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Weapon_Ranged_OneHand_Wand);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Weapon_Ranged_OneHand_Bow);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Weapon_Ranged_TwoHand_Staff);
	
	/**
	 * Item Base Types
	 */

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_ScrollOfIdentification);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_ScrollOfTeleportation);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_OrbOfEnchantment);

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Helmet_Armor);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Helmet_Evasion);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Helmet_EnergyShield);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_BodyArmor_Armor);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_BodyArmor_Evasion);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_BodyArmor_EnergyShield);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Amulet);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Ring);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Sword_OneHand_Faith);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Sword_OneHand_Dexterity);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Sword_OneHand_Strength);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Sword_OneHand_Intelligence);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Sword_TwoHand_Faith);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Sword_TwoHand_Dexterity);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Sword_TwoHand_Strength);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Sword_TwoHand_Intelligence);

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Axe_OneHand_Faith);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Axe_OneHand_Dexterity);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Axe_OneHand_Strength);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Axe_OneHand_Intelligence);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Axe_TwoHand_Faith);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Axe_TwoHand_Dexterity);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Axe_TwoHand_Strength);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Axe_TwoHand_Intelligence);

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Mace_OneHand_Faith);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Mace_OneHand_Dexterity);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Mace_OneHand_Strength);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Mace_OneHand_Intelligence);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Mace_TwoHand_Faith);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Mace_TwoHand_Dexterity);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Mace_TwoHand_Strength);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Mace_TwoHand_Intelligence);

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Dagger_Faith);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Dagger_Dexterity);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Dagger_Strength);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Dagger_Intelligence);

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Wand_Faith);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Wand_Dexterity);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Wand_Strength);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Wand_Intelligence);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Shield_Strength);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_BaseType_Shield_Dexterity);
	
	/**
	 * Equipment Slots.
	 */
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Slot_Equipment_Weapon_RightHand);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Slot_Equipment_Weapon_LeftHand);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_SwapSlot_Equipment_Weapon_RightHand);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_SwapSlot_Equipment_Weapon_LeftHand);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Slot_Equipment_Helmet);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Slot_Equipment_BodyArmor);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Slot_Equipment_Belt);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Slot_Equipment_Gloves);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Slot_Equipment_Boots);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Slot_Equipment_Amulet);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Slot_Equipment_Ring_RightHand);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Slot_Equipment_Ring_LeftHand);

	/** All Stack Types stored for convenience. */
	OBSIDIAN_API extern const TArray<FGameplayTag> EquipmentSlots;
	
	/** Default Item Category Attachment rules. */
	OBSIDIAN_API const TMap<FGameplayTag, FName>& GetSlotToAttachSocketMap();

	/** Accepted Equipment Categories for sister slot per weapon type. */
	OBSIDIAN_API const TMap<FGameplayTag, FGameplayTagContainer>& GetSisterSlotAcceptedCategoriesMap();

	/**
	 * Currency Slots.
	 */

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Slot_Functional_ScrollOfIdentification);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Slot_Functional_ScrollOfTeleportation);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Slot_Functional_OrbOfEnchantment);

	/**
	 * Stash Tabs.
	 */

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(StashTab_Grid_1);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(StashTab_Grid_2);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(StashTab_Grid_3);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(StashTab_Grid_4);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(StashTab_BigGrid_1);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(StashTab_Functional);

	/**
	 * Stacks Counts
	 */
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_StackCount_Current);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_StackCount_Max);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_StackCount_Limit);

	/** All Stack Types stored for convenience. */
	OBSIDIAN_API extern const TArray<FGameplayTag> StackTypes;
	
	/**
	 * ---- Affixes ----
	 */

	/**
	 * Affix Values
	 */
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_AffixValue_SingleValue);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_AffixValue_MinValue);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_AffixValue_MaxValue);
	
	/**
	 * Implicits
	 */

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Implicit_Life_LifeFlat);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Implicit_Mana_ManaFlat);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Implicit_EnergyShield_EnergyShieldFlat);

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Implicits_Resistance_Fire);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Implicits_Resistance_Lightning);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Implicits_Resistance_Cold);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Implicits_Resistance_Chaos);

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Implicits_MaxResistance_Fire);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Implicits_MaxResistance_Lightning);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Implicits_MaxResistance_Cold);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Implicits_MaxResistance_Chaos);

	/**
	 * Prefixes
	 */

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Prefix_Life_LifeFlat);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Prefix_Life_LifePercent);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Prefix_Mana_ManaFlat);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Prefix_Mana_ManaPercent);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Prefix_Utility_MagicFind);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Prefix_Defence_ArmorFlat);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Prefix_Defence_ArmorPercent);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Prefix_Defence_EvasionFlat);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Prefix_Defence_EvasionPercent);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Prefix_Defence_EnergyShieldFlat);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Prefix_Defence_EnergyShieldPercent);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Prefix_DamageMultiplier_IncreasePhysicalDamage);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Prefix_DamageMultiplier_IncreaseFireDamage);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Prefix_DamageMultiplier_IncreaseColdDamage);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Prefix_DamageMultiplier_IncreaseLightningDamage);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Prefix_DamageMultiplier_IncreaseChaosDamage);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Prefix_DamageRange_PhysicalDamageFlat);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Prefix_DamageRange_PhysicalDamagePercentage);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Prefix_DamageRange_FireDamageFlat);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Prefix_DamageRange_FireDamagePercentage);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Prefix_DamageRange_ColdDamageFlat);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Prefix_DamageRange_ColdDamagePercentage);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Prefix_DamageRange_LightningDamageFlat);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Prefix_DamageRanger_LightningDamagePercentage);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Prefix_DamageRanger_ChaosDamageFlat);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Prefix_DamageRanger_ChaosDamagePercentage);
	
	/**
	 * Suffixes
	 */
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Suffix_Life_LifeRegeneration);

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Suffix_EnergyShield_EnergyShieldRegeneration);

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Suffix_Mana_ManaRegeneration);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Suffix_Attribute_Dexterity);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Suffix_Attribute_Intelligence);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Suffix_Attribute_Strength);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Suffix_Attribute_Faith);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Suffix_Resistance_Fire);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Suffix_Resistance_Lightning);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Suffix_Resistance_Cold);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Suffix_Resistance_Chaos);

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Suffix_MaxResistance_Fire);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Suffix_MaxResistance_Lightning);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Suffix_MaxResistance_Cold);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Suffix_MaxResistance_Chaos);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Suffix_Utility_MagicFind);

	/**
	 * Enchanted Affixes
	 */
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Suffix_Enchant_MaximumLifePercentage)

	/**
	 * Item Primary Affix
	 */

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_ItemPrimaryAffix_DamageRange_PhysicalDamageFlat);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_ItemPrimaryAffix_DamageRange_FireDamageFlat);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_ItemPrimaryAffix_DamageRange_ColdDamageFlat);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_ItemPrimaryAffix_DamageRange_LightningDamageFlat);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_ItemPrimaryAffix_DamageRange_ChaosDamageFlat);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_ItemPrimaryAffix_Defence_EvasionFlat);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_ItemPrimaryAffix_Defence_ArmorFlat);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_ItemPrimaryAffix_Defence_EnergyShieldFlat);

	/**
	 * Skill Implicits
	 */

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Attack_Witch_FireBall);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Attack_Barbarian_Slash);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Attack_Assassin_FlyingKnife);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Attack_Paladin_BlessedHammer);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Attack_Paladin_MagneticHammer);

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Defence_MagmaBarrier);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Defence_FrozenArmor);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Defence_TransientArmor);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Defence_SparklingBarrier);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Defence_RaisedShield);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Movement_Roll);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Movement_Jump);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Movement_ShieldCharge);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Movement_Blink);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Movement_Teleport);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Aura_Health);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Aura_Mana);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Aura_Energy);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Aura_Evasion);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Aura_Armor);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Aura_CirclingElementalDamage);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Aura_CriticalDamage);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Aura_CriticalChance);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Aura_ChaosDamage);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Aura_PhysicalDamage);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Aura_HealthRegeneration);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Aura_ManaRegeneration);

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Ultimate_VoidSphere);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Ultimate_Combustion);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Ultimate_Shatter);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_SkillImplicits_Ultimate_Electrocution);
}


