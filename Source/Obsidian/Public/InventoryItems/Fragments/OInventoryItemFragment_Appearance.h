// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "InventoryItems/ObsidianInventoryItemFragment.h"
#include "OInventoryItemFragment_Appearance.generated.h"

class UObsidianItemWidget;

/**
 * 
 */
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
UCLASS(DisplayName="Appearance")
class OBSIDIAN_API UOInventoryItemFragment_Appearance : public UObsidianInventoryItemFragment
{
	GENERATED_BODY()

public:
	virtual void OnInstancedCreated(UObsidianInventoryItemInstance* Instance) const override;
	
	FIntPoint GetItemGridSpanFromDesc() const;
	
	UTexture2D* GetItemImage() const
	{
		return ItemImage;
	}
	
	UStaticMesh* GetItemDroppedMesh() const
	{
		return DroppedMesh;
	}

	FText GetItemDisplayName() const
	{
		return DisplayName;
	}

	FText GetItemDescription() const
	{
		return Description;
	}

	FText GetItemAdditionalDescription() const
	{
		return AdditionalDescription;
	}

	float GetItemSlotPadding() const
	{
		return ItemSlotPadding;
	}
	
protected:
	/** Enum that represents the items predefined grid size, will be later mapped to the actual TArray<FVector2D> containing the 'size'. */
	UPROPERTY(EditDefaultsOnly, Category = "Grid")
	EObsidianInventoryItemGridSize InventoryItemGridSizeDesc = EObsidianInventoryItemGridSize::IIGS_SingleSquare;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance")
	TObjectPtr<UStaticMesh> DroppedMesh;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance")
	TObjectPtr<UTexture2D> ItemImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance")
	FText DisplayName = FText::GetEmpty();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance")
	FText Description = FText::GetEmpty();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance")
	FText AdditionalDescription = FText::GetEmpty();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid")
	float ItemSlotPadding = 4.0f;
};
