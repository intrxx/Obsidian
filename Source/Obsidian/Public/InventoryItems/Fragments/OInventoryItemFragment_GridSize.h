// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "InventoryItems/ObsidianInventoryItemFragment.h"
#include "OInventoryItemFragment_GridSize.generated.h"

UENUM(BlueprintType)
enum class EObsidianInventoryItemGridSize : uint8
{
	/** Single 1x1 Square Item size. */
	IIGS_SingleSquare = 0 UMETA(DisplayName = "Single Square"),

	/** Two Square 1x2 Item size. */
	IIGS_TwoSquares_Vertical UMETA(DisplayName = "Two Squares - Vertical"),

	/** Two Square 2x1 Item size. */
	IIGS_TwoSquares_Horizontal UMETA(DisplayName = "Two Squares - Horizontal"),

	/** Three Square 1x3 Item size. */
	IIGS_ThreeSquares_Vertical UMETA(DisplayName = "Three Squares - Vertical"),

	/** Three Square 3x1 Item size. */
	IIGS_ThreeSquares_Horizontal UMETA(DisplayName = "Three Squares - Horizontal"),

	/** Four Square 1x4 Item size. */
	IIGS_FourSquares_Vertical UMETA(DisplayName = "Four Squares - Vertical"),

	/** Four Square 4x1 Item size. */
	IIGS_FourSquares_Horizontal UMETA(DisplayName = "Four Squares - Horizontal"),

	/** Four Square 2x2 Item size. */
	IIGS_FourSquares_Square UMETA(DisplayName = "Four Squares - Square"),
	
	/** Six Square 2x3 Item size. */
	IIGS_SixSquares_VerticalRectangle UMETA(DisplayName = "Six Squares Vertical Rectangle"),

	/** Eight Square 2x4 Item size. */
	IIGS_EightSquares_VerticalRectangle UMETA(DisplayName = "Eight Squares - Vertical Rectangle")
};

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UOInventoryItemFragment_GridSize : public UObsidianInventoryItemFragment
{
	GENERATED_BODY()

public:
	//~ Start of UObsidianInventoryItemFragment
	virtual void OnInstancedCreated(UObsidianInventoryItemInstance* Instance) const override;
	//~ End of UObsidianInventoryItemFragment

protected:
	/** Enum that represents the items predefined grid size, will be later mapped to the actual TArray<FVector2D> containing the 'size'. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|GridSize")
	EObsidianInventoryItemGridSize InventoryItemGridSizeDesc = EObsidianInventoryItemGridSize::IIGS_SingleSquare;
};
