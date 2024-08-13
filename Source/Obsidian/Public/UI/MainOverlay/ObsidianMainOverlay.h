// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ObsidianTypes/UserIterface/ObsidianUIData.h"
#include "UI/ObsidianWidgetBase.h"
#include "UI/ProgressBars/ObsidianProgressBarBase.h"
#include "UI/WidgetControllers/MainOverlayWidgetController.h"
#include "ObsidianMainOverlay.generated.h"

class UObsidianProgressGlobe;
class USizeBox;
class UObsidianBasicHealthBar;
class UObsidianOverlayRegularEnemyBar;
class UVerticalBox;
class UOverlay;
class UObsidianOverlayGameTabsMenu;
class UWrapBox;
class UOStackingDurationalEffectInfo;
class UObsidianCharacterStatus;
/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianMainOverlay : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void ToggleCharacterStatus();
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|MainOverlay")
	APlayerController* OwningPlayerController = nullptr;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite, Category = "Obisidian|MainOverlay")
	TObjectPtr<UWrapBox> BuffsEffectInfo_WrapBox;
	
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite, Category = "Obisidian|MainOverlay")
	TObjectPtr<UWrapBox> DeBuffsEffectInfo_WrapBox;

protected:
	UFUNCTION(BlueprintCallable, Category = "Obisidian|MainOverlay")
	void HandleStackingUIData(const FObsidianEffectUIDataWidgetRow Row, const FObsidianEffectUIStackingData StackingData);

	UFUNCTION(BlueprintCallable, Category = "Obisidian|MainOverlay")
	void HandleUIData(const FObsidianEffectUIDataWidgetRow Row);

	virtual void HandleWidgetControllerSet() override;

	UFUNCTION()
	void HandleOverlayBar(AActor* TargetActor, bool bDisplayBar);

protected:
	// This should be set in blueprint during initialization ;/ //
	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|MainOverlay")
	TObjectPtr<UObsidianProgressGlobe> HealthProgressGlobe;

	// This should be set in blueprint during initialization ;/ //
	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|MainOverlay")
	TObjectPtr<UObsidianProgressGlobe> ManaProgressGlobe;
	
	UPROPERTY()
	TObjectPtr<UMainOverlayWidgetController> MainOverlayWidgetController;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianOverlayGameTabsMenu> Overlay_GameTabsMenu;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> CharacterStatus_Overlay;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UVerticalBox> OverlayBars_VerticalBox;

	UPROPERTY()
	TArray<TObjectPtr<UObsidianEffectInfoBase>> AuraUIInfoArray;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|MainOverlay|HealthBars")
	TSubclassOf<UObsidianOverlayRegularEnemyBar> RegularEnemyHealthBarClass;

	UPROPERTY()
	TObjectPtr<UObsidianOverlayRegularEnemyBar> RegularEnemyHealthBar;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|MainOverlay|HealthBars")
	TSubclassOf<UObsidianBasicHealthBar> BossEnemyHealthBarClass;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|MainOverlay")
	TSubclassOf<UObsidianCharacterStatus> CharacterStatusClass;

	UPROPERTY()
	TMap<FGameplayTag, UOStackingDurationalEffectInfo*> StackingInfoWidgetsMap;

private:
	UPROPERTY()
	TObjectPtr<UObsidianCharacterStatus> CharacterStatus;
	
	TArray<FObsidianProgressBarEffectFillImage> EffectFillImages;
	
private:
	void DestroyStackingInfoWidget(UOStackingDurationalEffectInfo* WidgetToDestroy);

	UFUNCTION()
	void DestroyAuraInfoWidget(const FGameplayTag WidgetToDestroyWithTag);

	void HandleEffectFillImageRemoval(const FGameplayTag& EffectTag);
};
