// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/CharacterStatus/ObsidianCharacterStatus.h"

// ~ Core
#include "CommonTextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/ScrollBox.h"

// ~ Project
#include "UI/WidgetControllers/OCharacterStatusWidgetController.h"
#include "UI/CharacterStatus/Subwidgets/OCharacterStatusAttributeRow_WithToolTip.h"

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
	CharacterStatusWidgetController = Cast<UOCharacterStatusWidgetController>(WidgetController);
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
		PlayerName_TextBlock->SetText(CharacterStatusWidgetController->HeroNameText);
	}
}

void UObsidianCharacterStatus::SwitchToTab(UScrollBox* Tab)
{
	if(Tab == nullptr)
	{
		return;
	}

	CurrentlyShownTab->SetVisibility(ESlateVisibility::Collapsed);
	Tab->SetVisibility(ESlateVisibility::Visible);
	CurrentlyShownTab = Tab;
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

void UObsidianCharacterStatus::OnHeroLevelUp(const int32 NewLevel)
{
	if(HeroLevel_TextBlock)
	{
		const FText NewLevelText = FText::FromString(FString::FromInt(NewLevel));
		HeroLevel_TextBlock->SetText(NewLevelText);
	}
}

void UObsidianCharacterStatus::OnExperienceChanged(const float Value)
{
	Experience = Value;
	
	SetExperienceTextBlock();
	SetExperienceProgressBar();
}

void UObsidianCharacterStatus::OnMaxExperienceChanged(const float Value, const float OldValue)
{
	LastMaxExperience = OldValue;
	MaxExperience = Value;
	
	SetExperienceTextBlock();
	SetExperienceProgressBar();
}

void UObsidianCharacterStatus::OnStrengthChanged(const float Value)
{
	Strength_AttributeRow->SetAttributeValue(Value);
}

void UObsidianCharacterStatus::OnIntelligenceChanged(const float Value)
{
	Intelligence_AttributeRow->SetAttributeValue(Value);
}

void UObsidianCharacterStatus::OnDexterityChanged(const float Value)
{
	Dexterity_AttributeRow->SetAttributeValue(Value);
}

void UObsidianCharacterStatus::OnFaithChanged(const float Value)
{
	Faith_AttributeRow->SetAttributeValue(Value); 
}

void UObsidianCharacterStatus::OnMaxHealthChanged(const float Value)
{
	Life_AttributeRow->SetAttributeValue(Value);
}

void UObsidianCharacterStatus::OnMaxManaChanged(const float Value)
{
	Mana_AttributeRow->SetAttributeValue(Value);
}

void UObsidianCharacterStatus::OnMaxSpecialResourceChanged(const float Value)
{
	SpecialResource_AttributeRow->SetAttributeValue(Value);
}

void UObsidianCharacterStatus::OnMaxEnergyShieldChanged(const float Value)
{
	EnergyShield_AttributeRow->SetAttributeValue(Value);
}

void UObsidianCharacterStatus::OnAccuracyChanged(const float Value)
{
	Accuracy_AttributeRow->SetAttributeValue(Value);
}

void UObsidianCharacterStatus::OnAttackSpeedChanged(const float Value)
{
	AttackSpeed_AttributeRow->SetAttributeValueWithPercentage(Value);
}

void UObsidianCharacterStatus::OnCastSpeedChanged(const float Value)
{
	CastSpeed_AttributeRow->SetAttributeValueWithPercentage(Value);
}

void UObsidianCharacterStatus::OnCriticalStrikeChanceChanged(const float Value)
{
	CriticalStrikeChance_AttributeRow->SetAttributeValueWithPercentage(Value);
}

void UObsidianCharacterStatus::OnCriticalStrikeDamageMultiplierChanged(const float Value)
{
	CriticalStrikeMulti_AttributeRow->SetAttributeValueWithPercentage(Value);
}

void UObsidianCharacterStatus::OnPhysicalDamageMultiplierChanged(const float Value)
{
	PhysDamageMultiplier_AttributeRow->SetAttributeValueWithPercentage(Value);
}

void UObsidianCharacterStatus::OnFireDamageMultiplierChanged(const float Value)
{
	FireDamageMultiplier_AttributeRow->SetAttributeValueWithPercentage(Value);
}

void UObsidianCharacterStatus::OnLightningDamageMultiplierChanged(const float Value)
{
	LightningDamageMultiplier_AttributeRow->SetAttributeValueWithPercentage(Value);
}

void UObsidianCharacterStatus::OnColdDamageMultiplierChanged(const float Value)
{
	ColdDamageMultiplier_AttributeRow->SetAttributeValueWithPercentage(Value);
}

void UObsidianCharacterStatus::OnChaosDamageMultiplierChanged(const float Value)
{
	ChaosDamageMultiplier_AttributeRow->SetAttributeValueWithPercentage(Value);
}

void UObsidianCharacterStatus::OnFirePenetrationChanged(const float Value)
{
	FirePenetration_AttributeRow->SetAttributeValueWithPercentage(Value);
}

void UObsidianCharacterStatus::OnLightningPenetrationChanged(const float Value)
{
	LightningPenetration_AttributeRow->SetAttributeValueWithPercentage(Value);
}

void UObsidianCharacterStatus::OnColdPenetrationChanged(const float Value)
{
	ColdPenetration_AttributeRow->SetAttributeValueWithPercentage(Value);
}

void UObsidianCharacterStatus::OnChaosPenetrationChanged(const float Value)
{
	ChaosPenetration_AttributeRow->SetAttributeValueWithPercentage(Value);
}

void UObsidianCharacterStatus::OnArmorChanged(const float Value)
{
	Armor_AttributeRow->SetAttributeValue(Value);
}

void UObsidianCharacterStatus::OnEvasionChanged(const float Value)
{
	Evasion_AttributeRow->SetAttributeValue(Value);
}

void UObsidianCharacterStatus::OnHealthRegenerationChanged(const float Value)
{
	HealthRegeneration_AttributeRow->SetAttributeValue(Value);
}

void UObsidianCharacterStatus::OnEnergyShieldRegenerationChanged(const float Value)
{
	EnergyShieldRegeneration_AttributeRow->SetAttributeValue(Value);
}

void UObsidianCharacterStatus::OnFireResistanceChanged(const float Value, const float MaxValue)
{
	FireResistance_AttributeRow->SetTwoAttributeValuesWithPercent(Value, MaxValue);
}

void UObsidianCharacterStatus::OnColdResistanceChanged(const float Value, const float MaxValue)
{
	ColdResistance_AttributeRow->SetTwoAttributeValuesWithPercent(Value, MaxValue);
}

void UObsidianCharacterStatus::OnLightningResistanceChanged(const float Value, const float MaxValue)
{
	LightningResistance_AttributeRow->SetTwoAttributeValuesWithPercent(Value, MaxValue);
}

void UObsidianCharacterStatus::OnChaosResistanceChanged(const float Value, const float MaxValue)
{
	ChaosResistance_AttributeRow->SetTwoAttributeValuesWithPercent(Value, MaxValue);
}

void UObsidianCharacterStatus::OnSpellSuppressionChanceChanged(const float Value)
{
	SpellSuppressionChance_AttributeRow->SetAttributeValueWithPercentage(Value);
}

void UObsidianCharacterStatus::OnSpellSuppressionMagnitudeChanged(const float Value)
{
	SpellSuppressionMagnitude_AttributeRow->SetAttributeValueWithPercentage(Value);
}

void UObsidianCharacterStatus::OnHitBlockChanceChanged(const float Value, const float MaxValue)
{
	HitBlockChance_AttributeRow->SetTwoAttributeValuesWithPercent(Value, MaxValue);
}

void UObsidianCharacterStatus::OnSpellBlockChanceChanged(const float Value, const float MaxValue)
{
	SpellBlockChance_AttributeRow->SetTwoAttributeValuesWithPercent(Value, MaxValue);
}


