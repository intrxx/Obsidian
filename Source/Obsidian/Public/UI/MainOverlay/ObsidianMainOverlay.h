// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ObsidianTypes/ObsidianUIData.h"
#include "UI/ObsidianWidgetBase.h"
#include "UI/WidgetControllers/MainOverlayWidgetController.h"
#include "ObsidianMainOverlay.generated.h"

class UWrapBox;
class UOStackingDurationalEffectInfo;
/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianMainOverlay : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|MainOverlay")
	APlayerController* OwningPlayerController = nullptr;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite, Category = "Obisidian|MainOverlay")
	TObjectPtr<UWrapBox> BuffsEffectInfo_WrapBox;
	
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite, Category = "Obisidian|MainOverlay")
	TObjectPtr<UWrapBox> DeBuffsEffectInfo_WrapBox;

	UPROPERTY()
	TMap<FGameplayTag, UOStackingDurationalEffectInfo*> StackingInfoWidgetsMap;

protected:
	UFUNCTION(BlueprintCallable, Category = "Obisidian|MainOverlay")
	void HandleStackingUIData(const FObsidianEffectUIDataWidgetRow Row, const FObsidianEffectUIStackingData StackingData);

	UFUNCTION(BlueprintCallable, Category = "Obisidian|MainOverlay")
	void HandleUIData(const FObsidianEffectUIDataWidgetRow Row);

private:
	void DestroyStackingInfoWidget(UOStackingDurationalEffectInfo* WidgetToDestroy);
};
