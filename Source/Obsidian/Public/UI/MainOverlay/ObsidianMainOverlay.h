// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ObsidianTypes/ObsidianUITypes.h"
#include "UI/ObsidianWidgetBase.h"
#include "UI/WidgetControllers/MainOverlayWidgetController.h"
#include "ObsidianMainOverlay.generated.h"

class UObsidianPassiveSkillTree;
class UObsidianInventory;
class UObsidianProgressGlobe_Mana;
class UObsidianProgressGlobe_Health;
class UObsidianOverlayBossEnemyBar;
class UObsidianOverlayEnemyBar;
class UObsidianProgressGlobe;
class USizeBox;
class UObsidianBasicHealthBar;
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
	UFUNCTION()
	void ToggleInventory();
	UFUNCTION()
	void TogglePassiveSkillTree();
	
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
	void HandleRegularOverlayBar(AActor* TargetActor, bool bDisplayBar);
	UFUNCTION()
	void HandleBossOverlayBar(AActor* TargetActor, bool bDisplayBar);

protected:
	// This should be set in blueprint during initialization ;/ //
	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|MainOverlay")
	TObjectPtr<UObsidianProgressGlobe_Health> HealthProgressGlobe;

	// This should be set in blueprint during initialization ;/ //
	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|MainOverlay")
	TObjectPtr<UObsidianProgressGlobe_Mana> ManaProgressGlobe;
	
	UPROPERTY()
	TObjectPtr<UMainOverlayWidgetController> MainOverlayWidgetController;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianOverlayGameTabsMenu> Overlay_GameTabsMenu;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> CharacterStatus_Overlay;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Inventory_Overlay;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> PassiveSkillTree_Overlay;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> OverlayRegularBars_Overlay;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> OverlayBossBars_Overlay;

	UPROPERTY()
	TArray<TObjectPtr<UObsidianEffectInfoBase>> AuraUIInfoArray;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|MainOverlay|HealthBars")
	TSubclassOf<UObsidianOverlayEnemyBar> RegularEnemyOverlayHealthBarClass;

	UPROPERTY()
	TObjectPtr<UObsidianOverlayEnemyBar> RegularEnemyOverlayHealthBar;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|MainOverlay|HealthBars")
	TSubclassOf<UObsidianOverlayBossEnemyBar> BossEnemyOverlayHealthBarClass;

	UPROPERTY()
	TObjectPtr<UObsidianOverlayBossEnemyBar> BossEnemyOverlayHealthBar;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|MainOverlay")
	TSubclassOf<UObsidianCharacterStatus> CharacterStatusClass;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|MainOverlay")
	TSubclassOf<UObsidianInventory> InventoryClass;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|MainOverlay")
	TSubclassOf<UObsidianPassiveSkillTree> PassiveSkillTreeClass;

	UPROPERTY()
	TMap<FGameplayTag, UOStackingDurationalEffectInfo*> StackingInfoWidgetsMap;

private:
	UPROPERTY()
	TObjectPtr<UObsidianCharacterStatus> CharacterStatus;
	UPROPERTY()
	TObjectPtr<UObsidianInventory> Inventory;
	UPROPERTY()
	TObjectPtr<UObsidianPassiveSkillTree> PassiveSkillTree;
	
	TArray<FObsidianProgressBarEffectFillImage> EffectFillImages;
	
private:
	void DestroyStackingInfoWidget(UOStackingDurationalEffectInfo* WidgetToDestroy);

	UFUNCTION()
	void DestroyAuraInfoWidget(const FGameplayTag WidgetToDestroyWithTag);

	void HandleEffectFillImageRemoval(const FGameplayTag& EffectTag);
};
