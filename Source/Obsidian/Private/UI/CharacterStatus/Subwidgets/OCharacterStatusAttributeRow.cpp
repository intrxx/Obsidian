// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/CharacterStatus/Subwidgets/OCharacterStatusAttributeRow.h"
#include "CommonTextBlock.h"
#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Components/Spacer.h"

void UOCharacterStatusAttributeRow::NativePreConstruct()
{
	Super::NativePreConstruct();

	InitialSetup();
}

void UOCharacterStatusAttributeRow::NativeConstruct()
{
	Super::NativeConstruct();

	if(Tooltip_Button)
	{
		Tooltip_Button->OnHovered.AddDynamic(this, &ThisClass::OnToolTipButtonHovered);
		Tooltip_Button->OnUnhovered.AddDynamic(this, &ThisClass::OnToolTipButtonUnHovered);
	}
}

void UOCharacterStatusAttributeRow::BeginDestroy()
{
	if(Tooltip_Button)
	{
		Tooltip_Button->OnHovered.Clear();
		Tooltip_Button->OnUnhovered.Clear();
	}
	
	Super::BeginDestroy();
}

void UOCharacterStatusAttributeRow::InitialSetup()
{
	Root_SizeBox->SetWidthOverride(SizeBoxWidth);
	Root_SizeBox->SetHeightOverride(SizeBoxHeight);
	AttributeName_TextBlock->SetStyle(AttributeNameStyle);
	AttributeName_TextBlock->SetText(AttributeName);
	AttributeValue_TextBlock->SetStyle(AttributeValueStyle);
	NameAndValue_Spacer->SetSize(FVector2D(NameAndValueSpacing, 1.f));
	
	if(!bToolTipButtonEnabled)
	{
		/**
		* This does not actually disable the button, only makes it un-testable for hits
		* The reason is that there is a bug or design flaw in Unreal Engine that automatically greys out every child of disabled buttons.
		*/
		
		//Tooltip_Button->SetIsEnabled(false);
		Tooltip_Button->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		//Tooltip_Button->SetIsEnabled(true);
		Tooltip_Button->SetVisibility(ESlateVisibility::Visible);
	}
}

void UOCharacterStatusAttributeRow::OnToolTipButtonHovered()
{
}

void UOCharacterStatusAttributeRow::OnToolTipButtonUnHovered()
{
}

void UOCharacterStatusAttributeRow::SetAttributeValue(const float& Value)
{
	CurrentAttributeValue = Value;
	
	const FText TextValue = FText::FromString(FString::Printf(TEXT("%d"), FMath::FloorToInt(CurrentAttributeValue)));
	if(AttributeValue_TextBlock)
	{
		AttributeValue_TextBlock->SetText(TextValue);
	}
}

void UOCharacterStatusAttributeRow::SetAttributeValueWithPercentage(const float& Value)
{
	CurrentAttributeValue = Value;
	
	const FText TextValue = FText::FromString(FString::Printf(TEXT("%d%%"), FMath::FloorToInt(CurrentAttributeValue)));
	if(AttributeValue_TextBlock)
	{
		AttributeValue_TextBlock->SetText(TextValue);
	}
}

void UOCharacterStatusAttributeRow::UpdateAttributeValueWithPercentage(const float& Value) const
{
	const float TempValue = CurrentAttributeValue + Value;

	const FText TextValue = FText::FromString(FString::Printf(TEXT("%d%%"), FMath::FloorToInt(TempValue)));
	if(AttributeValue_TextBlock)
	{
		AttributeValue_TextBlock->SetText(TextValue);
	}
}

void UOCharacterStatusAttributeRow::SetTwoAttributeValuesWithPercent(const float& Value, const float& MaxValue)
{
	CurrentAttributeValue = Value;
	CurrentMaxAttributeValue = MaxValue;
	
	const FText TextValue = FText::FromString(FString::Printf(TEXT("%d%% (%d%%)"),
		FMath::FloorToInt(CurrentAttributeValue), FMath::FloorToInt(CurrentMaxAttributeValue)));
	
	if(AttributeValue_TextBlock)
	{
		AttributeValue_TextBlock->SetText(TextValue);
	}
}

void UOCharacterStatusAttributeRow::UpdateResistanceAttributeValue(const float& Value, const float& MaxValue) const
{
	const float TempValue = CurrentAttributeValue + Value;
	const float TempMaxValue = CurrentMaxAttributeValue + MaxValue;
	
	const FText TextValue = FText::FromString(FString::Printf(TEXT("%d%% (%d%%)"),
		FMath::FloorToInt(TempValue), FMath::FloorToInt(TempMaxValue)));
	
	if(AttributeValue_TextBlock)
	{
		AttributeValue_TextBlock->SetText(TextValue);
	}
}

