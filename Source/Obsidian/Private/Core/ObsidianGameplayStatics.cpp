// Copyright 2026 out of sCope team - intrxx

#include "Core/ObsidianGameplayStatics.h"

#include "Game/ObsidianGameMode.h"
#include "Obsidian/ObsidianGameplayTags.h"


FText UObsidianGameplayStatics::GetHeroClassText(const EObsidianHeroClass HeroClass)
{
	if(HeroClass == EObsidianHeroClass::Witch)
	{
		return FText::FromString(TEXT("Witch"));
	}
	if(HeroClass == EObsidianHeroClass::Barbarian)
	{
		return FText::FromString(TEXT("Barbarian"));
	}
	if(HeroClass == EObsidianHeroClass::Assassin)
	{
		return FText::FromString(TEXT("Assassin"));
	}
	if(HeroClass == EObsidianHeroClass::Paladin)
	{
		return FText::FromString(TEXT("Paladin"));
	}
	return FText::FromString(TEXT("Error - None!"));
}

bool UObsidianGameplayStatics::DoesTagMatchesAnySubTag(const FGameplayTag TagToCheck, const FGameplayTag& SubTagToCheck)
{
	return TagToCheck.ToString().Contains(SubTagToCheck.ToString());
}

FGameplayTag UObsidianGameplayStatics::GetOpposedEquipmentTagForTag(const FGameplayTag MainTag)
{
	if(MainTag == ObsidianGameplayTags::Item::Slot::Equipment::Weapon::LeftHand)
	{
		return ObsidianGameplayTags::Item::SwapSlot::Equipment::Weapon::LeftHand;
	}
	if(MainTag == ObsidianGameplayTags::Item::Slot::Equipment::Weapon::RightHand)
	{
		return ObsidianGameplayTags::Item::SwapSlot::Equipment::Weapon::RightHand;
	}
	if(MainTag == ObsidianGameplayTags::Item::SwapSlot::Equipment::Weapon::LeftHand)
	{
		return ObsidianGameplayTags::Item::Slot::Equipment::Weapon::LeftHand;
	}
	if(MainTag == ObsidianGameplayTags::Item::SwapSlot::Equipment::Weapon::RightHand)
	{
		return ObsidianGameplayTags::Item::Slot::Equipment::Weapon::RightHand;
	}
	
	return FGameplayTag::EmptyTag;
}

EObsidianGameNetworkType UObsidianGameplayStatics::GetCurrentNetworkType(const UObject* WorldContextObject)
{
	if (const AObsidianGameMode* ObsidianGameMode = Cast<AObsidianGameMode>(GetGameMode(WorldContextObject)))
	{
		return ObsidianGameMode->GetCurrentNetworkType();
	}
	return EObsidianGameNetworkType::None;
}

bool UObsidianGameplayStatics::IsOfflineNetworkType(const EObsidianGameNetworkType NetworkType)
{
	if (NetworkType == EObsidianGameNetworkType::None)
	{
		return false;
	}
	
	return NetworkType >= EObsidianGameNetworkType::OfflineSolo;
}
