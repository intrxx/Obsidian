// Copyright 2024 out of sCope team - Michał Ogiński


#include "Core/ObsidianGameplayStatics.h"

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
