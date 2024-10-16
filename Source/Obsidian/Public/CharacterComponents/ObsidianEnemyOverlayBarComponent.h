// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObsidianTypes/ObsidianUITypes.h"
#include "ObsidianEnemyOverlayBarComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnNewOverlayBarStyleNeededSignature, const FSlateBrush& /** New Style */);
DECLARE_MULTICAST_DELEGATE(FOnOverlayBarStyleResetSignature);

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

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|Attributes|Health")
	FOnAttributeValueChangedSignature OnHealthChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|Attributes|Health")
	FOnAttributeValueChangedSignature OnMaxHealthChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|Attributes|EnergyShield")
	FOnAttributeValueChangedSignature OnEnergyShieldChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|Attributes|EnergyShield")
	FOnAttributeValueChangedSignature OnMaxEnergyShieldChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|Attributes|StaggerMeter")
	FOnAttributeValueChangedSignature OnStaggerMeterChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|Attributes|StaggerMeter")
	FOnAttributeValueChangedSignature OnMaxStaggerMeterChangedDelegate;

	FOnNewOverlayBarStyleNeededSignature OnNewOverlayBarStyleNeededDelegate;
	FOnOverlayBarStyleResetSignature OnOverlayBarStyleResetDelegate;

protected:
	virtual void BeginPlay() override;
	
	void HandleEnemyEffectApplied(const FObsidianEffectUIData& UIData);
	void HandleStackingEffect(const FObsidianEffectUIDataWidgetRow& Row, const FObsidianEffectUIStackingData& StackingData);
	void HandleRegularEffect(const FObsidianEffectUIDataWidgetRow& Row);

	bool GetEffectFillImageForTag(FObsidianProgressBarEffectFillImage& OutFillImage, const FGameplayTag& TagToCheck);
	void HandleEffectFillImageRemoval(const FGameplayTag& EffectImageTag);

	void HealthChanged(const float NewValue);
	void MaxHealthChanged(const float NewValue);
	void EnergyShieldChanged(const float NewValue);
	void MaxEnergyShieldChanged(const float NewValue);
	void StaggerMeterChanged(const float NewValue);
	void MaxStaggerMeterChanged(const float NewValue);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian")
	TObjectPtr<UDataTable> UIEffectDataTable;
	
	/** Effect Progress Bar to choose from when specific Effect is applied. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian")
	TArray<FObsidianProgressBarEffectFillImage> ProgressBarEffectFillImages;
	
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
};
