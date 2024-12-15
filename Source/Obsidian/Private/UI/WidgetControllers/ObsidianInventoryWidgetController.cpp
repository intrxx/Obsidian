// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/WidgetControllers/ObsidianInventoryWidgetController.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "CharacterComponents/ObsidianHeroComponent.h"
#include "InventoryItems/ObsidianInventoryComponent.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "UI/Inventory/ObsidianDraggedItem.h"
#include "UI/Inventory/ObsidianItem.h"

void UObsidianInventoryWidgetController::OnWidgetControllerSetupCompleted()
{
	check(InventoryComponent);
	InternalInventoryComponent = InventoryComponent;
	InventoryComponent->OnItemAddedToInventoryDelegate.AddUObject(this, &ThisClass::OnItemAdded);

	const AActor* OwningActor = Cast<AActor>(PlayerController->GetPawn());
	check(OwningActor);
	
	InternalHeroComponent = UObsidianHeroComponent::FindHeroComponent(OwningActor);
	check(InternalHeroComponent);
}

void UObsidianInventoryWidgetController::OnItemAdded(UObsidianInventoryItemInstance* ItemInstance, const FVector2D DesiredPosition)
{
	check(ItemInstance);
	OnItemAddedDelegate.Broadcast(ItemInstance->GetItemImage(), DesiredPosition, ItemInstance->GetItemGridSpan());
}

void UObsidianInventoryWidgetController::OnInventoryOpen()
{
	//TODO This really needs profiling, for now let it be this way
	TMap<FVector2D, UObsidianInventoryItemInstance*> GridLocationToItemMap = InventoryComponent->Internal_GetLocationToInstanceMap();
	AddedItemWidgetMap.Empty(GridLocationToItemMap.Num());
	
	for(const TTuple<FVector2D, UObsidianInventoryItemInstance*>& LocToInstancePair : GridLocationToItemMap)
	{
		OnItemAddedDelegate.Broadcast(LocToInstancePair.Value->GetItemImage(), LocToInstancePair.Key, LocToInstancePair.Value->GetItemGridSpan());
	}
}

void UObsidianInventoryWidgetController::RequestAddingItemToInventory(const FVector2D& SlotPosition)
{
	if(InternalHeroComponent->IsDraggingAnItem() == false)
	{
		return;
	}
	check(InventoryComponent);
	
	const UObsidianDraggedItem* DraggedItem = InternalHeroComponent->GetCurrentlyDraggedItem();
	if(UObsidianInventoryItemInstance* Instance = DraggedItem->GetItemInstance())
	{
		if(InventoryComponent->AddItemInstanceToSpecificSlot(Instance, SlotPosition))
		{
			InternalHeroComponent->StopDragging();
		}
	}
	else
	{
		const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem->GetItemDef();
		const int32 ItemStackCount = DraggedItem->GetItemStacks();

		UObsidianInventoryItemInstance* NewInstance = InventoryComponent->AddItemDefinitionToSpecifiedSlot(ItemDef, SlotPosition, ItemStackCount);
		if(NewInstance != nullptr)
		{
			InternalHeroComponent->StopDragging();
		}
	}
}

void UObsidianInventoryWidgetController::RequestPickingUpItemFromInventory(const FVector2D& SlotPosition)
{
	if(InternalHeroComponent->IsDraggingAnItem() == true) //TODO This should be a valid case in the future, just replace the item
	{
		return;
	}
	check(InventoryComponent);
	check(DraggedItemWidgetClass);
	
	UObsidianInventoryItemInstance* ItemInstance = InventoryComponent->Internal_GetItemInstanceForLocation(SlotPosition);
	RemoveItemWidget(SlotPosition);
	
	UObsidianDraggedItem* DraggedItem = CreateWidget<UObsidianDraggedItem>(PlayerController, DraggedItemWidgetClass);
	DraggedItem->InitializeItemWidgetWithItemInstance(ItemInstance);
	DraggedItem->AddToViewport();
	InternalHeroComponent->DragItem(DraggedItem);

	InventoryComponent->RemoveItemInstance(ItemInstance);
}

bool UObsidianInventoryWidgetController::IsDraggingAnItem() const
{
	if(InternalHeroComponent)
	{
		return InternalHeroComponent->IsDraggingAnItem();
	}
	return false;
}

FVector2D UObsidianInventoryWidgetController::GetDraggedItemGridSpan() const
{
	if(!IsDraggingAnItem())
	{
		return FVector2D::Zero();
	}

	const UObsidianDraggedItem* DraggedItem = InternalHeroComponent->GetCurrentlyDraggedItem();
	if(!DraggedItem)
	{
		return FVector2D::Zero();
	}
	
	if(const UObsidianInventoryItemInstance* Instance = DraggedItem->GetItemInstance())
	{
		return Instance->GetItemGridSpan();		
	}
	
	if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem->GetItemDef())
	{
		if(const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef))
		{
			if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
			{
				return AppearanceFrag->GetItemGridSpanFromDesc();
			}
		}
	}
	return FVector2D::Zero();
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
