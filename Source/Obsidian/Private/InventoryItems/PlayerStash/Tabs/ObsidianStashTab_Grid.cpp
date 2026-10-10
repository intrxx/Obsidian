// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/PlayerStash/Tabs/ObsidianStashTab_Grid.h"

#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"


UObsidianStashTab_Grid::UObsidianStashTab_Grid(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
}

UObsidianInventoryItemInstance* UObsidianStashTab_Grid::GetInstanceAtPosition(const FObsidianItemPosition& InItemPosition)
{
	return GridLocationToItemMap.FindRef(InItemPosition.GetItemGridPosition());
}

bool UObsidianStashTab_Grid::DebugVerifyPositionFree(const FObsidianItemPosition& InPosition)
{
	return !GridLocationToItemMap.Contains(InPosition.GetItemGridPosition());
}

bool UObsidianStashTab_Grid::CanPlaceItemAtSpecificPosition(const FObsidianItemPosition& InSpecifiedPosition, const FGameplayTag& InItemCategory, const FGameplayTag& InItemBaseType, const FIntPoint& InItemGridSpan)
{
	const FIntPoint SpecificGridPosition = InSpecifiedPosition.GetItemGridPosition();
	
	bool bCanFit = false;
	const bool* InitialPositionFreePtr = GridStateMap.Find(SpecificGridPosition);
	if(InitialPositionFreePtr && *InitialPositionFreePtr == false) // Initial location is free
	{
		bCanFit = true;
		for(int32 SpanX = 0; SpanX < InItemGridSpan.X; ++SpanX)
		{
			for(int32 SpanY = 0; SpanY < InItemGridSpan.Y; ++SpanY)
			{
				const FIntPoint LocationToCheck = SpecificGridPosition + FIntPoint(SpanX, SpanY);
				const bool* ExistingOccupiedPtr = GridStateMap.Find(LocationToCheck);
				if(ExistingOccupiedPtr == nullptr || *ExistingOccupiedPtr)
				{
					bCanFit = false;
					break;
				}
			}
		}
	}
	return bCanFit;
}

bool UObsidianStashTab_Grid::FindFirstAvailablePositionForItem(FObsidianItemPosition& OutFirstAvailablePosition, const FGameplayTag& InItemCategory, const FGameplayTag& InItemBaseType, const FIntPoint& InItemGridSpan)
{
	bool bCanFit = false;
	
	for(const TTuple<FIntPoint, bool>& Location : GridStateMap)
	{
		if(Location.Value == false) // Location is free
		{
			bCanFit = true;
			
			for(int32 SpanX = 0; SpanX < InItemGridSpan.X; ++SpanX)
			{
				for(int32 SpanY = 0; SpanY < InItemGridSpan.Y; ++SpanY)
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
				OutFirstAvailablePosition = FObsidianItemPosition(Location.Key, StashTabTag);
				return bCanFit;
			}
		}
	}

	return bCanFit;
}

bool UObsidianStashTab_Grid::CanReplaceItemAtSpecificPosition(const FObsidianItemPosition& InSpecifiedPosition, const UObsidianInventoryItemInstance* InReplacingInstance)
{
	if (InReplacingInstance == nullptr)
	{
		return false;
	}

	return CheckReplacementPossible(InSpecifiedPosition, InReplacingInstance->GetItemGridSpan());
}

bool UObsidianStashTab_Grid::CanReplaceItemAtSpecificPosition(const FObsidianItemPosition& InSpecifiedPosition, const TSubclassOf<UObsidianInventoryItemDefinition>& InReplacingDef)
{
	if (InReplacingDef == nullptr)
	{
		return false;
	}

	if (const UObsidianInventoryItemDefinition* DefaultItem = InReplacingDef.GetDefaultObject())
	{
		if (const UOInventoryItemFragment_Appearance* Appearance = Cast<UOInventoryItemFragment_Appearance>(DefaultItem->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
		{
			return CheckReplacementPossible(InSpecifiedPosition, Appearance->GetItemGridSpanFromDesc());
		}
	}
	
	return false;
}

bool UObsidianStashTab_Grid::CheckReplacementPossible(const FObsidianItemPosition& InSpecifiedPosition, const FIntPoint& InReplacingItemGridSpan) const
{
	UObsidianInventoryItemInstance* InstanceAtGrid = GridLocationToItemMap.FindRef(InSpecifiedPosition.GetItemGridPosition());
	if (InstanceAtGrid == nullptr )
	{
		return false; 
	}

	const FIntPoint ItemOrigin = InSpecifiedPosition.GetItemGridPosition();
	const FIntPoint ItemGridSpan = InstanceAtGrid->GetItemGridSpan();
	TMap<FIntPoint, bool> TempInventoryStateMap = GridStateMap;
	
	for(int32 SpanX = 0; SpanX < ItemGridSpan.X; ++SpanX)
	{
		for(int32 SpanY = 0; SpanY < ItemGridSpan.Y; ++SpanY)
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
	
	const bool* InitialPositionFreePtr = TempInventoryStateMap.Find(ItemOrigin);
	if(InitialPositionFreePtr && *InitialPositionFreePtr == false) // Initial location is free
	{
		bCanReplace = true;
		for(int32 SpanX = 0; SpanX < InReplacingItemGridSpan.X; ++SpanX)
		{
			for(int32 SpanY = 0; SpanY < InReplacingItemGridSpan.Y; ++SpanY)
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

void UObsidianStashTab_Grid::MarkSpaceInTab(UObsidianInventoryItemInstance* InItemInstance, const FObsidianItemPosition& InAtPosition)
{
	const FIntPoint ItemGridSpan = InItemInstance->GetItemGridSpan();
	for(int32 SpanX = 0; SpanX < ItemGridSpan.X; ++SpanX)
	{
		for(int32 SpanY = 0; SpanY < ItemGridSpan.Y; ++SpanY)
		{
			const FIntPoint LocationToMark = InAtPosition.GetItemGridPosition() + FIntPoint(SpanX, SpanY);
			if(bool* Location = GridStateMap.Find(LocationToMark))
			{
				*Location = true;
			}
#if !UE_BUILD_SHIPPING
			else
			{
				FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Trying to Mark a Location [x: %d, y: %d] that doesn't"
				 "exist in the InventoryStateMap in [%hs]."), LocationToMark.X, LocationToMark.Y, __FUNCTION__), ELogVerbosity::Error);
			}
#endif
		}
	}

	GridLocationToItemMap.Add(InAtPosition.GetItemGridPosition(), InItemInstance);
}

void UObsidianStashTab_Grid::UnmarkSpaceInTab(UObsidianInventoryItemInstance* InItemInstance, const FObsidianItemPosition& InAtPosition)
{
	const FIntPoint ItemGridSpan = InItemInstance->GetItemGridSpan();
	for(int32 SpanX = 0; SpanX < ItemGridSpan.X; ++SpanX)
	{
		for(int32 SpanY = 0; SpanY < ItemGridSpan.Y; ++SpanY)
		{
			const FIntPoint LocationToUnmark = InAtPosition.GetItemGridPosition() + FIntPoint(SpanX, SpanY);
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

	GridLocationToItemMap.Remove(InAtPosition.GetItemGridPosition());
}

void UObsidianStashTab_Grid::Construct(UObsidianPlayerStashComponent* InStashComponent)
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
	
	//TODO(intrxx) Get already added items, mark space
}

int32 UObsidianStashTab_Grid::GetGridWidth() const
{
	return GridWidth;
}

int32 UObsidianStashTab_Grid::GetGridHeight() const
{
	return GridHeight;
}

