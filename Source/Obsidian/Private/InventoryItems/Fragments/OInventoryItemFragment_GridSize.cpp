// Copyright 2024 out of sCope team - Michał Ogiński


#include "InventoryItems/Fragments/OInventoryItemFragment_GridSize.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"

struct FObsidianInventoryItemGridSize
{
	TArray<FVector2D> SingleSquare;
	TArray<FVector2D> TwoSquares_Vertical;
	TArray<FVector2D> TwoSquares_Horizontal;
	TArray<FVector2D> ThreeSquares_Vertical;
	TArray<FVector2D> ThreeSquares_Horizontal;
	TArray<FVector2D> FourSquares_Vertical;
	TArray<FVector2D> FourSquares_Horizontal;
	TArray<FVector2D> FourSquares_Square;
	TArray<FVector2D> SixSquares_VerticalRectangle;
	TArray<FVector2D> EightSquares_VerticalRectangle;
	
	TMap<EObsidianInventoryItemGridSize, TArray<FVector2D>> ItemGridSizeMap;

	FObsidianInventoryItemGridSize()
	{
		SingleSquare =
		{
			FVector2D(0.0f, 0.0f)
		};
		TwoSquares_Vertical =
		{
			FVector2D(0.0f, 0.0f),
			FVector2D(0.0f, 1.0f)
		};
		TwoSquares_Horizontal =
		{
			FVector2D(0.0f, 0.0f),
			FVector2D(1.0f, 0.0f)
		};
		ThreeSquares_Vertical =
		{
			FVector2D(0.0f, 0.0f),
			FVector2D(0.0f, 1.0f),
			FVector2D(0.0f, 2.0f)
		};
		ThreeSquares_Horizontal =
		{
			FVector2D(0.0f, 0.0f),
			FVector2D(1.0f, 0.0f),
			FVector2D(2.0f, 0.0f)
		};
		FourSquares_Vertical =
		{
			FVector2D(0.0f, 0.0f),
			FVector2D(0.0f, 1.0f),
			FVector2D(0.0f, 2.0f),
			FVector2D(0.0f, 3.0f)
		};
		FourSquares_Horizontal =
		{
			FVector2D(0.0f, 0.0f),
			FVector2D(1.0f, 0.0f),
			FVector2D(2.0f, 0.0f),
			FVector2D(3.0f, 0.0f)
		};
		FourSquares_Square =
		{
			FVector2D(0.0f, 0.0f),
			FVector2D(1.0f, 0.0f),
			FVector2D(0.0f, 1.0f),
			FVector2D(1.0f, 1.0f)
		};
		SixSquares_VerticalRectangle =
		{
			FVector2D(0.0f, 0.0f),
			FVector2D(1.0f, 0.0f),
			FVector2D(0.0f, 1.0f),
			FVector2D(1.0f, 1.0f),
			FVector2D(0.0f, 2.0f),
			FVector2D(1.0f, 2.0f)
		};
		EightSquares_VerticalRectangle =
		{
			FVector2D(0.0f, 0.0f),
			FVector2D(1.0f, 0.0f),
			FVector2D(0.0f, 1.0f),
			FVector2D(1.0f, 1.0f),
			FVector2D(0.0f, 2.0f),
			FVector2D(1.0f, 2.0f),
			FVector2D(0.0f, 3.0f),
			FVector2D(2.0f, 3.0f)
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
	}
};

static const FObsidianInventoryItemGridSize& ObsidianItemGridSize()
{
	static FObsidianInventoryItemGridSize InventoryItemGridSize;
	return InventoryItemGridSize;
}

void UOInventoryItemFragment_GridSize::OnInstancedCreated(UObsidianInventoryItemInstance* Instance) const
{
	if(Instance)
	{
		Instance->SetItemGridSize(GetItemGridSizeFromDesc());
	}
}

TArray<FVector2D> UOInventoryItemFragment_GridSize::GetItemGridSizeFromDesc() const
{
	return ObsidianItemGridSize().ItemGridSizeMap[InventoryItemGridSizeDesc];
}
