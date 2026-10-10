// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "NativeGameplayTags.h"

namespace ObsidianGameplayTags
{
	OBSIDIAN_API FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);
	
	/**
	 * ---- User Interface ----
	 */

	namespace UI::Layer
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(MainMenu);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayMenu);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay);
	}
	
	/**
	 * ---- Damage Types ----
	 */
	
	namespace DamageType
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Physical);
	}

	namespace DamageType::Elemental
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Fire);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cold);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Lightning);
	}

	namespace DamageType
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Chaos);
	}
	
	/** All damage Types stored for convenience */
	OBSIDIAN_API extern const TArray<FGameplayTag> DamageTypes;
	
	/**
	 *  ---- Effects ----
	 */
	
	namespace Effect
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitReact)
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stagger)
	}

	/**
	 * ---- Movement ----
	 */

	namespace Movement::State
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Standing)
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Walking)
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Running)
	}
	
	/**
	 * ---- Statuses ----
	 */
	
	namespace Status::Death
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Death);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dying);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dead);
	}
	
	namespace Status
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Immunity);
	}

	/**
	 * ---- Input ----
	 */
	
	/**
	 * Native
	 */
	namespace Input::Native::Move
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Keyboard);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Mouse);
	}
	
	namespace Input::Native
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ReleaseUsingItem);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ReleaseContinouslyUsingItem);
	}

	namespace Input
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(DropItem);
	
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Interact);
	}
	
	namespace Input::Native
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CharacterStatus);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Inventory);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(PassiveSkillTree);

		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(WeaponSwap);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ToggleWalk);
	}

	namespace Input::UI
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(MainMenu);
	}

	namespace Input::UI::Action
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Backwards);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(OpenGameplayMenu);
	}
	
	namespace Input::UI
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ToggleHighlight);
	}
	
	/**
	 * Ability
	 */
	namespace Input::Ability::Move
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Roll);
	}

	namespace Input::Ability
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability1);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability2);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability3);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability4);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability5);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability6);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability7);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability8);
	}

	/*
	 * ---- Data ----
	 */

	namespace Data::AdvancedCombat
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hit);
	}
	
	/**
	 * UI Data
	 */
	namespace UI
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(DataSpecifierTag)
	}
	
	namespace UI::EffectData::Flask
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HealthHealing);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ManaHealing);
	}
	
	namespace UI::EffectData::Aura
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Health);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Mana);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Energy);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Evasion);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Armor);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CriticalDamage);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CriticalChance);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ChaosDamage);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(PhysicalDamage);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CirclingElementalDamage);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HealthRegeneration);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ManaRegeneration);
	}

	namespace UI::EffectData::Effect
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Poison);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Chill);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ignite);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shock);
	}
	
	namespace UI::EffectData::Effect::Special
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Immunity);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(DamageReduction);
	}
	
	namespace UI::EffectData::Effect::Curse
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ElementalWeakness);
	}
	
	namespace UI::GlobeData
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HealingHealth);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ReplenishingMana);
	}

	/**
	 * ---- Gameplay Messages ----
	 */

	/**
	 * Inventory
	 */
	namespace Message::Inventory
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Changed);
	}

	/**
	 * Equipment
	 */
	namespace Message::Equipment
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Changed);
	}

	/**
	 * Player Stash
	 */
	namespace Message::PlayerStash
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Changed);
	}
	
	/**
	 * ---- Gameplay Events ----
	 */

	/**
	 * Shared
	 */
	namespace GameplayEvent
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Death);
	}

	/**
	 * Hero
	 */
	namespace GameplayEvent::AbilityMontage::Player
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Firebolt);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(FlyingKnifes);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(MagneticHammer);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Slash);
	}

	/**
	 * Tree Orc
	 */
	namespace GameplayEvent::AbilityMontage::TreeOrc
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquipWeapon);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SpawnComboProjectile);
	}

	/**
	 * Ranged Goblin
	 */
	namespace GameplayEvent::AbilityMontage::RangedGoblin
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SpawnSlingShotProj);
	}
	
	/**
	 * Skeletal Mage
	 */
	namespace GameplayEvent::AbilityMontage::SkeletalMage
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SpawnFireNova);
	}
	
	/**
	 * Sockets
	 */
	namespace GameplayEvent::AbilityMontage::Socket
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(RightHandWeapon)
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(LeftHandWeapon)
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(RightHand)
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(LeftHand)
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(BetweenHands)
	}

	/**
	 * ---- Gameplay Cues ----
	 */

	namespace GameplayCue
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(MeleeImpact)
	}

	/**
	 * ---- Ability Activation Tags ----
	 */
	
	namespace Ability::ActivationFail
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(IsDead);
	}

	/**
	 * Shared
	 */
	namespace AbilityActivation
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitReact);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stagger);
	
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Introduction);
	}

	/**
	 * Zombie
	 */
	namespace AbilityActivation::Zombie
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(MeleeAttack);
	}

	/**
	 * Ranged Goblin
	 */
	namespace AbilityActivation::RangedGoblin
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(BowAttack);
	}

	/**
	 * Skeletal Mage
	 */
	namespace AbilityActivation::SkeletalMage
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(FireBall);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(FireNova);
	}

	/**
	 * Tree Orc
	 */
	namespace AbilityActivation::TreeOrc
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UnarmedSwing);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(LeapAttack);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equip);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ComboSwing);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ArmedSwing);
	}
	
	/**
	 * ---- Cooldowns ----
	 */

	/**
	 * Shared
	 */
	namespace Ability::Cooldown
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitReact);
	}

	/**
	 * Tree Orc
	 */
	namespace Ability::Cooldown::TreeOrc
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(LeapAttack);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ComboSwing);
	}
	
	/**
	 * Skeletal Mage
	 */
	namespace Ability::Cooldown::SkeletalMage
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(FireNova);
	}
	
	/**
	 * ---- Items ----
	 */

	/**
	 * Inventory
	 */

	namespace Inventory
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(BlockActions);
	}

	/**
	 * Equipment
	 */

	namespace Equipment
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(BlockActions);
	}

	/**
	 * Player Stash
	 */
	
	namespace PlayerStash
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(BlockActions);
	}

	/**
	 * Item Category.
	 */
	
	namespace Item::Category::Currency
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Resource);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Functional);
	}
	
	namespace Item::Category::Equipment::Armor
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Helmet);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(BodyArmor);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Belt);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gloves);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Boots);
	}
	
	namespace Item::Category::Equipment::Offhand
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shield);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Quiver);
	}
	
	namespace Item::Category::Equipment::Jewellery
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Amulet);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ring);
	}
	
	namespace Item::Category::Equipment::Weapon::Melee::OneHand
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dagger);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Flail);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Mace);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sword);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Axe);
	}
	
	namespace Item::Category::Equipment::Weapon::Melee::TwoHand
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Mace);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sword);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Axe);
	}
	
	namespace Item::Category::Equipment::Weapon::Ranged::OneHand
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Wand);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Bow);
	}
	
	namespace Item::Category::Equipment::Weapon::Ranged::TwoHand
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Staff);
	}
	
	/**
	 * Item Base Types
	 */

	namespace Item::BaseType::Orb
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ScrollOfIdentification);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ScrollOfTeleportation);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(OrbOfEnchantment);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(OrbOfRepentance);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(OrbOfEradication);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(OrbOfRescription);
	}

	namespace Item::BaseType::Helmet
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Armor);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Evasion);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(EnergyShield);
	}

	namespace Item::BaseType::BodyArmor
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Armor);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Evasion);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(EnergyShield);
	}
	
	namespace Item::BaseType
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Amulet);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ring);
	}
	
	namespace Item::BaseType::Sword::OneHand
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Faith);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dexterity);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Strength);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Intelligence);
	}

	namespace Item::BaseType::Sword::TwoHand
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Faith);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dexterity);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Strength);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Intelligence);
	}

	namespace Item::BaseType::Axe::OneHand
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Faith);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dexterity);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Strength);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Intelligence);
	}

	namespace Item::BaseType::Axe::TwoHand
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Faith);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dexterity);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Strength);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Intelligence);
	}

	namespace Item::BaseType::Mace::OneHand
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Faith);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dexterity);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Strength);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Intelligence);
	}

	namespace Item::BaseType::Mace::TwoHand
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Faith);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dexterity);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Strength);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Intelligence);
	}

	namespace Item::BaseType::Dagger
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Faith);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dexterity);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Strength);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Intelligence);
	}

	namespace Item::BaseType::Wand
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Faith);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dexterity);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Strength);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Intelligence);
	}
	
	namespace Item::BaseType::Shield
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Strength);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dexterity);
	}
	
	/**
	 * Equipment Slots.
	 */
	
	namespace Item::Slot::Equipment::Weapon
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(RightHand);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(LeftHand);
	}

	namespace Item::SwapSlot::Equipment::Weapon
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(RightHand);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(LeftHand);
	}
	
	namespace Item::Slot::Equipment
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Helmet);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(BodyArmor);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Belt);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gloves);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Boots);
	
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Amulet);
	}

	namespace Item::Slot::Equipment::Ring
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(RightHand);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(LeftHand);
	}

	/** All Stack Types stored for convenience. */
	OBSIDIAN_API extern const TArray<FGameplayTag> EquipmentSlots;
	
	/** Default Item Category Attachment rules. */
	OBSIDIAN_API const TMap<FGameplayTag, FName>& GetSlotToAttachSocketMap();

	/** Accepted Equipment Categories for sister slot per weapon type. */
	OBSIDIAN_API const TMap<FGameplayTag, FGameplayTagContainer>& GetSisterSlotAcceptedCategoriesMap();

	/**
	 * Currency Slots.
	 */

	namespace Item::Slot::Functional
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ScrollOfIdentification);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ScrollOfTeleportation);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(OrbOfEnchantment);
	}

	/**
	 * Stash Tabs.
	 */

	namespace StashTab
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Grid_1);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Grid_2);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Grid_3);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Grid_4);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(BigGrid_1);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Functional);
	}

	/**
	 * Stacks Counts
	 */
	namespace Item::StackCount
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Current);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Max);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Limit);
	}

	/** All Stack Types stored for convenience. */
	OBSIDIAN_API extern const TArray<FGameplayTag> StackTypes;
	
	/**
	 * ---- Affixes ----
	 */

	/**
	 * Affix Values
	 */
	
	namespace Item::AffixValue
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SingleValue);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(MinValue);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(MaxValue);
	}
	
	/**
	 * Implicits
	 */

	namespace Item::Affix::Implicit::Life
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(LifeFlat);
	}
	
	namespace Item::Affix::Implicit::Mana
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ManaFlat);
	}
	
	namespace Item::Affix::Implicit::EnergyShield
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(EnergyShieldFlat);
	}

	namespace Item::Affix::Implicit::Resistance
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Fire);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Lightning);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cold);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Chaos);
	}

	namespace Item::Affix::Implicit::MaxResistance
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Fire);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Lightning);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cold);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Chaos);
	}

	/**
	 * Prefixes
	 */

	namespace Item::Affix::Prefix::Life
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(LifeFlat);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(LifePercent);
	}
	
	namespace Item::Affix::Prefix::Mana
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ManaFlat);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ManaPercent);
	}
	
	namespace Item::Affix::Prefix::Utility
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(MagicFind);
	}
	
	namespace Item::Affix::Prefix::Defence
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ArmorFlat);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ArmorPercent);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(EvasionFlat);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(EvasionPercent);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(EnergyShieldFlat);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(EnergyShieldPercent);
	}
	
	namespace Item::Affix::Prefix::DamageMultiplier
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(IncreasePhysicalDamage);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(IncreaseFireDamage);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(IncreaseColdDamage);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(IncreaseLightningDamage);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(IncreaseChaosDamage);
	}
	
	namespace Item::Affix::Prefix::DamageRange
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(PhysicalDamageFlat);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(PhysicalDamagePercentage);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(FireDamageFlat);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(FireDamagePercentage);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ColdDamageFlat);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ColdDamagePercentage);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(LightningDamageFlat);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(LightningDamagePercentage);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ChaosDamageFlat);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ChaosDamagePercentage);
	}
	
	/**
	 * Suffixes
	 */
	
	namespace Item::Affix::Suffix::Life
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(LifeRegeneration);
	}

	namespace Item::Affix::Suffix::EnergyShield
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(EnergyShieldRegeneration);
	}

	namespace Item::Affix::Suffix::Mana
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ManaRegeneration);
	}
	
	namespace Item::Affix::Suffix::Attribute
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dexterity);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Intelligence);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Strength);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Faith);
	}
	
	namespace Item::Affix::Suffix::Resistance
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Fire);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Lightning);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cold);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Chaos);
	}

	namespace Item::Affix::Suffix::MaxResistance
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Fire);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Lightning);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cold);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Chaos);
	}
	
	namespace Item::Affix::Suffix::Utility
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(MagicFind);
	}

	/**
	 * Enchanted Affixes
	 */
	
	namespace Item::Affix::Suffix::Enchant
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(MaximumLifePercentage)
	}

	/**
	 * Item Primary Affix
	 */

	namespace Item::Affix::ItemPrimaryAffix::DamageRange
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(PhysicalDamageFlat);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(FireDamageFlat);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ColdDamageFlat);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(LightningDamageFlat);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ChaosDamageFlat);
	}

	namespace Item::Affix::ItemPrimaryAffix::Defence
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(EvasionFlat);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ArmorFlat);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(EnergyShieldFlat);
	}

	/**
	 * Skill Implicits
	 */

	namespace Item::Affix::SkillImplicits::Attack::Witch
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(FireBall);
	}

	namespace Item::Affix::SkillImplicits::Attack::Barbarian
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Slash);
	}

	namespace Item::Affix::SkillImplicits::Attack::Assassin
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(FlyingKnife);
	}

	namespace Item::Affix::SkillImplicits::Attack::Paladin
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(BlessedHammer);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(MagneticHammer);
	}

	namespace Item::Affix::SkillImplicits::Defence
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(MagmaBarrier);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(FrozenArmor);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TransientArmor);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SparklingBarrier);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(RaisedShield);
	}
	
	namespace Item::Affix::SkillImplicits::Movement
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Roll);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Jump);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ShieldCharge);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Blink);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Teleport);
	}
	
	namespace Item::Affix::SkillImplicits::Aura
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Health);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Mana);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Energy);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Evasion);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Armor);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CirclingElementalDamage);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CriticalDamage);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CriticalChance);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ChaosDamage);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(PhysicalDamage);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HealthRegeneration);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ManaRegeneration);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hatred);
	}

	namespace Item::Affix::SkillImplicits::Ultimate
	{
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(VoidSphere);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Combustion);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shatter);
		OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Electrocution);
	}
}


