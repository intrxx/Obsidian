// Copyright 2026 out of sCope team - intrxx

#include "ObsidianGameplayTags.h"

#include "GameplayTagsManager.h"

#include "ObsidianLogCategories.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"


namespace ObsidianGameplayTags
{
	FGameplayTag FindTagByString(const FString& InTagString, bool bInMatchPartialString)
	{
		const UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();
		FGameplayTag ReturnTag = TagsManager.RequestGameplayTag(FName(*InTagString), false);

		if(bInMatchPartialString && !ReturnTag.IsValid())
		{
			FGameplayTagContainer AllTags;
			TagsManager.RequestAllGameplayTags(AllTags, true);

			for(const FGameplayTag& Tag : AllTags)
			{
				if(Tag.ToString().Contains(InTagString))
				{
					UE_LOG(ObLogGeneral, Log, TEXT("Did not find exact match for [%s] but found partial match on tag [%s]."), *InTagString, *Tag.ToString());
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

	namespace UI::Layer
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(MainMenu, "UI.Layer.MainMenu", "Layer tag used when pushing widgets to layer in main menu.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayMenu, "UI.Layer.GameplayMenu", "Layer tag used when pushing Menu widgets to layer in Gameplay.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay, "UI.Layer.Gameplay", "Layer tag used when pushing gameplay widgets to layer in Gameplay.");
	}
	
	/**
	 * ---- Damage Types ----
	 */
	
	namespace DamageType
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Physical, "DamageType.Physical", "Physical Damage Type - also used for SetByCaller.");
	}

	namespace DamageType::Elemental
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Fire, "DamageType.Elemental.Fire", "Fire Damage Type - also used for SetByCaller.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cold, "DamageType.Elemental.Cold", "Cold Damage Type - also used for SetByCaller.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Lightning, "DamageType.Elemental.Lightning", "Lightning Damage Type - also used for SetByCaller.");
	}

	namespace DamageType
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Chaos, "DamageType.Chaos", "Chaos Damage Type - also used for SetByCaller.");
	}
	
	const TArray<FGameplayTag> DamageTypes =
		{
			DamageType::Physical,
			DamageType::Elemental::Fire,
			DamageType::Elemental::Cold,
			DamageType::Elemental::Lightning,
			DamageType::Chaos
		};
	
	/**
	 * ---- Effects ----
	 */
	
	namespace Effect
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitReact, "Effect.HitReact", "Tag used for activating the Hit React ability.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stagger, "Effect.Stagger", "Tag used for activating the Stagger ability.");
	}

	/**
	 * ---- Movement ----
	 */
	
	namespace Movement::State
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Standing, "Movement.State.Standing", "Player has this tag when he is standing.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Walking, "Movement.State.Walking", "Player has this tag when he is walking.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Running, "Movement.State.Running", "Player has this tag when he is running.");
	}
	
	/**
	 * ---- Statuses ----
	 */
	
	namespace Status::Death
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Death, "Status.Death", "Death has the death status.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Dying, "Status.Death.Dying", "Death started for the target.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Dead, "Status.Death.Dead", "Death finished for the target.");
	}
	
	namespace Status
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Immunity, "Status.Immunity", "Target has immunity to all effects.");
	}

	/**
	 * ---- Input ----
	 */
	
	/**
	 * Native
	 */
	namespace Input::Native::Move
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Keyboard, "Input.Native.Move.Keyboard", "Move input.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Mouse, "Input.Native.Move.Mouse", "Move input.");
	}
	
	namespace Input::Native
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ReleaseUsingItem, "Input.Native.ReleaseUsingItem", "Input for easy release of Using Item.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ReleaseContinouslyUsingItem, "Input.Native.ReleaseContinouslyUsingItem", "Input for release of continuous Using Item.");
	}

	namespace Input
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(DropItem, "Input.DropItem", "Additional input for dropping the item.");
	
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Interact, "Input.Interact", "Additional input for interacting with other actors in the world.");
	}
	
	namespace Input::Native
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(CharacterStatus, "Input.Native.CharacterStatus", "Character Status toggle input.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Inventory, "Input.Native.Inventory", "Inventory toggle input.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(PassiveSkillTree, "Input.Native.PassiveSkillTree", "Passive Skill Tree toggle input.");
	
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(WeaponSwap, "Input.Native.WeaponSwap", "Input for Weapon Swap.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ToggleWalk, "Input.Native.ToggleWalk", "Input for Toggling Walk.");
	}
	
	namespace Input::UI
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(MainMenu, "Input.UI.MainMenu", "Main Menu Inputs for Layers.");
	}

	namespace Input::UI::Action
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Backwards, "Input.UI.Action.Backwards", "Input for going back a layer in UI.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(OpenGameplayMenu, "Input.UI.Action.OpenGameplayMenu", "Input for opening Gameplay Menu.");
	}
	
	namespace Input::UI
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ToggleHighlight, "Input.UI.ToggleHighlight", "Input for toggling the game highlight functionality.");
	}

	/**
	 * Ability
	 */
	namespace Input::Ability::Move
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Roll, "Input.Ability.Move.Roll", "Roll input.");
	}

	namespace Input::Ability
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability1, "Input.Ability.Ability1", "Ability1 input.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability2, "Input.Ability.Ability2", "Ability2 input.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability3, "Input.Ability.Ability3", "Ability3 input.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability4, "Input.Ability.Ability4", "Ability4 input.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability5, "Input.Ability.Ability5", "Ability5 input.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability6, "Input.Ability.Ability6", "Ability6 input.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability7, "Input.Ability.Ability7", "Ability7 input.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability8, "Input.Ability.Ability8", "Ability8 input.");
	}

	/**
	 * ---- Data ----
	 */
	
	namespace Data::AdvancedCombat
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Hit, "Data.AdvancedCombat.Hit", "Event fires when the enemy character is hit by the Advanced Combat Component.");
	}
	
	/**
	 * UI Data
	 */
	namespace UI
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(DataSpecifierTag, "UI.DataSpecifierTag", "Tag that is owned by any effect that wish to be displayed on Player's screen.");
	}

	namespace UI::EffectData::Flask
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(HealthHealing, "UI.EffectData.Flask.HealthHealing", "Tag used for displaying Health healing from Flasks Info on the UI.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ManaHealing, "UI.EffectData.Flask.ManaHealing", "Tag used for displaying Mana healing from Flasks Info on the UI.");
	}
	
	namespace UI::EffectData::Aura
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Health, "UI.EffectData.Aura.Health", "Tag used for displaying Health Aura Info on the UI.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Mana, "UI.EffectData.Aura.Mana", "Tag used for displaying Mana Aura Info on the UI.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Energy, "UI.EffectData.Aura.Energy", "Tag used for displaying Energy Aura Info on the UI.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Evasion, "UI.EffectData.Aura.Evasion", "Tag used for displaying Evasion Aura Info on the UI.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Armor, "UI.EffectData.Aura.Armor", "Tag used for displaying Armor Aura Info on the UI.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(CriticalDamage, "UI.EffectData.Aura.CriticalDamage", "Tag used for displaying Critical Damage Aura Info on the UI.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(CriticalChance, "UI.EffectData.Aura.CriticalChance", "Tag used for displaying Critical Chance Aura Info on the UI.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ChaosDamage, "UI.EffectData.Aura.ChaosDamage", "Tag used for displaying Chaos Damage Aura Info on the UI.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(PhysicalDamage, "UI.EffectData.Aura.PhysicalDamage", "Tag used for displaying Physical Damage Aura Info on the UI.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(CirclingElementalDamage, "UI.EffectData.Aura.CirclingElementalDamage", "Tag used for displaying Circling Elemental Damage Aura Info on the UI.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(HealthRegeneration, "UI.EffectData.Aura.HealthRegeneration", "Tag used for displaying Health Regeneration Aura Info on the UI.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ManaRegeneration, "UI.EffectData.Aura.ManaRegeneration", "Tag used for displaying Mana Regeneration Aura Info on the UI.");
	}
	
	namespace UI::EffectData::Effect
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Poison, "UI.EffectData.Effect.Poison", "Tag used for displaying Poison Effect Info on the UI.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Chill, "UI.EffectData.Effect.Chill", "Tag used for displaying Chill Effect Info on the UI.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ignite, "UI.EffectData.Effect.Ignite", "Tag used for displaying Ignite Effect Info on the UI.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Shock, "UI.EffectData.Effect.Shock", "Tag used for displaying Shock Info on the UI.");
	}
	
	namespace UI::EffectData::Effect::Special
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Immunity, "UI.EffectData.Effect.Special.Immunity", "Tag used for displaying Immunity info on UI (specially Enemy Overlay Bar).");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(DamageReduction, "UI.EffectData.Effect.Special.DamageReduction", "Tag used for displaying Damage Reduction info on UI (specially Enemy Overlay Bar).");
	}
	
	namespace UI::EffectData::Effect::Curse
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ElementalWeakness, "UI.EffectData.Effect.Curse.ElementalWeakness", "Tag used for displaying Elemental Weakness Info on the UI.");
	}
	
	namespace UI::GlobeData
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(HealingHealth, "UI.GlobeData.HealingHealth", "Tag used for displaying the healing amount on the globe.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ReplenishingMana, "UI.GlobeData.ReplenishingMana", "Tag used for displaying the repleanish amount on the globe.");
	}

	/**
	 * ---- Gameplay Messages ----
	 */

	/**
	 * Inventory
	 */
	namespace Message::Inventory
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Changed, "Message.Inventory.Changed", "Tag used in Gameplay Message Subsystem to represent Inventory state change.")
	}

	/**
	 * Equipment
	 */
	namespace Message::Equipment
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Changed, "Message.Equipment.Changed", "Tag used in Gameplay Message Subsystem to represent Equipment state change.")
	}

	/**
	 * Player Stash
	 */
	namespace Message::PlayerStash
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Changed, "Message.PlayerStash.Changed", "Tag used in Gameplay Message Subsystem to represent Player Stash state change.")
	}
	
	/**
	 * ---- Gameplay Events ----
	 */
	
	namespace GameplayEvent
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Death, "GameplayEvent.Death", "Event fired on attributes component when character is out of health.");
	}

	/**
	 * Hero
	 */
	namespace GameplayEvent::AbilityMontage::Player
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Firebolt, "GameplayEvent.AbilityMontage.Player.Firebolt", "Tag used for triggering gameplay event for spawning firebolt.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(FlyingKnifes, "GameplayEvent.AbilityMontage.Player.FlyingKnifes", "Tag used for triggering gameplay event for spawning flying knifes.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(MagneticHammer, "GameplayEvent.AbilityMontage.Player.MagneticHammer", "Tag used for triggering gameplay event for spawning magnetic hammer.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Slash, "GameplayEvent.AbilityMontage.Player.Slash", "Tag used for triggering gameplay event for spawning slash.");
	}

	/**
	 * Tree Orc
	 */
	namespace GameplayEvent::AbilityMontage::TreeOrc
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(EquipWeapon, "GameplayEvent.AbilityMontage.TreeOrc.EquipWeapon", "Tag used for triggering the equip function on the enemy.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(SpawnComboProjectile, "GameplayEvent.AbilityMontage.TreeOrc.SpawnComboProjectile", "Tag used for triggering the projectile spawning in combo swing ability on the enemy.");
	}

	/**
	 * Ranged Goblin
	 */
	namespace GameplayEvent::AbilityMontage::RangedGoblin
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(SpawnSlingShotProj, "GameplayEvent.AbilityMontage.RangedGoblin.SpawnSlingShotProj", "Tag used for triggering the sling shot projectile on Ranged Goblin.");
	}
	
	/**
	 * Skeletal Mage
	 */
	namespace GameplayEvent::AbilityMontage::SkeletalMage
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(SpawnFireNova, "GameplayEvent.AbilityMontage.SkeletalMage.SpawnFireNova", "Tag used for triggering the fire nova spawn on Skeletal Mage ability.");
	}
	
	/**
	 * Sockets
	 */
	namespace GameplayEvent::AbilityMontage::Socket
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(RightHandWeapon, "GameplayEvent.AbilityMontage.Socket.RightHandWeapon", "Tag used for triggering gameplay event for damaging actor with right hand melee weapon. Used by Montage to retreive correct socket.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(LeftHandWeapon, "GameplayEvent.AbilityMontage.Socket.LeftHandWeapon", "Tag used for triggering gameplay event for damaging actor with left hand melee weapon. Used by Montage to retreive correct socket.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(RightHand, "GameplayEvent.AbilityMontage.Socket.RightHand", "Tag used for triggering gameplay event for damaging actor with melee right hand. Used by Montage to retreive correct socket.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(LeftHand, "GameplayEvent.AbilityMontage.Socket.LeftHand", "Tag used for triggering gameplay event for damaging actor with melee left hand. Used by Montage to retreive correct socket.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(BetweenHands, "GameplayEvent.AbilityMontage.Socket.BetweenHands", "Tag used for triggering gameplay event for damaging actor with melee attack between hands (could be used for slam or something). Used by Montage to retreive correct socket.");
	}

	/**
	* ---- Gameplay Cues ----
	*/
	
	namespace GameplayCue
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(MeleeImpact, "GameplayCue.MeleeImpact", "Tag used for Melee Impact GC.");
	}
	
	/**
	* ---- Ability Activation Tags ----
	*/
	
	namespace Ability::ActivationFail
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(IsDead, "Ability.ActivationFail.IsDead", "Ability failed to activate because its owner is dead.");
	}
	
	/**
	 * Shared
	 */
	namespace AbilityActivation
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitReact, "AbilityActivation.HitReact", "Tag used for activating hit react ability on owner.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stagger, "AbilityActivation.Stagger", "Tag used for activating stagger ability on owner.");
	
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Introduction, "AbilityActivation.Introduction", "Tag used for triggering activation of AI's Introduction ability (some roaring or equping a weapon).");
	}

	/**
	 * Zombie
	 */
	namespace AbilityActivation::Zombie
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(MeleeAttack, "AbilityActivation.Zombie.MeleeAttack", "Tag used for triggering activation of Zombie's melee attack.");
	}

	/**
	 * Ranged Goblin
	 */
	namespace AbilityActivation::RangedGoblin
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(BowAttack, "AbilityActivation.RangedGoblin.BowAttack", "Tag used for triggering activation of goblin's bow ranged attack.");
	}

	/**
	 * Skeletal Mage
	 */
	namespace AbilityActivation::SkeletalMage
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(FireBall, "AbilityActivation.SkeletalMage.FireBall", "Tag used for triggering activation of Skeletal Mage's fire ball spell.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(FireNova, "AbilityActivation.SkeletalMage.FireNova", "Tag used for triggering activation of Skeletal Mage's fire nova spell.");
	}
	
	/**
	 * Tree Orc
	 */
	namespace AbilityActivation::TreeOrc
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(UnarmedSwing, "AbilityActivation.TreeOrc.UnarmedSwing", "Tag used for triggering activation of TreeOrcs's swing melee attack.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(LeapAttack, "AbilityActivation.TreeOrc.LeapAttack", "Tag used for triggering activation of TreeOrcs's leap attack.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Equip, "AbilityActivation.TreeOrc.Equip", "Tag used for triggering activation of TreeOrcs's equip ability.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ComboSwing, "AbilityActivation.TreeOrc.ComboSwing", "Tag used for triggering activation of TreeOrcs's Combo Swing ability.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ArmedSwing, "AbilityActivation.TreeOrc.ArmedSwing", "Tag used for triggering activation of TreeOrcs's Armed Swing ability.");
	}

	/**
	 * ---- Cooldowns ----
	 */

	/**
	 * Shared
	 */
	namespace Ability::Cooldown
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitReact, "Ability.Cooldown.HitReact", "Tag used for Hit React Ability cooldown.");
	}

	/**
	 * Tree Orc
	 */
	namespace Ability::Cooldown::TreeOrc
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(LeapAttack, "Ability.Cooldown.TreeOrc.LeapAttack", "Tag used for Leap Attack Tree Orc's Ability cooldown.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ComboSwing, "Ability.Cooldown.TreeOrc.ComboSwing", "Tag used for Combo Swing Tree Orc's Ability cooldown.");
	}
	
	/**
	 * Skeletal Mage
	 */
	namespace Ability::Cooldown::SkeletalMage
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(FireNova, "Ability.Cooldown.SkeletalMage.FireNova", "Tag used for Fire Nova Skeletal Mage's Ability cooldown.");
	}

	/**
	 * ---- Items ----
	 */

	/**
	 * Inventory
	 */

	namespace Inventory
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(BlockActions, "Inventory.BlockActions", "When applied to player, he/she won't be able to modify inventory state.");
	}

	/**
	 * Equipment
	 */

	namespace Equipment
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(BlockActions, "Equipment.BlockActions", "When applied to player, he/she won't be able to modify equipment state.");
	}

	/**
	 * Player Stash
	 */

	namespace PlayerStash
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(BlockActions, "PlayerStash.BlockActions", "When applied to player, he/she won't be able to modify stash state.");
	}

	/**
	 * Item Category.
	 */

	namespace Item::Category::Currency
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Resource, "Item.Category.Currency.Resource", "Item Tag that represents Resource item category.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Functional, "Item.Category.Currency.Functional", "Item Tag that represents Functional item category.");
	}
	
	namespace Item::Category::Equipment::Armor
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Helmet, "Item.Category.Equipment.Armor.Helmet", "Item Tag that represents Helmet item category.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(BodyArmor, "Item.Category.Equipment.Armor.BodyArmor", "Item Tag that represents Body Armor item category.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Belt, "Item.Category.Equipment.Armor.Belt", "Item Tag that represents Belt item category.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gloves, "Item.Category.Equipment.Armor.Gloves", "Item Tag that represents Gloves item category.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Boots, "Item.Category.Equipment.Armor.Boots", "Item Tag that represents Boots item category.");
	}
	
	namespace Item::Category::Equipment::Offhand
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Shield, "Item.Category.Equipment.Offhand.Shield", "Item Tag that represents Shield item category.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Quiver, "Item.Category.Equipment.Offhand.Quiver", "Item Tag that represents Quiver item category.");
	}
	
	namespace Item::Category::Equipment::Jewellery
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Amulet, "Item.Category.Equipment.Jewellery.Amulet", "Item Tag that represents Amulet item category.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ring, "Item.Category.Equipment.Jewellery.Ring", "Item Tag that represents Ring item category.");
	}
	
	namespace Item::Category::Equipment::Weapon::Melee::OneHand
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Dagger, "Item.Category.Equipment.Weapon.Melee.OneHand.Dagger", "Item Tag that represents Dagger item category.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Flail, "Item.Category.Equipment.Weapon.Melee.OneHand.Flail", "Item Tag that represents Flail item category.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Mace, "Item.Category.Equipment.Weapon.Melee.OneHand.Mace", "Item Tag that represents One Hand Mace item category.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sword, "Item.Category.Equipment.Weapon.Melee.OneHand.Sword", "Item Tag that represents One Hand Sword item category.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Axe, "Item.Category.Equipment.Weapon.Melee.OneHand.Axe", "Item Tag that represents One Hand Axe item category.");
	}

	namespace Item::Category::Equipment::Weapon::Melee::TwoHand
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Mace, "Item.Category.Equipment.Weapon.Melee.TwoHand.Mace", "Item Tag that represents Two Hand Mace item category.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sword, "Item.Category.Equipment.Weapon.Melee.TwoHand.Sword", "Item Tag that represents Two Hand Sword item category.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Axe, "Item.Category.Equipment.Weapon.Melee.TwoHand.Axe", "Item Tag that represents Two Hand Axe item category.");
	}
	
	namespace Item::Category::Equipment::Weapon::Ranged::OneHand
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Wand, "Item.Category.Equipment.Weapon.Ranged.OneHand.Wand", "Item Tag that represents Wand item category.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Bow, "Item.Category.Equipment.Weapon.Ranged.OneHand.Bow", "Item Tag that represents Bow item category.");
	}

	namespace Item::Category::Equipment::Weapon::Ranged::TwoHand
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Staff, "Item.Category.Equipment.Weapon.Ranged.TwoHand.Staff", "Item Tag that represents Staff item category.");
	}

	/**
	 * Item Base Types.
	 */

	namespace Item::BaseType::Orb
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ScrollOfIdentification, "Item.BaseType.Orb.ScrollOfIdentification", "Item Tag that represents Scroll Of Identification item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ScrollOfTeleportation, "Item.BaseType.Orb.ScrollOfTeleportation", "Item Tag that represents Scroll Of Teleportation item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(OrbOfEnchantment, "Item.BaseType.Orb.OrbOfEnchantment", "Item Tag that represents Orb Of Enchantment item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(OrbOfRepentance, "Item.BaseType.Orb.OrbOfRepentance", "Item Tag that represents Orb Of Repentance item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(OrbOfEradication, "Item.BaseType.Orb.OrbOfEradication", "Item Tag that represents Orb Of Eradication item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(OrbOfRescription, "Item.BaseType.Orb.OrbOfRescription", "Item Tag that represents Orb Of Rescription item base type.");
	}

	namespace Item::BaseType::Helmet
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Armor, "Item.BaseType.Helmet.Armor", "Item Tag that represents Orb Of Armor Helmet item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Evasion, "Item.BaseType.Helmet.Evasion", "Item Tag that represents Evasion Helmet item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(EnergyShield, "Item.BaseType.Helmet.EnergyShield", "Item Tag that represents Energy Shield Helmet item base type.");
	}

	namespace Item::BaseType::BodyArmor
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Armor, "Item.BaseType.BodyArmor.Armor", "Item Tag that represents Armor Body Armor item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Evasion, "Item.BaseType.BodyArmor.Evasion", "Item Tag that represents Evasion Body Armor item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(EnergyShield, "Item.BaseType.BodyArmor.EnergyShield", "Item Tag that represents Energy Shield Body Armor item base type.");
	}
	
	namespace Item::BaseType
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ring, "Item.BaseType.Ring", "Item Tag that represents Ring item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Amulet, "Item.BaseType.Amulet", "Item Tag that represents Amulet item base type.");
	}

	namespace Item::BaseType::Sword::OneHand
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Faith, "Item.BaseType.Sword.OneHand.Faith", "Item Tag that represents One Hand Faith Sword item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Dexterity, "Item.BaseType.Sword.OneHand.Dexterity", "Item Tag that represents One Hand Dexterity Sword item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Strength, "Item.BaseType.Sword.OneHand.Strength", "Item Tag that represents One Hand Strength Sword item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Intelligence, "Item.BaseType.Sword.OneHand.Intelligence", "Item Tag that represents One Hand Intelligence Sword item base type.");
	}

	namespace Item::BaseType::Sword::TwoHand
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Faith, "Item.BaseType.Sword.TwoHand.Faith", "Item Tag that represents Two Hand Faith Sword item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Dexterity, "Item.BaseType.Sword.TwoHand.Dexterity", "Item Tag that represents Two Hand Sword Dexterity item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Strength, "Item.BaseType.Sword.TwoHand.Strength", "Item Tag that represents Two Hand Strength Sword item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Intelligence, "Item.BaseType.Sword.TwoHand.Intelligence", "Item Tag that represents Two Hand Intelligence Sword item base type.");
	}

	namespace Item::BaseType::Axe::OneHand
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Faith, "Item.BaseType.Axe.OneHand.Faith", "Item Tag that represents One Hand Faith Axe item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Dexterity, "Item.BaseType.Axe.OneHand.Dexterity", "Item Tag that represents One Hand Dexterity Axe item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Strength, "Item.BaseType.Axe.OneHand.Strength", "Item Tag that represents One Hand Strength Axe item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Intelligence, "Item.BaseType.Axe.OneHand.Intelligence", "Item Tag that represents One Hand Intelligence Axe item base type.");
	}

	namespace Item::BaseType::Axe::TwoHand
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Faith, "Item.BaseType.Axe.TwoHand.Faith", "Item Tag that represents Two Hand Faith Axe item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Dexterity, "Item.BaseType.Axe.TwoHand.Dexterity", "Item Tag that represents Two Hand Dexterity Axe item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Strength, "Item.BaseType.Axe.TwoHand.Strength", "Item Tag that represents Two Hand Strength Axe item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Intelligence, "Item.BaseType.Axe.TwoHand.Intelligence", "Item Tag that represents Two Hand Intelligence Axe item base type.");
	}

	namespace Item::BaseType::Mace::OneHand
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Faith, "Item.BaseType.Mace.OneHand.Faith", "Item Tag that represents One Hand Faith Mace item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Dexterity, "Item.BaseType.Mace.OneHand.Dexterity", "Item Tag that represents One Hand Dexterity Mace item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Strength, "Item.BaseType.Mace.OneHand.Strength", "Item Tag that represents One Hand Strength Mace item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Intelligence, "Item.BaseType.Mace.OneHand.Intelligence", "Item Tag that represents One Hand Intelligence Mace item base type.");
	}

	namespace Item::BaseType::Mace::TwoHand
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Faith, "Item.BaseType.Mace.TwoHand.Faith", "Item Tag that represents Two Hand Faith Mace item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Dexterity, "Item.BaseType.Mace.TwoHand.Dexterity", "Item Tag that represents Two Hand Dexterity Mace item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Strength, "Item.BaseType.Mace.TwoHand.Strength", "Item Tag that represents Two Hand Strength Mace item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Intelligence, "Item.BaseType.Mace.TwoHand.Intelligence", "Item Tag that represents Two Hand Intelligence Mace item base type.");
	}
	
	namespace Item::BaseType::Dagger
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Faith, "Item.BaseType.Dagger.Faith", "Item Tag that represents Faith Dagger item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Dexterity, "Item.BaseType.Dagger.Dexterity", "Item Tag that represents Dexterity Dagger item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Strength, "Item.BaseType.Dagger.Strength", "Item Tag that represents Strength Dagger item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Intelligence, "Item.BaseType.Dagger.Intelligence", "Item Tag that represents Intelligence Dagger item base type.");
	}

	namespace Item::BaseType::Wand
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Faith, "Item.BaseType.Wand.Faith", "Item Tag that represents Faith Wand item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Dexterity, "Item.BaseType.Wand.Dexterity", "Item Tag that represents Dexterity Wand item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Strength, "Item.BaseType.Wand.Strength", "Item Tag that represents Strength Wand item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Intelligence, "Item.BaseType.Wand.Intelligence", "Item Tag that represents Intelligence Wand item base type.");
	}
	
	namespace Item::BaseType::Shield
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Strength, "Item.BaseType.Shield.Strength", "Item Tag that represents Strength Shield item base type.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Dexterity, "Item.BaseType.Shield.Dexterity", "Item Tag that represents Dexterity Shield item base type.");
	}
	
	/**
	 * Equipment Slots.
	 */
	
	namespace Item::Slot::Equipment::Weapon
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(RightHand, "Item.Slot.Equipment.Weapon.RightHand", "Item Tag representing Right Hand Equipment Slot.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(LeftHand, "Item.Slot.Equipment.Weapon.LeftHand", "Item Tag representing Left Hand Equipment Slot.");
	}

	namespace Item::SwapSlot::Equipment::Weapon
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(RightHand, "Item.SwapSlot.Equipment.Weapon.RightHand", "Item Tag representing Right Hand Equipment Slot Swap.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(LeftHand, "Item.SwapSlot.Equipment.Weapon.LeftHand", "Item Tag representing Left Hand Equipment Slot Swap.");
	}
	
	namespace Item::Slot::Equipment
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Helmet, "Item.Slot.Equipment.Helmet", "Item Tag representing Helmet Equipment Slot.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(BodyArmor, "Item.Slot.Equipment.BodyArmor", "Item Tag representing Body Armor Equipment Slot.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Belt, "Item.Slot.Equipment.Belt", "Item Tag representing Belt Equipment Slot.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gloves, "Item.Slot.Equipment.Gloves", "Item Tag representing Gloves Equipment Slot.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Boots, "Item.Slot.Equipment.Boots", "Item Tag representing Boots Equipment Slot.");
	
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Amulet, "Item.Slot.Equipment.Amulet", "Item Tag representing Amulet Equipment Slot.");
	}

	namespace Item::Slot::Equipment::Ring
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(RightHand, "Item.Slot.Equipment.Ring.RightHand", "Item Tag representing Right Ring Equipment Slot.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(LeftHand, "Item.Slot.Equipment.Ring.LeftHand", "Item Tag representing Left Ring Equipment Slot.");
	}

	const TArray<FGameplayTag> EquipmentSlots =
		{
			Item::Slot::Equipment::Weapon::RightHand,
			Item::Slot::Equipment::Weapon::LeftHand,
			Item::Slot::Equipment::Helmet,
			Item::Slot::Equipment::BodyArmor,
			Item::Slot::Equipment::Belt,
			Item::Slot::Equipment::Gloves,
			Item::Slot::Equipment::Boots,
			Item::Slot::Equipment::Amulet,
			Item::Slot::Equipment::Ring::RightHand,
			Item::Slot::Equipment::Ring::LeftHand
		};

	
	const TMap<FGameplayTag, FName>& GetSlotToAttachSocketMap()
	{
		static const TMap<FGameplayTag, FName> SlotToAttachSocketMap =
		{
			{Item::Slot::Equipment::Weapon::RightHand, ObsidianMeshSocketNames::RightHandWeaponSocket},
			{Item::Slot::Equipment::Weapon::LeftHand, ObsidianMeshSocketNames::LeftHandWeaponSocket}
		};

		return SlotToAttachSocketMap;
	}

	const TMap<FGameplayTag, FGameplayTagContainer>& GetSisterSlotAcceptedCategoriesMap()
	{
		static const TArray<FGameplayTag> OneHandAcceptedEquipmentCategories =
		{
			Item::Category::Equipment::Weapon::Melee::OneHand::Dagger, Item::Category::Equipment::Weapon::Ranged::OneHand::Wand,
			Item::Category::Equipment::Weapon::Melee::OneHand::Flail, Item::Category::Equipment::Weapon::Melee::OneHand::Mace,
			Item::Category::Equipment::Weapon::Melee::OneHand::Sword, Item::Category::Equipment::Weapon::Melee::OneHand::Axe,
			Item::Category::Equipment::Offhand::Shield
		};

		static const TMap<FGameplayTag, FGameplayTagContainer> SisterSlotAcceptedEquipmentCategoriesForWeaponCategory =
		{
			{Item::Category::Equipment::Weapon::Melee::OneHand::Dagger, FGameplayTagContainer::CreateFromArray(OneHandAcceptedEquipmentCategories)},
			{Item::Category::Equipment::Weapon::Ranged::OneHand::Wand, FGameplayTagContainer::CreateFromArray(OneHandAcceptedEquipmentCategories)},
			{Item::Category::Equipment::Weapon::Melee::OneHand::Flail, FGameplayTagContainer::CreateFromArray(OneHandAcceptedEquipmentCategories)},
			{Item::Category::Equipment::Weapon::Ranged::OneHand::Bow, FGameplayTagContainer(Item::Category::Equipment::Offhand::Quiver)},
			{Item::Category::Equipment::Offhand::Quiver, FGameplayTagContainer(Item::Category::Equipment::Weapon::Ranged::OneHand::Bow)},
			{Item::Category::Equipment::Weapon::Ranged::TwoHand::Staff, FGameplayTagContainer::EmptyContainer},
			{Item::Category::Equipment::Weapon::Melee::OneHand::Mace, FGameplayTagContainer::CreateFromArray(OneHandAcceptedEquipmentCategories)},
			{Item::Category::Equipment::Weapon::Melee::TwoHand::Mace, FGameplayTagContainer::EmptyContainer},
			{Item::Category::Equipment::Weapon::Melee::OneHand::Sword, FGameplayTagContainer::CreateFromArray(OneHandAcceptedEquipmentCategories)},
			{Item::Category::Equipment::Weapon::Melee::TwoHand::Sword, FGameplayTagContainer::EmptyContainer},
			{Item::Category::Equipment::Weapon::Melee::OneHand::Axe, FGameplayTagContainer::CreateFromArray(OneHandAcceptedEquipmentCategories)},
			{Item::Category::Equipment::Weapon::Melee::TwoHand::Axe, FGameplayTagContainer::EmptyContainer},
			{Item::Category::Equipment::Offhand::Shield, FGameplayTagContainer::CreateFromArray(OneHandAcceptedEquipmentCategories)}	
		};

		return SisterSlotAcceptedEquipmentCategoriesForWeaponCategory;
	}

	/**
	 * Functional Slots.
	 */

	namespace Item::Slot::Functional
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ScrollOfIdentification, "Item.Slot.Functional.ScrollOfIdentification", "Item Tag representing functional Scroll of Identification slot.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ScrollOfTeleportation, "Item.Slot.Functional.ScrollOfTeleportation", "Item Tag representing functional Scroll of Teleportation slot.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(OrbOfEnchantment, "Item.Slot.Functional.OrbOfEnchantment", "Item Tag representing functional Orb of Enchantment slot.");
	}

	/**
	 * Stash Tabs.
	 */

	namespace StashTab
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Grid_1, "StashTab.Grid.1", "ID for Grid 1 Stash Tab.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Grid_2, "StashTab.Grid.2", "ID for Grid 2 Stash Tab.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Grid_3, "StashTab.Grid.3", "ID for Grid 3 Stash Tab.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Grid_4, "StashTab.Grid.4", "ID for Grid 4 Stash Tab.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(BigGrid_1, "StashTab.BigGrid.1", "ID for Big Grid 1 Stash Tab.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Functional, "StashTab.Functional", "ID for Functional Stash Tab.");
	}
	
	/**
	 * Stack Counts
	 */
	namespace Item::StackCount
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Current, "Item.StackCount.Current", "Item Tag representing the current number of stacked items.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Max, "Item.StackCount.Max", "Item Tag representing the Max Stack Count the item have, after which it will no longer stack and will need another space in the inventory.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Limit, "Item.StackCount.Limit", "Item Tag representing the Total number of items the Player can have, can be used for mission items which can be limited to 1.");
	}

	const TArray<FGameplayTag> StackTypes =
		{
			Item::StackCount::Current,
			Item::StackCount::Max,
			Item::StackCount::Limit
		};
	
	/**
	 * Affixes
	 */

	/**
	 * Affix Values
	 */

	namespace Item::AffixValue
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(SingleValue, "Item.AffixValue.SingleValue", "Single Affix Value identifier which is passed as SetByCaller data to GEs.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(MinValue, "Item.AffixValue.MinValue", "Range MinValue Affix Value identifier which is passed as SetByCaller data to GEs.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(MaxValue, "Item.AffixValue.MaxValue", "Range MaxValue Affix Value identifier which is passed as SetByCaller data to GEs.");
	}

	/**
	 * Item Primary Affix
	 */

	namespace Item::Affix::ItemPrimaryAffix::DamageRange
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(PhysicalDamageFlat, "Item.Affix.ItemPrimaryAffix.DamageRange.PhysicalDamageFlat", "Item Tag which represents Flat Physical Damage Range Primary Item Affix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(FireDamageFlat, "Item.Affix.ItemPrimaryAffix.DamageRange.FireDamageFlat", "Item Tag which represents Flat Fire Damage Range Primary Item Affix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ColdDamageFlat, "Item.Affix.ItemPrimaryAffix.DamageRange.ColdDamageFlat", "Item Tag which represents Flat Cold Damage Range Primary Item Affix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(LightningDamageFlat, "Item.Affix.ItemPrimaryAffix.DamageRange.LightningDamageFlat", "Item Tag which represents Flat Lightning Damage Range Primary Item Affix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ChaosDamageFlat, "Item.Affix.ItemPrimaryAffix.DamageRange.ChaosDamageFlat", "Item Tag which represents Flat Chaos Damage Range Primary Item Affix.");
	}

	namespace Item::Affix::ItemPrimaryAffix::Defence
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(EvasionFlat, "Item.Affix.ItemPrimaryAffix.Defence.EvasionFlat", "Item Tag which represents Flat Evasion Primary Item Affix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ArmorFlat, "Item.Affix.ItemPrimaryAffix.Defence.ArmorFlat", "Item Tag which represents Flat Armor Primary Item Affix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(EnergyShieldFlat, "Item.Affix.ItemPrimaryAffix.Defence.EnergyShieldFlat", "Item Tag which represents Flat Evasion Energy Shield Item Affix.");
	}
	
	/**
	 * Implicits
	 */

	namespace Item::Affix::Implicit::Life
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(LifeFlat, "Item.Affix.Implicit.Life.LifeFlat", "Item Tag which represents Flat Life Implicit.");
	}
	
	namespace Item::Affix::Implicit::Mana
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ManaFlat, "Item.Affix.Implicit.Mana.ManaFlat", "Item Tag which represents Flat Mana Implicit.");
	}
	
	namespace Item::Affix::Implicit::EnergyShield
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(EnergyShieldFlat, "Item.Affix.Implicit.EnergyShield.EnergyShieldFlat", "Item Tag which represents Flat Energy shield Implicit.");
	}
	
	namespace Item::Affix::Implicit::Resistance
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Fire, "Item.Affix.Implicit.Resistance.Fire", "Item Tag which represents Fire Resistance Implicits.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Lightning, "Item.Affix.Implicit.Resistance.Lightning", "Item Tag which represents Lightning Resistance Implicits.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cold, "Item.Affix.Implicit.Resistance.Cold", "Item Tag which represents Cold Resistance Implicits.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Chaos, "Item.Affix.Implicit.Resistance.Chaos", "Item Tag which represents Chaos Resistance Implicits.");
	}

	namespace Item::Affix::Implicit::MaxResistance
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Fire, "Item.Affix.Implicit.MaxResistance.Fire", "Item Tag which represents Max Fire Resistance Implicits.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Lightning, "Item.Affix.Implicit.MaxResistance.Lightning", "Item Tag which represents Max Lightning Resistance Implicits.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cold, "Item.Affix.Implicit.MaxResistance.Cold", "Item Tag which represents Max Cold Resistance Implicits.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Chaos, "Item.Affix.Implicit.MaxResistance.Chaos", "Item Tag which represents Max Chaos Resistance Implicits.");
	}

	/**
	 * Prefixes
	 */

	namespace Item::Affix::Prefix::Life
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(LifeFlat, "Item.Affix.Prefix.Life.LifeFlat", "Item Tag which represents Flat Life Prefix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(LifePercent, "Item.Affix.Prefix.Life.LifePercent", "Item Tag which represents Percent Life Prefix.");
	}
	
	namespace Item::Affix::Prefix::Mana
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ManaFlat, "Item.Affix.Prefix.Mana.ManaFlat", "Item Tag which represents Flat Mana Prefix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ManaPercent, "Item.Affix.Prefix.Mana.ManaPercent", "Item Tag which represents Percent Mana Prefix.");
	}

	namespace Item::Affix::Prefix::Utility
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(MagicFind, "Item.Affix.Prefix.Utility.MagicFind", "Item Tag which represents Magic Find Prefix.");
	}
	
	namespace Item::Affix::Prefix::Defence
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ArmorFlat, "Item.Affix.Prefix.Defence.ArmorFlat", "Item Tag which represents Flat Armor Prefix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ArmorPercent, "Item.Affix.Prefix.Defence.ArmorPercent", "Item Tag which represents Percent Armor Prefix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(EvasionFlat, "Item.Affix.Prefix.Defence.EvasionFlat", "Item Tag which represents Flat Evasion Prefix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(EvasionPercent, "Item.Affix.Prefix.Defence.EvasionPercent", "Item Tag which represents Percent Evasion Prefix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(EnergyShieldFlat, "Item.Affix.Prefix.Defence.EnergyShieldFlat", "Item Tag which represents Flat Energy Shield Prefix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(EnergyShieldPercent, "Item.Affix.Prefix.Defence.EnergyShieldPercent", "Item Tag which represents Percent Energy Shield Prefix.");
	}
	
	namespace Item::Affix::Prefix::DamageMultiplier
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(IncreasePhysicalDamage, "Item.Affix.Prefix.DamageMultiplier.IncreasePhysicalDamage", "Item Tag which represents Multiplier Physical Damage Increase Prefix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(IncreaseFireDamage, "Item.Affix.Prefix.DamageMultiplier.IncreaseFireDamage", "Item Tag which represents Multiplier Fire Damage Increase Prefix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(IncreaseColdDamage, "Item.Affix.Prefix.DamageMultiplier.IncreaseColdDamage", "Item Tag which represents Multiplier Cold Damage Increase Prefix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(IncreaseLightningDamage, "Item.Affix.Prefix.DamageMultiplier.IncreaseLightningDamage", "Item Tag which represents Multiplier Lightning Damage Increase Prefix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(IncreaseChaosDamage, "Item.Affix.Prefix.DamageMultiplier.IncreaseChaosDamage", "Item Tag which represents Multiplier Chaos Damage Increase Prefix.");
	}
	
	namespace Item::Affix::Prefix::DamageRange
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(PhysicalDamageFlat, "Item.Affix.Prefix.DamageRange.PhysicalDamageFlat", "Item Tag which represents Flat Physical Damage Range Prefix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(PhysicalDamagePercentage, "Item.Affix.Prefix.DamageRange.PhysicalDamagePercentage", "Item Tag which represents Percentage Physical Damage Range Prefix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(FireDamageFlat, "Item.Affix.Prefix.DamageRange.FireDamageFlat", "Item Tag which represents Flat Fire Damage Range Prefix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(FireDamagePercentage, "Item.Affix.Prefix.DamageRange.FireDamagePercentage", "Item Tag which represents Percentage Fire Damage Range Prefix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ColdDamageFlat, "Item.Affix.Prefix.DamageRange.ColdDamageFlat", "Item Tag which represents Flat Cold Damage Range Prefix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ColdDamagePercentage, "Item.Affix.Prefix.DamageRange.ColdDamagePercentage", "Item Tag which represents Percentage Cold Damage Range Prefix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(LightningDamageFlat, "Item.Affix.Prefix.DamageRange.LightningDamageFlat", "Item Tag which represents Flat Lightning Damage Range Prefix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(LightningDamagePercentage, "Item.Affix.Prefix.DamageRange.LightningDamagePercentage", "Item Tag which represents Percentage Lightning Damage Range Prefix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ChaosDamageFlat, "Item.Affix.Prefix.DamageRange.ChaosDamageFlat", "Item Tag which represents Flat Chaos Damage Range Prefix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ChaosDamagePercentage, "Item.Affix.Prefix.DamageRange.ChaosDamagePercentage", "Item Tag which represents Percentage Chaos Damage Range Prefix.");
	}
 
	/**
	 * Suffixes
	 */

	namespace Item::Affix::Suffix::Life
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(LifeRegeneration, "Item.Affix.Suffix.Life.LifeRegeneration", "Item Tag which represents Life Regeneration Suffix.");
	}

	namespace Item::Affix::Suffix::EnergyShield
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(EnergyShieldRegeneration, "Item.Affix.Suffix.EnergyShield.EnergyShieldRegeneration", "Item Tag which represents Energy Shield Regeneration Suffix.");
	}

	namespace Item::Affix::Suffix::Mana
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ManaRegeneration, "Item.Affix.Suffix.Mana.ManaRegeneration", "Item Tag which represents Mana Regeneration Suffix.");
	}
	
	namespace Item::Affix::Suffix::Attribute
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Dexterity, "Item.Affix.Suffix.Attribute.Dexterity", "Item Tag which represents Dexterity Suffix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Intelligence, "Item.Affix.Suffix.Attribute.Intelligence", "Item Tag which represents Intelligence Suffix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Strength, "Item.Affix.Suffix.Attribute.Strength", "Item Tag which represents Strength Suffix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Faith, "Item.Affix.Suffix.Attribute.Faith", "Item Tag which represents Faith Suffix.");
	}
	
	namespace Item::Affix::Suffix::Resistance
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Fire, "Item.Affix.Suffix.Resistance.Fire", "Item Tag which represents Fire Resistance Suffix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Lightning, "Item.Affix.Suffix.Resistance.Lightning", "Item Tag which represents Lightning Resistance Suffix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cold, "Item.Affix.Suffix.Resistance.Cold", "Item Tag which represents Cold Resistance Suffix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Chaos, "Item.Affix.Suffix.Resistance.Chaos", "Item Tag which represents Chaos Resistance Suffix.");
	}
	
	namespace Item::Affix::Suffix::MaxResistance
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Fire, "Item.Affix.Suffix.MaxResistance.Fire", "Item Tag which represents Max Fire Resistance Suffix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Lightning, "Item.Affix.Suffix.MaxResistance.Lightning", "Item Tag which represents Max Lightning Resistance Suffix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cold, "Item.Affix.Suffix.MaxResistance.Cold", "Item Tag which represents Max Cold Resistance Suffix.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Chaos, "Item.Affix.Suffix.MaxResistance.Chaos", "Item Tag which represents Max Chaos Resistance Suffix.");
	}

	namespace Item::Affix::Suffix::Utility
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(MagicFind, "Item.Affix.Suffix.Utility.MagicFind", "Item Tag which represents Magic Find Suffix.");
	}
	
	/**
	 * Enchanted Affixes
	 */
	
	namespace Item::Affix::Suffix::Enchant
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(MaximumLifePercentage, "Item.Affix.Suffix.Enchant.MaximumLifePercentage", "Item Tag which represents Enchant that gives Percentage Maximum Life Affix.");
	}

	/**
	 * Skill Implicits
	 */

	namespace Item::Affix::SkillImplicits::Attack::Witch
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(FireBall, "Item.Affix.SkillImplicits.Attack.Witch.FireBall", "Item Tag which represents Fire Ball Skill Implicit.");
	}

	namespace Item::Affix::SkillImplicits::Attack::Barbarian
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Slash, "Item.Affix.SkillImplicits.Attack.Barbarian.Slash", "Item Tag which represents Slash Skill Implicit.");
	}

	namespace Item::Affix::SkillImplicits::Attack::Assassin
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(FlyingKnife, "Item.Affix.SkillImplicits.Attack.Assassin.FlyingKnife", "Item Tag which represents Flying Knife Skill Implicit.");
	}

	namespace Item::Affix::SkillImplicits::Attack::Paladin
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(BlessedHammer, "Item.Affix.SkillImplicits.Attack.Paladin.BlessedHammer", "Item Tag which represents Blessed Hammer Skill Implicit.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(MagneticHammer, "Item.Affix.SkillImplicits.Attack.Paladin.MagneticHammer", "Item Tag which represents Magnetic Hammer Skill Implicit.");
	}
	
	namespace Item::Affix::SkillImplicits::Defence
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(MagmaBarrier, "Item.Affix.SkillImplicits.Defence.MagmaBarrier", "Item Tag which represents Magma Barrier Skill Implicit.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(FrozenArmor, "Item.Affix.SkillImplicits.Defence.FrozenArmor", "Item Tag which represents Frozen Armor Skill Implicit.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(TransientArmor, "Item.Affix.SkillImplicits.Defence.TransientArmor", "Item Tag which represents Transient Armor Skill Implicit.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(SparklingBarrier, "Item.Affix.SkillImplicits.Defence.SparklingBarrier", "Item Tag which represents Sparkling Barrier Skill Implicit.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(RaisedShield, "Item.Affix.SkillImplicits.Defence.RaisedShield", "Item Tag which represents Raised Shield Skill Implicit.");
	}
	
	namespace Item::Affix::SkillImplicits::Movement
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Roll, "Item.Affix.SkillImplicits.Movement.Roll", "Item Tag which represents Roll Skill Implicit.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Jump, "Item.Affix.SkillImplicits.Movement.Jump", "Item Tag which represents Jump Skill Implicit.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ShieldCharge, "Item.Affix.SkillImplicits.Movement.ShieldCharge", "Item Tag which represents Shield Charge Skill Implicit.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Blink, "Item.Affix.SkillImplicits.Movement.Blink", "Item Tag which represents Blink Skill Implicit.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Teleport, "Item.Affix.SkillImplicits.Movement.Teleport", "Item Tag which represents Teleport Skill Implicit.");
	}
	
	namespace Item::Affix::SkillImplicits::Aura
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Health, "Item.Affix.SkillImplicits.Aura.Health", "Item Tag which represents Life Skill Implicit.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Mana, "Item.Affix.SkillImplicits.Aura.Mana", "Item Tag which represents Mana Skill Implicit.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Energy, "Item.Affix.SkillImplicits.Aura.Energy", "Item Tag which represents Energy Skill Implicit.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Evasion, "Item.Affix.SkillImplicits.Aura.Evasion", "Item Tag which represents Evasion Skill Implicit.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Armor, "Item.Affix.SkillImplicits.Aura.Armor", "Item Tag which represents Armor Skill Implicit.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(CirclingElementalDamage, "Item.Affix.SkillImplicits.Aura.CirclingElementalDamage", "Item Tag which represents Circling Elemental Damage Skill Implicit.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(CriticalDamage, "Item.Affix.SkillImplicits.Aura.CriticalDamage", "Item Tag which represents Critical Damage Skill Implicit.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(CriticalChance, "Item.Affix.SkillImplicits.Aura.CriticalChance", "Item Tag which represents Critical Chance Skill Implicit.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ChaosDamage, "Item.Affix.SkillImplicits.Aura.ChaosDamage", "Item Tag which represents Chaos Damage Skill Implicit.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(PhysicalDamage, "Item.Affix.SkillImplicits.Aura.PhysicalDamage", "Item Tag which represents Physical Damage Skill Implicit.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ManaRegeneration, "Item.Affix.SkillImplicits.Aura.ManaRegeneration", "Item Tag which represents Mana Regeneration Skill Implicit.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(HealthRegeneration, "Item.Affix.SkillImplicits.Aura.HealthRegeneration", "Item Tag which represents Health Regeneration Skill Implicit.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Hatred, "Item.Affix.SkillImplicits.Aura.Hatred", "Item Tag which represents Hatred Skill Implicit.");
	}
	
	namespace Item::Affix::SkillImplicits::Ultimate
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(VoidSphere, "Item.Affix.SkillImplicits.Ultimate.VoidSphere", "Item Tag which represents Void Sphere Skill Implicit.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Combustion, "Item.Affix.SkillImplicits.Ultimate.Combustion", "Item Tag which represents Combustion Skill Implicit.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Shatter, "Item.Affix.SkillImplicits.Ultimate.Shatter", "Item Tag which represents Shatter Skill Implicit.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Electrocution, "Item.Affix.SkillImplicits.Ultimate.Electrocution", "Item Tag which represents Electrocution Skill Implicit.");
	}
}
