// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetControllerBase.h"
#include "ObsidianInventoryWidgetController.generated.h"

class UObsidianHeroComponent;
class UObsidianItemWidget;
class UObsidianItem;
class UObsidianDraggedItem;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnItemAddedSignature, UTexture2D* ItemImage, const FVector2D DesiredPosition, const FVector2D GridSpan);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class OBSIDIAN_API UObsidianInventoryWidgetController : public UObsidianHeroWidgetControllerBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Obsidian|InventoryWidgetController")
	void SetInventoryOpened(const bool bInOpened)
	{
		bInventoryOpened = bInOpened;
	}
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|InventoryWidgetController")
	bool GetInventoryOpened() const
	{
		return bInventoryOpened;
	}

	UObsidianHeroComponent* GetHeroComponent()
	{
		return InternalHeroComponent;
	}

	bool IsDraggingAnItem() const;
	FVector2D GetDraggedItemGridSpan() const;

	void AddItemWidget(const FVector2D& Location, UObsidianItem* ItemWidget);
	void RemoveItemWidget(const FVector2D& Location);
	
	//~ Start of UObsidianWidgetController
	virtual void OnWidgetControllerSetupCompleted() override;
	//~ End of UObsidianWidgetController

	void OnItemAdded(UObsidianInventoryItemInstance* ItemInstance, const FVector2D DesiredPosition);
	void OnInventoryOpen();

	void RequestAddingItemToInventory(const FVector2D& SlotPosition);
	void RequestPickingUpItemFromInventory(const FVector2D& SlotPosition);

public:
	FOnItemAddedSignature OnItemAddedDelegate;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UObsidianDraggedItem> DraggedItemWidgetClass;
	
private:
	bool bInventoryOpened = false;

	UPROPERTY()
	TObjectPtr<UObsidianInventoryComponent> InternalInventoryComponent;

	UPROPERTY()
	TObjectPtr<UObsidianHeroComponent> InternalHeroComponent;
	
	UPROPERTY()
	TMap<FVector2D, UObsidianItem*> AddedItemWidgetMap;
};
