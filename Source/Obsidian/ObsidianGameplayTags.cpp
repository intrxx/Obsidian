// Copyright 2024 out of sCope team - Michał Ogiński

#include "ObsidianGameplayTags.h"
#include "Engine/EngineTypes.h"
#include "GameplayTagsManager.h"
#include "Obsidian.h"

namespace ObsidianGameplayTags
{
	// Abilities
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivationFail_IsDead, "Ability.ActivationFail.IsDead", "Ability failed to activate because its owner is dead.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_HitReact, "Ability.HitReact", "Tag used for activating hit react.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Stagger, "Ability.Stagger", "Tag used for activating stagger.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Damage, "SetByCaller.Damage", "Tag used for passing damage with set by caller magnitude.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Heal, "SetByCaller.Heal", "Tag used for passing heal with set by caller magnitude.");

	// Damage Types
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
	
	// Cooldowns
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Cooldown_HitReact, "Ability.Cooldown.HitReact", "Tag used for Hit React Ability cooldown.")

	// Effects
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_HitReact, "Effect.HitReact", "Tag used for activating the Hit React ability.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Stagger, "Effect.Stagger", "Tag used for activating the Stagger ability.");

	// Statuses
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Death, "Status.Death", "Death has the death status.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Death_Dying, "Status.Death.Dying", "Death started for the target.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Death_Dead, "Status.Death.Dead", "Death finished for the target.");
	
	// Native Input
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Move_Keyboard, "Input.Native.Move.Keyboard", "Move input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Move_Mouse, "Input.Native.Move.Mouse", "Move input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_CharacterStatus, "Input.Native.CharacterStatus", "Character Status toggle input.");

	// Ability Input
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability_Move_Roll, "Input.Ability.Move.Roll", "Roll input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability1, "Input.Ability.Ability1", "Ability1 input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability2, "Input.Ability.Ability2", "Ability2 input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability3, "Input.Ability.Ability3", "Ability3 input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability4, "Input.Ability.Ability4", "Ability4 input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability5, "Input.Ability.Ability5", "Ability5 input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability6, "Input.Ability.Ability6", "Ability6 input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability7, "Input.Ability.Ability7", "Ability7 input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Ability8, "Input.Ability.Ability8", "Ability8 input.");

	// Data
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Data_EffectUIInfo, "Data.EffectUIInfo", "Tag that is owned by any effect that wish to be displayed on Player's screen.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_AdvancedCombat_Hit, "GameplayEvent.AdvancedCombat.Hit", "Event fires when the enemy character is hit by the Advanced Combat Component.");
	
	// UI Data
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_EffectData_Flask_HealthHealing, "UI.EffectData.Flask.HealthHealing", "Tag used for displaying Health healing from Flasks Info on the UI.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_EffectData_Flask_ManaHealing, "UI.EffectData.Flask.ManaHealing", "Tag used for displaying Mana healing from Flasks Info on the UI.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_EffectData_Aura_Defiance, "UI.EffectData.Aura.Defiance", "Tag used for displaying Defiance Aura Info on the UI.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_EffectData_Effect_Poison, "UI.EffectData.Effect.Poison", "Tag used for displaying Poison Effect Info on the UI.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_EffectData_Effect_Chill, "UI.EffectData.Effect.Chill", "Tag used for displaying Chill Effect Info on the UI.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_EffectData_Effect_Ignite, "UI.EffectData.Effect.Ignite", "Tag used for displaying Ignite Effect Info on the UI.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_EffectData_Effect_Shock, "UI.EffectData.Effect.Shock", "Tag used for displaying Shock Info on the UI.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_EffectData_Effect_Curse_ElementalWeakness, "UI.EffectData.Effect.Curse.ElementalWeakness", "Tag used for displaying Elemental Weakness Info on the UI.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_GlobeData_HealingHealth, "UI.GlobeData.HealingHealth", "Tag used for displaying the healing amount on the globe.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_GlobeData_RepleanishingMana, "UI.GlobeData.ReplenishingMana", "Tag used for displaying the repleanish amount on the globe.");

	// Events
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_Death, "GameplayEvent.Death", "Event fired on attributes component when character is out of health.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_AbilityMontage_Player_Firebolt, "GameplayEvent.AbilityMontage.Player.Firebolt", "Tag used for triggering gameplay event for spawning firebolt.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_AbilityMontage_Player_Aura, "GameplayEvent.AbilityMontage.Player.Aura", "Tag used for triggering gameplay event for applying aura.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_AbilityMontage_Socket_RightHandWeapon, "GameplayEvent.AbilityMontage.Socket.RightHandWeapon", "Tag used for triggering gameplay event for damaging actor with right hand melee weapon. Used by Montage to retreive correct socket.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_AbilityMontage_Socket_LeftHandWeapon, "GameplayEvent.AbilityMontage.Socket.LeftHandWeapon", "Tag used for triggering gameplay event for damaging actor with left hand melee weapon. Used by Montage to retreive correct socket.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_AbilityMontage_Socket_RightHand, "GameplayEvent.AbilityMontage.Socket.RightHand", "Tag used for triggering gameplay event for damaging actor with melee right hand. Used by Montage to retreive correct socket.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_AbilityMontage_Socket_LeftHand, "GameplayEvent.AbilityMontage.Socket.LeftHand", "Tag used for triggering gameplay event for damaging actor with melee left hand. Used by Montage to retreive correct socket.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_AbilityMontage_Socket_BetweenHands, "GameplayEvent.AbilityMontage.Socket.BetweenHands", "Tag used for triggering gameplay event for damaging actor with melee attack between hands (could be used for slam or something). Used by Montage to retreive correct socket.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityActivation_Melee, "AbilityActivation.Melee", "Tag used for triggering activation of AI's melee attack.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityActivation_Ranged, "AbilityActivation.Ranged", "Tag used for triggering activation of AI's ranged attack.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityActivation_Introduction, "AbilityActivation.Introduction", "Tag used for triggering activation of AI's Introduction ability (some roaring or equping a weapon).");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityActivation_TreeOrc_UnarmedSwing, "AbilityActivation.TreeOrc.UnarmedSwing", "Tag used for triggering activation of TreeOrcs's swing melee attack.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityActivation_TreeOrc_LeapAttack, "AbilityActivation.TreeOrc.LeapAttack", "Tag used for triggering activation of TreeOrcs's leap attack.");

	// Attributes UI Data - I will leave it here for now in case I would need it in the future
	/**
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Health, "Attribute.Health", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_MaxHealth, "Attribute.MaxHealth", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_EnergyShield, "Attribute.EnergyShield", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_MaxEnergyShield, "Attribute.MaxEnergyShield", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_HealthRegeneration, "Attribute.HealthRegeneration", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_EnergyShieldRegeneration, "Attribute.EnergyShieldRegeneration", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Armor, "Attribute.Armor", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Evasion, "Attribute.Evasion", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_FireResistance, "Attribute.FireResistance", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_MaxFireResistance, "Attribute.MaxFireResistance", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_ColdResistance, "Attribute.ColdResistance", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_MaxColdResistance, "Attribute.MaxColdResistance", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_LightningResistance, "Attribute.LightningResistance", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_MaxLightningResistance, "Attribute.MaxLightningResistance", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_ChaosResistance, "Attribute.ChaosResistance", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_MaxChaosResistance, "Attribute.MaxChaosResistance", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_CriticalStrikeChance, "Attribute.CriticalStrikeChance", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_CriticalStrikeMultiplier, "Attribute.CriticalStrikeMultiplier", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_AttackSpeed, "Attribute.AttackSpeed", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_CastSpeed, "Attribute.CastSpeed", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_FirePenetration, "Attribute.FirePenetration", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_ColdPenetration, "Attribute.ColdPenetration", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_LightningPenetration, "Attribute.LightningPenetration", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_ChaosPenetration, "Attribute.ChaosPenetration", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_FireDamageMultiplier, "Attribute.FireDamageMultiplier", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_ColdDamageMultiplier, "Attribute.ColdDamageMultiplier", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_LightningDamageMultiplier, "Attribute.LightningDamageMultiplier", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_PhysicalDamageMultiplier, "Attribute.PhysicalDamageMultiplier", "Tag used for identifying the attribute.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Mana, "Attribute.Mana", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_MaxMana, "Attribute.MaxMana", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_ManaRegeneration, "Attribute.ManaRegeneration", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Strength, "Attribute.Strength", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Intelligence, "Attribute.Intelligence", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Dexterity, "Attribute.Dexterity", "Tag used for identifying the attribute.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Faith, "Attribute.Faith", "Tag used for identifying the attribute.");
	*/
	
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
}
