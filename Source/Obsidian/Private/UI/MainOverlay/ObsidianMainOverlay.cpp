// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/MainOverlay/ObsidianMainOverlay.h"
#include "UI/ProgressBars/ProgressGlobe/ObsidianProgressGlobe_Health.h"
#include "CharacterComponents/ObsidianEnemyOverlayBarComponent.h"
#include "Components/Overlay.h"
#include "Components/VerticalBox.h"
#include "Components/WrapBox.h"
#include "Core/ObsidianUIFunctionLibrary.h"
#include "UI/WidgetControllers/OCharacterStatusWidgetController.h"
#include "ObsidianTypes/ObsidianUITypes.h"
#include "UI/CharacterStatus/ObsidianCharacterStatus.h"
#include "UI/GameTabsMenu/ObsidianOverlayGameTabsMenu.h"
#include "UI/ProgressBars/ObsidianOverlayBossEnemyBar.h"
#include "UI/GameTabsMenu/Subwidgets/ObsidianGameTabButton.h"
#include "UI/WidgetControllers/MainOverlayWidgetController.h"
#include "UI/MainOverlay/Subwidgets/OStackingDurationalEffectInfo.h"
#include "UI/MainOverlay/Subwidgets/ObsidianDurationalEffectInfo.h"
#include "UI/ProgressBars/UObsidianOverlayEnemyBar.h"
#include "UI/ProgressBars/ProgressGlobe/ObsidianProgressGlobeBase.h"

void UObsidianMainOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	OwningPlayerController = GetOwningPlayer();

	if(Overlay_GameTabsMenu)
	{
		Overlay_GameTabsMenu->OnCharacterStatusButtonClickedDelegate.AddUObject(this, &UObsidianMainOverlay::ToggleCharacterStatus);
	}
}

void UObsidianMainOverlay::ToggleCharacterStatus()
{
	if(!CharacterStatus)
	{
		CharacterStatus = CreateWidget<UObsidianCharacterStatus>(this, CharacterStatusClass);
		UOCharacterStatusWidgetController* CharacterStatusWidgetController = UObsidianUIFunctionLibrary::GetCharacterStatusWidgetController(this);
		CharacterStatus->SetWidgetController(CharacterStatusWidgetController);
		CharacterStatusWidgetController->SetInitialAttributeValues();
		
		CharacterStatus_Overlay->AddChildToOverlay(CharacterStatus);
		CharacterStatus->OnCharacterStatusDestroyedDelegate.AddLambda([this]()
		{
			CharacterStatus = nullptr;

			if(Overlay_GameTabsMenu && Overlay_GameTabsMenu->CharacterStatus_GameTabButton)
			{
				Overlay_GameTabsMenu->CharacterStatus_GameTabButton->bIsCorrespondingTabOpen = false;
			}
		});

		Overlay_GameTabsMenu->OnCharacterStatusTabStatusChangeDelegate.Broadcast(true);
	}
	else
	{
		CharacterStatus->RemoveFromParent();
		CharacterStatus = nullptr;
		Overlay_GameTabsMenu->OnCharacterStatusTabStatusChangeDelegate.Broadcast(false);
	}
}

void UObsidianMainOverlay::HandleStackingUIData(const FObsidianEffectUIDataWidgetRow Row, const FObsidianEffectUIStackingData StackingData)
{
	if(StackingInfoWidgetsMap.Contains(Row.EffectTag))
	{
		StackingInfoWidgetsMap[Row.EffectTag]->UpdateStackingInfoWidget(StackingData.EffectStackCount);
		return;
	}
	
	UOStackingDurationalEffectInfo* StackingInfoWidget = CreateWidget<UOStackingDurationalEffectInfo>(OwningPlayerController, Row.StackingDurationalEffectWidget);
	StackingInfoWidgetsMap.Add(Row.EffectTag, StackingInfoWidget);
	
	StackingInfoWidget->InitDurationalStackingEffectInfo(Row.EffectName, Row.EffectDesc, Row.EffectImage, Row.EffectDuration, StackingData);

	FObsidianProgressBarEffectFillImage FillImage;
	if(HealthProgressGlobe->GetEffectFillImageForTag(/* OUT */ FillImage, Row.EffectTag))
	{
		HealthProgressGlobe->SetProgressGlobeStyle(FillImage.ProgressBarFillImage);
		EffectFillImages.Add(FillImage);
	}
	
	StackingInfoWidget->OnStackingInfoWidgetTerminatedDelegate.AddLambda([Row, this](UOStackingDurationalEffectInfo* WidgetToDestroy)
	{
		DestroyStackingInfoWidget(WidgetToDestroy);
		HandleEffectFillImageRemoval(Row.EffectTag);
	});

	switch(Row.EffectClassification)
	{
	case EObsidianUIEffectClassification::EUEC_Buff:
		BuffsEffectInfo_WrapBox->AddChild(StackingInfoWidget);
		break;
	case EObsidianUIEffectClassification::EUEC_Debuff:
		DeBuffsEffectInfo_WrapBox->AddChild(StackingInfoWidget);
		break;
	default:
		BuffsEffectInfo_WrapBox->AddChild(StackingInfoWidget);
		break;
	}
}

void UObsidianMainOverlay::HandleUIData(const FObsidianEffectUIDataWidgetRow Row)
{
	if(Row.InfoWidgetType == EObsidianInfoWidgetType::EIWT_SimpleEffectInfo)
	{
		UObsidianEffectInfoBase* InfoWidget = CreateWidget<UObsidianEffectInfoBase>(OwningPlayerController, Row.SimpleEffectWidget);
		InfoWidget->InitEffectInfo(Row.EffectName, Row.EffectDesc, Row.EffectImage, Row.EffectTag);
		
		switch(Row.EffectClassification)
		{
		case EObsidianUIEffectClassification::EUEC_Buff:
			BuffsEffectInfo_WrapBox->AddChild(InfoWidget);
			break;
		case EObsidianUIEffectClassification::EUEC_Debuff:
			DeBuffsEffectInfo_WrapBox->AddChild(InfoWidget);
			break;
		default:
			BuffsEffectInfo_WrapBox->AddChild(InfoWidget);
			break;
		}

		if(Row.bAuraEffect)
		{
			AuraUIInfoArray.AddUnique(InfoWidget);
		}

		return;
	}
	
	if(Row.InfoWidgetType == EObsidianInfoWidgetType::EIWT_DurationalEffectInfo)
	{
		UObsidianDurationalEffectInfo* DurationalInfoWidget = CreateWidget<UObsidianDurationalEffectInfo>(OwningPlayerController, Row.DurationalEffectWidget);
		DurationalInfoWidget->InitDurationalEffectInfo(Row.EffectName, Row.EffectDesc, Row.EffectImage, Row.EffectDuration);

		FObsidianProgressBarEffectFillImage FillImage;
		if(HealthProgressGlobe->GetEffectFillImageForTag(/* OUT */FillImage, Row.EffectTag))
		{
			HealthProgressGlobe->SetProgressGlobeStyle(FillImage.ProgressBarFillImage);
			EffectFillImages.Add(FillImage);
			
			DurationalInfoWidget->OnDurationalInfoWidgetTerminatedDelegate.AddLambda([Row, this](UObsidianDurationalEffectInfo* WidgetToDestroy)
			{
				HandleEffectFillImageRemoval(Row.EffectTag);
			});
		}

		switch(Row.EffectClassification)
		{
		case EObsidianUIEffectClassification::EUEC_Buff:
			BuffsEffectInfo_WrapBox->AddChild(DurationalInfoWidget);
			break;
		case EObsidianUIEffectClassification::EUEC_Debuff:
			DeBuffsEffectInfo_WrapBox->AddChild(DurationalInfoWidget);
			break;
		default:
			BuffsEffectInfo_WrapBox->AddChild(DurationalInfoWidget);
			break;
		}
	}
}

void UObsidianMainOverlay::HandleWidgetControllerSet()
{
	MainOverlayWidgetController = CastChecked<UMainOverlayWidgetController>(WidgetController);

	MainOverlayWidgetController->OnAuraWidgetDestructionInfoReceivedDelegate.BindDynamic(this, &ThisClass::DestroyAuraInfoWidget);

	MainOverlayWidgetController->OnUpdateRegularEnemyTargetForHealthBarDelegate.AddDynamic(this, &ThisClass::HandleRegularOverlayBar);
	MainOverlayWidgetController->OnUpdateBossEnemyTargetForHealthBarDelegate.AddDynamic(this, &ThisClass::HandleBossOverlayBar);
}

void UObsidianMainOverlay::HandleRegularOverlayBar(AActor* TargetActor, bool bDisplayBar)
{
	if(bDisplayBar)
	{
		RegularEnemyOverlayHealthBar = CreateWidget<UObsidianOverlayEnemyBar>(OwningPlayerController, RegularEnemyOverlayHealthBarClass);
		
		UObsidianEnemyOverlayBarComponent* EnemyOverlayBarComponent = UObsidianEnemyOverlayBarComponent::FindEnemyOverlayComponent(TargetActor);
		check(EnemyOverlayBarComponent);
		
		RegularEnemyOverlayHealthBar->SetWidgetController(EnemyOverlayBarComponent);
		OverlayRegularBars_Overlay->AddChildToOverlay(RegularEnemyOverlayHealthBar);
	}
	else
	{
		if(RegularEnemyOverlayHealthBar)
		{
			RegularEnemyOverlayHealthBar->RemoveFromParent();
			RegularEnemyOverlayHealthBar = nullptr;
		}
	}
}

void UObsidianMainOverlay::HandleBossOverlayBar(AActor* TargetActor, bool bDisplayBar)
{
	if(bDisplayBar)
	{
		BossEnemyOverlayHealthBar = CreateWidget<UObsidianOverlayBossEnemyBar>(OwningPlayerController, BossEnemyOverlayHealthBarClass);
		
		UObsidianEnemyOverlayBarComponent* EnemyOverlayBarComponent = UObsidianEnemyOverlayBarComponent::FindEnemyOverlayComponent(TargetActor);
		check(EnemyOverlayBarComponent);
		
		BossEnemyOverlayHealthBar->SetWidgetController(EnemyOverlayBarComponent);
		OverlayBossBars_Overlay->AddChildToOverlay(BossEnemyOverlayHealthBar);
	}
	else
	{
		if(BossEnemyOverlayHealthBar)
		{
			BossEnemyOverlayHealthBar->RemoveFromParent();
			BossEnemyOverlayHealthBar = nullptr;
		}
	}
}

void UObsidianMainOverlay::DestroyStackingInfoWidget(UOStackingDurationalEffectInfo* WidgetToDestroy)
{
	if(const FGameplayTag* Key = StackingInfoWidgetsMap.FindKey(WidgetToDestroy))
	{
		StackingInfoWidgetsMap.Remove(*Key);
	}
}

void UObsidianMainOverlay::HandleEffectFillImageRemoval(const FGameplayTag& EffectTag)
{
	if(!EffectFillImages.IsEmpty())
	{
		for(int i = 0; i < EffectFillImages.Num(); i++)
		{
			if(EffectFillImages[i].EffectTag == EffectTag)
			{
				EffectFillImages.RemoveAt(i);
			}
		}

		if(EffectFillImages.Num() != 0)
		{
			HealthProgressGlobe->SetProgressGlobeStyle(EffectFillImages.Last().ProgressBarFillImage);
		}
		else
		{
			HealthProgressGlobe->ResetStyle();
		}
		
		return;
	}
	HealthProgressGlobe->ResetStyle();
}

void UObsidianMainOverlay::DestroyAuraInfoWidget(const FGameplayTag WidgetToDestroyWithTag)
{
	if(AuraUIInfoArray.IsEmpty())
	{
		return;
	}

	for(UObsidianEffectInfoBase* Widget : AuraUIInfoArray)
	{
		if(Widget->UIEffectTag == WidgetToDestroyWithTag)
		{
			Widget->RemoveAuraInfoWidget();
		}
	}
}



