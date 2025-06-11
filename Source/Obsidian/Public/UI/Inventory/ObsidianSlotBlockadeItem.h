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

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSlotBlockadeItemLeftMouseButtonPressedSignature, const UObsidianSlotBlockadeItem* ItemWidget);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSlotBlockadeItemMouseEnterSignature, const UObsidianSlotBlockadeItem* ItemWidget);
DECLARE_MULTICAST_DELEGATE(FOnSlotBlockadeItemMouseLeaveSignature);

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianSlotBlockadeItem : public UObsidianWidgetBase
{
	GENERATED_BODY()
	
public:
	void InitializeItemWidget(const FGameplayTag& EquipmentSlot, const FGameplayTag& InPrimaryWeaponSlot, const FVector2D& ItemGridSpan, UTexture2D* ItemImage);

	FGameplayTag GetEquipmentSlotTag() const
	{
		return ItemEquipmentSlot;
	}

	FGameplayTag GetPrimaryWeaponSlotTag() const
	{
		return PrimaryWeaponSlot;
	}

	void SetOwningSlot(UObsidianItemSlot_Equipment* InSlot)
	{
		OwningSlot = InSlot;
	}
	
	UObsidianItemSlot_Equipment* GetOwningSlot() const
	{
		return OwningSlot;
	}
	
public:
	FOnSlotBlockadeItemLeftMouseButtonPressedSignature OnSlotBlockadeItemLeftMouseButtonPressedDelegate;
	FOnSlotBlockadeItemMouseEnterSignature OnSlotBlockadeItemMouseEnterDelegate;
	FOnSlotBlockadeItemMouseLeaveSignature OnSlotBlockadeItemMouseLeaveDelegate;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
protected:
	/** Opacity to set for this blocking item. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	float ItemOpacity = 0.5f;
	
	UPROPERTY()
	FGameplayTag ItemEquipmentSlot = FGameplayTag::EmptyTag;
	UPROPERTY()
	FGameplayTag PrimaryWeaponSlot = FGameplayTag::EmptyTag;

	UPROPERTY()
	TObjectPtr<UObsidianItemSlot_Equipment> OwningSlot;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Item_Image;
};
