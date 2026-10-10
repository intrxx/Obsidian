// Copyright 2026 out of sCope team - intrxx

#include "UI/DamageNumbers/ObsidianDamageNumber.h"

#include "CommonTextBlock.h"

#include "ObsidianTypes/ObsidianUITypes.h"


void UObsidianDamageNumber::InitializeDamageNumber(const FObsidianDamageTextProps& InDamageTextProps)
{
	if(InDamageTextProps.bIsBlockedAttack)
	{
		SetDamageNumber(BlockedDamageNumber_Style, BlockedText);
		return;
	}

	if(InDamageTextProps.bIsEvadedHit)
	{
		SetDamageNumber(EvadedDamageNumber_Style, EvadedText);
		return;
	}

	if(InDamageTextProps.bIsTargetImmune)
	{
		SetDamageNumber(ImmuneDamageNumber_Style, ImmuneText);
		return;
	}

	const FText DamageNumberText = FText::AsNumber(FMath::FloorToInt(InDamageTextProps.DamageMagnitude));
	
	if(InDamageTextProps.bIsSuppressedSpell)
	{
		SetDamageNumber(SuppressedDamageNumber_Style, DamageNumberText);
		return;
	}

	if(InDamageTextProps.bIsCriticalAttack)
	{
		SetDamageNumber(CriticalDamageNumber_Style, DamageNumberText);
		return;
	}
	
	SetDamageNumber(RegularDamageNumber_Style, DamageNumberText);
}

void UObsidianDamageNumber::SetDamageNumber(const TSubclassOf<UCommonTextStyle>& InStyle, const FText& InText) const
{
	if(DamageNumber_TextBlock)
	{
		DamageNumber_TextBlock->SetStyle(InStyle);
		DamageNumber_TextBlock->SetText(InText);
	}
}
