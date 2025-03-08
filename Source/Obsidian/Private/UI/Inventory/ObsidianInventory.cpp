// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Inventory/ObsidianInventory.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/SizeBox.h"
#include "UI/Inventory/SubWidgets/ObsidianItemSlot.h"
#include "UI/Inventory/ObsidianItem.h"
#include "UI/Inventory/SubWidgets/ObsidianItemSlot_Equipment.h"
#include "UI/Inventory/SubWidgets/ObsidianItemSlot_Inventory.h"
#include "UI/WidgetControllers/ObsidianInventoryWidgetController.h"

UObsidianInventory::UObsidianInventory()
	: InventoryGridSize(InventoryGridWidth * InventoryGridHeight)
{
}

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

	InventoryWidgetController->OnItemAddedDelegate.AddUObject(this, &ThisClass::OnItemAdded);
	InventoryWidgetController->OnItemChangedDelegate.AddUObject(this, &ThisClass::OnItemChanged);
}

void UObsidianInventory::SetupInventoryGrid() 
{
	if(Slots_GridPanel->HasAnyChildren())
	{
		Slots_GridPanel->ClearChildren(); 
	}
	
	int16 GridX = 0;
	int16 GridY = 0;
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

	OnHoverOverInventorySlotDelegate.AddUObject(this, &ThisClass::OnInventorySlotHover);
	OnMouseButtonDownOnInventorySlotDelegate.AddUObject(this, &ThisClass::OnInventorySlotMouseButtonDown);
}

void UObsidianInventory::SetupEquipmentSlots()
{
	if(RightHand_EquipmentSlot && LeftHand_EquipmentSlot)
	{
		RightHand_EquipmentSlot->InitializeSlot(this);
		LeftHand_EquipmentSlot->InitializeSlot(this);
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

	OnHoverOverEquipmentSlotDelegate.AddUObject(this, &ThisClass::OnEquipmentSlotHover);
	OnMouseButtonDownOnEquipmentSlotDelegate.AddUObject(this, &ThisClass::OnEquipmentSlotMouseButtonDown);
}

void UObsidianInventory::OnItemAdded(const FObsidianItemVisuals& ItemVisuals)
{
	const FVector2D DesiredPosition = ItemVisuals.DesiredPosition;
	const FVector2D GridSpan = ItemVisuals.GridSpan;
	
	checkf(InventorySlotClass, TEXT("Tried to create widget without valid widget class in UObsidianInventory::OnItemAdded, fill it in ObsidianInventory instance."));
	UObsidianItem* ItemWidget = CreateWidget<UObsidianItem>(this, ItemWidgetClass);
	ItemWidget->InitializeItemWidget(DesiredPosition, GridSpan, ItemVisuals.ItemImage, ItemVisuals.StackCount);
	ItemWidget->OnItemLeftMouseButtonPressedDelegate.AddUObject(this, &ThisClass::OnItemLeftMouseButtonPressed);
	ItemWidget->OnItemMouseEnterDelegate.AddUObject(this, &ThisClass::OnItemMouseEntered);
	ItemWidget->OnItemMouseLeaveDelegate.AddUObject(this, &ThisClass::OnItemMouseLeave);
	InventoryWidgetController->AddItemWidget(DesiredPosition, ItemWidget);

	UGridSlot* GridSlot = Slots_GridPanel->AddChildToGrid(ItemWidget, DesiredPosition.Y, DesiredPosition.X);
	GridSlot->SetLayer(1);
	GridSlot->SetColumnSpan(GridSpan.X);
	GridSlot->SetRowSpan(GridSpan.Y);
}

void UObsidianInventory::OnItemChanged(const FObsidianItemVisuals& ItemVisuals)
{
	const FVector2D ItemPosition = ItemVisuals.DesiredPosition;
	if(UObsidianItem* ItemWidget = InventoryWidgetController->GetItemWidgetAtLocation(ItemPosition))
	{
		ItemWidget->OverrideCurrentStackCount(ItemVisuals.StackCount);
	}
}

void UObsidianInventory::OnItemLeftMouseButtonPressed(const FVector2D& ItemDesiredPosition, UObsidianItem* ItemWidget, const bool bShiftDown)
{
	if(bShiftDown)
	{
		InventoryWidgetController->HandleLeftClickingOnAnItemWithShiftDown(ItemDesiredPosition, ItemWidget);
		return;
	}
	InventoryWidgetController->HandleLeftClickingOnAnItem(ItemDesiredPosition, ItemWidget);
}

void UObsidianInventory::OnItemMouseEntered(const FVector2D& ItemDesiredPosition, UObsidianItem* ItemWidget)
{
	InventoryWidgetController->HandleHoveringOverItem(ItemDesiredPosition, ItemWidget);
}

void UObsidianInventory::OnItemMouseLeave(const FVector2D& ItemDesiredPosition)
{
	InventoryWidgetController->HandleUnhoveringItem(ItemDesiredPosition);
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
		
		//TODO Check if can equip item, display color on the slot based on result
		return;
	}
	
	AffectedSlot->ResetSlot();
}

void UObsidianInventory::OnEquipmentSlotMouseButtonDown(const UObsidianItemSlot_Equipment* AffectedSlot, const bool bShiftDown)
{
	//TODO Request equipping item
}
