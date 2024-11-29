// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetBase.h"
#include "ObsidianInventorySlot.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHoverOverSlotSignature)

class UImage;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianInventorySlot : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ItemImage_Image;

	FOnHoverOverSlotSignature OnHoverOverSlotDelegate;

protected:
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
};
