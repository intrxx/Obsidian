// Copyright 2024 Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/OverlaysSubwidgets/ObsidianEffectInfoBase.h"
#include "ObsidianDurationalEffectInfo.generated.h"

class UCommonTextStyle;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianDurationalEffectInfo : public UObsidianEffectInfoBase
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Obsidian|EffectInfo")
	TObjectPtr<UCommonTextBlock> EffectTimer_TextBlock;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|EffectInfo|Style")
	TSubclassOf<UCommonTextStyle> NormalTimerStyle;

protected:
	UFUNCTION(BlueprintCallable, Category = "Obsidian|EffectInfo")
	void StartEffectTimer();

private:
	void TimerCountDown();

private:
	FTimerHandle EffectDurationTimerHandle;
};
