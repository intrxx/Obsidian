// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetBase.h"
#include "GameplayTagContainer.h"
#include "ObsidianProgressBarBase.generated.h"

/*
 * Struct that ties the specific Effect Tag to Fill image so the system can for example replace the Red Progress globe with green one for poison.
 */
USTRUCT(BlueprintType)
struct FObsidianProgressBarEffectFillImage
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag EffectTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly)
	FSlateBrush ProgressBarFillImage = FSlateBrush();
};

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianProgressBarBase : public UObsidianWidgetBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Obsidian|ProgressBars")
	bool GetEffectFillImageForTag(FSlateBrush& FillImage, const FGameplayTag& EffectTag);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup|Globe")
	TArray<FObsidianProgressBarEffectFillImage> ProgressBarEffectFillImages;
};
