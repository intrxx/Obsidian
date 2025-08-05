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
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_Equipment_Changed, "Message.Equipment.Changed", "Tag used in Gameplay Message Subsystem to represent Equipment state change.")
	
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
	 * Item Category.
	 */

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Resource, "Item.Category.Resource", "Item Tag that represents Resource item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Functional, "Item.Category.Functional", "Item Tag that represents Functional item category.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Helmet, "Item.Category.Helmet", "Item Tag that represents Helmet item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_BodyArmor, "Item.Category.BodyArmor", "Item Tag that represents Body Armor item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Belt, "Item.Category.Belt", "Item Tag that represents Belt item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Gloves, "Item.Category.Gloves", "Item Tag that represents Gloves item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Boots, "Item.Category.Boots", "Item Tag that represents Boots item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Shield, "Item.Category.Shield", "Item Tag that represents Shield item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Quiver, "Item.Category.Quiver", "Item Tag that represents Quiver item category.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Amulet, "Item.Category.Amulet", "Item Tag that represents Amulet item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Ring, "Item.Category.Ring", "Item Tag that represents Ring item category.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Dagger, "Item.Category.Dagger", "Item Tag that represents Dagger item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Wand, "Item.Category.Wand", "Item Tag that represents Wand item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Flail, "Item.Category.Flail", "Item Tag that represents Flail item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Bow, "Item.Category.Bow", "Item Tag that represents Bow item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Staff_TwoHand, "Item.Category.Staff.TwoHand", "Item Tag that represents Staff item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Mace_OneHand, "Item.Category.Mace.OneHand", "Item Tag that represents One Hand Mace item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Mace_TwoHand, "Item.Category.Mace.TwoHand", "Item Tag that represents Two Hand Mace item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Sword_OneHand, "Item.Category.Sword.OneHand", "Item Tag that represents One Hand Sword item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Sword_TwoHand, "Item.Category.Sword.TwoHand", "Item Tag that represents Two Hand Sword item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Axe_OneHand, "Item.Category.Axe.OneHand", "Item Tag that represents One Hand Axe item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_Axe_TwoHand, "Item.Category.Axe.TwoHand", "Item Tag that represents Two Hand Axe item category.");

	/**
	 * Individual Item Tags.
	 */

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_ScrollOfIdentification, "Item.Category.ScrollOfIdentification", "Item Tag that represents Scroll Of Identification item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_ScrollOfTeleportation, "Item.Category.ScrollOfTeleportation", "Item Tag that represents Scroll Of Teleportation item category.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Category_OrbOfEnchantment, "Item.Category.OrbOfEnchantment", "Item Tag that represents Orb Of Enchantment item category.");

	/**
	 * Equipment Slots.
	 */
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Equipment_Slot_Weapon_RightHand, "Equipment.Slot.Weapon.RightHand", "Item Tag representing Right Hand Equipment Slot.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Equipment_Slot_Weapon_LeftHand, "Equipment.Slot.Weapon.LeftHand", "Item Tag representing Left Hand Equipment Slot.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Equipment_SwapSlot_Weapon_RightHand, "Equipment.SwapSlot.Weapon.RightHand", "Item Tag representing Right Hand Equipment Slot Swap.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Equipment_SwapSlot_Weapon_LeftHand, "Equipment.SwapSlot.Weapon.LeftHand", "Item Tag representing Left Hand Equipment Slot Swap.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Equipment_Slot_Helmet, "Equipment.Slot.Helmet", "Item Tag representing Helmet Equipment Slot.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Equipment_Slot_BodyArmor, "Equipment.Slot.BodyArmor", "Item Tag representing Body Armor Equipment Slot.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Equipment_Slot_Belt, "Equipment.Slot.Belt", "Item Tag representing Belt Equipment Slot.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Equipment_Slot_Gloves, "Equipment.Slot.Gloves", "Item Tag representing Gloves Equipment Slot.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Equipment_Slot_Boots, "Equipment.Slot.Boots", "Item Tag representing Boots Equipment Slot.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Equipment_Slot_Amulet, "Equipment.Slot.Amulet", "Item Tag representing Amulet Equipment Slot.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Equipment_Slot_RightRing, "Equipment.Slot.RightRing", "Item Tag representing Right Ring Equipment Slot.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Equipment_Slot_LeftRing, "Equipment.Slot.LeftRing", "Item Tag representing Left Ring Equipment Slot.");

	const TArray<FGameplayTag> EquipmentSlots =
		{
			Equipment_Slot_Weapon_RightHand,
			Equipment_Slot_Weapon_LeftHand,
			Equipment_Slot_Helmet,
			Equipment_Slot_BodyArmor,
			Equipment_Slot_Belt,
			Equipment_Slot_Gloves,
			Equipment_Slot_Boots,
			Equipment_Slot_Amulet,
			Equipment_Slot_RightRing,
			Equipment_Slot_LeftRing
		};

	
	const TMap<FGameplayTag, FName>& GetSlotToAttachSocketMap()
	{
		static const TMap<FGameplayTag, FName> SlotToAttachSocketMap =
		{
			{Equipment_Slot_Weapon_RightHand, ObsidianMeshSocketNames::RightHandWeaponSocket},
			{Equipment_Slot_Weapon_LeftHand, ObsidianMeshSocketNames::LeftHandWeaponSocket}
		};

		return SlotToAttachSocketMap;
	}

	const TMap<FGameplayTag, FGameplayTagContainer>& GetSisterSlotAcceptedCategoriesMap()
	{
		static const TArray<FGameplayTag> OneHandAcceptedEquipmentCategories =
		{
			Item_Category_Dagger, Item_Category_Wand, Item_Category_Flail, Item_Category_Mace_OneHand, Item_Category_Sword_OneHand,
			Item_Category_Axe_OneHand, Item_Category_Shield
		};

		static const TMap<FGameplayTag, FGameplayTagContainer> SisterSlotAcceptedEquipmentCategoriesForWeaponCategory =
		{
			{Item_Category_Dagger, FGameplayTagContainer::CreateFromArray(OneHandAcceptedEquipmentCategories)},
			{Item_Category_Wand, FGameplayTagContainer::CreateFromArray(OneHandAcceptedEquipmentCategories)},
			{Item_Category_Flail, FGameplayTagContainer::CreateFromArray(OneHandAcceptedEquipmentCategories)},
			{Item_Category_Bow, FGameplayTagContainer(Item_Category_Quiver)},
			{Item_Category_Quiver, FGameplayTagContainer(Item_Category_Bow)},
			{Item_Category_Staff_TwoHand, FGameplayTagContainer::EmptyContainer},
			{Item_Category_Mace_OneHand, FGameplayTagContainer::CreateFromArray(OneHandAcceptedEquipmentCategories)},
			{Item_Category_Mace_TwoHand, FGameplayTagContainer::EmptyContainer},
			{Item_Category_Sword_OneHand, FGameplayTagContainer::CreateFromArray(OneHandAcceptedEquipmentCategories)},
			{Item_Category_Sword_TwoHand, FGameplayTagContainer::EmptyContainer},
			{Item_Category_Axe_OneHand, FGameplayTagContainer::CreateFromArray(OneHandAcceptedEquipmentCategories)},
			{Item_Category_Axe_TwoHand, FGameplayTagContainer::EmptyContainer},
			{Item_Category_Shield, FGameplayTagContainer::CreateFromArray(OneHandAcceptedEquipmentCategories)}	
		};

		return SisterSlotAcceptedEquipmentCategoriesForWeaponCategory;
	}

	/**
	 * Currency Slots.
	 */

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Currency_Slot_ScrollOfIdentification, "Currency.Slot.ScrollOfIdentification", "Item Tag representing currency Scroll of Identification slot.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Currency_Slot_ScrollOfTeleportation, "Currency.Slot.ScrollOfTeleportation", "Item Tag representing currency Scroll of Teleportation slot.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Currency_Slot_OrbOfEnchantment, "Currency.Slot.OrbOfEnchantment", "Item Tag representing currency Orb of Enchantment slot.");

	/**
	 * Stash Tabs.
	 */

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(StashTab_Grid_1, "StashTab.Grid.1", "ID for Grid 1 Stash Tab.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(StashTab_Grid_2, "StashTab.Grid.2", "ID for Grid 2 Stash Tab.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(StashTab_Grid_3, "StashTab.Grid.3", "ID for Grid 3 Stash Tab.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(StashTab_Grid_4, "StashTab.Grid.4", "ID for Grid 4 Stash Tab.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(StashTab_BigGrid_1, "StashTab.BigGrid.1", "ID for Big Grid 1 Stash Tab.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(StashTab_Currency, "StashTab.Currency", "ID for Currency Stash Tab.");
	
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
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Affix_Enchant_MaximumLifePercentage, "Item.Affix.Enchant.MaximumLifePercentage", "Item Tag which represents Enchant that gives Percentage Maximum Life Affix.")
	
}
