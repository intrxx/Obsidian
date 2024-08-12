// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/MainOverlay/ObsidianMainOverlay.h"

#include "CharacterComponents/Attributes/ObsidianEnemyAttributesComponent.h"
#include "Components/Overlay.h"
#include "Components/SizeBox.h"
#include "Components/VerticalBox.h"
#include "Components/WrapBox.h"
#include "Core/ObsidianUIFunctionLibrary.h"
#include "UI/WidgetControllers/OCharacterStatusWidgetController.h"
#include "ObsidianTypes/UserIterface/ObsidianUIEffectClassification.h"
#include "UI/CharacterStatus/ObsidianCharacterStatus.h"
#include "UI/GameTabsMenu/ObsidianOverlayGameTabsMenu.h"
#include "UI/GameTabsMenu/Subwidgets/ObsidianGameTabButton.h"
#include "UI/WidgetControllers/MainOverlayWidgetController.h"
#include "UI/MainOverlay/Subwidgets/OStackingDurationalEffectInfo.h"
#include "UI/MainOverlay/Subwidgets/ObsidianDurationalEffectInfo.h"
#include "UI/ProgressBars/ObsidianOverlayRegularEnemyBar.h"
#include "UI/ProgressBars/ObsidianProgressGlobe.h"

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

	FSlateBrush FillImage;
	if(HealthProgressGlobe->GetEffectFillImageForTag(FillImage, Row.EffectTag))
	{
		HealthProgressGlobe->SetProgressGlobeStyle(FillImage);
	}
	
	StackingInfoWidget->OnStackingInfoWidgetTerminatedDelegate.AddUObject(this, &ThisClass::DestroyStackingInfoWidget);

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

		FSlateBrush FillImage;
		if(HealthProgressGlobe->GetEffectFillImageForTag(FillImage, Row.EffectTag))
		{
			HealthProgressGlobe->SetProgressGlobeStyle(FillImage);

			DurationalInfoWidget->OnDurationalInfoWidgetTerminatedDelegate.AddLambda([this](UObsidianDurationalEffectInfo* WidgetToDestroy)
			{
				HealthProgressGlobe->ResetStyle();
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

	MainOverlayWidgetController->OnUpdateEnemyTargetForHealthBarDelegate.AddDynamic(this, &ThisClass::HandleOverlayBar);
}

void UObsidianMainOverlay::HandleOverlayBar(AActor* TargetActor, bool bDisplayBar)
{
	if(bDisplayBar)
	{
		RegularEnemyHealthBar = CreateWidget<UObsidianOverlayRegularEnemyBar>(OwningPlayerController, RegularEnemyHealthBarClass);
		
		UObsidianEnemyAttributesComponent* EnemyAttributesComponent = UObsidianEnemyAttributesComponent::FindAttributesComponent(TargetActor);
		check(EnemyAttributesComponent);
		
		RegularEnemyHealthBar->SetWidgetController(EnemyAttributesComponent);
		OverlayBars_VerticalBox->AddChild(RegularEnemyHealthBar);
	}
	else
	{
		RegularEnemyHealthBar->RemoveFromParent();
		RegularEnemyHealthBar = nullptr;
	}

	
}

void UObsidianMainOverlay::DestroyStackingInfoWidget(UOStackingDurationalEffectInfo* WidgetToDestroy)
{
	if(const FGameplayTag* Key = StackingInfoWidgetsMap.FindKey(WidgetToDestroy))
	{
		StackingInfoWidgetsMap.Remove(*Key);
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


