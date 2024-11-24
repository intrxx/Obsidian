// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetControllerBase.h"
#include "ObsidianInventoryWidgetController.generated.h"

class UObsidianItemWidget;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAutomaticallyAddedSignature, TSubclassOf<UObsidianItemWidget> ItemWidgetClass);

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

	void OnItemAdded(UObsidianInventoryItemInstance* ItemInstance);

public:
	FOnItemAutomaticallyAddedSignature OnItemAutomaticallyAddedDelegate;
};
