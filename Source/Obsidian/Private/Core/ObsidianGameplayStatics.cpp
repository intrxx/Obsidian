// Copyright 2026 out of sCope team - intrxx

#include "Core/ObsidianGameplayStatics.h"

#include "Game/ObsidianGameMode.h"
#include "Obsidian/ObsidianGameplayTags.h"


FText UObsidianGameplayStatics::GetHeroClassText(const EObsidianHeroClass InHeroClass)
{
	if(InHeroClass == EObsidianHeroClass::Witch)
	{
		return FText::FromString(TEXT("Witch"));
	}
	if(InHeroClass == EObsidianHeroClass::Barbarian)
	{
		return FText::FromString(TEXT("Barbarian"));
	}
	if(InHeroClass == EObsidianHeroClass::Assassin)
	{
		return FText::FromString(TEXT("Assassin"));
	}
	if(InHeroClass == EObsidianHeroClass::Paladin)
	{
		return FText::FromString(TEXT("Paladin"));
	}
	return FText::FromString(TEXT("Error - None!"));
}

bool UObsidianGameplayStatics::DoesTagMatchesAnySubTag(const FGameplayTag InTagToCheck, const FGameplayTag& InSubTagToCheck)
{
	return InTagToCheck.ToString().Contains(InSubTagToCheck.ToString());
}

FGameplayTag UObsidianGameplayStatics::GetOpposedEquipmentTagForTag(const FGameplayTag InMainTag)
{
	if(InMainTag == ObsidianGameplayTags::Item::Slot::Equipment::Weapon::LeftHand)
	{
		return ObsidianGameplayTags::Item::SwapSlot::Equipment::Weapon::LeftHand;
	}
	if(InMainTag == ObsidianGameplayTags::Item::Slot::Equipment::Weapon::RightHand)
	{
		return ObsidianGameplayTags::Item::SwapSlot::Equipment::Weapon::RightHand;
	}
	if(InMainTag == ObsidianGameplayTags::Item::SwapSlot::Equipment::Weapon::LeftHand)
	{
		return ObsidianGameplayTags::Item::Slot::Equipment::Weapon::LeftHand;
	}
	if(InMainTag == ObsidianGameplayTags::Item::SwapSlot::Equipment::Weapon::RightHand)
	{
		return ObsidianGameplayTags::Item::Slot::Equipment::Weapon::RightHand;
	}
	
	return FGameplayTag::EmptyTag;
}

EObsidianGameNetworkType UObsidianGameplayStatics::GetCurrentNetworkType(const UObject* InWorldContextObject)
{
	if (const AObsidianGameMode* ObsidianGameMode = Cast<AObsidianGameMode>(GetGameMode(InWorldContextObject)))
	{
		return ObsidianGameMode->GetCurrentNetworkType();
	}
	return EObsidianGameNetworkType::None;
}

bool UObsidianGameplayStatics::IsOfflineNetworkType(const EObsidianGameNetworkType InNetworkType)
{
	if (InNetworkType == EObsidianGameNetworkType::None)
	{
		return false;
	}
	
	return InNetworkType >= EObsidianGameNetworkType::OfflineSolo;
}
