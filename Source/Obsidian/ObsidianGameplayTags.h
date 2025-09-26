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
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_AbilityMontage_Player_Firebolt)
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_AbilityMontage_Player_Aura)

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
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Resource);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Helmet);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_BodyArmor);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Belt);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Gloves);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Boots);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Shield);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Quiver);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Amulet);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Ring);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Dagger);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Wand);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Flail);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Bow); 
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Staff_TwoHand);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Mace_OneHand);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Mace_TwoHand);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Sword_OneHand);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Sword_TwoHand);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Axe_OneHand);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Equipment_Axe_TwoHand);

	/**
	 * Individual Item Category Tags
	 */
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Functional_ScrollOfIdentification);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Functional_ScrollOfTeleportation);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Category_Functional_OrbOfEnchantment);
	
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
	 * Rarity
	 */
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Rarity_Normal);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Rarity_Magic);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Rarity_Rare);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Rarity_Unique);

	/**
	 * Affixes
	 */
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Attribute_Dexterity);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Attribute_Intelligence);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Attribute_Strength);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Attribute_Faith);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Resistance_Fire);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Resistance_Lightning);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Resistance_Cold);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Resistance_Chaos);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Basic_FlatLife);
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Basic_FlatMana);

	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Affix_Enchant_MaximumLifePercentage)
}


