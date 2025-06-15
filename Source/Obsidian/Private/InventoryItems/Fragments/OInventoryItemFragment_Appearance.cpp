// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"

// ~ Core

// ~ Project
#include "InventoryItems/ObsidianInventoryItemInstance.h"

struct FObsidianInventoryItemGridSize
{
	TArray<FIntPoint> SingleSquare;
	TArray<FIntPoint> TwoSquares_Vertical;
	TArray<FIntPoint> TwoSquares_Horizontal;
	TArray<FIntPoint> ThreeSquares_Vertical;
	TArray<FIntPoint> ThreeSquares_Horizontal;
	TArray<FIntPoint> FourSquares_Vertical;
	TArray<FIntPoint> FourSquares_Horizontal;
	TArray<FIntPoint> FourSquares_Square;
	TArray<FIntPoint> SixSquares_VerticalRectangle;
	TArray<FIntPoint> EightSquares_VerticalRectangle;

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
	
	TMap<EObsidianInventoryItemGridSize, TArray<FIntPoint>> ItemGridSizeMap;
	TMap<EObsidianInventoryItemGridSize, FIntPoint> ItemGridSpanMap;

	FObsidianInventoryItemGridSize()
	{
		SingleSquare =
		{
			FIntPoint(0, 0)
		};
		TwoSquares_Vertical =
		{
			FIntPoint(0, 0),
			FIntPoint(0, 1)
		};
		TwoSquares_Horizontal =
		{
			FIntPoint(0, 0),
			FIntPoint(1, 0)
		};
		ThreeSquares_Vertical =
		{
			FIntPoint(0, 0),
			FIntPoint(0, 1),
			FIntPoint(0, 2)
		};
		ThreeSquares_Horizontal =
		{
			FIntPoint(0, 0),
			FIntPoint(1, 0),
			FIntPoint(2, 0)
		};
		FourSquares_Vertical =
		{
			FIntPoint(0, 0),
			FIntPoint(0, 1),
			FIntPoint(0, 2),
			FIntPoint(0, 3)
		};
		FourSquares_Horizontal =
		{
			FIntPoint(0, 0),
			FIntPoint(1, 0),
			FIntPoint(2, 0),
			FIntPoint(3, 0)
		};
		FourSquares_Square =
		{
			FIntPoint(0, 0),
			FIntPoint(1, 0),
			FIntPoint(0, 1),
			FIntPoint(1, 1)
		};
		SixSquares_VerticalRectangle =
		{
			FIntPoint(0, 0),
			FIntPoint(1, 0),
			FIntPoint(0, 1),
			FIntPoint(1, 1),
			FIntPoint(0, 2),
			FIntPoint(1, 2)
		};
		EightSquares_VerticalRectangle =
		{
			FIntPoint(0, 0),
			FIntPoint(1, 0),
			FIntPoint(0, 1),
			FIntPoint(1, 1),
			FIntPoint(0, 2),
			FIntPoint(1, 2),
			FIntPoint(0, 3),
			FIntPoint(1, 3)
		};
		
		ItemGridSizeMap =
		{
			{EObsidianInventoryItemGridSize::IIGS_SingleSquare, SingleSquare},
			{EObsidianInventoryItemGridSize::IIGS_TwoSquares_Vertical, TwoSquares_Vertical},
			{EObsidianInventoryItemGridSize::IIGS_TwoSquares_Horizontal, TwoSquares_Horizontal},
			{EObsidianInventoryItemGridSize::IIGS_ThreeSquares_Vertical, ThreeSquares_Vertical},
			{EObsidianInventoryItemGridSize::IIGS_ThreeSquares_Horizontal, ThreeSquares_Horizontal},
			{EObsidianInventoryItemGridSize::IIGS_FourSquares_Vertical, FourSquares_Vertical},
			{EObsidianInventoryItemGridSize::IIGS_FourSquares_Horizontal, FourSquares_Horizontal},
			{EObsidianInventoryItemGridSize::IIGS_FourSquares_Square, FourSquares_Square},
			{EObsidianInventoryItemGridSize::IIGS_SixSquares_VerticalRectangle, SixSquares_VerticalRectangle},
			{EObsidianInventoryItemGridSize::IIGS_EightSquares_VerticalRectangle, EightSquares_VerticalRectangle}
		};
		
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
		const TArray<FIntPoint> GridSize = GetItemGridSizeFromDesc();
		const FIntPoint GridSpan = GetItemGridSpanFromDesc();
		
		Instance->SetItemImage(ItemImage);
		Instance->SetItemDroppedMesh(DroppedMesh);
		Instance->SetItemDisplayName(DisplayName);
		Instance->SetItemGridSize(GridSize);
		Instance->SetItemGridSpan(GridSpan);
		Instance->SetItemDescription(Description);
		Instance->SetItemAdditionalDescription(AdditionalDescription);
		Instance->SetItemSlotPadding(ItemSlotPadding);
	}
}

TArray<FIntPoint> UOInventoryItemFragment_Appearance::GetItemGridSizeFromDesc() const
{
	return ObsidianItemGridSize().ItemGridSizeMap[InventoryItemGridSizeDesc];
}

FIntPoint UOInventoryItemFragment_Appearance::GetItemGridSpanFromDesc() const
{
	return ObsidianItemGridSize().ItemGridSpanMap[InventoryItemGridSizeDesc];
}
