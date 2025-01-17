// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "ObsidianItemTypes.generated.h"

class UObsidianInventoryItemInstance;

namespace ObsidianDefaultStackCounts 
{
	static constexpr int32 CurrentStackCount = 1;
	static constexpr int32 MaxStackCount = 1;
	static constexpr int32 LimitCount = 0;

	/** Returns the unified project default for given Stack Tag. */
	inline int32 GetUnifiedDefaultForTag(const FGameplayTag Tag)
	{
		if(Tag == ObsidianGameplayTags::Item_StackCount_Current)
		{
			return CurrentStackCount;
		}
		if(Tag == ObsidianGameplayTags::Item_StackCount_Max)
		{
			return MaxStackCount;
		}
		if(Tag == ObsidianGameplayTags::Item_StackCount_Limit)
		{
			return LimitCount;
		}
		return 0;
	}
}

USTRUCT()
struct FObsidianAddingStacksResult
{
	GENERATED_BODY()

public:
	/** The amount of stacks that was added from provided Item to other Item/Items. */
	UPROPERTY()
	int32 AddedStacks = 0;

	/** The amount of stacks that is left on the provided Item. */
	UPROPERTY()
	int32 StacksLeft = -1;
	
	/** Whole Item was added as stacks to other Item/Items. */
	UPROPERTY()
	bool bAddedWholeItemAsStacks = false;

	/** Added at least 1 stack from provided item to some other item. */
	UPROPERTY()
	bool bAddedSomeOfTheStacks = false;

	/** The last Item Instance that we added some stacks to. */
	UPROPERTY()
	UObsidianInventoryItemInstance* LastAddedToInstance = nullptr;
};


