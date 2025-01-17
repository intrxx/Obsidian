// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/WidgetControllers/ObsidianInventoryWidgetController.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "CharacterComponents/ObsidianHeroComponent.h"
#include "InventoryItems/ObsidianInventoryComponent.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "UI/Inventory/ObsidianDraggedItem.h"
#include "UI/Inventory/ObsidianItem.h"
#include "UI/Inventory/SubWidgets/ObsidianUnstackSlider.h"

void UObsidianInventoryWidgetController::OnWidgetControllerSetupCompleted()
{
	check(InventoryComponent);
	InternalInventoryComponent = InventoryComponent;
	InventoryComponent->OnItemAddedToInventoryDelegate.AddUObject(this, &ThisClass::OnItemAdded);
	InventoryComponent->OnItemsStacksChangedDelegate.AddUObject(this, &ThisClass::OnItemsStacksChanged);
	InventoryStateMap = InventoryComponent->Internal_GetInventoryStateMap();

	const AActor* OwningActor = Cast<AActor>(PlayerController->GetPawn());
	check(OwningActor);
	
	InternalHeroComponent = UObsidianHeroComponent::FindHeroComponent(OwningActor);
	check(InternalHeroComponent);
}

void UObsidianInventoryWidgetController::OnItemAdded(UObsidianInventoryItemInstance* ItemInstance, const FVector2D DesiredPosition)
{
	check(ItemInstance);
	//bInventoryChanged = true;
	InventoryStateMap = InventoryComponent->Internal_GetInventoryStateMap();
	
	const int32 StackCount = ItemInstance->IsStackable() ? ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current) : 0;
	const FObsidianItemVisuals ItemVisuals = FObsidianItemVisuals
	(
		ItemInstance->GetItemImage(),
		DesiredPosition,
		ItemInstance->GetItemGridSpan(),
		StackCount
	);
	OnItemAddedDelegate.Broadcast(ItemVisuals);
}

void UObsidianInventoryWidgetController::OnItemsStacksChanged(const TMap<FVector2D, int32>& LocationToStacksMap)
{
	for(TTuple<FVector2D, int32> LocationToStack : LocationToStacksMap)
	{
		UObsidianItem* InventoryItem = AddedItemWidgetMap[LocationToStack.Key];
		if(!IsValid(InventoryItem))
		{
			continue;
		}
		InventoryItem->OverrideCurrentStackCount(LocationToStack.Value);
	}
}

void UObsidianInventoryWidgetController::OnInventoryOpen()
{
	//TODO This really needs profiling, for now let it be this way
	/*
	if(bInventoryChanged == false)
	{
		for(const TTuple<FVector2D, UObsidianInventoryItemInstance*>& LocToInstancePair : GridLocationToItemMap)
		{
			OnItemAddedDelegate.Broadcast(LocToInstancePair.Value->GetItemImage(), LocToInstancePair.Key, LocToInstancePair.Value->GetItemGridSpan());
		}
		return;
	}
	*/
	
	GridLocationToItemMap.Empty();
	GridLocationToItemMap = InventoryComponent->Internal_GetLocationToInstanceMap();
	AddedItemWidgetMap.Empty(GridLocationToItemMap.Num());
	
	for(const TTuple<FVector2D, UObsidianInventoryItemInstance*>& LocToInstancePair : GridLocationToItemMap)
	{
		const UObsidianInventoryItemInstance* ItemInstance = LocToInstancePair.Value;
		ensure(ItemInstance);
		
		const int32 StackCount = ItemInstance->IsStackable() ? ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current) : 0;
		const FObsidianItemVisuals ItemVisuals = FObsidianItemVisuals
		(
			ItemInstance->GetItemImage(),
			LocToInstancePair.Key,
			ItemInstance->GetItemGridSpan(),
			StackCount
		);
		OnItemAddedDelegate.Broadcast(ItemVisuals);
	}

	//bInventoryChanged = false;
}

void UObsidianInventoryWidgetController::RequestAddingItemToInventory(const FVector2D& SlotPosition)
{
	if(InternalHeroComponent->IsDraggingAnItem() == false)
	{
		return;
	}
	check(InventoryComponent);
	
	UObsidianDraggedItem* DraggedItem = InternalHeroComponent->GetCurrentlyDraggedItem();
	if(UObsidianInventoryItemInstance* Instance = DraggedItem->GetItemInstance())
	{
		if(InventoryComponent->AddItemInstanceToSpecificSlot(Instance, SlotPosition))
		{
			InternalHeroComponent->StopDragging();
			return;
		}
		DraggedItem->SyncStackCountWithInstance();
	}
	if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem->GetItemDef())
	{
		const int32 ItemStackCount = DraggedItem->GetItemStacks();

		int32 StackLeft = ItemStackCount;
		InventoryComponent->AddItemDefinitionToSpecifiedSlot(ItemDef, SlotPosition, StackLeft, ItemStackCount);
		if(StackLeft == 0)
		{
			InternalHeroComponent->StopDragging();
			return;
		}
		DraggedItem->UpdateStackCount(StackLeft);
	}
}

void UObsidianInventoryWidgetController::HandleLeftClickingOnAnItem(const FVector2D& SlotPosition, UObsidianItem* ItemWidget)
{
	check(InventoryComponent);
	check(DraggedItemWidgetClass);
	check(ItemWidget);

	if(ActiveUnstackSlider != nullptr)
	{
		ActiveUnstackSlider->DestroyUnstackSlider();
		ActiveUnstackSlider = nullptr;
	}
	
	if(InternalHeroComponent->IsDraggingAnItem()) // If we carry an item, try to add it to this item or replace it with it.
	{
		UObsidianDraggedItem* DraggedItem = InternalHeroComponent->GetCurrentlyDraggedItem();
		if(UObsidianInventoryItemInstance* DraggedInstance = DraggedItem->GetItemInstance()) // We carry item instance.
		{
			if(DraggedInstance && DraggedInstance->IsStackable())
			{
				FObsidianAddingStacksResult AddingStacksResult;
				if(InventoryComponent->TryAddingStacksToSpecificSlotWithInstance(DraggedInstance, SlotPosition, /** OUT */ AddingStacksResult))
				{
					ItemWidget->AddCurrentStackCount(AddingStacksResult.AddedStacks);
					if(AddingStacksResult.bAddedWholeItemAsStacks) 
					{
						InternalHeroComponent->StopDragging();
						return;
					}
					DraggedItem->UpdateStackCount(AddingStacksResult.StacksLeft);
					return;
				}
			}
			
			if(InventoryComponent->CanReplaceItemAtSpecificSlotWithInstance(SlotPosition, DraggedInstance))
			{
				InternalHeroComponent->StopDragging();
				PickupItem(SlotPosition);
				InventoryComponent->AddItemInstanceToSpecificSlot(DraggedInstance, SlotPosition);
			}
			return;
		}
		
		if(const TSubclassOf<UObsidianInventoryItemDefinition> DraggedItemDef = DraggedItem->GetItemDef()) // We carry item def
		{
			const int32 ItemStackCount = DraggedItem->GetItemStacks();
			const UObsidianInventoryItemDefinition* DefaultObject = DraggedItemDef.GetDefaultObject();
			if(DefaultObject && DefaultObject->IsStackable())
			{
				FObsidianAddingStacksResult AddingStacksResult;
				if(InventoryComponent->TryAddingStacksToSpecificSlotWithItemDef(DraggedItemDef, ItemStackCount, SlotPosition, /** OUT */ AddingStacksResult))
				{
					ItemWidget->AddCurrentStackCount(AddingStacksResult.AddedStacks);
					if(AddingStacksResult.bAddedWholeItemAsStacks)
					{
						InternalHeroComponent->StopDragging();
						return;
					}
					DraggedItem->UpdateStackCount(AddingStacksResult.StacksLeft);
					return;
				}
			}
			
			if(InternalInventoryComponent->CanReplaceItemAtSpecificSlotWithDef(SlotPosition, DraggedItemDef, ItemStackCount))
			{
				InternalHeroComponent->StopDragging();
				PickupItem(SlotPosition);
				int32 StackLeft = ItemStackCount;
				InventoryComponent->AddItemDefinitionToSpecifiedSlot(DraggedItemDef, SlotPosition, StackLeft, ItemStackCount);
			}
			return;
		}
		return;
	}
	PickupItem(SlotPosition);
}

void UObsidianInventoryWidgetController::HandleLeftClickingOnAnItemWithShiftDown(const FVector2D& SlotPosition, UObsidianItem* ItemWidget)
{
	UObsidianInventoryItemInstance* ItemInstance = InventoryComponent->Internal_GetItemInstanceAtLocation(SlotPosition);
	
	if(InternalHeroComponent->IsDraggingAnItem())
	{
		UObsidianDraggedItem* DraggedItem = InternalHeroComponent->GetCurrentlyDraggedItem();
		check(DraggedItem);

		UObsidianInventoryItemInstance* DraggedInstance = DraggedItem->GetItemInstance();
		if(DraggedInstance && DraggedInstance->IsStackable())
		{
			FObsidianAddingStacksResult AddingStacksResult;
			if(InventoryComponent->TryAddingStacksToSpecificSlotWithInstance(DraggedInstance, SlotPosition, /** OUT */ AddingStacksResult, 1))
			{
				ItemWidget->AddCurrentStackCount(AddingStacksResult.AddedStacks);
				if(AddingStacksResult.bAddedWholeItemAsStacks)
				{
					InternalHeroComponent->StopDragging();
				}
				DraggedItem->UpdateStackCount(AddingStacksResult.StacksLeft);
				return;
			}
			return;
		}
		
		if(const TSubclassOf<UObsidianInventoryItemDefinition> DraggedItemDef = DraggedItem->GetItemDef())
		{
			UObsidianInventoryItemDefinition* DefaultObject = DraggedItemDef.GetDefaultObject();
			if(DefaultObject && DefaultObject->IsStackable())
			{
				const int32 ItemStackCount = DraggedItem->GetItemStacks();
				
				FObsidianAddingStacksResult AddingStacksResult;
				if(InventoryComponent->TryAddingStacksToSpecificSlotWithItemDef(DraggedItemDef, ItemStackCount, SlotPosition, /** OUT */ AddingStacksResult, 1))
				{
					ItemWidget->AddCurrentStackCount(AddingStacksResult.AddedStacks);
					if(AddingStacksResult.bAddedWholeItemAsStacks)
					{
						InternalHeroComponent->StopDragging();
						return;
					}
					DraggedItem->UpdateStackCount(AddingStacksResult.StacksLeft);
				}
			}
			return;
		}
		return;
	}
	
	if(ItemInstance->IsStackable() == false)
	{
		return;
	}
	
	const int32 CurrentItemStacks = ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	if(CurrentItemStacks <= 1)
	{
		return;
	}

	if(ActiveUnstackSlider != nullptr)
	{
		ActiveUnstackSlider->DestroyUnstackSlider();
		ActiveUnstackSlider = nullptr;
	}
	
	ActiveUnstackSlider = CreateWidget<UObsidianUnstackSlider>(PlayerController, UnstackSliderClass);
	ActiveUnstackSlider->InitializeUnstackSlider(CurrentItemStacks);
	const FVector2D SliderSize = ActiveUnstackSlider->GetSizeBoxSize();
	const float TopDesiredOffset = ActiveUnstackSlider->GetTopDesiredOffset();

	FVector2D UnstackSliderViewportPosition = FVector2D::Zero();

	const FGeometry& CachedGeometry = ItemWidget->GetCachedGeometry();
	if(UWorld* World = GetWorld())
	{
		const FVector2D LocalSize = CachedGeometry.GetLocalSize();

		FVector2D PixelPosition = FVector2D::Zero();
		FVector2D ViewportPosition = FVector2D::Zero();
		USlateBlueprintLibrary::LocalToViewport(World, CachedGeometry, FVector2D(0.f,0.f), PixelPosition, ViewportPosition);

		UnstackSliderViewportPosition = FVector2D((ViewportPosition.X - (SliderSize.X / 2)) + (LocalSize.X / 2), (ViewportPosition.Y - SliderSize.Y - TopDesiredOffset));
	}

	/*
	float LocationX = 0.0f;
	float LocationY = 0.0f;
	if(PlayerController->GetMousePosition(LocationX, LocationY))
	{
		UnstackSliderViewportPosition = FVector2D(LocationX, LocationY);
	}
	else
	{
		UE_LOG(LogInventory, Error, TEXT("Failed to extract Viewport Position for UnstackSlider Widget."));
	}
	*/
	
	ActiveUnstackSlider->SetPositionInViewport(UnstackSliderViewportPosition);
	ActiveUnstackSlider->AddToViewport();
	ActiveUnstackSlider->OnAcceptButtonPressedDelegate.AddLambda([this, ItemInstance, SlotPosition, ItemWidget, CurrentItemStacks](const int32 StackToTake)
	{
		HandleTakingOutStacks(ItemInstance, SlotPosition, ItemWidget, CurrentItemStacks, StackToTake);
		ActiveUnstackSlider = nullptr;
	});
}

void UObsidianInventoryWidgetController::PickupItem(const FVector2D& SlotPosition)
{
	UObsidianInventoryItemInstance* ItemInstance = InventoryComponent->Internal_GetItemInstanceAtLocation(SlotPosition);
	RemoveItemWidget(SlotPosition);
	
	UObsidianDraggedItem* DraggedItem = CreateWidget<UObsidianDraggedItem>(PlayerController, DraggedItemWidgetClass);
	DraggedItem->InitializeItemWidgetWithItemInstance(ItemInstance);
	DraggedItem->AddToViewport();
	InternalHeroComponent->DragItem(DraggedItem);

	InventoryComponent->RemoveItemInstance(ItemInstance);
	InventoryStateMap = InventoryComponent->Internal_GetInventoryStateMap();
}

void UObsidianInventoryWidgetController::HandleTakingOutStacks(UObsidianInventoryItemInstance* ItemInstance, const FVector2D& SlotPosition, UObsidianItem* ItemWidget, const int32 CurrentStacks, const int32 StacksToTake)
{
	if(StacksToTake == 0)
	{
		return;
	}
	
	if(CurrentStacks == StacksToTake)
	{
		PickupItem(SlotPosition);
		return;
	}
	
	UObsidianInventoryItemInstance* NewInstance = InventoryComponent->TakeOutItemInstance(ItemInstance, StacksToTake);
	if(NewInstance == nullptr)
	{
		return;
	}

	const int32 CurrentOldInstanceStacks = ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	ItemWidget->OverrideCurrentStackCount(CurrentOldInstanceStacks);

	UObsidianDraggedItem* DraggedItem = CreateWidget<UObsidianDraggedItem>(PlayerController, DraggedItemWidgetClass);
	DraggedItem->InitializeItemWidgetWithItemInstance(NewInstance);
	DraggedItem->AddToViewport();
	InternalHeroComponent->DragItem(DraggedItem);
}

bool UObsidianInventoryWidgetController::IsDraggingAnItem() const
{
	if(InternalHeroComponent)
	{
		return InternalHeroComponent->IsDraggingAnItem();
	}
	return false;
}

bool UObsidianInventoryWidgetController::CanPlaceDraggedItem(const FVector2D& HoveredSlot, const TArray<FVector2D>& ItemGridSize) const
{
	if(ItemGridSize.IsEmpty())
	{
		TArray<FVector2D> LocalItemGridSize;
		if(!GetDraggedItemGridSize(LocalItemGridSize))
		{
			return false;
		}
		return CanAddToSpecificSlot(LocalItemGridSize, HoveredSlot);
	}
	return CanAddToSpecificSlot(ItemGridSize, HoveredSlot);
}

bool UObsidianInventoryWidgetController::CanAddToSpecificSlot(const TArray<FVector2D>& ItemGridSize, const FVector2D& HoveredSlot) const
{
	bool bCanFit = false;
	
	if(InventoryStateMap[HoveredSlot] == false) // Initial location is free
	{
		bCanFit = true;
		for(FVector2D LocationComp : ItemGridSize)
		{
			const FVector2D Loc = HoveredSlot + LocationComp;
			if(!InventoryStateMap.Contains(Loc) || InventoryStateMap[Loc] == true)
			{
				bCanFit = false;
				break;
			}
		}
	}
	return bCanFit;
}

bool UObsidianInventoryWidgetController::GetDraggedItemGridSize(TArray<FVector2D>& OutItemGridSize) const
{
	if(!IsDraggingAnItem())
	{
		return false;
	}

	const UObsidianDraggedItem* DraggedItem = InternalHeroComponent->GetCurrentlyDraggedItem();
	if(!DraggedItem)
	{
		return false;
	}
	
	if(const UObsidianInventoryItemInstance* Instance = DraggedItem->GetItemInstance())
	{
		OutItemGridSize = Instance->GetItemGridSize();
		return true;	
	}
	
	if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem->GetItemDef())
	{
		if(const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef))
		{
			if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
			{
				OutItemGridSize = AppearanceFrag->GetItemGridSizeFromDesc();
				return true;	
			}
		}
	}
	return false;
}

void UObsidianInventoryWidgetController::AddItemWidget(const FVector2D& Location, UObsidianItem* ItemWidget)
{
	if(!AddedItemWidgetMap.Contains(Location))
	{
		AddedItemWidgetMap.Add(Location, ItemWidget);
	}
}

void UObsidianInventoryWidgetController::RemoveItemWidget(const FVector2D& Location)
{
	if(AddedItemWidgetMap.Contains(Location))
	{
		AddedItemWidgetMap[Location]->RemoveFromParent();
		AddedItemWidgetMap.Remove(Location);
	}
}
