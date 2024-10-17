// Copyright 2024 out of sCope team - Michał Ogiński


#include "CharacterComponents/ObsidianEnemyOverlayBarComponent.h"
#include "CharacterComponents/Attributes/ObsidianEnemyAttributesComponent.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Core/ObsidianUIFunctionLibrary.h"
#include "UI/ProgressBars/ObsidianProgressBarBase.h"

UObsidianEnemyOverlayBarComponent::UObsidianEnemyOverlayBarComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

bool UObsidianEnemyOverlayBarComponent::GetCurrentOverlayFillBarEffect(FSlateBrush& CurrentFillBarEffect)
{
	if(CachedEffectFillImages.IsEmpty())
	{
		return false;
	}
	CurrentFillBarEffect = CachedEffectFillImages.Last().ProgressBarFillImage;
	
	return true;
}

FText UObsidianEnemyOverlayBarComponent::GetEnemyName() const
{
	return EnemyAttributesComp ? EnemyAttributesComp->GetEnemyName() : FText();
}

bool UObsidianEnemyOverlayBarComponent::IsDeadOrDying() const
{
	return EnemyAttributesComp ? EnemyAttributesComp->IsDeadOrDying() : true;
}

void UObsidianEnemyOverlayBarComponent::FillInitialValues(float& OutHealth, float& OutMaxHealth, float& OutEnergyShield,
	float& OutMaxEnergyShield, float& OutStaggerMeter, float& OutMaxStaggerMeter) const
{
	if(EnemyAttributesComp)
	{
		OutHealth = EnemyAttributesComp->GetHealth();
		OutMaxHealth = EnemyAttributesComp->GetMaxHealth();
		OutEnergyShield = EnemyAttributesComp->GetEnergyShield();
		OutMaxEnergyShield = EnemyAttributesComp->GetMaxEnergyShield();
		OutStaggerMeter = EnemyAttributesComp->GetStaggerMeter();
		OutMaxStaggerMeter = EnemyAttributesComp->GetMaxStaggerMeter();
	}
}

void UObsidianEnemyOverlayBarComponent::InitializeOverlayBarComponent(UObsidianAbilitySystemComponent* InASC, UObsidianEnemyAttributesComponent* InEnemyAttributesComp)
{
	check(InASC);
	ObsidianASC = InASC;

	check(InEnemyAttributesComp)
	EnemyAttributesComp = InEnemyAttributesComp;

	HealthChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(EnemyAttributesComp->GetHealthAttribute()).AddUObject(this, &ThisClass::HealthChanged);
	MaxHealthChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(EnemyAttributesComp->GetMaxHealthAttribute()).AddUObject(this, &ThisClass::MaxHealthChanged);
	EnergyShieldChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(EnemyAttributesComp->GetEnergyShieldAttribute()).AddUObject(this, &ThisClass::EnergyShieldChanged);
	MaxEnergyShieldChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(EnemyAttributesComp->GetMaxEnergyShieldAttribute()).AddUObject(this, &ThisClass::MaxEnergyShieldChanged);
	StaggerMeterChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(EnemyAttributesComp->GetStaggerMeterAttribute()).AddUObject(this, &ThisClass::StaggerMeterChanged);
	MaxStaggerMeterChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(EnemyAttributesComp->GetMaxStaggerMeterAttribute()).AddUObject(this, &ThisClass::MaxStaggerMeterChanged);

	UIDataDelegateHandle = ObsidianASC->OnEffectAppliedAssetTags.AddUObject(this, &ThisClass::HandleEnemyEffectApplied);
}

void UObsidianEnemyOverlayBarComponent::UninitializeOverlayBarComponent()
{
	HealthChangedDelegateHandle.Reset();
	MaxHealthChangedDelegateHandle.Reset();
	EnergyShieldChangedDelegateHandle.Reset();
	MaxEnergyShieldChangedDelegateHandle.Reset();
	StaggerMeterChangedDelegateHandle.Reset();
	MaxStaggerMeterChangedDelegateHandle.Reset();
	
	UIDataDelegateHandle.Reset();
	
	ObsidianASC = nullptr;
	EnemyAttributesComp = nullptr;
}

void UObsidianEnemyOverlayBarComponent::HandleEnemyEffectApplied(const FObsidianEffectUIData& UIData)
{
	// We don't care about any non-durational effects as they will not affect the bar display change
	if(UIData.EffectDurationPolicy != EGameplayEffectDurationType::HasDuration)
	{
		return;
	}
	
	for(const FGameplayTag& Tag : UIData.AssetTags)
	{
		if(Tag.MatchesTag(EffectTag))
		{
			FObsidianEffectUIDataWidgetRow* Row = UObsidianUIFunctionLibrary::GetDataTableRowByTag<FObsidianEffectUIDataWidgetRow>(UIEffectDataTable, Tag);
			Row->EffectDuration = UIData.EffectDuration;
					
			if(UIData.bStackingEffect)
			{
				HandleStackingEffect(*Row, UIData.StackingData);
			}
			else
			{
				HandleRegularEffect(*Row);
			}
		}
	}
}

void UObsidianEnemyOverlayBarComponent::HandleStackingEffect(const FObsidianEffectUIDataWidgetRow& Row, const FObsidianEffectUIStackingData& StackingData)
{
	UE_LOG(LogTemp, Warning, TEXT("Effect [%s] on Enemy [%s]"), *Row.EffectName.ToString(), *EnemyAttributesComp->GetEnemyName().ToString());
	
	FObsidianProgressBarEffectFillImage FillImage;
	if(GetEffectFillImageForTag(/* OUT */ FillImage, Row.EffectTag))
	{
		//SetOverlayBarStyle(FillImage.ProgressBarFillImage);
		//CachedEffectFillImages.Add(FillImage);

		//TODO Handle Stacking effects removal
	}
}

void UObsidianEnemyOverlayBarComponent::HandleRegularEffect(const FObsidianEffectUIDataWidgetRow& Row)
{
	UE_LOG(LogTemp, Warning, TEXT("Effect [%s] on Enemy [%s]"), *Row.EffectName.ToString(), *EnemyAttributesComp->GetEnemyName().ToString());
	
	FObsidianProgressBarEffectFillImage FillImage;
	if(GetEffectFillImageForTag(/* OUT */FillImage, Row.EffectTag))
	{
		OnNewOverlayBarStyleNeededDelegate.Broadcast(FillImage.ProgressBarFillImage);
		CachedEffectFillImages.Add(FillImage);
			
		FTimerHandle EffectExpiredDelegateHandle;
		GetWorld()->GetTimerManager().SetTimer(EffectExpiredDelegateHandle, FTimerDelegate::CreateWeakLambda(this, [this, Row]()
		{
			HandleEffectFillImageRemoval(Row.EffectTag);
			
		}), Row.EffectDuration, false);
	}
}

bool UObsidianEnemyOverlayBarComponent::GetEffectFillImageForTag(FObsidianProgressBarEffectFillImage& OutFillImage, const FGameplayTag& TagToCheck)
{
	for(const FObsidianProgressBarEffectFillImage& EffectFillImage : ProgressBarEffectFillImages)
	{
		if(EffectFillImage.ProgressBarFillImage.IsSet() && (EffectFillImage.EffectTag == TagToCheck))
		{
			OutFillImage = EffectFillImage;
			return true;
		}
	}
	return false;
}

void UObsidianEnemyOverlayBarComponent::HandleEffectFillImageRemoval(const FGameplayTag& EffectImageTag)
{
	if(!CachedEffectFillImages.IsEmpty())
	{
		for(int i = 0; i < CachedEffectFillImages.Num(); i++)
		{
			if(CachedEffectFillImages[i].EffectTag == EffectImageTag)
			{
				CachedEffectFillImages.RemoveAt(i);
			}
		}

		if(CachedEffectFillImages.Num() != 0)
		{
			OnNewOverlayBarStyleNeededDelegate.Broadcast(CachedEffectFillImages.Last().ProgressBarFillImage);
		}
		else
		{
			OnOverlayBarStyleResetDelegate.Broadcast();
		}
		return;
	}
	OnOverlayBarStyleResetDelegate.Broadcast();
}

void UObsidianEnemyOverlayBarComponent::HealthChanged(const FOnAttributeChangeData& Data)
{
	OnHealthChangedDelegate.ExecuteIfBound(Data.NewValue);
}

void UObsidianEnemyOverlayBarComponent::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	OnMaxHealthChangedDelegate.ExecuteIfBound(Data.NewValue);
}

void UObsidianEnemyOverlayBarComponent::EnergyShieldChanged(const FOnAttributeChangeData& Data)
{
	OnEnergyShieldChangedDelegate.ExecuteIfBound(Data.NewValue);
}

void UObsidianEnemyOverlayBarComponent::MaxEnergyShieldChanged(const FOnAttributeChangeData& Data)
{
	OnMaxEnergyShieldChangedDelegate.ExecuteIfBound(Data.NewValue);
}

void UObsidianEnemyOverlayBarComponent::StaggerMeterChanged(const FOnAttributeChangeData& Data)
{
	OnStaggerMeterChangedDelegate.ExecuteIfBound(Data.NewValue);
}

void UObsidianEnemyOverlayBarComponent::MaxStaggerMeterChanged(const FOnAttributeChangeData& Data)
{
	OnMaxStaggerMeterChangedDelegate.ExecuteIfBound(Data.NewValue);
}

void UObsidianEnemyOverlayBarComponent::BeginPlay()
{
	Super::BeginPlay();
}

