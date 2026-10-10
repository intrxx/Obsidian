// Copyright 2026 out of sCope team - intrxx

#include "UI/CharacterStatus/ObsidianCharacterStatus.h"

#include "CommonTextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/ScrollBox.h"

#include "UI/CharacterStatus/Subwidgets/OCharacterStatusAttributeRow_WithToolTip.h"
#include "UI/WidgetControllers/ObCharacterStatusWidgetController.h"


void UObsidianCharacterStatus::NativeConstruct()
{
	Super::NativeConstruct();

	CurrentlyShownTab = Offence_ScrollBox;
	
	Strength_AttributeRow->SetCharacterStatus(this);
	Dexterity_AttributeRow->SetCharacterStatus(this);
	Intelligence_AttributeRow->SetCharacterStatus(this);
	Faith_AttributeRow->SetCharacterStatus(this);
}

void UObsidianCharacterStatus::HandleWidgetControllerSet()
{
	CharacterStatusWidgetController = Cast<UObCharacterStatusWidgetController>(WidgetController);
	if(CharacterStatusWidgetController == nullptr)
	{
		return;
	}

	/**
	 * Character
	 */
	CharacterStatusWidgetController->HeroLevelUpDelegate.AddUObject(this, &ThisClass::OnHeroLevelUp);
	CharacterStatusWidgetController->ExperienceChangedDelegate.BindUObject(this, &ThisClass::OnExperienceChanged);
	CharacterStatusWidgetController->MaxExperienceChangedDelegate.BindUObject(this, &ThisClass::OnMaxExperienceChanged);
	
	/**
	 * Attributes
	 */
	CharacterStatusWidgetController->StrengthValueChangedDelegate.BindUObject(this, &ThisClass::OnStrengthChanged);
	CharacterStatusWidgetController->IntelligenceValueChangedDelegate.BindUObject(this, &ThisClass::OnIntelligenceChanged);
	CharacterStatusWidgetController->DexterityValueChangedDelegate.BindUObject(this, &ThisClass::OnDexterityChanged);
	CharacterStatusWidgetController->FaithValueChangedDelegate.BindUObject(this, &ThisClass::OnFaithChanged);

	/**
	 * Vital Attributes
	 */
	CharacterStatusWidgetController->MaxHealthChangedDelegate.BindUObject(this, &ThisClass::OnMaxHealthChanged);
	CharacterStatusWidgetController->MaxManaChangedDelegate.BindUObject(this, &ThisClass::OnMaxManaChanged);
	CharacterStatusWidgetController->MaxSpecialResourceChangedDelegate.BindUObject(this, &ThisClass::OnMaxSpecialResourceChanged);
	CharacterStatusWidgetController->MaxEnergyShieldChangedDelegate.BindUObject(this, &ThisClass::OnMaxEnergyShieldChanged);
	CharacterStatusWidgetController->MaxStaminaChangedDelegate.BindUObject(this, &ThisClass::OnMaxStaminaChanged);
	CharacterStatusWidgetController->StaminaRegenerationChangedDelegate.BindUObject(this, &ThisClass::OnStaminaRegenerationChanged);

	/**
	 * Offence
	 */
	CharacterStatusWidgetController->AccuracyChangedDelegate.BindUObject(this, &ThisClass::OnAccuracyChanged);
	CharacterStatusWidgetController->AttackSpeedChangedDelegate.BindUObject(this, &ThisClass::OnAttackSpeedChanged);
	CharacterStatusWidgetController->CastSpeedChangedDelegate.BindUObject(this, &ThisClass::OnCastSpeedChanged);
	CharacterStatusWidgetController->CriticalStrikeChanceChangedDelegate.BindUObject(this, &ThisClass::OnCriticalStrikeChanceChanged);
	CharacterStatusWidgetController->CriticalStrikeDamageMultiplierChangedDelegate.BindUObject(this, &ThisClass::OnCriticalStrikeDamageMultiplierChanged);
	CharacterStatusWidgetController->PhysicalDamageMultiplierChangedDelegate.BindUObject(this, &ThisClass::OnPhysicalDamageMultiplierChanged);
	CharacterStatusWidgetController->FireDamageMultiplierChangedDelegate.BindUObject(this, &ThisClass::OnFireDamageMultiplierChanged);
	CharacterStatusWidgetController->LightningDamageMultiplierChangedDelegate.BindUObject(this, &ThisClass::OnLightningDamageMultiplierChanged);
	CharacterStatusWidgetController->ColdDamageMultiplierChangedDelegate.BindUObject(this, &ThisClass::OnColdDamageMultiplierChanged);
	CharacterStatusWidgetController->ChaosDamageMultiplierChangedDelegate.BindUObject(this, &ThisClass::OnChaosDamageMultiplierChanged);
	CharacterStatusWidgetController->FirePenetrationChangedDelegate.BindUObject(this, &ThisClass::OnFirePenetrationChanged);
	CharacterStatusWidgetController->LightningPenetrationChangedDelegate.BindUObject(this, &ThisClass::OnLightningPenetrationChanged);
	CharacterStatusWidgetController->ColdPenetrationChangedDelegate.BindUObject(this, &ThisClass::OnColdPenetrationChanged);
	CharacterStatusWidgetController->ChaosPenetrationChangedDelegate.BindUObject(this, &ThisClass::OnChaosPenetrationChanged);

	/**
	 * Defence
	 */
	CharacterStatusWidgetController->ArmorChangedDelegate.BindUObject(this, &ThisClass::OnArmorChanged);
	CharacterStatusWidgetController->EvasionChangedDelegate.BindUObject(this, &ThisClass::OnEvasionChanged);
	CharacterStatusWidgetController->HealthRegenerationChangedDelegate.BindUObject(this, &ThisClass::OnHealthRegenerationChanged);
	CharacterStatusWidgetController->EnergyShieldRegenerationChangedDelegate.BindUObject(this, &ThisClass::OnEnergyShieldRegenerationChanged);
	CharacterStatusWidgetController->FireResistanceChangedDelegate.BindUObject(this, &ThisClass::OnFireResistanceChanged);
	CharacterStatusWidgetController->MaxFireResistanceChangedDelegate.BindUObject(this, &ThisClass::OnFireResistanceChanged);
	CharacterStatusWidgetController->ColdResistanceChangedDelegate.BindUObject(this, &ThisClass::OnColdResistanceChanged);
	CharacterStatusWidgetController->MaxColdResistanceChangedDelegate.BindUObject(this, &ThisClass::OnColdResistanceChanged);
	CharacterStatusWidgetController->LightningResistanceChangedDelegate.BindUObject(this, &ThisClass::OnLightningResistanceChanged);
	CharacterStatusWidgetController->MaxLightningResistanceChangedDelegate.BindUObject(this, &ThisClass::OnLightningResistanceChanged);
	CharacterStatusWidgetController->ChaosResistanceChangedDelegate.BindUObject(this, &ThisClass::OnChaosResistanceChanged);
	CharacterStatusWidgetController->MaxChaosResistanceChangedDelegate.BindUObject(this, &ThisClass::OnChaosResistanceChanged);
	CharacterStatusWidgetController->SpellSuppressionChanceChangedDelegate.BindUObject(this, &ThisClass::OnSpellSuppressionChanceChanged);
	CharacterStatusWidgetController->SpellSuppressionMagnitudeChangedDelegate.BindUObject(this, &ThisClass::OnSpellSuppressionMagnitudeChanged);
	CharacterStatusWidgetController->HitBlockChanceChangedDelegate.BindUObject(this, &ThisClass::OnHitBlockChanceChanged);
	CharacterStatusWidgetController->MaxHitBlockChanceChangedDelegate.BindUObject(this, &ThisClass::OnHitBlockChanceChanged);
	CharacterStatusWidgetController->SpellBlockChanceChangedDelegate.BindUObject(this, &ThisClass::OnSpellBlockChanceChanged);
	CharacterStatusWidgetController->MaxSpellBlockChanceChangedDelegate.BindUObject(this, &ThisClass::OnSpellBlockChanceChanged);

	const FText HeroClassText = CharacterStatusWidgetController->HeroClassText;
	if(HeroClass_TextBlock)
	{
		HeroClass_TextBlock->SetText(HeroClassText);		
	}

	if(PlayerName_TextBlock)
	{
		PlayerName_TextBlock->SetText(FText::FromString(CharacterStatusWidgetController->HeroNameString));
	}
}

void UObsidianCharacterStatus::SwitchToTab(UScrollBox* InTab)
{
	if(InTab == nullptr)
	{
		return;
	}

	CurrentlyShownTab->SetVisibility(ESlateVisibility::Collapsed);
	InTab->SetVisibility(ESlateVisibility::Visible);
	CurrentlyShownTab = InTab;
}

void UObsidianCharacterStatus::SetExperienceTextBlock() const
{
	int32 PercentToTheNextLevel = 0;
	if(MaxExperience > 0.0f)
	{
		PercentToTheNextLevel = FMath::TruncToInt(((Experience - LastMaxExperience) / (MaxExperience - LastMaxExperience) * 100));
	}
	
	const FText ExperienceText = FText::FromString(FString::Printf(TEXT("%d of %d / (%d%%)"),
		FMath::TruncToInt(Experience), FMath::TruncToInt(MaxExperience), PercentToTheNextLevel));
	
	if(HeroExp_TextBlock)
	{
		HeroExp_TextBlock->SetText(ExperienceText);
	}
}

void UObsidianCharacterStatus::SetExperienceProgressBar() const
{
	float BarPercentage = 0.0f;
	if(MaxExperience > 0.0f)
	{
		BarPercentage = (Experience - LastMaxExperience) / (MaxExperience - LastMaxExperience);
	}
	
	if(HeroExp_ProgressBar)
	{
		HeroExp_ProgressBar->SetPercent(BarPercentage);
	}
}

void UObsidianCharacterStatus::OnHeroLevelUp(const uint8 InNewLevel)
{
	if(HeroLevel_TextBlock)
	{
		HeroLevel_TextBlock->SetText(FText::AsNumber(InNewLevel));
	}
}

void UObsidianCharacterStatus::OnExperienceChanged(const float InValue)
{
	Experience = InValue;
	
	SetExperienceTextBlock();
	SetExperienceProgressBar();
}

void UObsidianCharacterStatus::OnMaxExperienceChanged(const float InValue, const float InOldValue)
{
	LastMaxExperience = InOldValue;
	MaxExperience = InValue;
	
	SetExperienceTextBlock();
	SetExperienceProgressBar();
}

void UObsidianCharacterStatus::OnStrengthChanged(const float InValue)
{
	Strength_AttributeRow->SetAttributeValue(InValue);
}

void UObsidianCharacterStatus::OnIntelligenceChanged(const float InValue)
{
	Intelligence_AttributeRow->SetAttributeValue(InValue);
}

void UObsidianCharacterStatus::OnDexterityChanged(const float InValue)
{
	Dexterity_AttributeRow->SetAttributeValue(InValue);
}

void UObsidianCharacterStatus::OnFaithChanged(const float InValue)
{
	Faith_AttributeRow->SetAttributeValue(InValue); 
}

void UObsidianCharacterStatus::OnMaxHealthChanged(const float InValue)
{
	Life_AttributeRow->SetAttributeValue(InValue);
}

void UObsidianCharacterStatus::OnMaxManaChanged(const float InValue)
{
	Mana_AttributeRow->SetAttributeValue(InValue);
}

void UObsidianCharacterStatus::OnMaxSpecialResourceChanged(const float InValue)
{
	SpecialResource_AttributeRow->SetAttributeValue(InValue);
}

void UObsidianCharacterStatus::OnMaxEnergyShieldChanged(const float InValue)
{
	EnergyShield_AttributeRow->SetAttributeValue(InValue);
}

void UObsidianCharacterStatus::OnMaxStaminaChanged(const float InValue)
{
	Stamina_AttributeRow->SetAttributeValue(InValue);
}

void UObsidianCharacterStatus::OnStaminaRegenerationChanged(const float InValue)
{
	StaminaRegeneration_AttributeRow->SetAttributeValue(InValue);
}

void UObsidianCharacterStatus::OnAccuracyChanged(const float InValue)
{
	Accuracy_AttributeRow->SetAttributeValue(InValue);
}

void UObsidianCharacterStatus::OnAttackSpeedChanged(const float InValue)
{
	AttackSpeed_AttributeRow->SetAttributeValueWithPercentage(InValue);
}

void UObsidianCharacterStatus::OnCastSpeedChanged(const float InValue)
{
	CastSpeed_AttributeRow->SetAttributeValueWithPercentage(InValue);
}

void UObsidianCharacterStatus::OnCriticalStrikeChanceChanged(const float InValue)
{
	CriticalStrikeChance_AttributeRow->SetAttributeValueWithPercentage(InValue);
}

void UObsidianCharacterStatus::OnCriticalStrikeDamageMultiplierChanged(const float InValue)
{
	CriticalStrikeMulti_AttributeRow->SetAttributeValueWithPercentage(InValue);
}

void UObsidianCharacterStatus::OnPhysicalDamageMultiplierChanged(const float InValue)
{
	PhysDamageMultiplier_AttributeRow->SetAttributeValueWithPercentage(InValue);
}

void UObsidianCharacterStatus::OnFireDamageMultiplierChanged(const float InValue)
{
	FireDamageMultiplier_AttributeRow->SetAttributeValueWithPercentage(InValue);
}

void UObsidianCharacterStatus::OnLightningDamageMultiplierChanged(const float InValue)
{
	LightningDamageMultiplier_AttributeRow->SetAttributeValueWithPercentage(InValue);
}

void UObsidianCharacterStatus::OnColdDamageMultiplierChanged(const float InValue)
{
	ColdDamageMultiplier_AttributeRow->SetAttributeValueWithPercentage(InValue);
}

void UObsidianCharacterStatus::OnChaosDamageMultiplierChanged(const float InValue)
{
	ChaosDamageMultiplier_AttributeRow->SetAttributeValueWithPercentage(InValue);
}

void UObsidianCharacterStatus::OnFirePenetrationChanged(const float InValue)
{
	FirePenetration_AttributeRow->SetAttributeValueWithPercentage(InValue);
}

void UObsidianCharacterStatus::OnLightningPenetrationChanged(const float InValue)
{
	LightningPenetration_AttributeRow->SetAttributeValueWithPercentage(InValue);
}

void UObsidianCharacterStatus::OnColdPenetrationChanged(const float InValue)
{
	ColdPenetration_AttributeRow->SetAttributeValueWithPercentage(InValue);
}

void UObsidianCharacterStatus::OnChaosPenetrationChanged(const float InValue)
{
	ChaosPenetration_AttributeRow->SetAttributeValueWithPercentage(InValue);
}

void UObsidianCharacterStatus::OnArmorChanged(const float InValue)
{
	Armor_AttributeRow->SetAttributeValue(InValue);
}

void UObsidianCharacterStatus::OnEvasionChanged(const float InValue)
{
	Evasion_AttributeRow->SetAttributeValue(InValue);
}

void UObsidianCharacterStatus::OnHealthRegenerationChanged(const float InValue)
{
	HealthRegeneration_AttributeRow->SetAttributeValue(InValue);
}

void UObsidianCharacterStatus::OnEnergyShieldRegenerationChanged(const float InValue)
{
	EnergyShieldRegeneration_AttributeRow->SetAttributeValue(InValue);
}

void UObsidianCharacterStatus::OnFireResistanceChanged(const float InValue, const float InMaxValue)
{
	FireResistance_AttributeRow->SetTwoAttributeValuesWithPercent(InValue, InMaxValue);
}

void UObsidianCharacterStatus::OnColdResistanceChanged(const float InValue, const float InMaxValue)
{
	ColdResistance_AttributeRow->SetTwoAttributeValuesWithPercent(InValue, InMaxValue);
}

void UObsidianCharacterStatus::OnLightningResistanceChanged(const float InValue, const float InMaxValue)
{
	LightningResistance_AttributeRow->SetTwoAttributeValuesWithPercent(InValue, InMaxValue);
}

void UObsidianCharacterStatus::OnChaosResistanceChanged(const float InValue, const float InMaxValue)
{
	ChaosResistance_AttributeRow->SetTwoAttributeValuesWithPercent(InValue, InMaxValue);
}

void UObsidianCharacterStatus::OnSpellSuppressionChanceChanged(const float InValue)
{
	SpellSuppressionChance_AttributeRow->SetAttributeValueWithPercentage(InValue);
}

void UObsidianCharacterStatus::OnSpellSuppressionMagnitudeChanged(const float InValue)
{
	SpellSuppressionMagnitude_AttributeRow->SetAttributeValueWithPercentage(InValue);
}

void UObsidianCharacterStatus::OnHitBlockChanceChanged(const float InValue, const float InMaxValue)
{
	HitBlockChance_AttributeRow->SetTwoAttributeValuesWithPercent(InValue, InMaxValue);
}

void UObsidianCharacterStatus::OnSpellBlockChanceChanged(const float InValue, const float InMaxValue)
{
	SpellBlockChance_AttributeRow->SetTwoAttributeValuesWithPercent(InValue, InMaxValue);
}


