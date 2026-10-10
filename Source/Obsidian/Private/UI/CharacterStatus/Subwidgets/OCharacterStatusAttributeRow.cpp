// Copyright 2026 out of sCope team - intrxx

#include "UI/CharacterStatus/Subwidgets/OCharacterStatusAttributeRow.h"

#include "CommonTextBlock.h"
#include "Components/SizeBox.h"
#include "Components/Spacer.h"


void UOCharacterStatusAttributeRow::NativePreConstruct()
{
	Super::NativePreConstruct();

	InitialSetup();
}

void UOCharacterStatusAttributeRow::InitialSetup() const
{
	Root_SizeBox->SetWidthOverride(SizeBoxWidth);
	Root_SizeBox->SetHeightOverride(SizeBoxHeight);
	AttributeName_TextBlock->SetStyle(AttributeNameStyle);
	AttributeName_TextBlock->SetText(AttributeName);
	AttributeValue_TextBlock->SetStyle(AttributeValueStyle);
	NameAndValue_Spacer->SetSize(FVector2D(NameAndValueSpacing, 1.f));
}

void UOCharacterStatusAttributeRow::SetAttributeValue(const float InValue) const
{
	if(AttributeValue_TextBlock)
	{
		AttributeValue_TextBlock->SetText(FText::AsNumber(FMath::FloorToInt(InValue)));
	}
}

void UOCharacterStatusAttributeRow::SetAttributeValueWithPercentage(const float InValue) const
{
	const FText TextValue = FText::FromString(FString::Printf(TEXT("%d%%"), FMath::FloorToInt(InValue)));
	if(AttributeValue_TextBlock)
	{
		AttributeValue_TextBlock->SetText(TextValue);
	}
}

void UOCharacterStatusAttributeRow::SetTwoAttributeValuesWithPercent(const float InValue, const float InMaxValue) const
{
	const FText TextValue = FText::FromString(FString::Printf(TEXT("%d%% (%d%%)"),
		FMath::FloorToInt(InValue), FMath::FloorToInt(InMaxValue)));
	
	if(AttributeValue_TextBlock)
	{
		AttributeValue_TextBlock->SetText(TextValue);
	}
}

