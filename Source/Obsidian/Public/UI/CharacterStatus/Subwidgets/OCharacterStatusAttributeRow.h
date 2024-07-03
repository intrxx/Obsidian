// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetBase.h"
#include "OCharacterStatusAttributeRow.generated.h"

class USpacer;
class UCommonTextStyle;
class UCommonTextBlock;
class USizeBox;
/**
 * 
 */
UCLASS()
class OBSIDIAN_API UOCharacterStatusAttributeRow : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;

	/** Sets the Attribute Value on the widget and the private attribute variable in OCharacterStatusAttributeRow */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|CharacterStatusWidgetRow")
	void SetAttributeValue(const float& Value);

	/** Sets the Attribute Value on the widget, adds a "%" to the end and sets the private attribute variable in OCharacterStatusAttributeRow */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|CharacterStatusWidgetRow")
	void SetAttributeValueWithPercentage(const float& Value);

	/**
	 * Sets both the current Attribute Value and the Max Attribute Value on the widget, adds a "%" to the end and sets the private attribute variables in OCharacterStatusAttributeRow.
	 */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|CharacterStatusWidgetRow")
	void SetTwoAttributeValuesWithPercent(const float& Value, const float& MaxValue);

	/**
	 * Updates the value on the widget, designed to take "All*" attributes and update all components of it.
	 * Example:
	 * It takes the value of "AllElementalPenetration" and only updates the value of "FirePenetration" value on the widget, without setting the value on the class.
	 */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|CharacterStatusWidgetRow")
	void UpdateAttributeValueWithPercentage(const float& Value) const;
	
	/**
	 * Updates both the current and max values on the widget, designed to take "All*" attributes and update all components of it.
	 * Example:
	 * It takes the value of "AllElementalResistances" and only updates the value of "FireResistance" and "MaxFireResistance" on the widget, without setting values on the class.
	 */
	void UpdateResistanceAttributeValue(const float& Value = 0, const float& MaxValue = 0) const;
	
	float GetCurrentAttributeValue() const { return CurrentAttributeValue; }
	float GetCurrentMaxAttributeValue() const { return CurrentMaxAttributeValue; }
	
protected:
	UFUNCTION(BlueprintCallable, Category = "Obsidian|CharacterStatusWidgetRow")
	void InitialSetup();

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> AttributeName_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> AttributeValue_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USpacer> NameAndValue_Spacer;
	
	/**
	 *  Initial Set up
	 */

	UPROPERTY(EditAnywhere, Category = "Obsidian|Properties")
	TSubclassOf<UCommonTextStyle> AttributeNameStyle;

	UPROPERTY(EditAnywhere, Category = "Obsidian|Properties")
	TSubclassOf<UCommonTextStyle> AttributeValueStyle;
	
	UPROPERTY(EditAnywhere, Category = "Obsidian|Properties")
	float SizeBoxWidth = 0.f;

	UPROPERTY(EditAnywhere, Category = "Obsidian|Properties")
	float SizeBoxHeight = 0.f;

	/** Spacer is places to the right of the text */
	UPROPERTY(EditAnywhere, Category = "Obsidian|Properties")
	float NameAndValueSpacing = 0.f;

	UPROPERTY(EditAnywhere, Category = "Obsidian|Properties")
	FText AttributeName = {};

	/**
	 * Cashed attributes
	 */
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Obsidian")
	float CurrentAttributeValue = 0.f;

	UPROPERTY(VisibleDefaultsOnly, Category = "Obsidian")
	float CurrentMaxAttributeValue = 0.f;
};
