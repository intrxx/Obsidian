// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// ~ Project


#include "UI/ObsidianWidgetBase.h"
#include "ObsidianSlotBlockadeItem.generated.h"

class USizeBox;
class UImage;
class UObsidianItemSlot_Equipment;
class UObsidianSlotBlockadeItem;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianSlotBlockadeItem : public UObsidianWidgetBase
{
	GENERATED_BODY()
	
public:
	void InitializeItemWidget(const FIntPoint& ItemGridSpan, UTexture2D* ItemImage, const bool bIsForSwapSlot);
	
	void SetOwningSlot(UObsidianItemSlot_Equipment* InSlot)
	{
		OwningSlot = InSlot;
	}
	
	UObsidianItemSlot_Equipment* GetOwningSlot() const
	{
		return OwningSlot;
	}

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	/** Opacity to set for this blocking item. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	float ItemOpacity = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	float SwapSlotSizeMultiplier = 0.5f;
	
	UPROPERTY()
	TObjectPtr<UObsidianItemSlot_Equipment> OwningSlot;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Item_Image;
};
