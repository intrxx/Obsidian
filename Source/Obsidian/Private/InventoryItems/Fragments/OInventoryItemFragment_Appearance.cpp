// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"

// ~ Core

// ~ Project
#include "InventoryItems/ObsidianInventoryItemInstance.h"

struct FObsidianInventoryItemGridSize
{
	FIntPoint SingleSquare_GridSpan;
	FIntPoint TwoSquares_Vertical_GridSpan;
	FIntPoint TwoSquares_Horizontal_GridSpan;
	FIntPoint ThreeSquares_Vertical_GridSpan;
	FIntPoint ThreeSquares_Horizontal_GridSpan;
	FIntPoint FourSquares_Vertical_GridSpan;
	FIntPoint FourSquares_Horizontal_GridSpan;
	FIntPoint FourSquares_Square_GridSpan;
	FIntPoint SixSquares_VerticalRectangle_GridSpan;
	FIntPoint EightSquares_VerticalRectangle_GridSpan;
	
	TMap<EObsidianInventoryItemGridSize, FIntPoint> ItemGridSpanMap;

	FObsidianInventoryItemGridSize()
	{
		SingleSquare_GridSpan = FIntPoint(1, 1);
		TwoSquares_Vertical_GridSpan = FIntPoint(1, 2);
		TwoSquares_Horizontal_GridSpan = FIntPoint(2, 1);
		ThreeSquares_Vertical_GridSpan = FIntPoint(1, 3);
		ThreeSquares_Horizontal_GridSpan = FIntPoint(3, 1);
		FourSquares_Vertical_GridSpan = FIntPoint(1, 4);
		FourSquares_Horizontal_GridSpan = FIntPoint(4, 1);
		FourSquares_Square_GridSpan = FIntPoint(2, 2);
		SixSquares_VerticalRectangle_GridSpan = FIntPoint(2, 3);
		EightSquares_VerticalRectangle_GridSpan = FIntPoint(2, 4);

		ItemGridSpanMap =
		{
			{EObsidianInventoryItemGridSize::IIGS_SingleSquare, SingleSquare_GridSpan},
			{EObsidianInventoryItemGridSize::IIGS_TwoSquares_Vertical, TwoSquares_Vertical_GridSpan},
			{EObsidianInventoryItemGridSize::IIGS_TwoSquares_Horizontal, TwoSquares_Horizontal_GridSpan},
			{EObsidianInventoryItemGridSize::IIGS_ThreeSquares_Vertical, ThreeSquares_Vertical_GridSpan},
			{EObsidianInventoryItemGridSize::IIGS_ThreeSquares_Horizontal, ThreeSquares_Horizontal_GridSpan},
			{EObsidianInventoryItemGridSize::IIGS_FourSquares_Vertical, FourSquares_Vertical_GridSpan},
			{EObsidianInventoryItemGridSize::IIGS_FourSquares_Horizontal, FourSquares_Horizontal_GridSpan},
			{EObsidianInventoryItemGridSize::IIGS_FourSquares_Square, FourSquares_Square_GridSpan},
			{EObsidianInventoryItemGridSize::IIGS_SixSquares_VerticalRectangle, SixSquares_VerticalRectangle_GridSpan},
			{EObsidianInventoryItemGridSize::IIGS_EightSquares_VerticalRectangle, EightSquares_VerticalRectangle_GridSpan}
		};
	}
};

static const FObsidianInventoryItemGridSize& ObsidianItemGridSize()
{
	static FObsidianInventoryItemGridSize InventoryItemGridSize;
	return InventoryItemGridSize;
}

void UOInventoryItemFragment_Appearance::OnInstancedCreated(UObsidianInventoryItemInstance* Instance) const
{
	if(Instance)
	{
		const FIntPoint GridSpan = GetItemGridSpanFromDesc();
		
		Instance->SetItemImage(ItemImage);
		Instance->SetItemDroppedMesh(DroppedMesh);
		Instance->SetItemDisplayName(DisplayName);
		Instance->SetItemGridSpan(GridSpan);
		Instance->SetItemDescription(Description);
		Instance->SetItemAdditionalDescription(AdditionalDescription);
		Instance->SetItemSlotPadding(ItemSlotPadding);
	}
}

FIntPoint UOInventoryItemFragment_Appearance::GetItemGridSpanFromDesc() const
{
	return ObsidianItemGridSize().ItemGridSpanMap[InventoryItemGridSizeDesc];
}
