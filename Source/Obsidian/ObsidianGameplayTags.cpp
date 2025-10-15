// Copyright 2024 out of sCope team - Michał Ogiński

// ~ Core

#include "ObsidianGameplayTags.h"
#include "GameplayTagsManager.h"

// ~ Project
#include "ObsidianGameModule.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"

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
	 * ---- User Interface ----
	 */

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_Layer_MainMenu, "UI.Layer.MainMenu", "Layer tag used when pushing widgets to layer in main menu.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_Layer_GameplayMenu, "UI.Layer.GameplayMenu", "Layer tag used when pushing Menu widgets to layer in Gameplay.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_Layer_Gameplay, "UI.Layer.Gameplay", "Layer tag used when pushing gameplay widgets to layer in Gameplay.");
	
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
			SetByCaller_DamageType_Physical,
			SetByCaller_DamageType_Elemental_Fire,
			SetByCaller_DamageType_Elemental_Cold,
			SetByCaller_DamageType_Elemental_Lightning,
			SetByCaller_DamageType_Chaos
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
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_ReleaseUsingItem, "Input.Native.ReleaseUsingItem", "Input for easy release of Using Item.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_ReleaseContinouslyUsingItem, "Input.Native.ReleaseContinouslyUsingItem", "Input for release of continuous Using Item.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_DropItem, "Input.DropItem", "Additional input for dropping the item.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Interact, "Input.Interact", "Additional input for interacting with other actors in the world.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_CharacterStatus, "Input.Native.CharacterStatus", "Character Status toggle input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Inventory, "Input.Native.Inventory", "Inventory toggle input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_PassiveSkillTree, "Input.Native.PassiveSkillTree", "Passive Skill Tree toggle input.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_WeaponSwap, "Input.Native.WeaponSwap", "Input for Weapon Swap.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_UI_MainMenu, "Input.UI.MainMenu", "Main Menu Inputs for Layers.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_UI_Action_Backwards, "UI.Action.Backwards", "Input for going back a layer in UI.");

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
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_Inventory_Changed, "Message.Inventory.Changed", "Tag used in Gameplay Message Subsystem to represent Inventory state change.")

	/**
	 * Equipment
	 */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_Equipment_Changed, "Message.Equipment.Changed", "Tag used in Gameplay Message Subsystem to represent Equipment state change.")

	/**
	 * Player Stash
	 */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_PlayerStash_Changed, "Message.PlayerStash.Changed", "Tag used in Gameplay Message Subsystem to represent Player Stash state change.")
	
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
	 * Inventory
	 */

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory_BlockActions, "Inventory.BlockActions", "When applied to player, he/she won't be able to modify inventory state.");

	/**
	 * Equipment
	 */

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Equipment_BlockActions, "Equipment.BlockActions", "When applied to player, he/she won't be able to modify equipment state.");

	/**
	 * Player Stash
	 */

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(PlayerStash_BlockActions, "PlayerStash.BlockActions", "When applied to player, he/she won't be able to modify stash state.");

	/**
	 * Item Category.
	 */

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Resource, "Item.Category.Resource", "Item Tag that represents Resource item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Functional, "Item.Category.Functional", "Item Tag that represents Functional item category.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Equipment_Helmet, "Item.Category.Equipment.Helmet", "Item Tag that represents Helmet item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Equipment_BodyArmor, "Item.Category.Equipment.BodyArmor", "Item Tag that represents Body Armor item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Equipment_Belt, "Item.Category.Equipment.Belt", "Item Tag that represents Belt item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Equipment_Gloves, "Item.Category.Equipment.Gloves", "Item Tag that represents Gloves item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Equipment_Boots, "Item.Category.Equipment.Boots", "Item Tag that represents Boots item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Equipment_Shield, "Item.Category.Equipment.Shield", "Item Tag that represents Shield item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Equipment_Quiver, "Item.Category.Equipment.Quiver", "Item Tag that represents Quiver item category.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Equipment_Amulet, "Item.Category.Equipment.Amulet", "Item Tag that represents Amulet item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Equipment_Ring, "Item.Category.Equipment.Ring", "Item Tag that represents Ring item category.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Equipment_Dagger, "Item.Category.Equipment.Dagger", "Item Tag that represents Dagger item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Equipment_Wand, "Item.Category.Equipment.Wand", "Item Tag that represents Wand item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Equipment_Flail, "Item.Category.Equipment.Flail", "Item Tag that represents Flail item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Equipment_Bow, "Item.Category.Equipment.Bow", "Item Tag that represents Bow item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Equipment_Staff_TwoHand, "Item.Category.Equipment.Staff.TwoHand", "Item Tag that represents Staff item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Equipment_Mace_OneHand, "Item.Category.Equipment.Mace.OneHand", "Item Tag that represents One Hand Mace item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Equipment_Mace_TwoHand, "Item.Category.Equipment.Mace.TwoHand", "Item Tag that represents Two Hand Mace item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Equipment_Sword_OneHand, "Item.Category.Equipment.Sword.OneHand", "Item Tag that represents One Hand Sword item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Equipment_Sword_TwoHand, "Item.Category.Equipment.Sword.TwoHand", "Item Tag that represents Two Hand Sword item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Equipment_Axe_OneHand, "Item.Category.Equipment.Axe.OneHand", "Item Tag that represents One Hand Axe item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Equipment_Axe_TwoHand, "Item.Category.Equipment.Axe.TwoHand", "Item Tag that represents Two Hand Axe item category.");

	/**
	 * Item Base Types.
	 */

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_BaseType_ScrollOfIdentification, "Item.BaseType.ScrollOfIdentification", "Item Tag that represents Scroll Of Identification item base type.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_BaseType_ScrollOfTeleportation, "Item.BaseType.ScrollOfTeleportation", "Item Tag that represents Scroll Of Teleportation item base type.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_BaseType_OrbOfEnchantment, "Item.BaseType.OrbOfEnchantment", "Item Tag that represents Orb Of Enchantment item base type.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_BaseType_Helmet_Armor, "Item.BaseType.Helmet.Armor", "Item Tag that represents Orb Of Armor Helmet item base type.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_BaseType_Helmet_Evasion, "Item.BaseType.Helmet.Evasion", "Item Tag that represents Evasion Helmet item base type.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_BaseType_Helmet_EnergyShield, "Item.BaseType.Helmet.EnergyShield", "Item Tag that represents Energy Shield Helmet item base type.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_BaseType_BodyArmor_Armor, "Item.BaseType.BodyArmor.Armor", "Item Tag that represents Armor Body Armor item base type.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_BaseType_BodyArmor_Evasion, "Item.BaseType.BodyArmor.Evasion", "Item Tag that represents Evasion Body Armor item base type.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_BaseType_BodyArmor_EnergyShield, "Item.BaseType.BodyArmor.EnergyShield", "Item Tag that represents Energy Shield Body Armor item base type.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_BaseType_Ring, "Item.BaseType.Ring", "Item Tag that represents Ring item base type.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_BaseType_Amulet, "Item.BaseType.Amulet", "Item Tag that represents Amulet item base type.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_BaseType_Sword_OneHand_Faith, "Item.BaseType.Sword.OneHand.Faith", "Item Tag that represents One Hand Faith Sword item base type.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_BaseType_Sword_OneHand_Dexterity, "Item.BaseType.Sword.OneHand.Dexterity", "Item Tag that represents One Hand Dexterity Sword item base type.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_BaseType_Sword_OneHand_Strength, "Item.BaseType.Sword.OneHand.Strength", "Item Tag that represents One Hand Strength Sword item base type.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_BaseType_Sword_OneHand_Intelligence, "Item.BaseType.Sword.OneHand.Intelligence", "Item Tag that represents One Hand Intelligence Sword item base type.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_BaseType_Sword_TwoHand_Faith, "Item.BaseType.Sword.TwoHand.Faith", "Item Tag that represents Two Hand Faith Sword item base type.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_BaseType_Sword_TwoHand_Dexterity, "Item.BaseType.Sword.TwoHand.Dexterity", "Item Tag that represents Two Hand Sword Dexterity item base type.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_BaseType_Sword_TwoHand_Strength, "Item.BaseType.Sword.TwoHand.Strength", "Item Tag that represents Two Hand Strength Sword item base type.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_BaseType_Sword_TwoHand_Intelligence, "Item.BaseType.Sword.TwoHand.Intelligence", "Item Tag that represents Two Hand Intelligence Sword item base type.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_BaseType_Axe_OneHand_Faith, "Item.BaseType.Axe.OneHand.Faith", "Item Tag that represents One Hand Faith Axe item base type.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_BaseType_Axe_OneHand_Dexterity, "Item.BaseType.Axe.OneHand.Dexterity", "Item Tag that represents One Hand Dexterity Axe item base type.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_BaseType_Axe_OneHand_Strength, "Item.BaseType.Axe.OneHand.Strength", "Item Tag that represents One Hand Strength Axe item base type.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_BaseType_Axe_OneHand_Intelligence, "Item.BaseType.Axe.OneHand.Intelligence", "Item Tag that represents One Hand Intelligence Axe item base type.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_BaseType_Axe_TwoHand_Faith, "Item.BaseType.Axe.TwoHand.Faith", "Item Tag that represents Two Hand Faith Axe item base type.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_BaseType_Axe_TwoHand_Dexterity, "Item.BaseType.Axe.OneHand.Dexterity", "Item Tag that represents Two Hand Dexterity Axe item base type.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_BaseType_Axe_TwoHand_Strength, "Item.BaseType.Axe.OneHand.Strength", "Item Tag that represents Two Hand Strength Axe item base type.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_BaseType_Axe_TwoHand_Intelligence, "Item.BaseType.Axe.OneHand.Intelligence", "Item Tag that represents Two Hand Intelligence Axe item base type.");

	/**
	 * Equipment Slots.
	 */
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Slot_Equipment_Weapon_RightHand, "Item.Slot.Equipment.Weapon.RightHand", "Item Tag representing Right Hand Equipment Slot.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Slot_Equipment_Weapon_LeftHand, "Item.Slot.Equipment.Weapon.LeftHand", "Item Tag representing Left Hand Equipment Slot.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_SwapSlot_Equipment_Weapon_RightHand, "Item.SwapSlot.Equipment.Weapon.RightHand", "Item Tag representing Right Hand Equipment Slot Swap.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_SwapSlot_Equipment_Weapon_LeftHand, "Item.SwapSlot.Equipment.Weapon.LeftHand", "Item Tag representing Left Hand Equipment Slot Swap.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Slot_Equipment_Helmet, "Item.Slot.Equipment.Helmet", "Item Tag representing Helmet Equipment Slot.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Slot_Equipment_BodyArmor, "Item.Slot.Equipment.BodyArmor", "Item Tag representing Body Armor Equipment Slot.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Slot_Equipment_Belt, "Item.Slot.Equipment.Belt", "Item Tag representing Belt Equipment Slot.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Slot_Equipment_Gloves, "Item.Slot.Equipment.Gloves", "Item Tag representing Gloves Equipment Slot.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Slot_Equipment_Boots, "Item.Slot.Equipment.Boots", "Item Tag representing Boots Equipment Slot.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Slot_Equipment_Amulet, "Item.Slot.Equipment.Amulet", "Item Tag representing Amulet Equipment Slot.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Slot_Equipment_Ring_RightHand, "Item.Slot.Equipment.Ring.RightHand", "Item Tag representing Right Ring Equipment Slot.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Slot_Equipment_Ring_LeftHand, "Item.Slot.Equipment.Ring.LeftHand", "Item Tag representing Left Ring Equipment Slot.");

	const TArray<FGameplayTag> EquipmentSlots =
		{
			Item_Slot_Equipment_Weapon_RightHand,
			Item_Slot_Equipment_Weapon_LeftHand,
			Item_Slot_Equipment_Helmet,
			Item_Slot_Equipment_BodyArmor,
			Item_Slot_Equipment_Belt,
			Item_Slot_Equipment_Gloves,
			Item_Slot_Equipment_Boots,
			Item_Slot_Equipment_Amulet,
			Item_Slot_Equipment_Ring_RightHand,
			Item_Slot_Equipment_Ring_LeftHand
		};

	
	const TMap<FGameplayTag, FName>& GetSlotToAttachSocketMap()
	{
		static const TMap<FGameplayTag, FName> SlotToAttachSocketMap =
		{
			{Item_Slot_Equipment_Weapon_RightHand, ObsidianMeshSocketNames::RightHandWeaponSocket},
			{Item_Slot_Equipment_Weapon_LeftHand, ObsidianMeshSocketNames::LeftHandWeaponSocket}
		};

		return SlotToAttachSocketMap;
	}

	const TMap<FGameplayTag, FGameplayTagContainer>& GetSisterSlotAcceptedCategoriesMap()
	{
		static const TArray<FGameplayTag> OneHandAcceptedEquipmentCategories =
		{
			Item_Category_Equipment_Dagger, Item_Category_Equipment_Wand, Item_Category_Equipment_Flail,
			Item_Category_Equipment_Mace_OneHand, Item_Category_Equipment_Sword_OneHand,
			Item_Category_Equipment_Axe_OneHand, Item_Category_Equipment_Shield
		};

		static const TMap<FGameplayTag, FGameplayTagContainer> SisterSlotAcceptedEquipmentCategoriesForWeaponCategory =
		{
			{Item_Category_Equipment_Dagger, FGameplayTagContainer::CreateFromArray(OneHandAcceptedEquipmentCategories)},
			{Item_Category_Equipment_Wand, FGameplayTagContainer::CreateFromArray(OneHandAcceptedEquipmentCategories)},
			{Item_Category_Equipment_Flail, FGameplayTagContainer::CreateFromArray(OneHandAcceptedEquipmentCategories)},
			{Item_Category_Equipment_Bow, FGameplayTagContainer(Item_Category_Equipment_Quiver)},
			{Item_Category_Equipment_Quiver, FGameplayTagContainer(Item_Category_Equipment_Bow)},
			{Item_Category_Equipment_Staff_TwoHand, FGameplayTagContainer::EmptyContainer},
			{Item_Category_Equipment_Mace_OneHand, FGameplayTagContainer::CreateFromArray(OneHandAcceptedEquipmentCategories)},
			{Item_Category_Equipment_Mace_TwoHand, FGameplayTagContainer::EmptyContainer},
			{Item_Category_Equipment_Sword_OneHand, FGameplayTagContainer::CreateFromArray(OneHandAcceptedEquipmentCategories)},
			{Item_Category_Equipment_Sword_TwoHand, FGameplayTagContainer::EmptyContainer},
			{Item_Category_Equipment_Axe_OneHand, FGameplayTagContainer::CreateFromArray(OneHandAcceptedEquipmentCategories)},
			{Item_Category_Equipment_Axe_TwoHand, FGameplayTagContainer::EmptyContainer},
			{Item_Category_Equipment_Shield, FGameplayTagContainer::CreateFromArray(OneHandAcceptedEquipmentCategories)}	
		};

		return SisterSlotAcceptedEquipmentCategoriesForWeaponCategory;
	}

	/**
	 * Functional Slots.
	 */

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Slot_Functional_ScrollOfIdentification, "Item.Slot.Functional.ScrollOfIdentification", "Item Tag representing functional Scroll of Identification slot.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Slot_Functional_ScrollOfTeleportation, "Item.Slot.Functional.ScrollOfTeleportation", "Item Tag representing functional Scroll of Teleportation slot.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Slot_Functional_OrbOfEnchantment, "Item.Slot.Functional.OrbOfEnchantment", "Item Tag representing functional Orb of Enchantment slot.");

	/**
	 * Stash Tabs.
	 */

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(StashTab_Grid_1, "StashTab.Grid.1", "ID for Grid 1 Stash Tab.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(StashTab_Grid_2, "StashTab.Grid.2", "ID for Grid 2 Stash Tab.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(StashTab_Grid_3, "StashTab.Grid.3", "ID for Grid 3 Stash Tab.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(StashTab_Grid_4, "StashTab.Grid.4", "ID for Grid 4 Stash Tab.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(StashTab_BigGrid_1, "StashTab.BigGrid.1", "ID for Big Grid 1 Stash Tab.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(StashTab_Functional, "StashTab.Functional", "ID for Functional Stash Tab.");
	
	/**
	 * Stack Counts
	 */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_StackCount_Current, "Item.StackCount.Current", "Item Tag representing the current number of stacked items.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_StackCount_Max, "Item.StackCount.Max", "Item Tag representing the Max Stack Count the item have, after which it will no longer stack and will need another space in the inventory.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_StackCount_Limit, "Item.StackCount.Limit", "Item Tag representing the Total number of items the Player can have, can be used for mission items which can be limited to 1.");

	const TArray<FGameplayTag> StackTypes =
		{
			Item_StackCount_Current,
			Item_StackCount_Max,
			Item_StackCount_Limit
		};
	
	/**
	 * Affixes
	 */

	/**
	 * Skill Implicits
	 */

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_SkillImplicits_Attack_Witch_FireBall, "Item.Affix.SkillImplicits.Attack.Witch.FireBall", "Item Tag which represents Fire Ball Skill Implicit.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_SkillImplicits_Attack_Barbarian_Slash, "Item.Affix.SkillImplicits.Attack.Barbarian.Slash", "Item Tag which represents Slash Skill Implicit.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_SkillImplicits_Attack_Assassin_FlyingKnife, "Item.Affix.SkillImplicits.Attack.Assassin.FlyingKnife", "Item Tag which represents Flying Knife Skill Implicit.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_SkillImplicits_Attack_Paladin_BlessedHammer, "Item.Affix.SkillImplicits.Attack.Paladin.BlessedHammer", "Item Tag which represents Blessed Hammer Skill Implicit.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_SkillImplicits_Defence_MagmaBarrier, "Item.Affix.SkillImplicits.Defence.MagmaBarrier", "Item Tag which represents Magma Barrier Skill Implicit.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_SkillImplicits_Defence_FrozenArmor, "Item.Affix.SkillImplicits.Defence.FrozenArmor", "Item Tag which represents Frozen Armor Skill Implicit.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_SkillImplicits_Defence_TransientArmor, "Item.Affix.SkillImplicits.Defence.TransientArmor", "Item Tag which represents Transient Armor Skill Implicit.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_SkillImplicits_Defence_SparklingBarrier, "Item.Affix.SkillImplicits.Defence.SparklingBarrier", "Item Tag which represents Sparkling Barrier Skill Implicit.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_SkillImplicits_Defence_RaisedShield, "Item.Affix.SkillImplicits.Defence.RaisedShield", "Item Tag which represents Raised Shield Skill Implicit.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_SkillImplicits_Defence_VivitRegeneration, "Item.Affix.SkillImplicits.Defence.VivitRegeneration", "Item Tag which represents Vivit Regeneration Skill Implicit.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_SkillImplicits_Movement_Roll, "Item.Affix.SkillImplicits.Movement.Roll", "Item Tag which represents Roll Skill Implicit.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_SkillImplicits_Movement_Jump, "Item.Affix.SkillImplicits.Movement.Jump", "Item Tag which represents Jump Skill Implicit.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_SkillImplicits_Movement_ShieldCharge, "Item.Affix.SkillImplicits.Movement.ShieldCharge", "Item Tag which represents Shield Charge Skill Implicit.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_SkillImplicits_Movement_Blink, "Item.Affix.SkillImplicits.Movement.Blink", "Item Tag which represents Blink Skill Implicit.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_SkillImplicits_Movement_Teleport, "Item.Affix.SkillImplicits.Movement.Teleport", "Item Tag which represents Teleport Skill Implicit.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_SkillImplicits_Aura_Defiance, "Item.Affix.SkillImplicits.Aura.Defiance", "Item Tag which represents Defiance Skill Implicit.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_SkillImplicits_Aura_Hatred, "Item.Affix.SkillImplicits.Aura.Hatred", "Item Tag which represents Hatred Skill Implicit.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_SkillImplicits_Aura_FoulPresence, "Item.Affix.SkillImplicits.Aura.FoulPresence", "Item Tag which represents Foul Presence Skill Implicit.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_SkillImplicits_Aura_Domination, "Item.Affix.SkillImplicits.Aura.Domination", "Item Tag which represents Domination Skill Implicit.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_SkillImplicits_Aura_ElementalConflux, "Item.Affix.SkillImplicits.Aura.ElementalConflux", "Item Tag which represents Elemental Conflux Skill Implicit.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_SkillImplicits_Ultimate_VoidSphere, "Item.Affix.SkillImplicits.Ultimate.VoidSphere", "Item Tag which represents Void Sphere Skill Implicit.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_SkillImplicits_Ultimate_Combustion, "Item.Affix.SkillImplicits.Ultimate.Combustion", "Item Tag which represents Combustion Skill Implicit.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_SkillImplicits_Ultimate_Shatter, "Item.Affix.SkillImplicits.Ultimate.Shatter", "Item Tag which represents Shatter Skill Implicit.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_SkillImplicits_Ultimate_Electrocution, "Item.Affix.SkillImplicits.Ultimate.Electrocution", "Item Tag which represents Electrocution Skill Implicit.");

	/**
	 * Implicits
	 */

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Implicit_Life_LifeFlat, "Item.Affix.Implicit.Life.LifeFlat", "Item Tag which represents Flat Life Implicit.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Implicit_Mana_ManaFlat, "Item.Affix.Implicit.Mana.ManaFlat", "Item Tag which represents Flat Mana Implicit.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Implicits_Resistance_Fire, "Item.Affix.Implicits.Resistance.Fire", "Item Tag which represents Fire Resistance Implicits.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Implicits_Resistance_Lightning, "Item.Affix.Implicits.Resistance.Lightning", "Item Tag which represents Lightning Resistance Implicits.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Implicits_Resistance_Cold, "Item.Affix.Implicits.Resistance.Cold", "Item Tag which represents Cold Resistance Implicits.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Implicits_Resistance_Chaos, "Item.Affix.Implicits.Resistance.Chaos", "Item Tag which represents Chaos Resistance Implicits.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Implicits_MaxResistance_Fire, "Item.Affix.Implicits.MaxResistance.Fire", "Item Tag which represents Max Fire Resistance Implicits.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Implicits_MaxResistance_Lightning, "Item.Affix.Implicits.MaxResistance.Lightning", "Item Tag which represents Max Lightning Resistance Implicits.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Implicits_MaxResistance_Cold, "Item.Affix.Implicits.MaxResistance.Cold", "Item Tag which represents Max Cold Resistance Implicits.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Implicits_MaxResistance_Chaos, "Item.Affix.Implicits.MaxResistance.Chaos", "Item Tag which represents Max Chaos Resistance Implicits.");

	/**
	 * Prefixes
	 */

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Prefix_Life_LifeFlat, "Item.Affix.Prefix.Life.LifeFlat", "Item Tag which represents Flat Life Prefix.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Prefix_Mana_ManaFlat, "Item.Affix.Prefix.Mana.ManaFlat", "Item Tag which represents Flat Mana Prefix.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Prefix_Utility_MagicFind, "Item.Affix.Prefix.Utility.MagicFind", "Item Tag which represents Magic Find Prefix.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Prefix_Defence_ArmorFlat, "Item.Affix.Prefix.Defence.ArmorFlat", "Item Tag which represents Flat Armor Prefix.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Prefix_Defence_ArmorPercent, "Item.Affix.Prefix.Defence.ArmorPercent", "Item Tag which represents Percent Armor Prefix.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Prefix_Defence_EvasionFlat, "Item.Affix.Prefix.Defence.EvasionFlat", "Item Tag which represents Flat Evasion Prefix.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Prefix_Defence_EvasionPercent, "Item.Affix.Prefix.Defence.EvasionPercent", "Item Tag which represents Percent Evasion Prefix.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Prefix_Defence_EnergyShieldFlat, "Item.Affix.Prefix.Defence.EnergyShieldFlat", "Item Tag which represents Flat Energy Shield Prefix.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Prefix_Defence_EnergyShieldPercent, "Item.Affix.Prefix.Defence.EnergyShieldPercent", "Item Tag which represents Percent Energy Shield Prefix.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Prefix_DamageMultiplier_IncreasePhysicalDamage, "Item.Affix.Prefix.DamageMultiplier.IncreasePhysicalDamage", "Item Tag which represents Multiplier Physical Damage Increase Prefix.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Prefix_DamageMultiplier_IncreaseFireDamage, "Item.Affix.Prefix.DamageMultiplier.IncreaseFireDamage", "Item Tag which represents Multiplier Fire Damage Increase Prefix.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Prefix_DamageMultiplier_IncreaseColdDamage, "Item.Affix.Prefix.DamageMultiplier.IncreaseColdDamage", "Item Tag which represents Multiplier Cold Damage Increase Prefix.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Prefix_DamageMultiplier_IncreaseLightningDamage, "Item.Affix.Prefix.DamageMultiplier.IncreaseLightningDamage", "Item Tag which represents Multiplier Lightning Damage Increase Prefix.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Prefix_DamageMultiplier_IncreaseChaosDamage, "Item.Affix.Prefix.DamageMultiplier.IncreaseChaosDamage", "Item Tag which represents Multiplier Chaos Damage Increase Prefix.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Prefix_DamageRange_PhysicalDamageFlat, "Item.Affix.Prefix.DamageRange.PhysicalDamageFlat", "Item Tag which represents Flat Physical Damage Range Prefix.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Prefix_DamageRange_PhysicalDamagePercentage, "Item.Affix.Prefix.DamageRange.PhysicalDamagePercentage", "Item Tag which represents Percentage Physical Damage Range Prefix.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Prefix_DamageRange_FireDamageFlat, "Item.Affix.Prefix.DamageRange.FireDamageFlat", "Item Tag which represents Flat Fire Damage Range Prefix.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Prefix_DamageRange_FireDamagePercentage, "Item.Affix.Prefix.DamageRange.FireDamagePercentage", "Item Tag which represents Percentage Fire Damage Range Prefix.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Prefix_DamageRange_ColdDamageFlat, "Item.Affix.Prefix.DamageRange.ColdDamageFlat", "Item Tag which represents Flat Cold Damage Range Prefix.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Prefix_DamageRange_ColdDamagePercentage, "Item.Affix.Prefix.DamageRange.ColdDamagePercentage", "Item Tag which represents Percentage Cold Damage Range Prefix.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Prefix_DamageRange_LightningDamageFlat, "Item.Affix.Prefix.DamageRange.LightningDamageFlat", "Item Tag which represents Flat Lightning Damage Range Prefix.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Prefix_DamageRanger_LightningDamagePercentage, "Item.Affix.Prefix.DamageRange.LightningDamagePercentage", "Item Tag which represents Percentage Lightning Damage Range Prefix.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Prefix_DamageRanger_ChaosDamageFlat, "Item.Affix.Prefix.DamageRange.ChaosDamageFlat", "Item Tag which represents Flat Chaos Damage Range Prefix.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Prefix_DamageRanger_ChaosDamagePercentage, "Item.Affix.Prefix.DamageRange.ChaosDamagePercentage", "Item Tag which represents Percentage Chaos Damage Range Prefix.");
 
	/**
	 * Suffixes
	 */
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Suffix_Attribute_Dexterity, "Item.Affix.Suffix.Attribute.Dexterity", "Item Tag which represents Dexterity Suffix.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Suffix_Attribute_Intelligence, "Item.Affix.Suffix.Attribute.Intelligence", "Item Tag which represents Intelligence Suffix.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Suffix_Attribute_Strength, "Item.Affix.Suffix.Attribute.Strength", "Item Tag which represents Strength Suffix.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Suffix_Attribute_Faith, "Item.Affix.Suffix.Attribute.Faith", "Item Tag which represents Faith Suffix.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Suffix_Resistance_Fire, "Item.Affix.Suffix.Resistance.Fire", "Item Tag which represents Fire Resistance Suffix.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Suffix_Resistance_Lightning, "Item.Affix.Suffix.Resistance.Lightning", "Item Tag which represents Lightning Resistance Suffix.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Suffix_Resistance_Cold, "Item.Affix.Suffix.Resistance.Cold", "Item Tag which represents Cold Resistance Suffix.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Suffix_Resistance_Chaos, "Item.Affix.Suffix.Resistance.Chaos", "Item Tag which represents Chaos Resistance Suffix.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Suffix_MaxResistance_Fire, "Item.Affix.Suffix.MaxResistance.Fire", "Item Tag which represents Max Fire Resistance Suffix.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Suffix_MaxResistance_Lightning, "Item.Affix.Suffix.MaxResistance.Lightning", "Item Tag which represents Max Lightning Resistance Suffix.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Suffix_MaxResistance_Cold, "Item.Affix.Suffix.MaxResistance.Cold", "Item Tag which represents Max Cold Resistance Suffix.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Suffix_MaxResistance_Chaos, "Item.Affix.Suffix.MaxResistance.Chaos", "Item Tag which represents Max Chaos Resistance Suffix.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Suffix_Utility_MagicFind, "Item.Affix.Suffix.Utility.MagicFind", "Item Tag which represents Magic Find Suffix.");
	
	/**
	 * Enchanted Affixes
	 */
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Suffix_Enchant_MaximumLifePercentage, "Item.Affix.Suffix.Enchant.MaximumLifePercentage", "Item Tag which represents Enchant that gives Percentage Maximum Life Affix.");
	
}
