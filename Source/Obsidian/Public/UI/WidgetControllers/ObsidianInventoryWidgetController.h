// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetControllerBase.h"
#include "ObsidianInventoryWidgetController.generated.h"

class UObsidianItemWidget;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnItemAutomaticallyAddedSignature, UTexture2D* ItemImage, const FVector2D DesiredPosition, const FVector2D GridSpan);

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianInventoryWidgetController : public UObsidianHeroWidgetControllerBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Obsidian|InventoryWidgetController")
	void SetInventoryOpened(const bool bInOpened)
	{
		bInventoryOpened = bInOpened;
	}
	
	//~ Start of UObsidianWidgetController
	virtual void OnWidgetControllerSetupCompleted() override;
	//~ End of UObsidianWidgetController

	void OnItemAdded(UObsidianInventoryItemInstance* ItemInstance, const FVector2D DesiredPosition);

	void OnInventoryOpen();

public:
	FOnItemAutomaticallyAddedSignature OnItemAutomaticallyAddedDelegate;
	
private:
	TMap<FVector2D, UObsidianInventoryItemInstance*> GridLocationToItemMap;

	bool bInventoryOpened = false;
};