#pragma once

#include "NativeGameplayTags.h"

namespace ObsidianGameplayTags
{
	OBSIDIAN_API FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);
	
	OBSIDIAN_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Move);
}


