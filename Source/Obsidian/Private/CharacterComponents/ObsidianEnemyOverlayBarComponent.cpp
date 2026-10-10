// Copyright 2026 out of sCope team - intrxx

#include "CharacterComponents/ObsidianEnemyOverlayBarComponent.h"

#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "CharacterComponents/Attributes/ObsidianEnemyAttributesComponent.h"
#include "Core/FunctionLibraries/ObsidianUIFunctionLibrary.h"
#include "Obsidian/ObsidianLogCategories.h"
#include "UI/ProgressBars/ObsidianProgressBarBase.h"


UObsidianEnemyOverlayBarComponent::UObsidianEnemyOverlayBarComponent(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	EffectTag = FGameplayTag::RequestGameplayTag(FName("UI.EffectData.Effect"));
	SpecialEffectTag = FGameplayTag::RequestGameplayTag(FName("UI.EffectData.Effect.Special"));
}

bool UObsidianEnemyOverlayBarComponent::GetCurrentOverlayFillBarEffect(FSlateBrush& OutCurrentFillBarEffect)
{
	if(CachedEffectFillImages.IsEmpty())
	{
		return false;
	}
	OutCurrentFillBarEffect = CachedEffectFillImages.Last().ProgressBarFillImage;
	
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

void UObsidianEnemyOverlayBarComponent::HandleEnemyEffectApplied(const FObsidianEffectUIData& InUIData)
{
	// We don't care about any Instant gameplay effects, might want to change it later.
	if(InUIData.EffectDurationPolicy == EGameplayEffectDurationType::Instant)
	{
		return;
	}
	
	for(const FGameplayTag& Tag : InUIData.AssetTags)
	{
		if(Tag.MatchesTag(SpecialEffectTag)) // "UI.EffectData.Effect.Special"
		{
			UE_LOG(ObLogUI, Verbose, TEXT("Special Effect [%s] is applied to enemy."), *Tag.GetTagName().ToString());

			HandleSpecialEffect(Tag);

			continue;
		}
		
		if(Tag.MatchesTag(EffectTag)) // "UI.EffectData.Effect"
		{
			FObsidianEffectUIDataWidgetRow* Row = UObsidianUIFunctionLibrary::GetDataTableRowByTag<FObsidianEffectUIDataWidgetRow>(UIEffectDataTable, Tag);
			Row->EffectDuration = InUIData.EffectDuration;
					
			if(InUIData.bStackingEffect)
			{
				HandleStackingEffect(*Row, InUIData.StackingData);
			}
			else
			{
				HandleRegularEffect(*Row);
			}
		}
	}
}

void UObsidianEnemyOverlayBarComponent::HandleStackingEffect(const FObsidianEffectUIDataWidgetRow& InRow, const FObsidianEffectUIStackingData& InStackingData)
{
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return;
	}

	FObsidianProgressBarEffectFillImage FillImage;
	if(!GetEffectFillImageForTag(ProgressBarEffectFillImages, /* OUT */ FillImage, InRow.EffectTag))
	{
		return;
	}
	
	bool bAlreadyApplied = false;
	for(const FObsidianProgressBarEffectFillImage& EffectImage : CachedEffectFillImages)
	{
		if(EffectImage.EffectTag == InRow.EffectTag)
		{
			bAlreadyApplied = true;
		}
	}
	
	if(bAlreadyApplied && InStackingData.EffectStackingDurationPolicy == EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication)
	{
		EffectStackCount++;
		
		RefreshStackingEffectDuration(InStackingData.EffectExpirationDurationPolicy, InRow.EffectDuration, InRow.EffectTag);
		
		return;
	}
	
#if !UE_BUILD_SHIPPING
	if(bDebugEnabled)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Emerald,
			FString::Printf(TEXT("Adding Stacking Effect [%s] on Enemy [%s]."), *InRow.EffectName.ToString(), *EnemyAttributesComp->GetEnemyName().ToString()));
	}
#endif
		
	OnNewOverlayBarStyleNeededDelegate.Broadcast(FillImage.ProgressBarFillImage);
	CachedEffectFillImages.Add(FillImage);
	EffectStackCount = 1;
		
	World->GetTimerManager().SetTimer(StackingEffectTimerHandle, FTimerDelegate::CreateWeakLambda(this, [InStackingData, InRow, this]()
		{
			HandleStackingEffectExpiration(InStackingData.EffectExpirationDurationPolicy, InRow.EffectDuration, InRow.EffectTag);
		}), InRow.EffectDuration, false);
}

void UObsidianEnemyOverlayBarComponent::HandleRegularEffect(const FObsidianEffectUIDataWidgetRow& InRow)
{
#if !UE_BUILD_SHIPPING
	if(bDebugEnabled)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
			FString::Printf(TEXT("Adding Regular Effect [%s] on Enemy [%s]."), *InRow.EffectName.ToString(), *EnemyAttributesComp->GetEnemyName().ToString()));
	}
#endif

	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return;
	}
	
	FObsidianProgressBarEffectFillImage FillImage;
	if(GetEffectFillImageForTag(ProgressBarEffectFillImages, /* OUT */FillImage, InRow.EffectTag))
	{
		OnNewOverlayBarStyleNeededDelegate.Broadcast(FillImage.ProgressBarFillImage);
		CachedEffectFillImages.Add(FillImage);
			
		FTimerHandle EffectExpiredDelegateHandle;
		World->GetTimerManager().SetTimer(EffectExpiredDelegateHandle, FTimerDelegate::CreateWeakLambda(this, [this, InRow]()
			{
			
#if !UE_BUILD_SHIPPING
if(bDebugEnabled)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
		FString::Printf(TEXT("Removing Regular Effect for Tag [%s]."), *InRow.EffectTag.GetTagName().ToString()));
}
#endif
			
				HandleEffectFillImageRemoval(InRow.EffectTag);
				
			}), InRow.EffectDuration, false);
	}
}


void UObsidianEnemyOverlayBarComponent::HandleSpecialEffect(const FGameplayTag& InEffectImageTag)
{
#if !UE_BUILD_SHIPPING
	if(bDebugEnabled)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
			FString::Printf(TEXT("Adding Special Effect with Tag [%s] on Enemy [%s]."), *InEffectImageTag.ToString(), *EnemyAttributesComp->GetEnemyName().ToString()));
	}
#endif
	
	FObsidianProgressBarEffectFillImage SpecialImage;
	if(GetEffectFillImageForTag(ProgressBarSpecialEffects, SpecialImage, InEffectImageTag))
	{
		OnNewOverlayBarSpecialEffectNeededDelegate.Broadcast(SpecialImage.ProgressBarFillImage);
		CachedSpecialEffectFillImages.Add(SpecialImage);
	}
}

void UObsidianEnemyOverlayBarComponent::HandleStackingEffectExpiration(const EGameplayEffectStackingExpirationPolicy& InExpirationPolicy, const float InDuration, const FGameplayTag& InStackingEffectTag)
{
	switch(InExpirationPolicy)
	{
	case EGameplayEffectStackingExpirationPolicy::ClearEntireStack:
		
#if !UE_BUILD_SHIPPING
		if(bDebugEnabled)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Emerald,
				FString::Printf(TEXT("Removing Stacking Effect for Tag [%s]."), *InStackingEffectTag.GetTagName().ToString()));
		}
#endif
		
		HandleEffectFillImageRemoval(InStackingEffectTag);
		break;
	case EGameplayEffectStackingExpirationPolicy::RemoveSingleStackAndRefreshDuration:
		if(EffectStackCount-1 == 0)
		{
			
#if !UE_BUILD_SHIPPING
			if(bDebugEnabled)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Emerald,
					FString::Printf(TEXT("Removing Stacking Effect for Tag [%s]."), *InStackingEffectTag.GetTagName().ToString()));
			}
#endif
			
			HandleEffectFillImageRemoval(InStackingEffectTag);
			return;
		}
		
		EffectStackCount--;
		RefreshStackingEffectDuration(InExpirationPolicy, InDuration, InStackingEffectTag);
		break;
	case EGameplayEffectStackingExpirationPolicy::RefreshDuration:
		RefreshStackingEffectDuration(InExpirationPolicy, InDuration, InStackingEffectTag);
		break;
	default:
		break;
	}
}

void UObsidianEnemyOverlayBarComponent::RefreshStackingEffectDuration(const EGameplayEffectStackingExpirationPolicy& InExpirationPolicy, const float InDuration, const FGameplayTag& InStackingEffectTag)
{
#if !UE_BUILD_SHIPPING
	if(bDebugEnabled)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
			FString::Printf(TEXT("Refreshing duration for Effect with tag [%s]."), *InStackingEffectTag.GetTagName().ToString()));
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

	World->GetTimerManager().SetTimer(StackingEffectTimerHandle, FTimerDelegate::CreateWeakLambda(this, [InExpirationPolicy, InDuration, InStackingEffectTag, this]()
		{
			HandleStackingEffectExpiration(InExpirationPolicy, InDuration, InStackingEffectTag);
		}), InDuration, false);
}

bool UObsidianEnemyOverlayBarComponent::GetEffectFillImageForTag(const TArray<FObsidianProgressBarEffectFillImage>& InImages, FObsidianProgressBarEffectFillImage& OutFillImage, const FGameplayTag& InTagToCheck)
{
	if(InImages.IsEmpty())
	{
		return false;
	}
	
	for(const FObsidianProgressBarEffectFillImage& EffectFillImage : InImages)
	{
		if(EffectFillImage.ProgressBarFillImage.IsSet() && (EffectFillImage.EffectTag == InTagToCheck))
		{
			OutFillImage = EffectFillImage;
			return true;
		}
	}
	
#if !UE_BUILD_SHIPPING
	if(bDebugEnabled)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
			FString::Printf(TEXT("There is no Fill Image for Tag [%s]."), *InTagToCheck.GetTagName().ToString()));
	}
#endif
	
	return false;
}

void UObsidianEnemyOverlayBarComponent::HandleEffectFillImageRemoval(const FGameplayTag& InEffectImageTag)
{
	if(!CachedEffectFillImages.IsEmpty())
	{
		for(int i = 0; i < CachedEffectFillImages.Num(); i++)
		{
			if(CachedEffectFillImages[i].EffectTag == InEffectImageTag)
			{
#if !UE_BUILD_SHIPPING
				if(bDebugEnabled)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Emerald,
						FString::Printf(TEXT("Removing Special Effect for Tag [%s]."), *InEffectImageTag.GetTagName().ToString()));
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

void UObsidianEnemyOverlayBarComponent::HandleSpecialEffectImageRemoval(const FGameplayTag& InEffectImageTag)
{
	if(!CachedSpecialEffectFillImages.IsEmpty())
	{
		for(int i = 0; i < CachedSpecialEffectFillImages.Num(); i++)
		{
			if(CachedSpecialEffectFillImages[i].EffectTag == InEffectImageTag)
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

void UObsidianEnemyOverlayBarComponent::HealthChanged(const FOnAttributeChangeData& InData) const
{
	OnHealthChangedDelegate.ExecuteIfBound(InData.NewValue);
}

void UObsidianEnemyOverlayBarComponent::MaxHealthChanged(const FOnAttributeChangeData& InData) const
{
	OnMaxHealthChangedDelegate.ExecuteIfBound(InData.NewValue);
}

void UObsidianEnemyOverlayBarComponent::EnergyShieldChanged(const FOnAttributeChangeData& InData) const
{
	OnEnergyShieldChangedDelegate.ExecuteIfBound(InData.NewValue);
}

void UObsidianEnemyOverlayBarComponent::MaxEnergyShieldChanged(const FOnAttributeChangeData& InData) const
{
	OnMaxEnergyShieldChangedDelegate.ExecuteIfBound(InData.NewValue);
}

void UObsidianEnemyOverlayBarComponent::StaggerMeterChanged(const FOnAttributeChangeData& InData) const
{
	OnStaggerMeterChangedDelegate.ExecuteIfBound(InData.NewValue);
}

void UObsidianEnemyOverlayBarComponent::MaxStaggerMeterChanged(const FOnAttributeChangeData& InData) const
{
	OnMaxStaggerMeterChangedDelegate.ExecuteIfBound(InData.NewValue);
}


