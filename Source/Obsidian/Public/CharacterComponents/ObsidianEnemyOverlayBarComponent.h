// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "ObsidianTypes/ObsidianUITypes.h"

#include "ObsidianEnemyOverlayBarComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnNewOverlayBarStyleNeededSignature, const FSlateBrush& /** New Style */);
DECLARE_MULTICAST_DELEGATE(FOnOverlayBarStyleResetSignature);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnNewOverlayBarSpecialEffectNeededSignature, const FSlateBrush& /** New Style */);
DECLARE_MULTICAST_DELEGATE(FOnOverlayBarSpecialEffectResetSignature);

struct FOnAttributeChangeData;
struct FObsidianEffectUIData;
class UObsidianEnemyAttributesComponent;
class UObsidianAbilitySystemComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OBSIDIAN_API UObsidianEnemyOverlayBarComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UObsidianEnemyOverlayBarComponent(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	/** Returns Enemy Overlay Component if one exists on the specified actor, will be nullptr otherwise */
	UFUNCTION(BlueprintPure, Category = "Obsidian|EnemyOverlayBarComp")
	static UObsidianEnemyOverlayBarComponent* FindEnemyOverlayComponent(const AActor* InActor)
	{
		return (InActor ? InActor->FindComponentByClass<UObsidianEnemyOverlayBarComponent>() : nullptr);
	}

	/** Returns most recent fill bar effect image if one exists currently. */ 
	bool GetCurrentOverlayFillBarEffect(FSlateBrush& OutCurrentFillBarEffect);

	FText GetEnemyName() const;
	bool IsDeadOrDying() const;
	void FillInitialValues(float& OutHealth, float& OutMaxHealth, float& OutEnergyShield, float& OutMaxEnergyShield, float& OutStaggerMeter, float& OutMaxStaggerMeter) const;
	
	void InitializeOverlayBarComponent(UObsidianAbilitySystemComponent* InASC, UObsidianEnemyAttributesComponent* InEnemyAttributesComp);
	void UninitializeOverlayBarComponent();

public:
	UPROPERTY(BlueprintAssignable, Category = "Obsidian|EnemyOverlayBarComp|Data")
	FStackingEffectUIDataWidgetRow EffectStackingUIDataDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|EnemyOverlayBarComp|Data")
	FEffectUIDataWidgetRow EffectUIDataWidgetRowDelegate;

	FOnAttributeValueChangedOneParam OnHealthChangedDelegate;
	FOnAttributeValueChangedOneParam OnMaxHealthChangedDelegate;
	FOnAttributeValueChangedOneParam OnEnergyShieldChangedDelegate;
	FOnAttributeValueChangedOneParam OnMaxEnergyShieldChangedDelegate;
	FOnAttributeValueChangedOneParam OnStaggerMeterChangedDelegate;
	FOnAttributeValueChangedOneParam OnMaxStaggerMeterChangedDelegate;
	
	FOnNewOverlayBarStyleNeededSignature OnNewOverlayBarStyleNeededDelegate;
	FOnOverlayBarStyleResetSignature OnOverlayBarStyleResetDelegate;

	FOnNewOverlayBarSpecialEffectNeededSignature OnNewOverlayBarSpecialEffectNeededDelegate;
	FOnOverlayBarSpecialEffectResetSignature OnOverlayBarSpecialEffectResetDelegate;

protected:
	void HandleEnemyEffectApplied(const FObsidianEffectUIData& InUIData);
	void HandleStackingEffect(const FObsidianEffectUIDataWidgetRow& InRow, const FObsidianEffectUIStackingData& InStackingData);
	void HandleRegularEffect(const FObsidianEffectUIDataWidgetRow& InRow);
	void HandleSpecialEffect(const FGameplayTag& InEffectImageTag);

	void HandleStackingEffectExpiration(const EGameplayEffectStackingExpirationPolicy& InExpirationPolicy, const float InDuration, const FGameplayTag& InStackingEffectTag);
	void RefreshStackingEffectDuration(const EGameplayEffectStackingExpirationPolicy& InExpirationPolicy, const float InDuration, const FGameplayTag& InStackingEffectTag);
	
	bool GetEffectFillImageForTag(const TArray<FObsidianProgressBarEffectFillImage>& InImages, FObsidianProgressBarEffectFillImage& OutFillImage, const FGameplayTag& InTagToCheck);
	void HandleEffectFillImageRemoval(const FGameplayTag& InEffectImageTag);

	UFUNCTION(BlueprintCallable, Category = "Obsidian|EnemyOverlayBarComp")
	void HandleSpecialEffectImageRemoval(const FGameplayTag& InEffectImageTag);

	void HealthChanged(const FOnAttributeChangeData& InData) const;
	void MaxHealthChanged(const FOnAttributeChangeData& InData) const;
	void EnergyShieldChanged(const FOnAttributeChangeData& InData) const;
	void MaxEnergyShieldChanged(const FOnAttributeChangeData& InData) const;
	void StaggerMeterChanged(const FOnAttributeChangeData& InData) const;
	void MaxStaggerMeterChanged(const FOnAttributeChangeData& InData) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnemyOverlayBarComp|Data")
	TObjectPtr<UDataTable> UIEffectDataTable;
	
	/** Effect Progress Bar to choose from when specific Effect is applied. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|EnemyOverlayBarComp")
	TArray<FObsidianProgressBarEffectFillImage> ProgressBarEffectFillImages;

	/** Special Effects Progress Bar images to choose from when specific Effect is applied. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|EnemyOverlayBarComp")
	TArray<FObsidianProgressBarEffectFillImage> ProgressBarSpecialEffects;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|EnemyOverlayBarComp")
	bool bDebugEnabled = false;
private:
	UPROPERTY()
	TObjectPtr<UObsidianAbilitySystemComponent> ObsidianASC;

	UPROPERTY()
	TObjectPtr<UObsidianEnemyAttributesComponent> EnemyAttributesComp;

	UPROPERTY()
	TArray<FObsidianProgressBarEffectFillImage> CachedEffectFillImages;

	UPROPERTY()
	TArray<FObsidianProgressBarEffectFillImage> CachedSpecialEffectFillImages;

	/* Regular Effects Tag like Chill, Ignite etc. **/
	FGameplayTag EffectTag;

	/* Special Effects like Immunity or Damage Reduction, this won't affect the fill image, it will display its own. **/
	FGameplayTag SpecialEffectTag;

	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle EnergyShieldChangedDelegateHandle;
	FDelegateHandle MaxEnergyShieldChangedDelegateHandle;
	FDelegateHandle StaggerMeterChangedDelegateHandle;
	FDelegateHandle MaxStaggerMeterChangedDelegateHandle;
	
	FDelegateHandle UIDataDelegateHandle;

	FTimerHandle StackingEffectTimerHandle;

	int32 EffectStackCount = 0;
};
