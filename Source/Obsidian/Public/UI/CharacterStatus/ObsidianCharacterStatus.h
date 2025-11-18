// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "UI/ObsidianMainOverlayWidgetBase.h"
#include "ObsidianCharacterStatus.generated.h"

class UProgressBar;
class UOCharacterStatusAttributeRow_WithToolTip;
class UButton;
class UScrollBox;
class UOCharacterStatusAttributeRow;
class UImage;
class UCommonTextBlock;
class UOCharacterStatusWidgetController;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianCharacterStatus : public UObsidianMainOverlayWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|CharacterStatus")
	void SwitchToTab(UScrollBox* Tab);

protected:
	// ~ Start of Obsidian Widget Base
	virtual void HandleWidgetControllerSet() override;
	// ~ End of Obsidian Widget Base
	
	void SetExperienceTextBlock() const;
	void SetExperienceProgressBar() const;

	void OnHeroLevelUp(const uint8 NewLevel);
	
	void OnExperienceChanged(const float Value);
	void OnMaxExperienceChanged(const float Value, const float OldValue);
	
	void OnStrengthChanged(const float Value);
	void OnIntelligenceChanged(const float Value);
	void OnDexterityChanged(const float Value);
	void OnFaithChanged(const float Value);
	
	void OnMaxHealthChanged(const float Value);
	void OnMaxManaChanged(const float Value);
	void OnMaxSpecialResourceChanged(const float Value);
	void OnMaxEnergyShieldChanged(const float Value);
	
	void OnAccuracyChanged(const float Value);
	void OnAttackSpeedChanged(const float Value);
	void OnCastSpeedChanged(const float Value);
	void OnCriticalStrikeChanceChanged(const float Value);
	void OnCriticalStrikeDamageMultiplierChanged(const float Value);
	void OnPhysicalDamageMultiplierChanged(const float Value);
	void OnFireDamageMultiplierChanged(const float Value);
	void OnLightningDamageMultiplierChanged(const float Value);
	void OnColdDamageMultiplierChanged(const float Value);
	void OnChaosDamageMultiplierChanged(const float Value);
	void OnFirePenetrationChanged(const float Value);
	void OnLightningPenetrationChanged(const float Value);
	void OnColdPenetrationChanged(const float Value);
	void OnChaosPenetrationChanged(const float Value);
	
	void OnArmorChanged(const float Value);
	void OnEvasionChanged(const float Value);
	void OnHealthRegenerationChanged(const float Value);
	void OnEnergyShieldRegenerationChanged(const float Value);
	void OnFireResistanceChanged(const float Value, const float MaxValue);
	void OnColdResistanceChanged(const float Value, const float MaxValue);
	void OnLightningResistanceChanged(const float Value, const float MaxValue);
	void OnChaosResistanceChanged(const float Value, const float MaxValue);
	void OnSpellSuppressionChanceChanged(const float Value);
	void OnSpellSuppressionMagnitudeChanged(const float Value);
	void OnHitBlockChanceChanged(const float Value, const float MaxValue);
	void OnSpellBlockChanceChanged(const float Value, const float MaxValue);
	
protected:
	/**
	 *  Character
	 */
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> HeroLevel_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> PlayerName_TextBlock;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> HeroClass_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Hero_Image;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> HeroExp_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> HeroExp_ProgressBar;;

	/**
	 * Main Attributes
	 */
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow_WithToolTip> Strength_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow_WithToolTip> Dexterity_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow_WithToolTip> Intelligence_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow_WithToolTip> Faith_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> Life_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> Mana_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> SpecialResource_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> EnergyShield_AttributeRow;

	/**
	 * Category Buttons
	 */

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Offence_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Defence_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Misc_Button;

	/**
	 * Attributes - Offence
	 */

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category = "Obsidian|CharacterStatus|AttributeBoxes")
	TObjectPtr<UScrollBox> Offence_ScrollBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> MainHandDamage_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> OffHandDamage_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> Accuracy_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> AttackSpeed_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> CastSpeed_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> CriticalStrikeChance_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> CriticalStrikeMulti_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> PhysDamageMultiplier_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> FireDamageMultiplier_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> LightningDamageMultiplier_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> ColdDamageMultiplier_AttributeRow;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> ChaosDamageMultiplier_AttributeRow;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> FirePenetration_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> LightningPenetration_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> ColdPenetration_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> ChaosPenetration_AttributeRow;
	
	/**
	 * Attributes - Defence
	 */

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category = "Obsidian|CharacterStatus|AttributeBoxes")
	TObjectPtr<UScrollBox> Defence_ScrollBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> Armor_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> PhysDamageTakenReduction_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> Evasion_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> ChanceToDodge_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> EnergyShieldRegeneration_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> HealthRegeneration_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> FireResistance_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> LightningResistance_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> ColdResistance_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> ChaosResistance_AttributeRow;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> SpellSuppressionChance_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> SpellSuppressionMagnitude_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> HitBlockChance_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> SpellBlockChance_AttributeRow;
	
	/**
	 * Attributes - Misc
	 */

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category = "Obsidian|CharacterStatus|AttributeBoxes")
	TObjectPtr<UScrollBox> Misc_ScrollBox;

private:
	UPROPERTY()
	TObjectPtr<UScrollBox> CurrentlyShownTab;

	UPROPERTY()
	TObjectPtr<UOCharacterStatusWidgetController> CharacterStatusWidgetController;

	/**
	 * Cached attributes
	 */
	
	float Experience = 0.0f;
	float MaxExperience = 0.0f;
	float LastMaxExperience = 0.0f;
};
