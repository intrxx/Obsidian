// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/MainOverlay/ObsidianMainOverlay.h"
#include "Components/Overlay.h"
#include "Components/WrapBox.h"
#include "ObsidianTypes/ObsidianUIEffectClassification.h"
#include "UI/CharacterStatus/ObsidianCharacterStatus.h"
#include "UI/GameTabsMenu/ObsidianOverlayGameTabsMenu.h"
#include "UI/WidgetControllers/MainOverlayWidgetController.h"
#include "UI/MainOverlay/Subwidgets/OStackingDurationalEffectInfo.h"
#include "UI/MainOverlay/Subwidgets/ObsidianDurationalEffectInfo.h"

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
		CharacterStatus_Overlay->AddChildToOverlay(CharacterStatus);
		CharacterStatus->OnCharacterStatusDestroyedDelegate.AddLambda([this]()
		{
			CharacterStatus = nullptr;
		});
	}
	else
	{
		CharacterStatus->RemoveFromParent();
		CharacterStatus = nullptr;
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
	StackingInfoWidget->OnStackingInfoWidgetTerminatedDelegate.AddUObject(this, &ThisClass::DestroyStackingInfoWidget);

	switch(Row.EffectClassification)
	{
	case EObsidianUIEffectClassification::Buff:
		BuffsEffectInfo_WrapBox->AddChild(StackingInfoWidget);
		break;
	case EObsidianUIEffectClassification::Debuff:
		DeBuffsEffectInfo_WrapBox->AddChild(StackingInfoWidget);
		break;
	default:
		BuffsEffectInfo_WrapBox->AddChild(StackingInfoWidget);
		break;
	}
}

void UObsidianMainOverlay::HandleUIData(const FObsidianEffectUIDataWidgetRow Row)
{
	if(Row.InfoWidgetType == EObsidianInfoWidgetType::SimpleEffectInfo)
	{
		UObsidianEffectInfoBase* InfoWidget = CreateWidget<UObsidianEffectInfoBase>(OwningPlayerController, Row.SimpleEffectWidget);
		InfoWidget->InitEffectInfo(Row.EffectName, Row.EffectDesc, Row.EffectImage);

		switch(Row.EffectClassification)
		{
		case EObsidianUIEffectClassification::Buff:
			BuffsEffectInfo_WrapBox->AddChild(InfoWidget);
			break;
		case EObsidianUIEffectClassification::Debuff:
			DeBuffsEffectInfo_WrapBox->AddChild(InfoWidget);
			break;
		default:
			BuffsEffectInfo_WrapBox->AddChild(InfoWidget);
			break;
		}

		return;
	}
	
	if(Row.InfoWidgetType == EObsidianInfoWidgetType::DurationalEffectInfo)
	{
		UObsidianDurationalEffectInfo* DurationalInfoWidget = CreateWidget<UObsidianDurationalEffectInfo>(OwningPlayerController, Row.DurationalEffectWidget);
		DurationalInfoWidget->InitDurationalEffectInfo(Row.EffectName, Row.EffectDesc, Row.EffectImage, Row.EffectDuration);

		switch(Row.EffectClassification)
		{
		case EObsidianUIEffectClassification::Buff:
			BuffsEffectInfo_WrapBox->AddChild(DurationalInfoWidget);
			break;
		case EObsidianUIEffectClassification::Debuff:
			DeBuffsEffectInfo_WrapBox->AddChild(DurationalInfoWidget);
			break;
		default:
			BuffsEffectInfo_WrapBox->AddChild(DurationalInfoWidget);
			break;
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

