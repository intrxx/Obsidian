// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/MainOverlay/Subwidgets/ObsidianEffectInfoBase.h"
#include "ObsidianDurationalEffectInfo.generated.h"

class UObsidianDurationalEffectInfo;
class UCommonTextStyle;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnDurationalInfoWidgetTerminated, UObsidianDurationalEffectInfo* /** Durational Info Widget */);

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianDurationalEffectInfo : public UObsidianEffectInfoBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Obsidian|EffectInfo")
	void InitDurationalEffectInfo(const FText& InEffectName, const FText& InEffectDesc, UTexture2D* InEffectImage, const float Duration);
	
public:
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Obsidian|EffectInfo")
	TObjectPtr<UCommonTextBlock> EffectTimer_TextBlock;

	FOnDurationalInfoWidgetTerminated OnDurationalInfoWidgetTerminatedDelegate;

protected:
	UFUNCTION(BlueprintCallable, Category = "Obsidian|EffectInfo")
	void StartEffectTimer();

private:
	void TimerCountDown();

private:
	UPROPERTY()
	APlayerController* OwningPlayer = nullptr;
	
	FTimerHandle EffectDurationTimerHandle;
};
