// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/MainOverlay/ObsidianMainOverlay.h"
#include "UI/ProgressBars/ProgressGlobe/ObsidianProgressGlobe_Health.h"
#include "UI/ProgressBars/ProgressGlobe/ObsidianProgressGlobe_Mana.h"
#include "CharacterComponents/ObsidianEnemyOverlayBarComponent.h"
#include "Components/Overlay.h"
#include "Components/VerticalBox.h"
#include "Components/WrapBox.h"
#include "CharacterComponents/ObsidianHeroComponent.h"
#include "Core/ObsidianUIFunctionLibrary.h"
#include "UI/WidgetControllers/ObsidianInventoryWidgetController.h"
#include "UI/WidgetControllers/OCharacterStatusWidgetController.h"
#include "ObsidianTypes/ObsidianUITypes.h"
#include "UI/CharacterStatus/ObsidianCharacterStatus.h"
#include "UI/GameTabsMenu/ObsidianOverlayGameTabsMenu.h"
#include "UI/ProgressBars/ObsidianOverlayBossEnemyBar.h"
#include "UI/PassiveSkillTree/ObsidianPassiveSkillTree.h"
#include "UI/GameTabsMenu/Subwidgets/ObsidianGameTabButton.h"
#include "UI/Inventory/ObsidianInventory.h"
#include "UI/WidgetControllers/MainOverlayWidgetController.h"
#include "UI/MainOverlay/Subwidgets/OStackingDurationalEffectInfo.h"
#include "UI/MainOverlay/Subwidgets/ObsidianDurationalEffectInfo.h"
#include "UI/ProgressBars/UObsidianOverlayEnemyBar.h"

void UObsidianMainOverlay::HandleWidgetControllerSet()
{
	MainOverlayWidgetController = CastChecked<UMainOverlayWidgetController>(WidgetController);
	MainOverlayWidgetController->EffectUIDataWidgetRowDelegate.AddDynamic(this, &ThisClass::HandleUIData);
	MainOverlayWidgetController->EffectStackingUIDataDelegate.AddDynamic(this, &ThisClass::HandleStackingUIData);

	MainOverlayWidgetController->OnAuraWidgetDestructionInfoReceivedDelegate.BindDynamic(this, &ThisClass::DestroyAuraInfoWidget);

	MainOverlayWidgetController->OnUpdateRegularEnemyTargetForHealthBarDelegate.AddDynamic(this, &ThisClass::HandleRegularOverlayBar);
	MainOverlayWidgetController->OnUpdateBossEnemyTargetForHealthBarDelegate.AddDynamic(this, &ThisClass::HandleBossOverlayBar);

	OwningPlayerController = OwningPlayerController == nullptr ? GetOwningPlayer() : OwningPlayerController;
	check(OwningPlayerController);
	const AActor* OwningActor = Cast<AActor>(OwningPlayerController->GetPawn());
	HeroComp = UObsidianHeroComponent::FindHeroComponent(OwningActor);
}

void UObsidianMainOverlay::PostHandleWidgetControllerSet()
{
	HealthProgressGlobe->OnMouseEnterLeaveDelegate.AddUObject(this, &UObsidianMainOverlay::SetPlayerMouseOverGlobe);
	ManaProgressGlobe->OnMouseEnterLeaveDelegate.AddUObject(this, &UObsidianMainOverlay::SetPlayerMouseOverGlobe);
}

void UObsidianMainOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	OwningPlayerController = GetOwningPlayer();

	if(Overlay_GameTabsMenu)
	{
		Overlay_GameTabsMenu->OnCharacterStatusButtonClickedDelegate.AddUObject(this, &UObsidianMainOverlay::ToggleCharacterStatus);
		Overlay_GameTabsMenu->OnInventoryButtonClickedDelegate.AddUObject(this, &ThisClass::ToggleInventory);
		Overlay_GameTabsMenu->OnPassiveSkillTreeButtonClickedDelegate.AddUObject(this, &ThisClass::TogglePassiveSkillTree);
	}
}

void UObsidianMainOverlay::ToggleCharacterStatus()
{
	if(!CharacterStatus)
	{
		UOCharacterStatusWidgetController* CharacterStatusWidgetController = UObsidianUIFunctionLibrary::GetCharacterStatusWidgetController(this);
		CharacterStatus = CreateWidget<UObsidianCharacterStatus>(this, CharacterStatusClass);
		CharacterStatus->SetWidgetController(CharacterStatusWidgetController);
		
		CharacterStatusWidgetController->SetInitialAttributeValues();
		
		CharacterStatus_Overlay->AddChildToOverlay(CharacterStatus);
		CharacterStatus->OnMouseEnterLeaveDelegate.AddUObject(this, &ThisClass::SetPlayerMouseOverCharacterStatus);
		CharacterStatus->OnWidgetDestroyedDelegate.AddLambda([this]()
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
		SetPlayerMouseOverCharacterStatus(false);
		CharacterStatus = nullptr;
		Overlay_GameTabsMenu->OnCharacterStatusTabStatusChangeDelegate.Broadcast(false);
	}
}

void UObsidianMainOverlay::ToggleInventory()
{
	if(!Inventory)
	{
		InventoryWidgetController = UObsidianUIFunctionLibrary::GetInventoryWidgetController(this);
		Inventory = CreateWidget<UObsidianInventory>(this, InventoryClass);
		Inventory->SetWidgetController(InventoryWidgetController);
		
		Inventory_Overlay->AddChildToOverlay(Inventory);
		Inventory->OnMouseEnterLeaveDelegate.AddUObject(this, &ThisClass::SetPlayerMouseOverInventory);
		Inventory->OnWidgetDestroyedDelegate.AddLambda([this]()
			{
				Inventory = nullptr;

				if(Overlay_GameTabsMenu && Overlay_GameTabsMenu->Inventory_GameTabButton)
				{
					Overlay_GameTabsMenu->Inventory_GameTabButton->bIsCorrespondingTabOpen = false;
				}
				InventoryWidgetController->SetInventoryOpened(false);
			});

		InventoryWidgetController->OnInventoryOpen();
		InventoryWidgetController->SetInventoryOpened(true);
	}
	else
	{
		Inventory->RemoveFromParent();
		SetPlayerMouseOverInventory(false);
		Inventory = nullptr;
		Overlay_GameTabsMenu->OnInventoryTabStatusChangeDelegate.Broadcast(false);
		if(InventoryWidgetController)
		{
			InventoryWidgetController->SetInventoryOpened(false);
		}
		else
		{
			InventoryWidgetController = UObsidianUIFunctionLibrary::GetInventoryWidgetController(this);
			InventoryWidgetController->SetInventoryOpened(false);
		}
	}
}

void UObsidianMainOverlay::TogglePassiveSkillTree()
{
	if(!PassiveSkillTree)
	{
		PassiveSkillTree = CreateWidget<UObsidianPassiveSkillTree>(this, PassiveSkillTreeClass);

		PassiveSkillTree_Overlay->AddChildToOverlay(PassiveSkillTree);
		PassiveSkillTree->OnMouseEnterLeaveDelegate.AddUObject(this, &ThisClass::SetPlayerMouseOverPassiveSkillTree);
		PassiveSkillTree->OnWidgetDestroyedDelegate.AddLambda([this]()
			{
				PassiveSkillTree = nullptr;

				if(Overlay_GameTabsMenu && Overlay_GameTabsMenu->PassiveSkillTree_GameTabButton)
				{
					Overlay_GameTabsMenu->PassiveSkillTree_GameTabButton->bIsCorrespondingTabOpen = false;
				}
			});
	}
	else
	{
		PassiveSkillTree->RemoveFromParent();
		SetPlayerMouseOverPassiveSkillTree(false);
		PassiveSkillTree = nullptr;
		Overlay_GameTabsMenu->OnPassiveSkillTreeTabStatusChangeDelegate.Broadcast(false);
	}
}

void UObsidianMainOverlay::SetPlayerMouseOverInventory(const bool bInMouseOver)
{
	bPlayerMouseOverInventory = bInMouseOver;
	UpdatePlayerMouseOverUIElem();
}

void UObsidianMainOverlay::SetPlayerMouseOverCharacterStatus(const bool bInMouseOver)
{
	bPlayerMouseOverCharacterStatus = bInMouseOver;
	UpdatePlayerMouseOverUIElem();
}

void UObsidianMainOverlay::SetPlayerMouseOverPassiveSkillTree(const bool bInMouseOver)
{
	bPlayerMouseOverPassiveSkillTree = bInMouseOver;
	UpdatePlayerMouseOverUIElem();
}

void UObsidianMainOverlay::SetPlayerMouseOverGlobe(const bool bInMouseOver)
{
	bPlayerMouseOverGlobe = bInMouseOver;
	UpdatePlayerMouseOverUIElem();
}

void UObsidianMainOverlay::SetPlayerMouseOverButtonMenu(const bool bInMouseOver)
{
	bPlayerMouseOverButtonMenu = bInMouseOver;
	UpdatePlayerMouseOverUIElem();
}

void UObsidianMainOverlay::UpdatePlayerMouseOverUIElem() const
{
	const bool bMouseOverAnyUIElem = bPlayerMouseOverInventory || bPlayerMouseOverCharacterStatus ||
		bPlayerMouseOverPassiveSkillTree || bPlayerMouseOverGlobe || bPlayerMouseOverButtonMenu;
	HeroComp->SetCursorOverUI(bMouseOverAnyUIElem);
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



