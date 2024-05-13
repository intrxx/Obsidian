// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/Data/ObsidianAttributeInfo.h"

FOAttributeInfo UObsidianAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogIfNotFound) const
{
	for(const FOAttributeInfo& Info : AttributeInformation)
	{
		if(Info.AttributeTag.MatchesTagExact(AttributeTag))
		{
			return Info;
		}
	}
	
	if(bLogIfNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find info for given attribute tag [%s] on Attribute Info [%s]"),
			*AttributeTag.GetTagName().ToString(), *GetNameSafe(this));
	}

	return FOAttributeInfo();
}
