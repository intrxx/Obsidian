// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/WidgetControllers/ObsidianInventoryWidgetController.h"

#include <GameFramework/GameplayMessageSubsystem.h>

#include "UI/Inventory/ObsidianItemDescriptionBase.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "CharacterComponents/ObsidianHeroComponent.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "InventoryItems/ObsidianInventoryComponent.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "UI/ObsidianHUD.h"
#include "UI/Inventory/ObsidianDraggedItem.h"
#include "UI/Inventory/ObsidianItem.h"
#include "UI/Inventory/SubWidgets/ObsidianUnstackSlider.h"
#include "UI/MainOverlay/ObsidianMainOverlay.h"

void UObsidianInventoryWidgetController::OnWidgetControllerSetupCompleted()
{
	check(InventoryComponent);
	InternalInventoryComponent = InventoryComponent;
	//InventoryComponent->OnItemAddedToInventoryDelegate.AddUObject(this, &ThisClass::ClientOnItemAdded);
	InventoryComponent->OnItemsStacksChangedDelegate.AddUObject(this, &ThisClass::OnItemsStacksChanged);
	InventoryStateMap = InventoryComponent->Internal_GetInventoryStateMap();

	const AActor* OwningActor = Cast<AActor>(PlayerController->GetPawn());
	check(OwningActor);

	ObsidianPC = Cast<AObsidianPlayerController>(PlayerController);
	check(ObsidianPC);
	
	InternalHeroComponent = UObsidianHeroComponent::FindHeroComponent(OwningActor);
	check(InternalHeroComponent);

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(OwningActor->GetWorld());
	MessageSubsystem.RegisterListener(ObsidianGameplayTags::Message_Inventory_Changed, this, &ThisClass::OnInventoryStateChanged);
}

void UObsidianInventoryWidgetController::OnInventoryStateChanged(FGameplayTag Channel, const FObsidianInventoryChangeMessage& InventoryChangeMessage)
{
	const UObsidianInventoryItemInstance* Instance = InventoryChangeMessage.ItemInstance;
	if(Instance == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Instance is invalid in UObsidianInventoryWidgetController::OnInventoryStateChanged."));
		return;
	}
	
	if(InventoryChangeMessage.NewCount == 0) // Removed
	{
		UE_LOG(LogInventory, Warning, TEXT("Removed item: [%s]"), *Instance->GetItemDisplayName().ToString());
	}

	if(InventoryChangeMessage.NewCount == InventoryChangeMessage.Delta) // Added
	{
		UE_LOG(LogInventory, Warning, TEXT("Added item: [%s]"), *Instance->GetItemDisplayName().ToString());
		
		FObsidianItemVisuals ItemVisuals;
		ItemVisuals.ItemImage = Instance->GetItemImage();
		ItemVisuals.DesiredPosition = InventoryChangeMessage.GridItemPosition;
		ItemVisuals.GridSpan = Instance->GetItemGridSpan();
		ItemVisuals.StackCount = Instance->IsStackable() ? InventoryChangeMessage.NewCount : 0;
		
		OnItemAddedDelegate.Broadcast(ItemVisuals);
	}

	if (InventoryChangeMessage.NewCount != InventoryChangeMessage.Delta) // Changed
	{
		UE_LOG(LogInventory, Warning, TEXT("Changed item: [%s]"), *Instance->GetItemDisplayName().ToString());
		
		FObsidianItemVisuals ItemVisuals;
		ItemVisuals.ItemImage = Instance->GetItemImage();
		ItemVisuals.DesiredPosition = InventoryChangeMessage.GridItemPosition;
		ItemVisuals.GridSpan = Instance->GetItemGridSpan();
		ItemVisuals.StackCount = Instance->IsStackable() ? InventoryChangeMessage.NewCount : 0;
		
		OnItemChangedDelegate.Broadcast(ItemVisuals);
	}	
}

void UObsidianInventoryWidgetController::ClientOnItemAdded_Implementation(UObsidianInventoryItemInstance* ItemInstance, const FVector2D DesiredPosition)
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
	if(IsInventoryOpened() == false)
	{
		return;
	}
	
	for(TTuple<FVector2D, int32> LocationToStack : LocationToStacksMap)
	{
		checkf(AddedItemWidgetMap.Contains(LocationToStack.Key), TEXT("There is a missmatch in visual inventory state and actual items location in Inventory Component."));
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
	//
	//  Design change
	//
	// GridLocationToItemMap.Empty();
	// GridLocationToItemMap = InventoryComponent->Internal_GetLocationToInstanceMap();
	// AddedItemWidgetMap.Empty(GridLocationToItemMap.Num());
	//
	// for(const TTuple<FVector2D, UObsidianInventoryItemInstance*>& LocToInstancePair : GridLocationToItemMap)
	// {
	// 	const UObsidianInventoryItemInstance* ItemInstance = LocToInstancePair.Value;
	// 	ensure(ItemInstance);
	// 	
	// 	const int32 StackCount = ItemInstance->IsStackable() ? ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current) : 0;
	// 	const FObsidianItemVisuals ItemVisuals = FObsidianItemVisuals
	// 	(
	// 		ItemInstance->GetItemImage(),
	// 		LocToInstancePair.Key,
	// 		ItemInstance->GetItemGridSpan(),
	// 		StackCount
	// 	);
	// 	OnItemAddedDelegate.Broadcast(ItemVisuals);
	// }
	//
	// End of Design change
	//

	
	TArray<UObsidianInventoryItemInstance*> Items = InventoryComponent->GetAllItems();
	AddedItemWidgetMap.Empty(Items.Num());
	
	for(const UObsidianInventoryItemInstance* Item : Items)
	{
		ensure(Item);
		
		FObsidianItemVisuals ItemVisuals;
		ItemVisuals.ItemImage = Item->GetItemImage();
		ItemVisuals.GridSpan = Item->GetItemGridSpan();
		ItemVisuals.DesiredPosition = Item->GetItemCurrentGridLocation();
		ItemVisuals.StackCount = Item->IsStackable() ? Item->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current) : 0;

		OnItemAddedDelegate.Broadcast(ItemVisuals);
	}
}

void UObsidianInventoryWidgetController::RequestAddingItemToInventory(const FVector2D& SlotPosition, const bool bShiftDown)
{
	check(InternalHeroComponent);
	if(InternalHeroComponent->IsDraggingAnItem() == false)
	{
		return;
	}
	InternalHeroComponent->ServerAddItemToInventoryAtSlot(SlotPosition, bShiftDown);
}

void UObsidianInventoryWidgetController::HandleLeftClickingOnAnItem(const FVector2D& SlotPosition, UObsidianItem* ItemWidget)
{
	check(InventoryComponent);
	check(DraggedItemWidgetClass);
	check(ItemWidget);

	RemoveItemUIElements();
	
	// if(InternalHeroComponent->IsDraggingAnItem()) // If we carry an item, try to add it to this item or replace it with it.
	// {
	// 	 UObsidianDraggedItem* DraggedItem = InternalHeroComponent->GetCurrentlyDraggedItem();
	// 	 if(UObsidianInventoryItemInstance* DraggedInstance = DraggedItem->GetItemInstance()) // We carry item instance.
	// 	 {
	// 	 	if(DraggedInstance->IsStackable())
	// 	 	{
	// 	 		FObsidianAddingStacksResult AddingStacksResult;
	// 	 		if(InventoryComponent->TryAddingStacksToSpecificSlotWithInstance(DraggedInstance, SlotPosition, /** OUT */ AddingStacksResult))
	// 	 		{
	// 	 			ItemWidget->AddCurrentStackCount(AddingStacksResult.AddedStacks);
	// 	 			if(AddingStacksResult.bAddedWholeItemAsStacks) 
	// 	 			{
	// 	 				InternalHeroComponent->StopDragging();
	// 	 				return;
	// 	 			}
	// 	 			DraggedItem->UpdateStackCount(AddingStacksResult.StacksLeft);
	// 	 			return;
	// 	 		}
	// 	 	}
	// 		
	// 	 	if(InventoryComponent->CanReplaceItemAtSpecificSlotWithInstance(SlotPosition, DraggedInstance))
	// 	 	{
	// 	 		InternalHeroComponent->StopDragging();
	// 	 		PickupItem(SlotPosition);
	// 	 		InventoryComponent->AddItemInstanceToSpecificSlot(DraggedInstance, SlotPosition);
	// 	 	}
	// 	 	return;
	// 	 }
	// 	
	// 	 if(const TSubclassOf<UObsidianInventoryItemDefinition> DraggedItemDef = DraggedItem->GetItemDef()) // We carry item def
	// 	 {
	// 	 	const int32 ItemStackCount = DraggedItem->GetItemStacks();
	// 	 	const UObsidianInventoryItemDefinition* DefaultObject = DraggedItemDef.GetDefaultObject();
	// 	 	if(DefaultObject && DefaultObject->IsStackable())
	// 	 	{
	// 	 		FObsidianAddingStacksResult AddingStacksResult;
	// 	 		if(InventoryComponent->TryAddingStacksToSpecificSlotWithItemDef(DraggedItemDef, ItemStackCount, SlotPosition, /** OUT */ AddingStacksResult))
	// 	 		{
	// 	 			ItemWidget->AddCurrentStackCount(AddingStacksResult.AddedStacks);
	// 	 			if(AddingStacksResult.bAddedWholeItemAsStacks)
	// 	 			{
	// 	 				InternalHeroComponent->StopDragging();
	// 	 				return;
	// 	 			}
	// 	 			DraggedItem->UpdateStackCount(AddingStacksResult.StacksLeft);
	// 	 			return;
	// 	 		}
	// 	 	}
	// 		
	// 		 if(InternalInventoryComponent->CanReplaceItemAtSpecificSlotWithDef(SlotPosition, DraggedItemDef, ItemStackCount))
	// 		 {
	// 		 	InternalHeroComponent->StopDragging();
	// 		 	PickupItem(SlotPosition);
	// 		 	int32 StackLeft = ItemStackCount;
	// 		 	InventoryComponent->AddItemDefinitionToSpecifiedSlot(DraggedItemDef, SlotPosition, StackLeft, ItemStackCount);
	// 		 }
	// 		 return;
	// 	}
	// 	return;
	// }
	PickupItem(SlotPosition);
}

void UObsidianInventoryWidgetController::HandleLeftClickingOnAnItemWithShiftDown(const FVector2D& SlotPosition, UObsidianItem* ItemWidget)
{
	check(InternalHeroComponent);
	
	if(InternalHeroComponent->IsDraggingAnItem())
	{
		InternalHeroComponent->ServerAddStacksFromDraggedItemToItemAtSlot(SlotPosition, 1);
		return;
	}
	
	UObsidianInventoryItemInstance* ItemInstance = InventoryComponent->Internal_GetItemInstanceAtLocation(SlotPosition);
	if(ItemInstance->IsStackable() == false)
	{
		return;
	}
	
	const int32 CurrentItemStacks = ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	if(CurrentItemStacks <= 1)
	{
		return;
	}

	RemoveItemUIElements();

	checkf(UnstackSliderClass, TEXT("Tried to create widget without valid widget class in UObsidianInventoryWidgetController::HandleLeftClickingOnAnItemWithShiftDown, fill it in ObsidianInventoryWidgetController instance."));
	ActiveUnstackSlider = CreateWidget<UObsidianUnstackSlider>(PlayerController, UnstackSliderClass);
	ActiveUnstackSlider->InitializeUnstackSlider(CurrentItemStacks);

	const FVector2D UnstackSliderViewportPosition = CalculateUnstackSliderPosition(ItemWidget);
	ActiveUnstackSlider->SetPositionInViewport(UnstackSliderViewportPosition);
	ActiveUnstackSlider->AddToViewport();
	bUnstackSliderActive = true;
	
	ActiveUnstackSlider->OnAcceptButtonPressedDelegate.AddWeakLambda(this,[this, ItemInstance, SlotPosition, ItemWidget, CurrentItemStacks](const int32 StackToTake)
		{
			HandleTakingOutStacks(ItemInstance, SlotPosition, ItemWidget, CurrentItemStacks, StackToTake);
		});
	ActiveUnstackSlider->OnCloseButtonPressedDelegate.AddUObject(this, &ThisClass::RemoveUnstackSlider);
}

void UObsidianInventoryWidgetController::HandleHoveringOverItem(const FVector2D& SlotPosition, UObsidianItem* ItemWidget)
{
	if(!CanShowDescription())
	{
		return;
	}
	
	RemoveItemDescription();
	
	const FObsidianItemStats ItemStats = InventoryComponent->GetItemStatsByInventoryPosition(SlotPosition);

	checkf(ItemDescriptionClass, TEXT("Tried to create widget without valid widget class in UObsidianInventoryWidgetController::HandleHoveringOverItem, fill it in ObsidianInventoryWidgetController instance."));
	ActiveItemDescription = CreateWidget<UObsidianItemDescriptionBase>(PlayerController, ItemDescriptionClass);
	ActiveItemDescription->InitializeWidgetWithItemStats(ItemStats);
	ActiveItemDescription->AddToViewport();
	
	const FVector2D DescriptionViewportPosition = CalculateDescriptionPosition(ItemWidget);
	ActiveItemDescription->SetPositionInViewport(DescriptionViewportPosition);
	bDescriptionActive = true;
}

bool UObsidianInventoryWidgetController::CanShowDescription() const
{
	return !bUnstackSliderActive;
}

void UObsidianInventoryWidgetController::HandleUnhoveringItem(const FVector2D& SlotPosition)
{
	RemoveItemDescription();
}

UObsidianItemDescriptionBase* UObsidianInventoryWidgetController::CreateItemDescriptionForDroppedItem(const UObsidianInventoryItemInstance* Instance)
{
	if(!CanShowDescription())
	{
		return nullptr;
	}

	AObsidianHUD* ObsidianHUD = ObsidianPC->GetObsidianHUD();
	if(ObsidianHUD == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Unable to get ObsidianHUD in UObsidianInventoryWidgetController::CreateItemDescriptionForDroppedItem."));
		return nullptr;
	}

	const UObsidianMainOverlay* MainOverlay = ObsidianHUD->GetMainOverlay();
	if(MainOverlay == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Unable to get ObsidianMainOverlay in UObsidianInventoryWidgetController::CreateItemDescriptionForDroppedItem."));
		return nullptr;
	}

	const FObsidianItemStats ItemStats = InventoryComponent->GetItemStatForInstance(Instance);

	checkf(ItemDescriptionClass, TEXT("Tried to create widget without valid widget class in UObsidianInventoryWidgetController::CreateItemDescriptionForDroppedItem, fill it in ObsidianInventoryWidgetController instance."));
	ActiveItemDescription = CreateWidget<UObsidianItemDescriptionBase>(PlayerController, ItemDescriptionClass);
	ActiveItemDescription->InitializeWidgetWithItemStats(ItemStats, true);
	MainOverlay->AddItemDescriptionToOverlay(ActiveItemDescription);

	return ActiveItemDescription;
}

UObsidianItemDescriptionBase* UObsidianInventoryWidgetController::CreateItemDescriptionForDroppedItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const int32 CurrentItemStacks)
{
	if(!CanShowDescription())
	{
		return nullptr;
	}

	AObsidianHUD* ObsidianHUD = ObsidianPC->GetObsidianHUD();
	if(ObsidianHUD == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Unable to get ObsidianHUD in UObsidianInventoryWidgetController::CreateItemDescriptionForDroppedItem."));
		return nullptr;
	}

	const UObsidianMainOverlay* MainOverlay = ObsidianHUD->GetMainOverlay();
	if(MainOverlay == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Unable to get ObsidianMainOverlay in UObsidianInventoryWidgetController::CreateItemDescriptionForDroppedItem."));
		return nullptr;
	}
	
	const FObsidianItemStats ItemStats = InventoryComponent->GetItemStatsForItemDefinition(ItemDef, CurrentItemStacks);

	checkf(ItemDescriptionClass, TEXT("Tried to create widget without valid widget class in UObsidianInventoryWidgetController::CreateItemDescriptionForDroppedItem, fill it in ObsidianInventoryWidgetController instance."));
	ActiveItemDescription = CreateWidget<UObsidianItemDescriptionBase>(PlayerController, ItemDescriptionClass);
	ActiveItemDescription->InitializeWidgetWithItemStats(ItemStats, true);
	MainOverlay->AddItemDescriptionToOverlay(ActiveItemDescription);

	return ActiveItemDescription;
}

void UObsidianInventoryWidgetController::PickupItem(const FVector2D& SlotPosition)
{
	UObsidianInventoryItemInstance* ItemInstance = InventoryComponent->Internal_GetItemInstanceAtLocation(SlotPosition);
	RemoveItemWidget(SlotPosition);

	check(InternalHeroComponent);
	InternalHeroComponent->ServerGrabInventoryItemToCursor(ItemInstance);
	InventoryStateMap = InventoryComponent->Internal_GetInventoryStateMap();
}

void UObsidianInventoryWidgetController::HandleTakingOutStacks(UObsidianInventoryItemInstance* ItemInstance, const FVector2D& SlotPosition, UObsidianItem* ItemWidget, const int32 CurrentStacks, const int32 StacksToTake)
{
	RemoveUnstackSlider();
	
	if(StacksToTake == 0)
	{
		return;
	}
	
	if(CurrentStacks == StacksToTake)
	{
		PickupItem(SlotPosition);
		return;
	}

	//TODO This should be server authoritative 
	UObsidianInventoryItemInstance* NewInstance = InventoryComponent->TakeOutFromItemInstance(ItemInstance, StacksToTake);
	if(NewInstance == nullptr)
	{
		return;
	}

	const int32 CurrentOldInstanceStacks = ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	ItemWidget->OverrideCurrentStackCount(CurrentOldInstanceStacks);
	
	check(InternalHeroComponent);
	InternalHeroComponent->ServerGrabInventoryItemToCursor(NewInstance);
}

void UObsidianInventoryWidgetController::RemoveItemUIElements()
{
	RemoveUnstackSlider();
	RemoveItemDescription();
}

void UObsidianInventoryWidgetController::RemoveUnstackSlider()
{
	if(bUnstackSliderActive && ActiveUnstackSlider != nullptr)
	{
		ActiveUnstackSlider->DestroyUnstackSlider();
		ActiveUnstackSlider = nullptr;
		bUnstackSliderActive = false;
	}
}

void UObsidianInventoryWidgetController::RemoveItemDescription()
{
	if(bDescriptionActive && ActiveItemDescription)
	{
		ActiveItemDescription->DestroyDescriptionWidget();
		ActiveItemDescription = nullptr;
		bDescriptionActive = false;
	}
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
	
	const FDraggedItem DraggedItem = InternalHeroComponent->GetDraggedItem();
	if(DraggedItem.IsEmpty())
	{
		return false;
	}
	
	if(const UObsidianInventoryItemInstance* Instance = DraggedItem.Instance)
	{
		OutItemGridSize = Instance->GetItemGridSize();
		return true;	
	}
	
	if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem.ItemDef)
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

UObsidianItem* UObsidianInventoryWidgetController::GetItemWidgetAtLocation(const FVector2D& Location) const
{
	if(AddedItemWidgetMap.Contains(Location))
	{
		return AddedItemWidgetMap[Location];
	}
	return nullptr;
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

FVector2D UObsidianInventoryWidgetController::CalculateUnstackSliderPosition(const UObsidianItem* ItemWidget) const
{
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Failed to calculate Unstack Slider Position"));
		return FVector2D::Zero();
	}

	if(ItemWidget == nullptr || ActiveUnstackSlider == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Failed to calculate Unstack Slider Position"));
		return FVector2D::Zero();
	}
	
	FVector2D SliderSize = ActiveUnstackSlider->GetSizeBoxSize();
		
	const FGeometry& CachedGeometry = ItemWidget->GetCachedGeometry();
	FVector2D ItemLocalSize = ItemWidget->GetItemSize();

	// Adjusting sizes based on viewport scale
	const float DPIScale = UWidgetLayoutLibrary::GetViewportScale(World);
	SliderSize *= DPIScale; 
	ItemLocalSize *= DPIScale;

	FVector2D ItemPixelPosition = FVector2D::Zero();
	FVector2D ItemViewportPosition = FVector2D::Zero();
	USlateBlueprintLibrary::LocalToViewport(World, CachedGeometry, FVector2D(0.f,0.f), ItemPixelPosition, ItemViewportPosition);

	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(World);
	return GetItemUIElementPositionBoundByViewport(ViewportSize, ItemPixelPosition, ItemLocalSize, SliderSize);
}

FVector2D UObsidianInventoryWidgetController::CalculateDescriptionPosition(const UObsidianItem* ItemWidget) const
{
	FVector2D FinalPosition = FVector2D::Zero();
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Failed to calculate Description Position"));
		return FinalPosition;
	}

	if(ItemWidget == nullptr || ActiveItemDescription == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Failed to calculate Description Position"));
		return FinalPosition;
	}

	// @HACK this is quite ugly, but without prepass the desired size is [0, 0], if the performance is the problem,
	// I could delay the calculation for a frame and see how reliable it is to retrieve the sie information,
	// Other system with delegates could be implemented to get the size reliably, but it just needs testing cuz if it's not bad I don't really care for now.
	ActiveItemDescription->ForceLayoutPrepass();
	FVector2D DescriptionSize = ActiveItemDescription->GetDesiredSize();
		
	const FGeometry& CachedGeometry = ItemWidget->GetCachedGeometry();
	FVector2D ItemLocalSize = ItemWidget->GetItemSize();
		
	// Adjusting sizes based on viewport scale
	const float DPIScale = UWidgetLayoutLibrary::GetViewportScale(World);
	ItemLocalSize *= DPIScale; 
	DescriptionSize *= DPIScale;
		
	FVector2D ItemPixelPosition = FVector2D::Zero();
	FVector2D ItemViewportPosition = FVector2D::Zero();
	USlateBlueprintLibrary::LocalToViewport(World, CachedGeometry, FVector2D(0.f,0.f), ItemPixelPosition, ItemViewportPosition);
	
	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(World);
	return GetItemUIElementPositionBoundByViewport(ViewportSize, ItemPixelPosition, ItemLocalSize, DescriptionSize);
}

FVector2D UObsidianInventoryWidgetController::GetItemUIElementPositionBoundByViewport(const FVector2D& ViewportSize, const FVector2D& ItemPosition, const FVector2D& ItemSize, const FVector2D& UIElementSize) const
{
	FVector2D BoundedPosition = FVector2D((ItemPosition.X - (UIElementSize.X / 2)) + (ItemSize.X / 2),
										  (ItemPosition.Y - UIElementSize.Y));
	
	const bool bFitsLeft = BoundedPosition.X > 0.0f;
	const bool bFitsRight = (BoundedPosition.X + UIElementSize.X) < ViewportSize.X;
	const bool bFitsTop = (BoundedPosition.Y - UIElementSize.Y) > 0.0f;
	
	if(bFitsLeft && bFitsRight && bFitsTop) // We fit in the default position [top-middle]
	{
		return BoundedPosition;
	}
	
	if(bFitsLeft && bFitsRight && bFitsTop == false)
	{
		BoundedPosition = FVector2D((ItemPosition.X - (UIElementSize.X / 2)) + (ItemSize.X / 2),(ItemPosition.Y + ItemSize.Y));
		if((BoundedPosition.Y + UIElementSize.Y) < ViewportSize.Y) // Desc fit below [bottom-middle]
		{
			return BoundedPosition;
		}
	}
	
	if(bFitsRight == false)
	{
		BoundedPosition = FVector2D((ItemPosition.X - UIElementSize.X),(ItemPosition.Y - (UIElementSize.Y / 2)) + (ItemSize.Y / 2));
		if(BoundedPosition.X > 0.0f) // Desc fit left [left-middle]
		{
			return BoundedPosition;
		}
	}
	
	if(bFitsLeft == false)
	{
		BoundedPosition = FVector2D((ItemPosition.X + ItemSize.X),(ItemPosition.Y - (UIElementSize.Y / 2)) + (ItemSize.Y / 2));
		if((BoundedPosition.X + UIElementSize.X) < ViewportSize.X) // Desc Fit right [right-middle]
		{
			return BoundedPosition;
		}
	}
	
	// Falling back to the default not so happy position which is most likely to fit if all above cases fail,
	// could improve it later to fit the screen in every case but is not necessary now [middle-middle].
	BoundedPosition = FVector2D((ItemPosition.X - (UIElementSize.X / 2)) + (ItemSize.X / 2),(ItemPosition.Y - (UIElementSize.Y / 2)) + (ItemSize.Y / 2));
	
	return BoundedPosition;
}

