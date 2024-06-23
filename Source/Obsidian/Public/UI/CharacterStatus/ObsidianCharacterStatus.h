// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetBase.h"
#include "ObsidianCharacterStatus.generated.h"

class UButton;
class UScrollBox;
class UOCharacterStatusAttributeRow;
class UImage;
class UCommonTextBlock;
class UOCharacterStatusWidgetController;

DECLARE_MULTICAST_DELEGATE(FOnCharacterStatusDestroyed);

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianCharacterStatus : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|CharacterStatus")
	void SwitchToTab(UScrollBox* Tab);

public:
	FOnCharacterStatusDestroyed OnCharacterStatusDestroyedDelegate;
	
protected:
	UFUNCTION()
	void OnCloseButtonClicked();

	// ~ Start of Obsidian Widget Base
	virtual void HandleWidgetControllerSet() override;
	// ~ End of Obsidian Widget Base
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> HeroLevel_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> PlayerName_TextBlock;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> HeroClass_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Hero_Image;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Close_Button;

	/**
	 * Main Attributes
	 */
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> Strength_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> Dexterity_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> Intelligence_AttributeRow;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOCharacterStatusAttributeRow> Faith_AttributeRow;

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

	
};
