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

void UObsidianSlotBlockadeItem::InitializeItemWidget(const FGameplayTag& EquipmentSlot, const FGameplayTag& InPrimaryWeaponSlot, const FVector2D& ItemGridSpan, UTexture2D* ItemImage)
{
	Root_SizeBox->SetWidthOverride(ItemGridSpan.X * ObsidianInventoryItemsStatics::InventorySlotSize.X);
	Root_SizeBox->SetHeightOverride(ItemGridSpan.Y * ObsidianInventoryItemsStatics::InventorySlotSize.Y);
	Item_Image->SetBrushFromTexture(ItemImage);
	ItemEquipmentSlot = EquipmentSlot;
	PrimaryWeaponSlot = InPrimaryWeaponSlot;
}

FReply UObsidianSlotBlockadeItem::NativeOnMouseButtonDown(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
	if(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		OnSlotBlockadeItemLeftMouseButtonPressedDelegate.Broadcast(this);
	}
	
	return FReply::Handled();
}

void UObsidianSlotBlockadeItem::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnSlotBlockadeItemMouseEnterDelegate.Broadcast(this);
}

void UObsidianSlotBlockadeItem::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	OnSlotBlockadeItemMouseLeaveDelegate.Broadcast();
}




