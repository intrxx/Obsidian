// Copyright 2024 out of sCope team - Michał Ogiński

#include "Core/ObsidianGameplayStatics.h"

// ~ Core

// ~ Project
#include "Obsidian/ObsidianGameplayTags.h"

FText UObsidianGameplayStatics::GetHeroClassText(const EObsidianHeroClass HeroClass)
{
	if(HeroClass == EObsidianHeroClass::OHC_Witch)
	{
		return FText::FromString(TEXT("Witch"));
	}
	if(HeroClass == EObsidianHeroClass::OHC_Barbarian)
	{
		return FText::FromString(TEXT("Barbarian"));
	}
	if(HeroClass == EObsidianHeroClass::OHC_Assassin)
	{
		return FText::FromString(TEXT("Assassin"));
	}
	if(HeroClass == EObsidianHeroClass::OHC_Paladin)
	{
		return FText::FromString(TEXT("Paladin"));
	}
	return FText::FromString(TEXT("Error - None!"));
}

bool UObsidianGameplayStatics::DoesTagMatchesAnySubTag(const FGameplayTag TagToCheck, const FGameplayTag& SubTagToCheck)
{
	return TagToCheck.ToString().Contains(SubTagToCheck.ToString());
}

FGameplayTag UObsidianGameplayStatics::GetOpposedEuipmentTagForTag(const FGameplayTag MainTag)
{
	if(MainTag == ObsidianGameplayTags::Equipment_Slot_Weapon_LeftHand)
	{
		return ObsidianGameplayTags::Equipment_SwapSlot_Weapon_LeftHand;
	}
	if(MainTag == ObsidianGameplayTags::Equipment_Slot_Weapon_RightHand)
	{
		return ObsidianGameplayTags::Equipment_SwapSlot_Weapon_RightHand;
	}
	if(MainTag == ObsidianGameplayTags::Equipment_SwapSlot_Weapon_LeftHand)
	{
		return ObsidianGameplayTags::Equipment_Slot_Weapon_LeftHand;
	}
	if(MainTag == ObsidianGameplayTags::Equipment_SwapSlot_Weapon_RightHand)
	{
		return ObsidianGameplayTags::Equipment_Slot_Weapon_RightHand;
	}
	
	return FGameplayTag::EmptyTag;
}
