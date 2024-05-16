// Copyright 2024 out of sCope team - Michał Ogiński


#include "Input/ObsidianInputConfig.h"

#include "Obsidian/Obsidian.h"

UObsidianInputConfig::UObsidianInputConfig(const FObjectInitializer& ObjectInitializer)
{
}

const UInputAction* UObsidianInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for(const FObsidianInputAction& ObsidianInputAction : NativeInputActions)
	{
		if(ObsidianInputAction.InputAction && (ObsidianInputAction.InputTag == InputTag))
		{
			return ObsidianInputAction.InputAction;
		}
	}

	if(bLogNotFound)
	{
		UE_LOG(LogObsidian, Display, TEXT("Could not find NativeInputAction for [%s] on ObsidianInputConfig [%s]"),
			*InputTag.ToString(), *GetNameSafe(this));
	}
	
	return nullptr;
}

const UInputAction* UObsidianInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for(const FObsidianInputAction& ObsidianInputAction : AbilityInputActions)
	{
		if(ObsidianInputAction.InputAction && (ObsidianInputAction.InputTag == InputTag))
		{
			return ObsidianInputAction.InputAction;
		}
	}

	if(bLogNotFound)
	{
		UE_LOG(LogObsidian, Display, TEXT("Could not find AbilityInputAction for [%s] on ObsidianInputConfig [%s]"),
			*InputTag.ToString(), *GetNameSafe(this));
	}
	
	return nullptr;
}


