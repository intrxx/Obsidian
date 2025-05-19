// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetBase.h"
#include "GameplayTagContainer.h"
#include "ObsidianSlotBlockadeItem.generated.h"

class USizeBox;
class UImage;
class UObsidianItemSlot_Equipment;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianSlotBlockadeItem : public UObsidianWidgetBase
{
	GENERATED_BODY()
	
public:
	void InitializeItemWidget(const FGameplayTag& EquipmentSlot, const FVector2D& ItemGridSpan, UTexture2D* ItemImage);

	FGameplayTag GetEquipmentSlotTag() const
	{
		return ItemEquipmentSlot;
	}

	void SetOwningSlot(UObsidianItemSlot_Equipment* InSlot)
	{
		OwningSlot = InSlot;
	}

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
protected:
	/** Opacity to set for this blocking item. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	float ItemOpacity = 0.5f;
	
	UPROPERTY()
	FGameplayTag ItemEquipmentSlot = FGameplayTag::EmptyTag;

	UPROPERTY()
	TObjectPtr<UObsidianItemSlot_Equipment> OwningSlot;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Item_Image;
};
