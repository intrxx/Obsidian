// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "Obsidian/ObsidianGameplayTags.h"

namespace ObsidianDefaultStackCounts 
{
	static constexpr int32 DefaultStackCount = 1;
	static constexpr int32 CurrentStackCount = 1;
	static constexpr int32 MaxStackCount = 1;
	static constexpr int32 TotalCount = 0;

	/** Returns the unified project default for given Stack Tag. */
	inline int32 GetUnifiedDefaultForTag(const FGameplayTag Tag)
	{
		if(Tag == ObsidianGameplayTags::Item_StackCount_Default)
		{
			return DefaultStackCount;
		}
		if(Tag == ObsidianGameplayTags::Item_StackCount_Current)
		{
			return CurrentStackCount;
		}
		if(Tag == ObsidianGameplayTags::Item_StackCount_Max)
		{
			return MaxStackCount;
		}
		if(Tag == ObsidianGameplayTags::Item_TotalCount_Max)
		{
			return TotalCount;
		}
		return 0;
	}
}