// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianMainOverlayWidgetBase.h"
#include "ObsidianInventory.generated.h"

class UUniformGridPanel;
/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianInventory : public UObsidianMainOverlayWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUniformGridPanel> Slots_GridPanel;
};
