// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObsidianTypes/ObsidianUITypes.h"
#include "ObsidianEnemyOverlayBarComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnNewOverlayBarStyleNeededSignature, const FSlateBrush& /** New Style */);
DECLARE_MULTICAST_DELEGATE(FOnOverlayBarStyleResetSignature);

struct FOnAttributeChangeData;
struct FObsidianEffectUIData;
class UObsidianEnemyAttributesComponent;
class UObsidianAbilitySystemComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OBSIDIAN_API UObsidianEnemyOverlayBarComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UObsidianEnemyOverlayBarComponent();

	/** Returns Enemy Overlay Component if one exists on the specified actor, will be nullptr otherwise */
	UFUNCTION(BlueprintPure, Category = "Obsidian|EnemyOverlayBarComp")
	static UObsidianEnemyOverlayBarComponent* FindEnemyOverlayComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UObsidianEnemyOverlayBarComponent>() : nullptr);
	}

	/** Returns most recent fill bar effect image if one exists currently. */ 
	bool GetCurrentOverlayFillBarEffect(FSlateBrush& CurrentFillBarEffect);

	FText GetEnemyName() const;
	bool IsDeadOrDying() const;
	void FillInitialValues(float& OutHealth, float& OutMaxHealth, float& OutEnergyShield, float& OutMaxEnergyShield, float& OutStaggerMeter, float& OutMaxStaggerMeter) const;
	
	void InitializeOverlayBarComponent(UObsidianAbilitySystemComponent* InASC, UObsidianEnemyAttributesComponent* InEnemyAttributesComp);
	void UninitializeOverlayBarComponent();

public:
	UPROPERTY(BlueprintAssignable, Category = "Obsidian|UIData")
	FStackingEffectUIDataWidgetRow EffectStackingUIDataDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|UIData")
	FEffectUIDataWidgetRow EffectUIDataWidgetRowDelegate;

	FOnAttributeValueChangedOneParam OnHealthChangedDelegate;
	FOnAttributeValueChangedOneParam OnMaxHealthChangedDelegate;
	FOnAttributeValueChangedOneParam OnEnergyShieldChangedDelegate;
	FOnAttributeValueChangedOneParam OnMaxEnergyShieldChangedDelegate;
	FOnAttributeValueChangedOneParam OnStaggerMeterChangedDelegate;
	FOnAttributeValueChangedOneParam OnMaxStaggerMeterChangedDelegate;
	
	FOnNewOverlayBarStyleNeededSignature OnNewOverlayBarStyleNeededDelegate;
	FOnOverlayBarStyleResetSignature OnOverlayBarStyleResetDelegate;

protected:
	void HandleEnemyEffectApplied(const FObsidianEffectUIData& UIData);
	void HandleStackingEffect(const FObsidianEffectUIDataWidgetRow& Row, const FObsidianEffectUIStackingData& StackingData);
	void HandleRegularEffect(const FObsidianEffectUIDataWidgetRow& Row);

	void HandleStackingEffectExpiration(const EGameplayEffectStackingExpirationPolicy& ExpirationPolicy, const float& Duration, const FGameplayTag& StackingEffectTag);
	void RefreshStackingEffectDuration(const EGameplayEffectStackingExpirationPolicy& ExpirationPolicy, const float& Duration, const FGameplayTag& StackingEffectTag);
	
	bool GetEffectFillImageForTag(FObsidianProgressBarEffectFillImage& OutFillImage, const FGameplayTag& TagToCheck);
	void HandleEffectFillImageRemoval(const FGameplayTag& EffectImageTag);

	void HealthChanged(const FOnAttributeChangeData& Data);
	void MaxHealthChanged(const FOnAttributeChangeData& Data);
	void EnergyShieldChanged(const FOnAttributeChangeData& Data);
	void MaxEnergyShieldChanged(const FOnAttributeChangeData& Data);
	void StaggerMeterChanged(const FOnAttributeChangeData& Data);
	void MaxStaggerMeterChanged(const FOnAttributeChangeData& Data);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian")
	TObjectPtr<UDataTable> UIEffectDataTable;
	
	/** Effect Progress Bar to choose from when specific Effect is applied. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian")
	TArray<FObsidianProgressBarEffectFillImage> ProgressBarEffectFillImages;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian")
	bool bDebugEnabled = false;
private:
	UPROPERTY()
	TObjectPtr<UObsidianAbilitySystemComponent> ObsidianASC;

	UPROPERTY()
	TObjectPtr<UObsidianEnemyAttributesComponent> EnemyAttributesComp;

	UPROPERTY()
	TArray<FObsidianProgressBarEffectFillImage> CachedEffectFillImages;
	
	const FGameplayTag EffectTag = FGameplayTag::RequestGameplayTag(FName("UI.EffectData.Effect"));

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
