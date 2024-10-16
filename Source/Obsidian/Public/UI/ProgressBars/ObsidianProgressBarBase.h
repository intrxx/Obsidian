// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetBase.h"
#include "GameplayTagContainer.h"
#include "ObsidianTypes/ObsidianUITypes.h"
#include "ObsidianProgressBarBase.generated.h"

class UProgressBar;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianProgressBarBase : public UObsidianWidgetBase
{
	GENERATED_BODY()
	
public:
	/** Gets the Fill Image for specified effect tag on a particular Progress Bar. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|ProgressBars")
	bool GetEffectFillImageForTag(FObsidianProgressBarEffectFillImage& OutFillImage, const FGameplayTag& EffectTag);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	TArray<FObsidianProgressBarEffectFillImage> ProgressBarEffectFillImages;

protected:
	/** Helper function to set the percent, this can actually be moved to some helper static class */
	static void SetProgressBarPercent(const float Value, const float MaxValue, UProgressBar* ProgressBar);
};
