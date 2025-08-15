// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "UI/ObsidianWidgetBase.h"
#include "ObsidianStashTabWidget.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianStashTabWidget : public UObsidianWidgetBase
{
	GENERATED_BODY()
public:
	void ShowStashTab();
	void HideStashTab();

	bool IsTabActive();
	
protected:
	virtual void NativeConstruct() override;

	bool bActive = false;
};
