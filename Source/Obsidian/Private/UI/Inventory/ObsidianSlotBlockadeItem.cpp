// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Inventory/ObsidianSlotBlockadeItem.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "ObsidianTypes/ObsidianItemTypes.h"
#include "UI/Inventory/SubWidgets/ObsidianItemSlot_Equipment.h"

void UObsidianSlotBlockadeItem::NativeConstruct()
{
	Super::NativeConstruct();

	SetRenderOpacity(ItemOpacity);
}

void UObsidianSlotBlockadeItem::NativeDestruct()
{
	if(OwningSlot)
	{
		OwningSlot->SetSlotState(ISS_Neutral);
	}
	
	Super::NativeDestruct();
}

void UObsidianSlotBlockadeItem::InitializeItemWidget(const FGameplayTag& EquipmentSlot, const FVector2D& ItemGridSpan, UTexture2D* ItemImage)
{
	Root_SizeBox->SetWidthOverride(ItemGridSpan.X * ObsidianInventoryItemsStatics::InventorySlotSize.X);
	Root_SizeBox->SetHeightOverride(ItemGridSpan.Y * ObsidianInventoryItemsStatics::InventorySlotSize.Y);
	Item_Image->SetBrushFromTexture(ItemImage);
	ItemEquipmentSlot = EquipmentSlot;
}


