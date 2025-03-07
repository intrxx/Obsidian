﻿// Copyright 2024 out of sCope team - Michał Ogiński

#include "ObsidianGameplayTags.h"
#include "GameplayTagsManager.h"
#include "ObsidianGameModule.h"

namespace ObsidianGameplayTags
{
	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString)
	{
		const UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();
		FGameplayTag ReturnTag = TagsManager.RequestGameplayTag(FName(*TagString), false);

		if(bMatchPartialString && !ReturnTag.IsValid())
		{
			FGameplayTagContainer AllTags;
			TagsManager.RequestAllGameplayTags(AllTags, true);

			for(const FGameplayTag& Tag : AllTags)
			{
				if(Tag.ToString().Contains(TagString))
				{
					UE_LOG(LogObsidian, Display, TEXT("Did not find exact match for [%s] but found partial match on tag [%s]."), *TagString, *Tag.ToString());
					ReturnTag = Tag;
					break;
				}	
			}
		}
		return ReturnTag;
	}
	
	/**
	 * ---- Damage Types ----
	 */
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_DamageType_Physical, "DamageType.Physical", "Physical Damage Type - also used for SetByCaller.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_DamageType_Elemental_Fire, "DamageType.Elemental.Fire", "Fire Damage Type - also used for SetByCaller.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_DamageType_Elemental_Cold, "DamageType.Elemental.Cold", "Cold Damage Type - also used for SetByCaller.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_DamageType_Elemental_Lightning, "DamageType.Elemental.Lightning", "Lightning Damage Type - also used for SetByCaller.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_DamageType_Chaos, "DamageType.Chaos", "Chaos Damage Type - also used for SetByCaller.");
	
	const TArray<FGameplayTag> DamageTypes =
	{
	{
			SetByCaller_DamageType_Physical,
			SetByCaller_DamageType_Elemental_Fire,
			SetByCaller_DamageType_Elemental_Cold,
			SetByCaller_DamageType_Elemental_Lightning,
			SetByCaller_DamageType_Chaos
		  }
	};
	
	/**
	 * ---- Effects ----
	 */
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_HitReact, "Effect.HitReact", "Tag used for activating the Hit React ability.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Stagger, "Effect.Stagger", "Tag used for activating the Stagger ability.");

	/**
	 * ---- Statuses ----
	 */
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Death, "Status.Death", "Death has the death status.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Death_Dying, "Status.Death.Dying", "Death started for the target.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Death_Dead, "Status.Death.Dead", "Death finished for the target.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Immunity, "Status.Immunity", "Target has immunity to all effects.");

	/**
	 * ---- Input ----
	 */
	
	/**
	 * Native
	 */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Move_Keyboard, "Input.Native.Move.Keyboard", "Move input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Move_Mouse, "Input.Native.Move.Mouse", "Move input.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_CharacterStatus, "Input.Native.CharacterStatus", "Character Status toggle input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Inventory, "Input.Native.Inventory", "Inventory toggle input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_PassiveSkillTree, "Input.Native.PassiveSkillTree", "Passive Skill Tree toggle input.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_DropItem, "Input.DropItem", "Aditional input for droping the item.");

	/**
	 * Ability
	 */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability_Move_Roll, "Input.Ability.Move.Roll", "Roll input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability1, "Input.Ability.Ability1", "Ability1 input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability2, "Input.Ability.Ability2", "Ability2 input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability3, "Input.Ability.Ability3", "Ability3 input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability4, "Input.Ability.Ability4", "Ability4 input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability5, "Input.Ability.Ability5", "Ability5 input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability6, "Input.Ability.Ability6", "Ability6 input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability7, "Input.Ability.Ability7", "Ability7 input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability8, "Input.Ability.Ability8", "Ability8 input.");

	/**
	 * ---- Data ----
	 */
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Data_AdvancedCombat_Hit, "Data.AdvancedCombat.Hit", "Event fires when the enemy character is hit by the Advanced Combat Component.");
	
	/**
	 * UI Data
	 */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_DataSpecifierTag, "UI.DataSpecifierTag", "Tag that is owned by any effect that wish to be displayed on Player's screen.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_EffectData_Flask_HealthHealing, "UI.EffectData.Flask.HealthHealing", "Tag used for displaying Health healing from Flasks Info on the UI.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_EffectData_Flask_ManaHealing, "UI.EffectData.Flask.ManaHealing", "Tag used for displaying Mana healing from Flasks Info on the UI.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_EffectData_Aura_Defiance, "UI.EffectData.Aura.Defiance", "Tag used for displaying Defiance Aura Info on the UI.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_EffectData_Effect_Poison, "UI.EffectData.Effect.Poison", "Tag used for displaying Poison Effect Info on the UI.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_EffectData_Effect_Chill, "UI.EffectData.Effect.Chill", "Tag used for displaying Chill Effect Info on the UI.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_EffectData_Effect_Ignite, "UI.EffectData.Effect.Ignite", "Tag used for displaying Ignite Effect Info on the UI.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_EffectData_Effect_Shock, "UI.EffectData.Effect.Shock", "Tag used for displaying Shock Info on the UI.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_EffectData_Effect_Special_Immunity, "UI.EffectData.Effect.Special.Immunity", "Tag used for displaying Immunity info on UI (specially Enemy Overlay Bar).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_EffectData_Effect_Special_DamageReduction, "UI.EffectData.Effect.Special.DamageReduction", "Tag used for displaying Damage Reduction info on UI (specially Enemy Overlay Bar).");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_EffectData_Effect_Curse_ElementalWeakness, "UI.EffectData.Effect.Curse.ElementalWeakness", "Tag used for displaying Elemental Weakness Info on the UI.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_GlobeData_HealingHealth, "UI.GlobeData.HealingHealth", "Tag used for displaying the healing amount on the globe.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_GlobeData_RepleanishingMana, "UI.GlobeData.ReplenishingMana", "Tag used for displaying the repleanish amount on the globe.");

	/**
	 * ---- Gameplay Messages ----
	 */

	/**
	 * Inventory
	 */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_Inventory_Changed, "Message.Inventory.Changed", "Tag used in Gameplay Message Subsystem to represent inventory state change.")
	
	/**
	 * ---- Gameplay Events ----
	 */
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_Death, "GameplayEvent.Death", "Event fired on attributes component when character is out of health.");

	/**
	 * Hero
	 */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_AbilityMontage_Player_Firebolt, "GameplayEvent.AbilityMontage.Player.Firebolt", "Tag used for triggering gameplay event for spawning firebolt.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_AbilityMontage_Player_Aura, "GameplayEvent.AbilityMontage.Player.Aura", "Tag used for triggering gameplay event for applying aura.");

	/**
	 * Tree Orc
	 */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_AbilityMontage_TreeOrc_EquipWeapon, "GameplayEvent.AbilityMontage.TreeOrc.EquipWeapon", "Tag used for triggering the equip function on the enemy.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_AbilityMontage_TreeOrc_SpawnComboProjectile, "GameplayEvent.AbilityMontage.TreeOrc.SpawnComboProjectile", "Tag used for triggering the projectile spawning in combo swing ability on the enemy.");

	/**
	 * Ranged Goblin
	 */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_AbilityMontage_RangedGoblin_SpawnSlingShotProj, "GameplayEvent.AbilityMontage.RangedGoblin.SpawnSlingShotProj", "Tag used for triggering the sling shot projectile on Ranged Goblin.");
	
	/**
	 * Skeletal Mage
	 */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_AbilityMontage_SkeletalMage_SpawnFireNova, "GameplayEvent.AbilityMontage.SkeletalMage.SpawnFireNova", "Tag used for triggering the fire nova spawn on Skeletal Mage ability.");
	
	/**
	 * Sockets
	 */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_AbilityMontage_Socket_RightHandWeapon, "GameplayEvent.AbilityMontage.Socket.RightHandWeapon", "Tag used for triggering gameplay event for damaging actor with right hand melee weapon. Used by Montage to retreive correct socket.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_AbilityMontage_Socket_LeftHandWeapon, "GameplayEvent.AbilityMontage.Socket.LeftHandWeapon", "Tag used for triggering gameplay event for damaging actor with left hand melee weapon. Used by Montage to retreive correct socket.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_AbilityMontage_Socket_RightHand, "GameplayEvent.AbilityMontage.Socket.RightHand", "Tag used for triggering gameplay event for damaging actor with melee right hand. Used by Montage to retreive correct socket.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_AbilityMontage_Socket_LeftHand, "GameplayEvent.AbilityMontage.Socket.LeftHand", "Tag used for triggering gameplay event for damaging actor with melee left hand. Used by Montage to retreive correct socket.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_AbilityMontage_Socket_BetweenHands, "GameplayEvent.AbilityMontage.Socket.BetweenHands", "Tag used for triggering gameplay event for damaging actor with melee attack between hands (could be used for slam or something). Used by Montage to retreive correct socket.");

	/**
	* ---- Gameplay Cues ----
	*/
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_MeleeImpact, "GameplayCue.MeleeImpact", "Tag used for Melee Impact GC.");
	
	/**
	* ---- Ability Activation Tags ----
	*/
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivationFail_IsDead, "Ability.ActivationFail.IsDead", "Ability failed to activate because its owner is dead.");
	
	/**
	 * Shared
	 */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityActivation_HitReact, "AbilityActivation.HitReact", "Tag used for activating hit react ability on owner.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityActivation_Stagger, "AbilityActivation.Stagger", "Tag used for activating stagger ability on owner.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityActivation_Introduction, "AbilityActivation.Introduction", "Tag used for triggering activation of AI's Introduction ability (some roaring or equping a weapon).");

	/**
	 * Zombie
	 */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityActivation_Zombie_MeleeAttack, "AbilityActivation.Zombie.MeleeAttack", "Tag used for triggering activation of Zombie's melee attack.");

	/**
	 * Ranged Goblin
	 */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityActivation_RangedGoblin_BowAttack, "AbilityActivation.RangedGoblin.BowAttack", "Tag used for triggering activation of goblin's bow ranged attack.");

	/**
	 * Skeletal Mage
	 */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityActivation_SkeletalMage_FireBall, "AbilityActivation.SkeletalMage.FireBall", "Tag used for triggering activation of Skeletal Mage's fire ball spell.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityActivation_SkeletalMage_FireNova, "AbilityActivation.SkeletalMage.FireNova", "Tag used for triggering activation of Skeletal Mage's fire nova spell.");
	
	/**
	 * Tree Orc
	 */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityActivation_TreeOrc_UnarmedSwing, "AbilityActivation.TreeOrc.UnarmedSwing", "Tag used for triggering activation of TreeOrcs's swing melee attack.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityActivation_TreeOrc_LeapAttack, "AbilityActivation.TreeOrc.LeapAttack", "Tag used for triggering activation of TreeOrcs's leap attack.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityActivation_TreeOrc_Equip, "AbilityActivation.TreeOrc.Equip", "Tag used for triggering activation of TreeOrcs's equip ability.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityActivation_TreeOrc_ComboSwing, "AbilityActivation.TreeOrc.ComboSwing", "Tag used for triggering activation of TreeOrcs's Combo Swing ability.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityActivation_TreeOrc_ArmedSwing, "AbilityActivation.TreeOrc.ArmedSwing", "Tag used for triggering activation of TreeOrcs's Armed Swing ability.");

	/**
	 * ---- Cooldowns ----
	 */

	/**
	 * Shared
	 */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Cooldown_HitReact, "Ability.Cooldown.HitReact", "Tag used for Hit React Ability cooldown.");

	/**
	 * Tree Orc
	 */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Cooldown_TreeOrc_LeapAttack, "Ability.Cooldown.TreeOrc.LeapAttack", "Tag used for Leap Attack Tree Orc's Ability cooldown.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Cooldown_TreeOrc_ComboSwing, "Ability.Cooldown.TreeOrc.ComboSwing", "Tag used for Combo Swing Tree Orc's Ability cooldown.");
	
	/**
	 * Skeletal Mage
	 */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Cooldown_SkeletalMage_FireNova, "Ability.Cooldown.SkeletalMage.FireNova", "Tag used for Fire Nova Skeletal Mage's Ability cooldown.");

	/**
	 * ---- Items ----
	 */

	/**
	 * Stack Counts
	 */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_StackCount_Current, "Item.StackCount.Current", "Item Tag representing the current number of stacked items.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_StackCount_Max, "Item.StackCount.Max", "Item Tag representing the Max Stack Count the item have, after which it will no longer stack and will need another space in the inventory.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_StackCount_Limit, "Item.StackCount.Limit", "Item Tag representing the Total number of items the Player can have, can be used for mission items which can be limited to 1.");

	const TArray<FGameplayTag> StackTypes =
	{
		{
			Item_StackCount_Current,
			Item_StackCount_Max,
			Item_StackCount_Limit
		  }
	};

	/**
	 * Rarity
	 */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Rarity_Normal, "Item.Rarity.Normal", "Item Tag representing the normal rarity of the item.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Rarity_Magic, "Item.Rarity.Magic", "Item Tag representing the magic rarity of the item.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Rarity_Rare, "Item.Rarity.Rare", "Item Tag representing the rare rarity of the item.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Rarity_Unique, "Item.Rarity.Unique", "Item Tag representing the Unique rarity of the item.")

	/**
	 * Affixes
	 */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Attribute_Dexterity, "Item.Affix.Attribute.Dexterity", "Item Tag which represents Dexterity Affix.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Attribute_Intelligence, "Item.Affix.Attribute.Intelligence", "Item Tag which represents Intelligence Affix.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Attribute_Strenght, "Item.Affix.Attribute.Strenght", "Item Tag which represents Strenght Affix.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Attribute_Faith, "Item.Affix.Attribute.Dexterity", "Item Tag which represents Faith Affix.")
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Resistance_Fire, "Item.Affix.Resistance.Fire", "Item Tag which represents Fire Resistance Affix.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Resistance_Lightning, "Item.Affix.Resistance.Lightning", "Item Tag which represents Lightning Resistance Affix.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Resistance_Cold, "Item.Affix.Resistance.Cold", "Item Tag which represents Cold Resistance Affix.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Resistance_Chaos, "Item.Affix.Resistance.Chaos", "Item Tag which represents Chaos Resistance Affix.")
	
}
