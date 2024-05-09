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

void UOCharacterStatusAttributeRow::SetAttributeValue(const float Value) const
{
	const FText TextValue = FText::FromString(FString::Printf(TEXT("%f"), Value));
	AttributeValue_TextBlock->SetText(TextValue);
}
