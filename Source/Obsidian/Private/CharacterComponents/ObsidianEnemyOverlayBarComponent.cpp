// Copyright 2024 out of sCope team - Michał Ogiński

#include "CharacterComponents/ObsidianEnemyOverlayBarComponent.h"

// ~ Core

// ~ Project
#include "CharacterComponents/Attributes/ObsidianEnemyAttributesComponent.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Core/ObsidianUIFunctionLibrary.h"
#include "UI/ProgressBars/ObsidianProgressBarBase.h"

UObsidianEnemyOverlayBarComponent::UObsidianEnemyOverlayBarComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	EffectTag = FGameplayTag::RequestGameplayTag(FName("UI.EffectData.Effect"));
	SpecialEffectTag = FGameplayTag::RequestGameplayTag(FName("UI.EffectData.Effect.Special"));
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
	// We don't care about any Instant gameplay effects, might want to change it later.
	if(UIData.EffectDurationPolicy == EGameplayEffectDurationType::Instant)
	{
		return;
	}
	
	for(const FGameplayTag& Tag : UIData.AssetTags)
	{
		if(Tag.MatchesTag(SpecialEffectTag)) // "UI.EffectData.Effect.Special"
		{
			UE_LOG(LogTemp, Error, TEXT("Effect [%s] is applied to enemy."), *Tag.GetTagName().ToString());

			HandleSpecialEffect(Tag);

			continue;
		}
		
		if(Tag.MatchesTag(EffectTag)) // "UI.EffectData.Effect"
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
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return;
	}

	FObsidianProgressBarEffectFillImage FillImage;
	if(!GetEffectFillImageForTag(ProgressBarEffectFillImages, /* OUT */ FillImage, Row.EffectTag))
	{
		return;
	}
	
	bool bAlreadyApplied = false;
	for(const FObsidianProgressBarEffectFillImage& EffectImage : CachedEffectFillImages)
	{
		if(EffectImage.EffectTag == Row.EffectTag)
		{
			bAlreadyApplied = true;
		}
	}
	
	if(bAlreadyApplied && StackingData.EffectStackingDurationPolicy == EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication)
	{
		EffectStackCount++;
		
		RefreshStackingEffectDuration(StackingData.EffectExpirationDurationPolicy, Row.EffectDuration, Row.EffectTag);
		
		return;
	}
	
#if !UE_BUILD_SHIPPING
	if(bDebugEnabled)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Emerald,
			FString::Printf(TEXT("Adding Stacking Effect [%s] on Enemy [%s]."), *Row.EffectName.ToString(), *EnemyAttributesComp->GetEnemyName().ToString()));
	}
#endif
		
	OnNewOverlayBarStyleNeededDelegate.Broadcast(FillImage.ProgressBarFillImage);
	CachedEffectFillImages.Add(FillImage);
	EffectStackCount = 1;
		
	World->GetTimerManager().SetTimer(StackingEffectTimerHandle, FTimerDelegate::CreateWeakLambda(this, [StackingData, Row, this]()
		{
			HandleStackingEffectExpiration(StackingData.EffectExpirationDurationPolicy, Row.EffectDuration, Row.EffectTag);
		}), Row.EffectDuration, false);
}

void UObsidianEnemyOverlayBarComponent::HandleRegularEffect(const FObsidianEffectUIDataWidgetRow& Row)
{
#if !UE_BUILD_SHIPPING
	if(bDebugEnabled)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
			FString::Printf(TEXT("Adding Regular Effect [%s] on Enemy [%s]."), *Row.EffectName.ToString(), *EnemyAttributesComp->GetEnemyName().ToString()));
	}
#endif

	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return;
	}
	
	FObsidianProgressBarEffectFillImage FillImage;
	if(GetEffectFillImageForTag(ProgressBarEffectFillImages, /* OUT */FillImage, Row.EffectTag))
	{
		OnNewOverlayBarStyleNeededDelegate.Broadcast(FillImage.ProgressBarFillImage);
		CachedEffectFillImages.Add(FillImage);
			
		FTimerHandle EffectExpiredDelegateHandle;
		World->GetTimerManager().SetTimer(EffectExpiredDelegateHandle, FTimerDelegate::CreateWeakLambda(this, [this, Row]()
			{
			
#if !UE_BUILD_SHIPPING
if(bDebugEnabled)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
		FString::Printf(TEXT("Removing Regular Effect for Tag [%s]."), *Row.EffectTag.GetTagName().ToString()));
}
#endif
			
				HandleEffectFillImageRemoval(Row.EffectTag);
				
			}), Row.EffectDuration, false);
	}
}


void UObsidianEnemyOverlayBarComponent::HandleSpecialEffect(const FGameplayTag& EffectImageTag)
{
#if !UE_BUILD_SHIPPING
	if(bDebugEnabled)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
			FString::Printf(TEXT("Adding Special Effect with Tag [%s] on Enemy [%s]."), *EffectImageTag.ToString(), *EnemyAttributesComp->GetEnemyName().ToString()));
	}
#endif
	
	FObsidianProgressBarEffectFillImage SpecialImage;
	if(GetEffectFillImageForTag(ProgressBarSpecialEffects, SpecialImage, EffectImageTag))
	{
		OnNewOverlayBarSpecialEffectNeededDelegate.Broadcast(SpecialImage.ProgressBarFillImage);
		CachedSpecialEffectFillImages.Add(SpecialImage);
	}
}

void UObsidianEnemyOverlayBarComponent::HandleStackingEffectExpiration(const EGameplayEffectStackingExpirationPolicy& ExpirationPolicy, const float Duration, const FGameplayTag& StackingEffectTag)
{
	switch(ExpirationPolicy)
	{
	case EGameplayEffectStackingExpirationPolicy::ClearEntireStack:
		
#if !UE_BUILD_SHIPPING
		if(bDebugEnabled)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Emerald,
				FString::Printf(TEXT("Removing Stacking Effect for Tag [%s]."), *StackingEffectTag.GetTagName().ToString()));
		}
#endif
		
		HandleEffectFillImageRemoval(StackingEffectTag);
		break;
	case EGameplayEffectStackingExpirationPolicy::RemoveSingleStackAndRefreshDuration:
		if(EffectStackCount-1 == 0)
		{
			
#if !UE_BUILD_SHIPPING
			if(bDebugEnabled)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Emerald,
					FString::Printf(TEXT("Removing Stacking Effect for Tag [%s]."), *StackingEffectTag.GetTagName().ToString()));
			}
#endif
			
			HandleEffectFillImageRemoval(StackingEffectTag);
			return;
		}
		
		EffectStackCount--;
		RefreshStackingEffectDuration(ExpirationPolicy, Duration, StackingEffectTag);
		break;
	case EGameplayEffectStackingExpirationPolicy::RefreshDuration:
		RefreshStackingEffectDuration(ExpirationPolicy, Duration, StackingEffectTag);
		break;
	default:
		break;
	}
}

void UObsidianEnemyOverlayBarComponent::RefreshStackingEffectDuration(const EGameplayEffectStackingExpirationPolicy& ExpirationPolicy, const float Duration, const FGameplayTag& StackingEffectTag)
{
#if !UE_BUILD_SHIPPING
	if(bDebugEnabled)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
			FString::Printf(TEXT("Refreshing duration for Effect with tag [%s]."), *StackingEffectTag.GetTagName().ToString()));
	}
#endif
	
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return;
	}
	
	if(StackingEffectTimerHandle.IsValid())
	{
		World->GetTimerManager().ClearTimer(StackingEffectTimerHandle);
	}

	World->GetTimerManager().SetTimer(StackingEffectTimerHandle, FTimerDelegate::CreateWeakLambda(this, [ExpirationPolicy, Duration, StackingEffectTag, this]()
		{
			HandleStackingEffectExpiration(ExpirationPolicy, Duration, StackingEffectTag);
		}), Duration, false);
}

bool UObsidianEnemyOverlayBarComponent::GetEffectFillImageForTag(const TArray<FObsidianProgressBarEffectFillImage>& Images, FObsidianProgressBarEffectFillImage& OutFillImage, const FGameplayTag& TagToCheck)
{
	if(Images.IsEmpty())
	{
		return false;
	}
	
	for(const FObsidianProgressBarEffectFillImage& EffectFillImage : Images)
	{
		if(EffectFillImage.ProgressBarFillImage.IsSet() && (EffectFillImage.EffectTag == TagToCheck))
		{
			OutFillImage = EffectFillImage;
			return true;
		}
	}
	
#if !UE_BUILD_SHIPPING
	if(bDebugEnabled)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
			FString::Printf(TEXT("There is no Fill Image for Tag [%s]."), *TagToCheck.GetTagName().ToString()));
	}
#endif
	
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
#if !UE_BUILD_SHIPPING
				if(bDebugEnabled)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Emerald,
						FString::Printf(TEXT("Removing Special Effect for Tag [%s]."), *EffectImageTag.GetTagName().ToString()));
				}
#endif
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

void UObsidianEnemyOverlayBarComponent::HandleSpecialEffectImageRemoval(const FGameplayTag& EffectImageTag)
{
	if(!CachedSpecialEffectFillImages.IsEmpty())
	{
		for(int i = 0; i < CachedSpecialEffectFillImages.Num(); i++)
		{
			if(CachedSpecialEffectFillImages[i].EffectTag == EffectImageTag)
			{
				CachedSpecialEffectFillImages.RemoveAt(i);
			}
		}

		if(CachedSpecialEffectFillImages.Num() != 0)
		{
			OnNewOverlayBarSpecialEffectNeededDelegate.Broadcast(CachedSpecialEffectFillImages.Last().ProgressBarFillImage);
		}
		else
		{
			OnOverlayBarSpecialEffectResetDelegate.Broadcast();
		}
		return;
	}
	OnOverlayBarSpecialEffectResetDelegate.Broadcast();
}

void UObsidianEnemyOverlayBarComponent::HealthChanged(const FOnAttributeChangeData& Data) const
{
	OnHealthChangedDelegate.ExecuteIfBound(Data.NewValue);
}

void UObsidianEnemyOverlayBarComponent::MaxHealthChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxHealthChangedDelegate.ExecuteIfBound(Data.NewValue);
}

void UObsidianEnemyOverlayBarComponent::EnergyShieldChanged(const FOnAttributeChangeData& Data) const
{
	OnEnergyShieldChangedDelegate.ExecuteIfBound(Data.NewValue);
}

void UObsidianEnemyOverlayBarComponent::MaxEnergyShieldChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxEnergyShieldChangedDelegate.ExecuteIfBound(Data.NewValue);
}

void UObsidianEnemyOverlayBarComponent::StaggerMeterChanged(const FOnAttributeChangeData& Data) const
{
	OnStaggerMeterChangedDelegate.ExecuteIfBound(Data.NewValue);
}

void UObsidianEnemyOverlayBarComponent::MaxStaggerMeterChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxStaggerMeterChangedDelegate.ExecuteIfBound(Data.NewValue);
}


