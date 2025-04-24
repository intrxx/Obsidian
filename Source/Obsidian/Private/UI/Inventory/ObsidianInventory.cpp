// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Inventory/ObsidianInventory.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/SizeBox.h"
#include "UI/Inventory/SubWidgets/ObsidianItemSlot.h"
#include "UI/Inventory/ObsidianItem.h"
#include "UI/Inventory/SubWidgets/ObsidianItemSlot_Equipment.h"
#include "UI/Inventory/SubWidgets/ObsidianItemSlot_Inventory.h"
#include "UI/WidgetControllers/ObsidianInventoryWidgetController.h"

void UObsidianInventory::NativePreConstruct()
{
	Super::NativePreConstruct();

	Root_SizeBox->SetHeightOverride(RootSizeBoxHeight);
	Root_SizeBox->SetWidthOverride(RootSizeBoxWidth);
}

void UObsidianInventory::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetupInventoryGrid();
	SetupEquipmentSlots();

	EquipmentSlots =
	{
		RightHand_EquipmentSlot,
		LeftHand_EquipmentSlot,
		RightHandSwap_EquipmentSlot,
		LeftHandSwap_EquipmentSlot,
		Helmet_EquipmentSlot,
		BodyArmor_EquipmentSlot,
		Belt_EquipmentSlot,
		Gloves_EquipmentSlot,
		Boots_EquipmentSlot,
		Amulet_EquipmentSlot,
		RightRing_EquipmentSlot,
		LeftRing_EquipmentSlot
	};
}

void UObsidianInventory::NativeDestruct()
{
	if(InventoryWidgetController)
	{
		InventoryWidgetController->RemoveItemUIElements();
	}
	Super::NativeDestruct();
}

void UObsidianInventory::HandleWidgetControllerSet()
{
	InventoryWidgetController = Cast<UObsidianInventoryWidgetController>(WidgetController);
	check(InventoryWidgetController);

	InventoryWidgetController->OnItemEquippedDelegate.AddUObject(this, &ThisClass::OnItemEquipped);
	
	InventoryWidgetController->OnItemAddedDelegate.AddUObject(this, &ThisClass::OnItemAdded);
	InventoryWidgetController->OnItemChangedDelegate.AddUObject(this, &ThisClass::OnItemChanged);
}

UObsidianItemSlot_Equipment* UObsidianInventory::FindEquipmentSlotForTag(const FGameplayTag& Tag) const
{
	for(UObsidianItemSlot_Equipment* EquipmentSlot : EquipmentSlots)
	{
		if(EquipmentSlot->GetSlotTag() == Tag)
		{
			return EquipmentSlot;
		}
	}
	return nullptr;
}

void UObsidianInventory::SetupInventoryGrid() 
{
	if(Slots_GridPanel->HasAnyChildren())
	{
		Slots_GridPanel->ClearChildren(); 
	}
	
	int16 GridX = 0;
	int16 GridY = 0;
	
	const int32 InventoryGridWidth = InventoryWidgetController->GetInventoryGridWidth();
	const int32 InventoryGridSize = InventoryWidgetController->GetInventoryGridHeight() * InventoryGridWidth;
	InventoryLocationToSlotMap.Reserve(InventoryGridSize);
	
	for(int32 i = 0; i < InventoryGridSize; i++)
	{
		const FVector2D SlotPosition = FVector2D(GridX, GridY);
		checkf(InventorySlotClass, TEXT("Tried to create widget without valid widget class in UObsidianInventory::SetupGrid, fill it in ObsidianInventory instance."));
		UObsidianItemSlot_Inventory* InventorySlot = CreateWidget<UObsidianItemSlot_Inventory>(this, InventorySlotClass);
		InventorySlot->InitializeSlot(this, SlotPosition);
		
		UGridSlot* GridSlot = Slots_GridPanel->AddChildToGrid(InventorySlot, GridY, GridX);
		GridSlot->SetLayer(0);

		InventoryLocationToSlotMap.Add(SlotPosition, InventorySlot);
		
		if(GridX == InventoryGridWidth - 1)
		{
			GridX = 0;
			GridY++;
		}
		else
		{
			GridX++;
		}
	}
}

void UObsidianInventory::SetupEquipmentSlots()
{
	if(RightHand_EquipmentSlot && LeftHand_EquipmentSlot && RightHandSwap_EquipmentSlot && LeftHandSwap_EquipmentSlot)
	{
		RightHand_EquipmentSlot->InitializeSlot(this);
		LeftHand_EquipmentSlot->InitializeSlot(this);
		RightHandSwap_EquipmentSlot->InitializeSlot(this);
		LeftHandSwap_EquipmentSlot->InitializeSlot(this);
	}

	if(Helmet_EquipmentSlot && BodyArmor_EquipmentSlot && Belt_EquipmentSlot && Gloves_EquipmentSlot && Boots_EquipmentSlot)
	{
		Helmet_EquipmentSlot->InitializeSlot(this);
		BodyArmor_EquipmentSlot->InitializeSlot(this);
		Belt_EquipmentSlot->InitializeSlot(this);
		Gloves_EquipmentSlot->InitializeSlot(this);
		Boots_EquipmentSlot->InitializeSlot(this);
	}

	if(Amulet_EquipmentSlot && LeftRing_EquipmentSlot && RightRing_EquipmentSlot)
	{
		Amulet_EquipmentSlot->InitializeSlot(this);
		RightRing_EquipmentSlot->InitializeSlot(this);
		LeftRing_EquipmentSlot->InitializeSlot(this);
	}
}

void UObsidianInventory::OnItemEquipped(const FObsidianItemWidgetData& ItemWidgetData)
{
	const FGameplayTag DesiredSlot = ItemWidgetData.DesiredSlot;
	
	checkf(InventorySlotClass, TEXT("Tried to create widget without valid widget class in UObsidianInventory::OnItemAdded, fill it in ObsidianInventory instance."));
	UObsidianItem* ItemWidget = CreateWidget<UObsidianItem>(this, ItemWidgetClass);
	ItemWidget->InitializeItemWidget(DesiredSlot, ItemWidgetData.GridSpan, ItemWidgetData.ItemImage);
	ItemWidget->OnItemLeftMouseButtonPressedDelegate.AddUObject(this, &ThisClass::OnEquipmentItemLeftMouseButtonPressed);
	ItemWidget->OnItemMouseEnterDelegate.AddUObject(this, &ThisClass::OnEquipmentItemMouseEntered);
	ItemWidget->OnItemMouseLeaveDelegate.AddUObject(this, &ThisClass::OnItemMouseLeave);
	InventoryWidgetController->AddEquipmentItemWidget(DesiredSlot, ItemWidget, ItemWidgetData.bSwappedWithAnotherItem);

	UObsidianItemSlot_Equipment* EquipmentSlot = FindEquipmentSlotForTag(DesiredSlot);
	const UGridSlot* DesiredGridSlot = UWidgetLayoutLibrary::SlotAsGridSlot(EquipmentSlot);
	UGridSlot* GridSlot = Equipment_GridPanel->AddChildToGrid(ItemWidget, DesiredGridSlot->GetRow(), DesiredGridSlot->GetColumn());
	GridSlot->SetLayer(1);
	GridSlot->SetNudge(DesiredGridSlot->GetNudge());
}

void UObsidianInventory::OnItemAdded(const FObsidianItemWidgetData& ItemWidgetData)
{
	const FVector2D DesiredPosition = ItemWidgetData.DesiredPosition;
	const FVector2D GridSpan = ItemWidgetData.GridSpan;
	
	checkf(InventorySlotClass, TEXT("Tried to create widget without valid widget class in UObsidianInventory::OnItemAdded, fill it in ObsidianInventory instance."));
	UObsidianItem* ItemWidget = CreateWidget<UObsidianItem>(this, ItemWidgetClass);
	ItemWidget->InitializeItemWidget(DesiredPosition, GridSpan, ItemWidgetData.ItemImage, ItemWidgetData.StackCount);
	ItemWidget->OnItemLeftMouseButtonPressedDelegate.AddUObject(this, &ThisClass::OnInventoryItemLeftMouseButtonPressed);
	ItemWidget->OnItemMouseEnterDelegate.AddUObject(this, &ThisClass::OnInventoryItemMouseEntered);
	ItemWidget->OnItemMouseLeaveDelegate.AddUObject(this, &ThisClass::OnItemMouseLeave);
	
	if(ItemWidgetData.bUsable)
	{
		ItemWidget->OnItemRightMouseButtonPressedDelegate.AddUObject(this, &ThisClass::OnInventoryItemRightMouseButtonPressed);
	}
	
	InventoryWidgetController->AddInventoryItemWidget(DesiredPosition, ItemWidget);

	UGridSlot* GridSlot = Slots_GridPanel->AddChildToGrid(ItemWidget, DesiredPosition.Y, DesiredPosition.X);
	GridSlot->SetLayer(1);
	GridSlot->SetColumnSpan(GridSpan.X);
	GridSlot->SetRowSpan(GridSpan.Y);
}

void UObsidianInventory::OnItemChanged(const FObsidianItemWidgetData& ItemWidgetData)
{
	const FVector2D ItemPosition = ItemWidgetData.DesiredPosition;
	if(UObsidianItem* ItemWidget = InventoryWidgetController->GetItemWidgetAtInventoryLocation(ItemPosition))
	{
		ItemWidget->OverrideCurrentStackCount(ItemWidgetData.StackCount);
	}
}

void UObsidianInventory::OnInventoryItemLeftMouseButtonPressed(const UObsidianItem* ItemWidget, const bool bShiftDown)
{
	ensureMsgf(ItemWidget, TEXT("Item Widget is invalid in UObsidianInventory::OnInventoryItemLeftMouseButtonPressed"));

	if(InventoryWidgetController)
	{
		if(bShiftDown)
		{
			InventoryWidgetController->HandleLeftClickingOnInventoryItemWithShiftDown(ItemWidget->GetInventoryPosition(), ItemWidget);
			return;
		}
		InventoryWidgetController->HandleLeftClickingOnInventoryItem(ItemWidget->GetInventoryPosition());
	}
}

void UObsidianInventory::OnInventoryItemRightMouseButtonPressed(UObsidianItem* ItemWidget)
{
	ensureMsgf(ItemWidget, TEXT("Item Widget is invalid in UObsidianInventory::OnInventoryItemRightMouseButtonPressed"));
	if(InventoryWidgetController)
	{
		InventoryWidgetController->HandleRightClickingOnInventoryItem(ItemWidget->GetInventoryPosition(), ItemWidget);
	}
}

void UObsidianInventory::OnEquipmentItemLeftMouseButtonPressed(const UObsidianItem* ItemWidget, const bool bShiftDown)
{
	ensureMsgf(ItemWidget, TEXT("Item Widget is invalid in UObsidianInventory::OnEquipmentItemLeftMouseButtonPressed"));
	if(InventoryWidgetController)
	{
		InventoryWidgetController->HandleLeftClickingOnEquipmentItem(ItemWidget->GetEquipmentSlotTag());
	}
}

void UObsidianInventory::OnEquipmentItemMouseEntered(const UObsidianItem* ItemWidget)
{
	ensureMsgf(ItemWidget, TEXT("Item Widget is invalid in UObsidianInventory::OnEquipmentItemMouseEntered"));
	if(InventoryWidgetController)
	{
		InventoryWidgetController->HandleHoveringOverEquipmentItem(ItemWidget);
	}
}

void UObsidianInventory::OnInventoryItemMouseEntered(const UObsidianItem* ItemWidget)
{
	ensureMsgf(ItemWidget, TEXT("Item Widget is invalid in UObsidianInventory::OnInventoryItemMouseEntered"));
	if(InventoryWidgetController)
	{
		InventoryWidgetController->HandleHoveringOverInventoryItem(ItemWidget);
	}
}

void UObsidianInventory::OnItemMouseLeave()
{
	InventoryWidgetController->HandleUnhoveringItem();
}

void UObsidianInventory::OnInventorySlotHover(const UObsidianItemSlot_Inventory* AffectedSlot, const bool bEntered)
{
	if(bEntered)
	{
		if(!InventoryWidgetController || !InventoryWidgetController->IsDraggingAnItem())
		{
			return;
		}

		TArray<FVector2D> ItemGridSize;
		const bool bSuccess = InventoryWidgetController->GetDraggedItemGridSize(ItemGridSize);
		if(!bSuccess)
		{
			return;
		}
		
		const FVector2D HoveredSlotPosition = AffectedSlot->GetSlotPosition();
		const bool bCanPlace = InventoryWidgetController->CanPlaceDraggedItem(HoveredSlotPosition, ItemGridSize);
		
		for(FVector2D SizeComp : ItemGridSize)
		{
			const FVector2D LocationToCheck = HoveredSlotPosition + SizeComp;
			if(InventoryLocationToSlotMap.Contains(LocationToCheck))
			{
				UObsidianItemSlot_Inventory* LocalSlot = InventoryLocationToSlotMap[LocationToCheck];
				LocalSlot->SetSlotState(bCanPlace);
				AffectedInventorySlots.Add(LocalSlot);
			}
		}
	}
	else
	{
		if(AffectedInventorySlots.IsEmpty())
		{
			return;
		}

		for(UObsidianItemSlot_Inventory* InventorySlot : AffectedInventorySlots)
		{
			InventorySlot->ResetSlot();
		}
		AffectedInventorySlots.Empty();
	}
}

void UObsidianInventory::OnInventorySlotMouseButtonDown(const UObsidianItemSlot_Inventory* AffectedSlot, const bool bShiftDown)
{
	if(InventoryWidgetController)
	{
		InventoryWidgetController->RequestAddingItemToInventory(AffectedSlot->GetSlotPosition(), bShiftDown);
	}
}

void UObsidianInventory::OnEquipmentSlotHover(UObsidianItemSlot_Equipment* AffectedSlot, const bool bEntered)
{
	if(bEntered)
	{
		if(!InventoryWidgetController || !InventoryWidgetController->IsDraggingAnItem())
		{
			return;
		}
		
		const bool bCanEquip = InventoryWidgetController->CanEquipDraggedItem(AffectedSlot->GetSlotTag());
		AffectedSlot->SetSlotState(bCanEquip);
		return;
	}
	AffectedSlot->ResetSlot();
}

void UObsidianInventory::OnEquipmentSlotMouseButtonDown(const UObsidianItemSlot_Equipment* AffectedSlot)
{
	if(InventoryWidgetController)
	{
		InventoryWidgetController->RequestEquippingItem(AffectedSlot->GetSlotTag());
	}
}
