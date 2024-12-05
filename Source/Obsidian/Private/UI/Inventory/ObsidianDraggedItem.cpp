// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Inventory/ObsidianDraggedItem.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"

void UObsidianDraggedItem::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetAlignmentInViewport(FVector2D(0.5f));
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UObsidianDraggedItem::InitializeItemWidgetWithItemDef(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const int32 Stacks)
{
	if(ItemDef == nullptr)
	{
		FFrame::KismetExecutionMessage(TEXT("Provided ItemDef is invalid in UObsidianDraggedItem::InitializeItemWidgetWithItemDef."), ELogVerbosity::Error);
		return;
	}

	InternalItemDef = ItemDef;
	InternalStacks = Stacks;
	
	const UOInventoryItemFragment_Appearance* AppearanceFragment = Cast<UOInventoryItemFragment_Appearance>(
			ItemDef.GetDefaultObject()->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass()));

	if(AppearanceFragment)
	{
		const FVector2D ItemGridSpan = AppearanceFragment->GetItemGridSpanFromDesc();
		Root_SizeBox->SetWidthOverride(ItemGridSpan.X * WidthConstant);
		Root_SizeBox->SetHeightOverride(ItemGridSpan.Y * HeightConstant);

		SetDesiredSizeInViewport(ItemGridSpan * WidthConstant);
		
		UTexture2D* ItemImage = AppearanceFragment->GetItemImage();
		Item_Image->SetBrushFromTexture(ItemImage);
	}
}

void UObsidianDraggedItem::InitializeItemWidgetWithItemInstance(UObsidianInventoryItemInstance* ItemInstance)
{
	if(ItemInstance == nullptr)
	{
		FFrame::KismetExecutionMessage(TEXT("Provided ItemInstance is invalid in UObsidianDraggedItem::InitializeItemWidgetWithItemInstance."), ELogVerbosity::Error);
		return;
	}

	InternalItemInstance = ItemInstance;

	const FVector2D ItemGridSpan = InternalItemInstance->GetItemGridSpan();
	Root_SizeBox->SetWidthOverride(ItemGridSpan.X * WidthConstant);
	Root_SizeBox->SetHeightOverride(ItemGridSpan.Y * HeightConstant);

	SetDesiredSizeInViewport(ItemGridSpan * WidthConstant);

	UTexture2D* ItemImage = InternalItemInstance->GetItemImage();
	Item_Image->SetBrushFromTexture(ItemImage);
}
