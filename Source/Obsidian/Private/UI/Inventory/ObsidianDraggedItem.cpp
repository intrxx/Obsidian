// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Inventory/ObsidianDraggedItem.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_GridSize.h"

void UObsidianDraggedItem::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetAlignmentInViewport(FVector2D(0.5f));
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UObsidianDraggedItem::InitializeItemWidget(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const int32 Stacks)
{
	if(ItemDef == nullptr)
	{
		FFrame::KismetExecutionMessage(TEXT("Provided ItemDef is invalid in UObsidianDraggedItem::InitializeItemWidget."), ELogVerbosity::Error);
		return;
	}
	
	const UOInventoryItemFragment_Appearance* AppearanceFragment = Cast<UOInventoryItemFragment_Appearance>(
			ItemDef.GetDefaultObject()->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass()));
	const UOInventoryItemFragment_GridSize* GridSizeFragment = Cast<UOInventoryItemFragment_GridSize>(
			ItemDef.GetDefaultObject()->FindFragmentByClass(UOInventoryItemFragment_GridSize::StaticClass()));

	if(AppearanceFragment && GridSizeFragment)
	{
		const FVector2D ItemGridSpan = GridSizeFragment->GetItemGridSpanFromDesc();
		Root_SizeBox->SetWidthOverride(ItemGridSpan.X * WidthConstant);
		Root_SizeBox->SetHeightOverride(ItemGridSpan.Y * HeightConstant);

		SetDesiredSizeInViewport(ItemGridSpan * WidthConstant);
		
		UTexture2D* ItemImage = AppearanceFragment->ItemImage;
		Item_Image->SetBrushFromTexture(ItemImage);
	}
}