// Copyright 2026 out of sCope team - intrxx

#include "UI/DamageNumbers/ObsidianDamageNumber.h"

// ~ Core
#include "CommonTextBlock.h"

// ~ Project
#include "ObsidianTypes/ObsidianUITypes.h"

void UObsidianDamageNumber::InitializeDamageNumber(const FObsidianDamageTextProps& DamageTextProps)
{
	if(DamageTextProps.bIsBlockedAttack)
	{
		SetDamageNumber(BlockedDamageNumber_Style, BlockedText);
		return;
	}

	if(DamageTextProps.bIsEvadedHit)
	{
		SetDamageNumber(EvadedDamageNumber_Style, EvadedText);
		return;
	}

	if(DamageTextProps.bIsTargetImmune)
	{
		SetDamageNumber(ImmuneDamageNumber_Style, ImmuneText);
		return;
	}

	const FText DamageNumberText = FText::AsNumber(FMath::FloorToInt(DamageTextProps.DamageMagnitude));
	
	if(DamageTextProps.bIsSuppressedSpell)
	{
		SetDamageNumber(SuppressedDamageNumber_Style, DamageNumberText);
		return;
	}

	if(DamageTextProps.bIsCriticalAttack)
	{
		SetDamageNumber(CriticalDamageNumber_Style, DamageNumberText);
		return;
	}
	
	SetDamageNumber(RegularDamageNumber_Style, DamageNumberText);
}

void UObsidianDamageNumber::SetDamageNumber(const TSubclassOf<UCommonTextStyle>& Style, const FText& Text) const
{
	if(DamageNumber_TextBlock)
	{
		DamageNumber_TextBlock->SetStyle(Style);
		DamageNumber_TextBlock->SetText(Text);
	}
}
