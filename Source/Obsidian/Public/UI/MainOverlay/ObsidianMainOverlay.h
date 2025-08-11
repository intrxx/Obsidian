// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// ~ Project
#include "ObsidianTypes/ObsidianUITypes.h"
#include "UI/WidgetControllers/MainOverlayWidgetController.h"

#include "UI/ObsidianWidgetBase.h"
#include "ObsidianMainOverlay.generated.h"

class UObsidianSkillPointsNotification;
class UObsidianOverlayExperienceBar;
class UObsidianItemDescriptionBase;
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
class UObsidianPlayerInputManager;
class UObsidianPlayerStashWidget;

UENUM()
enum EObsidianAddingWindowRule
{
	
};

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianMainOverlay : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	bool IsCharacterStatusOpen() const;
	bool IsInventoryOpen() const;
	bool IsPassiveSkillTree() const;
	bool IsPlayerStashOpen() const;

	UFUNCTION()
	void ToggleCharacterStatus();
	UFUNCTION()
	void ToggleInventory();
	UFUNCTION()
	void TogglePassiveSkillTree();
	
	void TogglePlayerStash(const bool bShowStash);
	
	void AddItemDescriptionToOverlay(UObsidianItemDescriptionBase* ItemDescription) const;

protected:
	virtual void HandleWidgetControllerSet() override;
	
	UFUNCTION(BlueprintCallable, Category = "Obisidian|MainOverlay")
	void HandleStackingUIData(const FObsidianEffectUIDataWidgetRow Row, const FObsidianEffectUIStackingData StackingData);

	UFUNCTION(BlueprintCallable, Category = "Obisidian|MainOverlay")
	void HandleUIData(const FObsidianEffectUIDataWidgetRow Row);
	
	UFUNCTION()
	void HandleRegularOverlayBar(AActor* TargetActor, bool bDisplayBar);
	UFUNCTION()
	void HandleBossOverlayBar(AActor* TargetActor, bool bDisplayBar);

	UFUNCTION()
	void UpdatePassiveSkillPointsNotification(float NewSkillPoints);
	UFUNCTION()
	void UpdateAscensionSkillPointsNotification(float NewSkillPoints);
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianProgressGlobe_Health> HealthProgressGlobe;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianProgressGlobe_Mana> ManaProgressGlobe;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianOverlayExperienceBar> ExperienceProgressBar;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianOverlayGameTabsMenu> Overlay_GameTabsMenu;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> LeftSideContainer_Overlay;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> RightSideContainer_Overlay;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> PassiveSkillTree_Overlay;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> DroppedItemDesc_Overlay;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> OverlayRegularBars_Overlay;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> OverlayBossBars_Overlay;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWrapBox> BuffsEffectInfo_WrapBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWrapBox> DeBuffsEffectInfo_WrapBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWrapBox> PassiveSkillPoints_WrapBox;

protected:
	UPROPERTY()
	APlayerController* OwningPlayerController = nullptr;
	
	UPROPERTY()
	TObjectPtr<UMainOverlayWidgetController> MainOverlayWidgetController;

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

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|PlayerStash")
	TSubclassOf<UObsidianPlayerStashWidget> PlayerStashClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|MainOverlay")
	TSubclassOf<UObsidianSkillPointsNotification> PassiveSkillPointsNotificationClass;
	UPROPERTY()
	TObjectPtr<UObsidianSkillPointsNotification> PassiveSkillPointsNotification;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|MainOverlay")
	TSubclassOf<UObsidianSkillPointsNotification> AscensionSkillPointsNotificationClass;
	UPROPERTY()
	TObjectPtr<UObsidianSkillPointsNotification> AscensionSkillPointsNotification;

	UPROPERTY()
	TMap<FGameplayTag, UOStackingDurationalEffectInfo*> StackingInfoWidgetsMap;
	
	UPROPERTY()
	TArray<TObjectPtr<UObsidianEffectInfoBase>> AuraUIInfoArray;
	
private:
	void DestroyStackingInfoWidget(UOStackingDurationalEffectInfo* WidgetToDestroy);

	UFUNCTION()
	void DestroyAuraInfoWidget(const FGameplayTag WidgetToDestroyWithTag);

	void HandleEffectFillImageRemoval(const FGameplayTag& EffectTag);

	void MoveDroppedItemDescOverlay(const bool bInventoryOpen);
	
private:
	UPROPERTY()
	TObjectPtr<UObsidianCharacterStatus> CharacterStatus;
	UPROPERTY()
	TObjectPtr<UObsidianInventory> Inventory;
	UPROPERTY()
	TObjectPtr<UObsidianPassiveSkillTree> PassiveSkillTree;
	UPROPERTY()
	TObjectPtr<UObsidianPlayerStashWidget> PlayerStash; 
	
	TArray<FObsidianProgressBarEffectFillImage> EffectFillImages;

	UPROPERTY()
	TObjectPtr<UObsidianInventoryItemsWidgetController> InventoryItemsWidgetController;
	
	UPROPERTY()
	TObjectPtr<UObsidianPlayerInputManager> PlayerInputManager;
};

