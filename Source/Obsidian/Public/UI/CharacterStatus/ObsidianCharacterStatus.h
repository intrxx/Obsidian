// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "UI/ObsidianMainOverlayWidgetBase.h"

#include "ObsidianCharacterStatus.generated.h"

class UProgressBar;
class UOCharacterStatusAttributeRow_WithToolTip;
class UButton;
class UScrollBox;
class UOCharacterStatusAttributeRow;
class UImage;
class UCommonTextBlock;
class UObCharacterStatusWidgetController;

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
	void SwitchToTab(UScrollBox* InTab);

protected:
	// ~ Start of Obsidian Widget Base
	virtual void HandleWidgetControllerSet() override;
	// ~ End of Obsidian Widget Base
	
	void SetExperienceTextBlock() const;
	void SetExperienceProgressBar() const;

	void OnHeroLevelUp(const uint8 InNewLevel);
	
	void OnExperienceChanged(const float InValue);
	void OnMaxExperienceChanged(const float InValue, const float InOldValue);
	
	void OnStrengthChanged(const float InValue);
	void OnIntelligenceChanged(const float InValue);
	void OnDexterityChanged(const float InValue);
	void OnFaithChanged(const float InValue);
	
	void OnMaxHealthChanged(const float InValue);
	void OnMaxManaChanged(const float InValue);
	void OnMaxSpecialResourceChanged(const float InValue);
	void OnMaxEnergyShieldChanged(const float InValue);
	void OnMaxStaminaChanged(const float InValue);
	void OnStaminaRegenerationChanged(const float InValue);
	
	void OnAccuracyChanged(const float InValue);
	void OnAttackSpeedChanged(const float InValue);
	void OnCastSpeedChanged(const float InValue);
	void OnCriticalStrikeChanceChanged(const float InValue);
	void OnCriticalStrikeDamageMultiplierChanged(const float InValue);
	void OnPhysicalDamageMultiplierChanged(const float InValue);
	void OnFireDamageMultiplierChanged(const float InValue);
	void OnLightningDamageMultiplierChanged(const float InValue);
	void OnColdDamageMultiplierChanged(const float InValue);
	void OnChaosDamageMultiplierChanged(const float InValue);
	void OnFirePenetrationChanged(const float InValue);
	void OnLightningPenetrationChanged(const float InValue);
	void OnColdPenetrationChanged(const float InValue);
	void OnChaosPenetrationChanged(const float InValue);
	
	void OnArmorChanged(const float InValue);
	void OnEvasionChanged(const float InValue);
	void OnHealthRegenerationChanged(const float InValue);
	void OnEnergyShieldRegenerationChanged(const float InValue);
	void OnFireResistanceChanged(const float InValue, const float InMaxValue);
	void OnColdResistanceChanged(const float InValue, const float InMaxValue);
	void OnLightningResistanceChanged(const float InValue, const float InMaxValue);
	void OnChaosResistanceChanged(const float InValue, const float InMaxValue);
	void OnSpellSuppressionChanceChanged(const float InValue);
	void OnSpellSuppressionMagnitudeChanged(const float InValue);
	void OnHitBlockChanceChanged(const float InValue, const float InMaxValue);
	void OnSpellBlockChanceChanged(const float InValue, const float InMaxValue);
	
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
	TObjectPtr<UOCharacterStatusAttributeRow> Stamina_AttributeRow;

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
	TObjectPtr<UOCharacterStatusAttributeRow> StaminaRegeneration_AttributeRow;

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
	TObjectPtr<UObCharacterStatusWidgetController> CharacterStatusWidgetController;

	/**
	 * Cached attributes
	 */
	
	float Experience = 0.0f;
	float MaxExperience = 0.0f;
	float LastMaxExperience = 0.0f;
};
