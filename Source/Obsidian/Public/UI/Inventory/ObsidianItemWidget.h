// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetBase.h"
#include "ObsidianItemWidget.generated.h"

class USizeBox;
class UImage;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemLeftMouseButtonPressedSignature, const FVector2D ItemDesiredPosition)

/**
 * Item Widget that is displayed in the inventory.
 */
UCLASS()
class OBSIDIAN_API UObsidianItemWidget : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	void InitializeItemWidget(const FVector2D& DesiredPosition, const FVector2D& ItemGridSpan, UTexture2D* ItemImage);
	
public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Item_Image;

	/** I don't know the better way for know, we use it to notify the slot that the item went here even if the item image does not overlap with the slot. */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Visibility_Image;

	FOnItemLeftMouseButtonPressedSignature OnItemLeftMouseButtonPressedDelegate;
	
protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	float WidthConstant = 64.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	float HeightConstant = 64.0f;

	FVector2D ItemDesiredPosition = FVector2D::Zero();
};
