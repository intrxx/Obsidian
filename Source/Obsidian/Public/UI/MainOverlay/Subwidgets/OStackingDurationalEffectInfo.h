// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "ObsidianTypes/ObsidianUITypes.h"

#include "UI/MainOverlay/Subwidgets/ObsidianEffectInfoBase.h"
#include "OStackingDurationalEffectInfo.generated.h"

class UOStackingDurationalEffectInfo;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStackingInfoWidgetTerminated, UOStackingDurationalEffectInfo* /** Stacking Info Widget */);

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UOStackingDurationalEffectInfo : public UObsidianEffectInfoBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Obsidian|EffectInfo")
	void InitDurationalStackingEffectInfo(const FText& InEffectName, const FText& InEffectDesc, UTexture2D* InEffectImage, const float Duration, const FObsidianEffectUIStackingData& InData);
	
	void UpdateStackingInfoWidget(const int32 NewCount);

public:
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Obsidian|EffectInfo")
	TObjectPtr<UCommonTextBlock> EffectTimer_TextBlock;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Obsidian|EffectInfo")
	TObjectPtr<UCommonTextBlock> StackCount_TextBlock;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|EffectInfo")
	EGameplayEffectStackingExpirationPolicy EffectExpirationPolicy;
	
	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|EffectInfo")
	EGameplayEffectStackingDurationPolicy EffectDurationPolicy;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|EffectInfo")
	FObsidianEffectUIStackingData StackingData;
	
	FOnStackingInfoWidgetTerminated OnStackingInfoWidgetTerminatedDelegate;

protected:
	UFUNCTION(BlueprintCallable, Category = "Obsidian|EffectInfo")
	void StartEffectTimer();

	UFUNCTION(BlueprintCallable, Category = "Obsidian|EffectInfo")
	void SetStackCount(const int32 Count);

private:
	void TimerCountDown();
	void ResetEffectTimer();
	void HandleEffectExpiration();
	void TerminateTheEffect();
	
private:
	UPROPERTY()
	APlayerController* OwningPlayer = nullptr;
	
	/**
	 * Effect data
	 */
	
	// Amount to fall back on to in case of resetting the duration of the stacking gameplay effect
	float WholeEffectDuration = 0;
	
	int32 EffectStackCount = 0;
	
	FTimerHandle EffectDurationTimerHandle;
};
