// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/PlayerStash/Tabs/ObsidianStashTab_Grid.h"

// ~ Core

// ~ Project
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"

UObsidianStashTab_Grid::UObsidianStashTab_Grid(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UObsidianInventoryItemInstance* UObsidianStashTab_Grid::GetInstanceAtPosition(const FObsidianItemPosition& ItemPosition)
{
	return GridLocationToItemMap.FindRef(ItemPosition.GetItemGridLocation());
}

bool UObsidianStashTab_Grid::DebugVerifyPositionFree(const FObsidianItemPosition& Position)
{
	return !GridLocationToItemMap.Contains(Position.GetItemGridLocation());
}

bool UObsidianStashTab_Grid::CanPlaceItemAtSpecificPosition(const FObsidianItemPosition& SpecifiedPosition, const FGameplayTag& ItemCategory, const FIntPoint& ItemGridSpan)
{
	const FIntPoint SpecificGridPosition = SpecifiedPosition.GetItemGridLocation();
	
	bool bCanFit = false;
	if(GridStateMap[SpecificGridPosition] == false) // Initial location is free
	{
		bCanFit = true;
		for(int32 SpanX = 0; SpanX < ItemGridSpan.X; ++SpanX)
		{
			for(int32 SpanY = 0; SpanY < ItemGridSpan.Y; ++SpanY)
			{
				const FIntPoint LocationToCheck = SpecificGridPosition + FIntPoint(SpanX, SpanY);
				const bool* bExistingOccupied = GridStateMap.Find(LocationToCheck);
				if(bExistingOccupied == nullptr || *bExistingOccupied)
				{
					bCanFit = false;
					break;
				}
			}
		}
	}
	return bCanFit;
}

bool UObsidianStashTab_Grid::FindFirstAvailablePositionForItem(FObsidianItemPosition& OutFirstAvailablePosition, const FGameplayTag& ItemCategory, const FIntPoint& ItemGridSpan)
{
	bool bCanFit = false;
	
	for(const TTuple<FIntPoint, bool>& Location : GridStateMap)
	{
		if(Location.Value == false) // Location is free
		{
			bCanFit = true;
			
			for(int32 SpanX = 0; SpanX < ItemGridSpan.X; ++SpanX)
			{
				for(int32 SpanY = 0; SpanY < ItemGridSpan.Y; ++SpanY)
				{
					const FIntPoint LocationToCheck = Location.Key + FIntPoint(SpanX, SpanY);
					const bool* bExistingOccupied = GridStateMap.Find(LocationToCheck);
					if(bExistingOccupied == nullptr || *bExistingOccupied)
					{
						bCanFit = false;
						break;
					}
				}
			}
			
			if(bCanFit) // Return if we get Available Position
			{
				OutFirstAvailablePosition = Location.Key;
				return bCanFit;
			}
		}
	}

	return bCanFit;
}

bool UObsidianStashTab_Grid::CanReplaceItemAtSpecificPosition(const FObsidianItemPosition& SpecifiedPosition, const UObsidianInventoryItemInstance* ReplacingInstance)
{
	if (ReplacingInstance == nullptr)
	{
		return false;
	}

	return CheckReplacementPossible(SpecifiedPosition, ReplacingInstance->GetItemGridSpan());
}

bool UObsidianStashTab_Grid::CanReplaceItemAtSpecificPosition(const FObsidianItemPosition& SpecifiedPosition, const TSubclassOf<UObsidianInventoryItemDefinition>& ReplacingDef)
{
	if (ReplacingDef == nullptr)
	{
		return false;
	}

	if (const UObsidianInventoryItemDefinition* DefaultItem = ReplacingDef.GetDefaultObject())
	{
		if (const UOInventoryItemFragment_Appearance* Appearance = Cast<UOInventoryItemFragment_Appearance>(DefaultItem->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
		{
			return CheckReplacementPossible(SpecifiedPosition, Appearance->GetItemGridSpanFromDesc());
		}
	}
	
	return false;
}

bool UObsidianStashTab_Grid::CheckReplacementPossible(const FObsidianItemPosition& SpecifiedPosition, const FIntPoint& ReplacingItemGridSpan) const
{
	UObsidianInventoryItemInstance* InstanceAtGrid = GridLocationToItemMap.FindRef(SpecifiedPosition.GetItemGridLocation());
	if (InstanceAtGrid == nullptr )
	{
		return false; 
	}

	const FIntPoint ItemOrigin = SpecifiedPosition.GetItemGridLocation();
	TMap<FIntPoint, bool> TempInventoryStateMap = GridStateMap;
	
	for(int32 SpanX = 0; SpanX < ReplacingItemGridSpan.X; ++SpanX)
	{
		for(int32 SpanY = 0; SpanY < ReplacingItemGridSpan.Y; ++SpanY)
		{
			const FIntPoint GridSlotToCheck = ItemOrigin + FIntPoint(SpanX, SpanY);
			if(bool* TempLocation = TempInventoryStateMap.Find(GridSlotToCheck))
			{
				*TempLocation = false;
			}
#if !UE_BUILD_SHIPPING
			else
			{
				FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Trying to UnMark a Location [x: %d, y: %d] that doesn't"
				"exist in the TempInventoryStateMap in UObsidianStashTab_Grid::CanReplaceItemAtSpecificPosition."), GridSlotToCheck.X, GridSlotToCheck.Y), ELogVerbosity::Warning);
			}
#endif
		}
	}
	
	bool bCanReplace = false;
	
	if(TempInventoryStateMap[ItemOrigin] == false) // Initial location is free
	{
		bCanReplace = true;
		for(int32 SpanX = 0; SpanX < ReplacingItemGridSpan.X; ++SpanX)
		{
			for(int32 SpanY = 0; SpanY < ReplacingItemGridSpan.Y; ++SpanY)
			{
				const FIntPoint GridSlotToCheck = ItemOrigin + FIntPoint(SpanX, SpanY);
				const bool* bExistingOccupied = TempInventoryStateMap.Find(GridSlotToCheck);
				if(bExistingOccupied == nullptr || *bExistingOccupied)
				{
					bCanReplace = false;
					break;
				}
			}
		}
	}
	return bCanReplace;
}

void UObsidianStashTab_Grid::MarkSpaceInTab(UObsidianInventoryItemInstance* ItemInstance, const FObsidianItemPosition& AtPosition)
{
	const FIntPoint ItemGridSpan = ItemInstance->GetItemGridSpan();
	for(int32 SpanX = 0; SpanX < ItemGridSpan.X; ++SpanX)
	{
		for(int32 SpanY = 0; SpanY < ItemGridSpan.Y; ++SpanY)
		{
			const FIntPoint LocationToMark = AtPosition.GetItemGridLocation() + FIntPoint(SpanX, SpanY);
			if(bool* Location = GridStateMap.Find(LocationToMark))
			{
				*Location = true;
			}
#if !UE_BUILD_SHIPPING
			else
			{
				FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Trying to Mark a Location [x: %d, y: %d] that doesn't"
				 "exist in the InventoryStateMap in [%ls]."), LocationToMark.X, LocationToMark.Y, ANSI_TO_TCHAR(__FUNCTION__)), ELogVerbosity::Error);
			}
#endif
		}
	}

	GridLocationToItemMap.Add(AtPosition.GetItemGridLocation(), ItemInstance);
}

void UObsidianStashTab_Grid::UnmarkSpaceInTab(UObsidianInventoryItemInstance* ItemInstance, const FObsidianItemPosition& AtPosition)
{
	const FIntPoint ItemGridSpan = ItemInstance->GetItemGridSpan();
	for(int32 SpanX = 0; SpanX < ItemGridSpan.X; ++SpanX)
	{
		for(int32 SpanY = 0; SpanY < ItemGridSpan.Y; ++SpanY)
		{
			const FIntPoint LocationToUnmark = AtPosition.GetItemGridLocation() + FIntPoint(SpanX, SpanY);
			if(bool* Location = GridStateMap.Find(LocationToUnmark))
			{
				*Location = false;
			}
#if !UE_BUILD_SHIPPING
			else
			{
				FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Trying to UnMark a Location [x: %d, y: %d] that doesn't"
				"exist in the InventoryStateMap in UObsidianInventoryComponent::Item_UnMarkSpace."), LocationToUnmark.X, LocationToUnmark.Y), ELogVerbosity::Error);
			}
#endif
		}
	}

	GridLocationToItemMap.Remove(AtPosition.GetItemGridLocation());
}

void UObsidianStashTab_Grid::Construct(UObsidianPlayerStashComponent* StashComponent)
{
	int16 GridX = 0;
	int16 GridY = 0;

	const int32 GridSize = GridWidth * GridHeight;
	for(int32 i = 0; i < GridSize; i++)
	{
		GridStateMap.Add(FIntPoint(GridX, GridY), false);
		
		if(GridX == GridWidth - 1)
		{
			GridX = 0;
			GridY++;
		}
		else
		{
			GridX++;
		}
	}
	
	//TODO Get already added items, mark space
}

int32 UObsidianStashTab_Grid::GetGridWidth() const
{
	return GridWidth;
}

int32 UObsidianStashTab_Grid::GetGridHeight() const
{
	return GridHeight;
}

