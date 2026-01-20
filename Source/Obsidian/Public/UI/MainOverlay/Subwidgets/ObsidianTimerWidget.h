// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "UI/ObsidianWidgetBase.h"
#include "ObsidianTimerWidget.generated.h"

class UCommonTextBlock;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianTimerWidget : public UObsidianWidgetBase
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void UpdateTimer();
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> Timer_Text;

	/** Intervals in which the clock is synced. */
	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	float SyncClockFrequency = 10.0f;

private:
	FTimerHandle SyncTimerHandle;
	
};
