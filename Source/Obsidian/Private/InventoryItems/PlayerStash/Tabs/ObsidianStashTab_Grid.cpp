// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/PlayerStash/Tabs/ObsidianStashTab_Grid.h"

// ~ Core

// ~ Project
#include "InventoryItems/ObsidianInventoryItemInstance.h"

UObsidianStashTab_Grid::UObsidianStashTab_Grid(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
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
