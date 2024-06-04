// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/CharacterStatus/Subwidgets/OCharacterStatusAttributeRow.h"
#include "CommonTextBlock.h"
#include "Components/SizeBox.h"
#include "Components/Spacer.h"

void UOCharacterStatusAttributeRow::NativePreConstruct()
{
	Super::NativePreConstruct();

	InitialSetup();
}

void UOCharacterStatusAttributeRow::InitialSetup()
{
	Root_SizeBox->SetWidthOverride(SizeBoxWidth);
	Root_SizeBox->SetHeightOverride(SizeBoxHeight);
	AttributeName_TextBlock->SetStyle(AttributeNameStyle);
	AttributeName_TextBlock->SetText(AttributeName);
	AttributeValue_TextBlock->SetStyle(AttributeValueStyle);
	NameAndValue_Spacer->SetSize(FVector2D(NameAndValueSpacing, 1.f));
}

void UOCharacterStatusAttributeRow::SetAttributeValue(const float& Value)
{
	CurrentAttributeValue = Value;
	
	const FText TextValue = FText::FromString(FString::Printf(TEXT("%d"), FMath::FloorToInt(Value)));
	if(AttributeValue_TextBlock)
	{
		AttributeValue_TextBlock->SetText(TextValue);
	}
}

void UOCharacterStatusAttributeRow::SetAttributeValueWithPercentage(const float& Value)
{
	CurrentAttributeValue = Value;
	
	const FText TextValue = FText::FromString(FString::Printf(TEXT("%d%%"), FMath::FloorToInt(Value)));
	if(AttributeValue_TextBlock)
	{
		AttributeValue_TextBlock->SetText(TextValue);
	}
}

void UOCharacterStatusAttributeRow::SetResistanceAttributeValue(const float& Value, const float& MaxValue)
{
	CurrentAttributeValue = Value;
	CurrentMaxAttributeValue = MaxValue;
	
	const FText TextValue = FText::FromString(FString::Printf(TEXT("%d%% (%d%%)"), FMath::FloorToInt(Value), FMath::FloorToInt(MaxValue)));
	if(AttributeValue_TextBlock)
	{
		AttributeValue_TextBlock->SetText(TextValue);
	}
}

void UOCharacterStatusAttributeRow::UpdateResistanceAttributeValue(const float& Value, const float& MaxValue)
{
	CurrentAttributeValue += Value;
	CurrentMaxAttributeValue += MaxValue;
	
	const FText TextValue = FText::FromString(FString::Printf(TEXT("%d%% (%d%%)"),
		FMath::FloorToInt(CurrentAttributeValue), FMath::FloorToInt(CurrentMaxAttributeValue)));
	if(AttributeValue_TextBlock)
	{
		AttributeValue_TextBlock->SetText(TextValue);
	}
}

