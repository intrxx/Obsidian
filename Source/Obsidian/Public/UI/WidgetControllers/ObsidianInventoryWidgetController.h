// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetControllerBase.h"
#include "ObsidianInventoryWidgetController.generated.h"

class UObsidianItemWidget;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnItemAutomaticallyAddedSignature, UTexture2D* ItemImage, FVector2D DesiredPosition, FVector2D GridSpan);

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianInventoryWidgetController : public UObsidianHeroWidgetControllerBase
{
	GENERATED_BODY()

public:
	//~ Start of UObsidianWidgetController
	virtual void OnWidgetControllerSetupCompleted() override;
	//~ End of UObsidianWidgetController

	void OnItemAdded(UObsidianInventoryItemInstance* ItemInstance, FVector2D DesiredPosition);

	void OnInventoryOpen();

public:
	FOnItemAutomaticallyAddedSignature OnItemAutomaticallyAddedDelegate;
	
private:
	TMap<FVector2D, UObsidianInventoryItemInstance*> GridLocationToItemMap;
};
