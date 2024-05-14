// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/CharacterStatus/ObsidianCharacterStatus.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "UI/WidgetControllers/OCharacterStatusWidgetController.h"
#include "UI/CharacterStatus/Subwidgets/OCharacterStatusAttributeRow.h"

void UObsidianCharacterStatus::NativeConstruct()
{
	Super::NativeConstruct();

	CurrentlyShownTab = Offence_ScrollBox;

	Close_Button->OnClicked.AddDynamic(this, &ThisClass::OnCloseButtonClicked);
}

void UObsidianCharacterStatus::NativeDestruct()
{
	OnCharacterStatusDestroyedDelegate.Broadcast();
	
	Super::NativeDestruct();
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

void UObsidianCharacterStatus::OnCloseButtonClicked()
{
	RemoveFromParent();
}

void UObsidianCharacterStatus::HandleWidgetControllerSet()
{
	CharacterStatusWidgetController = Cast<UOCharacterStatusWidgetController>(WidgetController);
	if(CharacterStatusWidgetController == nullptr)
	{
		return;
	}
	
	/**
	 * Attributes
	 */
	CharacterStatusWidgetController->StrengthValueChangedDelegate.BindLambda([this](const float Value)
	{
		Strength_AttributeRow->SetAttributeValue(Value);
	});

	CharacterStatusWidgetController->IntelligenceValueChangedDelegate.BindLambda([this](const float Value)
	{
		Intelligence_AttributeRow->SetAttributeValue(Value);
	});

	CharacterStatusWidgetController->DexterityValueChangedDelegate.BindLambda([this](const float Value)
	{
		Dexterity_AttributeRow->SetAttributeValue(Value);
	});

	CharacterStatusWidgetController->FaithValueChangedDelegate.BindLambda([this](const float Value)
	{
		Faith_AttributeRow->SetAttributeValue(Value);
	});

	/**
	 * Vital Attributes
	 */
	CharacterStatusWidgetController->MaxHealthChangedDelegate.BindLambda([this](const float Value)
	{
		Life_AttributeRow->SetAttributeValue(Value);
	});

	CharacterStatusWidgetController->MaxManaChangedDelegate.BindLambda([this](const float Value)
	{
		Mana_AttributeRow->SetAttributeValue(Value);
	});

	CharacterStatusWidgetController->MaxEnergyShieldChangedDelegate.BindLambda([this](const float Value)
	{
		EnergyShield_AttributeRow->SetAttributeValue(Value);
	});

	/**
	 * Offence
	 */
	CharacterStatusWidgetController->AttackSpeedChangedDelegate.BindLambda([this](const float Value)
	{
		AttackSpeed_AttributeRow->SetAttributeValueWithPercentage(Value);
	});

	CharacterStatusWidgetController->CastSpeedChangedDelegate.BindLambda([this](const float Value)
	{
		CastSpeed_AttributeRow->SetAttributeValueWithPercentage(Value);
	});

	CharacterStatusWidgetController->CriticalStrikeChanceChangedDelegate.BindLambda([this](const float Value)
	{
		CriticalStrikeChance_AttributeRow->SetAttributeValueWithPercentage(Value);
	});

	CharacterStatusWidgetController->CriticalStrikeMultiplierChangedDelegate.BindLambda([this](const float Value)
	{
		CriticalStrikeMulti_AttributeRow->SetAttributeValueWithPercentage(Value);
	});

	CharacterStatusWidgetController->PhysicalDamageMultiplierChangedDelegate.BindLambda([this](const float Value)
	{
		PhysDamageMultiplier_AttributeRow->SetAttributeValueWithPercentage(Value);
	});

	CharacterStatusWidgetController->FireDamageMultiplierChangedDelegate.BindLambda([this](const float Value)
	{
		FireDamageMultiplier_AttributeRow->SetAttributeValueWithPercentage(Value);
	});

	CharacterStatusWidgetController->LightningDamageMultiplierChangedDelegate.BindLambda([this](const float Value)
	{
		LightningDamageMultiplier_AttributeRow->SetAttributeValueWithPercentage(Value);
	});

	CharacterStatusWidgetController->ColdDamageMultiplierChangedDelegate.BindLambda([this](const float Value)
	{
		ColdDamageMultiplier_AttributeRow->SetAttributeValueWithPercentage(Value);
	});

	CharacterStatusWidgetController->ChaosDamageMultiplierChangedDelegate.BindLambda([this](const float Value)
	{
		ChaosDamageMultiplier_AttributeRow->SetAttributeValueWithPercentage(Value);
	});

	CharacterStatusWidgetController->FirePenetrationChangedDelegate.BindLambda([this](const float Value)
	{
		FirePenetration_AttributeRow->SetAttributeValueWithPercentage(Value);
	});

	CharacterStatusWidgetController->LightningPenetrationChangedDelegate.BindLambda([this](const float Value)
	{
		LightningPenetration_AttributeRow->SetAttributeValueWithPercentage(Value);
	});

	CharacterStatusWidgetController->ColdPenetrationChangedDelegate.BindLambda([this](const float Value)
	{
		ColdPenetration_AttributeRow->SetAttributeValueWithPercentage(Value);
	});

	/**
	 * Defence
	 */
	CharacterStatusWidgetController->ArmorChangedDelegate.BindLambda([this](const float Value)
	{
		Armor_AttributeRow->SetAttributeValue(Value);
	});

	CharacterStatusWidgetController->EvasionChangedDelegate.BindLambda([this](const float Value)
	{
		Evasion_AttributeRow->SetAttributeValue(Value);
	});
	
	CharacterStatusWidgetController->HealthRegenerationChangedDelegate.BindLambda([this](const float Value)
	{
		HealthRegeneration_AttributeRow->SetAttributeValue(Value);
	});

	CharacterStatusWidgetController->EnergyShieldRegenerationChangedDelegate.BindLambda([this](const float Value)
	{
		EnergyShieldRegeneration_AttributeRow->SetAttributeValue(Value);
	});

	CharacterStatusWidgetController->FireResistanceChangedDelegate.BindLambda([this](const float Value, const float MaxValue)
	{
		FireResistance_AttributeRow->SetResistanceAttributeValue(Value, MaxValue);
	});

	CharacterStatusWidgetController->MaxFireResistanceChangedDelegate.BindLambda([this](const float Value, const float MaxValue)
	{
		FireResistance_AttributeRow->SetResistanceAttributeValue(Value, MaxValue);
	});

	CharacterStatusWidgetController->ColdResistanceChangedDelegate.BindLambda([this](const float Value, const float MaxValue)
	{
		ColdResistance_AttributeRow->SetResistanceAttributeValue(Value, MaxValue);
	});

	CharacterStatusWidgetController->MaxColdResistanceChangedDelegate.BindLambda([this](const float Value, const float MaxValue)
	{
		ColdResistance_AttributeRow->SetResistanceAttributeValue(Value, MaxValue);
	});

	CharacterStatusWidgetController->LightningResistanceChangedDelegate.BindLambda([this](const float Value, const float MaxValue)
	{
		LightningResistance_AttributeRow->SetResistanceAttributeValue(Value, MaxValue);
	});

	CharacterStatusWidgetController->MaxLightningResistanceChangedDelegate.BindLambda([this](const float Value, const float MaxValue)
	{
		LightningResistance_AttributeRow->SetResistanceAttributeValue(Value, MaxValue);
	});

	CharacterStatusWidgetController->ChaosResistanceChangedDelegate.BindLambda([this](const float Value, const float MaxValue)
	{
		ChaosResistance_AttributeRow->SetResistanceAttributeValue(Value, MaxValue);
	});

	CharacterStatusWidgetController->MaxChaosResistanceChangedDelegate.BindLambda([this](const float Value, const float MaxValue)
	{
		ChaosResistance_AttributeRow->SetResistanceAttributeValue(Value, MaxValue);
	});
}


