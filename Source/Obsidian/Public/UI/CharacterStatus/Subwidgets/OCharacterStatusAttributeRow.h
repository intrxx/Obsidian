// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "UI/ObsidianWidgetBase.h"
#include "OCharacterStatusAttributeRow.generated.h"

class UObsidianCharacterStatus;
class UButton;
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

	/** Sets the Attribute Value on the widget and the private attribute variable in OCharacterStatusAttributeRow */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|CharacterStatusWidgetRow")
	void SetAttributeValue(const float Value) const;

	/** Sets the Attribute Value on the widget, adds a "%" to the end and sets the private attribute variable in OCharacterStatusAttributeRow */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|CharacterStatusWidgetRow")
	void SetAttributeValueWithPercentage(const float Value) const;

	/**
	 * Sets both the current Attribute Value and the Max Attribute Value on the widget, adds a "%" to the end and sets the private attribute variables in OCharacterStatusAttributeRow.
	 */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|CharacterStatusWidgetRow")
	void SetTwoAttributeValuesWithPercent(const float Value, const float MaxValue) const;
	
protected:
	virtual void NativePreConstruct() override;
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|CharacterStatusWidgetRow")
	void InitialSetup() const;

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
};
