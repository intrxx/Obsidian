// Copyright 2026 out of sCope team - intrxx

#include "UI/MainOverlay/ObsidianMainOverlay.h"

#include <Blueprint/WidgetLayoutLibrary.h>
#include <Components/Overlay.h>
#include <Components/VerticalBox.h>
#include <Components/WrapBox.h>
#include <Components/CanvasPanelSlot.h>

#include "UI/ProgressBars/ProgressGlobe/ObsidianProgressGlobe_Health.h"
#include "UI/ProgressBars/ProgressGlobe/ObsidianProgressGlobe_Mana.h"
#include "CharacterComponents/ObsidianEnemyOverlayBarComponent.h"
#include "CharacterComponents/ObsidianPlayerInputManager.h"
#include "Core/FunctionLibraries/ObsidianUIFunctionLibrary.h"
#include "UI/WidgetControllers/ObInventoryItemsWidgetController.h"
#include "UI/WidgetControllers/ObCharacterStatusWidgetController.h"
#include "ObsidianTypes/ObsidianUITypes.h"
#include "UI/CharacterStatus/ObsidianCharacterStatus.h"
#include "UI/GameTabsMenu/ObsidianOverlayGameTabsMenu.h"
#include "UI/ProgressBars/ObsidianOverlayBossEnemyBar.h"
#include "UI/PassiveSkillTree/ObsidianPassiveSkillTree.h"
#include "UI/GameTabsMenu/Subwidgets/ObsidianGameTabButton.h"
#include "UI/InventoryItems/ObsidianInventory.h"
#include "UI/WidgetControllers/ObMainOverlayWidgetController.h"
#include "UI/MainOverlay/Subwidgets/OStackingDurationalEffectInfo.h"
#include "UI/MainOverlay/Subwidgets/ObsidianDurationalEffectInfo.h"
#include "UI/ProgressBars/UObsidianOverlayEnemyBar.h"
#include "UI/InventoryItems/Items/ObsidianItemDescriptionBase.h"
#include "UI/ProgressBars/ObsidianOverlayExperienceBar.h"
#include "UI/MainOverlay/SkillPoints/ObsidianSkillPointsNotification.h"
#include "UI/InventoryItems/ObsidianPlayerStashWidget.h"

void UObsidianMainOverlay::HandleWidgetControllerSet()
{
	MainOverlayWidgetController = CastChecked<UObMainOverlayWidgetController>(WidgetController);
	MainOverlayWidgetController->EffectUIDataWidgetRowDelegate.AddDynamic(this, &ThisClass::HandleUIData);
	MainOverlayWidgetController->EffectStackingUIDataDelegate.AddDynamic(this, &ThisClass::HandleStackingUIData);

	MainOverlayWidgetController->OnAuraWidgetDestructionInfoReceivedDelegate.BindDynamic(this, &ThisClass::DestroyAuraInfoWidget);

	MainOverlayWidgetController->OnUpdateRegularEnemyTargetForHealthBarDelegate.AddDynamic(this, &ThisClass::HandleRegularOverlayBar);
	MainOverlayWidgetController->OnUpdateBossEnemyTargetForHealthBarDelegate.AddDynamic(this, &ThisClass::HandleBossOverlayBar);

	MainOverlayWidgetController->OnPassiveSkillPointsChangedDelegate.AddDynamic(this, &ThisClass::UpdatePassiveSkillPointsNotification);
	MainOverlayWidgetController->OnAscensionPointsChangedDelegate.AddDynamic(this, &ThisClass::UpdateAscensionSkillPointsNotification);

	OwningPlayerController = OwningPlayerController == nullptr ? GetOwningPlayer() : OwningPlayerController;
	check(OwningPlayerController);
	const AActor* OwningActor = Cast<AActor>(OwningPlayerController->GetPawn());
	PlayerInputManager = UObsidianPlayerInputManager::FindPlayerInputManager(OwningActor);

	HealthProgressGlobe->SetWidgetController(WidgetController);
	ManaProgressGlobe->SetWidgetController(WidgetController);
	ExperienceProgressBar->SetWidgetController(WidgetController);
}

void UObsidianMainOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	OwningPlayerController = GetOwningPlayer();

	if(Overlay_GameTabsMenu)
	{
		Overlay_GameTabsMenu->OnCharacterStatusButtonClickedDelegate.AddUObject(this, &ThisClass::ToggleCharacterStatus);
		Overlay_GameTabsMenu->OnInventoryButtonClickedDelegate.AddUObject(this, &ThisClass::ToggleInventory);
		Overlay_GameTabsMenu->OnPassiveSkillTreeButtonClickedDelegate.AddUObject(this, &ThisClass::TogglePassiveSkillTree);
	}
}

bool UObsidianMainOverlay::IsCharacterStatusOpen() const
{
	return CharacterStatus != nullptr;
}

bool UObsidianMainOverlay::IsInventoryOpen() const
{
	return Inventory != nullptr;
}

bool UObsidianMainOverlay::IsPassiveSkillTreeOpen() const
{
	return PassiveSkillTree != nullptr;
}

bool UObsidianMainOverlay::IsPlayerStashOpen() const
{
	return PlayerStash != nullptr;
}

FGameplayTag UObsidianMainOverlay::GetActivePlayerStashTabTag() const
{
	if(PlayerStash)
	{
		return PlayerStash->GetActiveStashTabTag();
	}
	return FGameplayTag::EmptyTag;
}

void UObsidianMainOverlay::ToggleCharacterStatus()
{
	if(IsPlayerStashOpen())
	{
		TogglePlayerStash(false);
	}
	
	if(IsCharacterStatusOpen() == false)
	{
		UObCharacterStatusWidgetController* CharacterStatusWidgetController = UObsidianUIFunctionLibrary::GetCharacterStatusWidgetController(this);
		check(CharacterStatusWidgetController);
		
		checkf(CharacterStatusClass, TEXT("Tried to create widget without valid widget class in UObsidianMainOverlay::ToggleCharacterStatus, fill it in ObsidianMainOverlay instance."));
		CharacterStatus = CreateWidget<UObsidianCharacterStatus>(this, CharacterStatusClass);
		CharacterStatus->SetWidgetController(CharacterStatusWidgetController);
		
		CharacterStatusWidgetController->SetInitialAttributeValues();
		
		LeftSideContainer_Overlay->AddChildToOverlay(CharacterStatus);
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
		CharacterStatus = nullptr;
		Overlay_GameTabsMenu->OnCharacterStatusTabStatusChangeDelegate.Broadcast(false);
	}
}

void UObsidianMainOverlay::ToggleInventory()
{
	if(IsInventoryOpen() == false)
	{
		if(InventoryItemsWidgetController == nullptr)
		{
			InventoryItemsWidgetController = UObsidianUIFunctionLibrary::GetInventoryItemsWidgetController(this);
		}
		check(InventoryItemsWidgetController);

		checkf(InventoryClass, TEXT("Tried to create widget without valid widget class in UObsidianMainOverlay::ToggleInventory, fill it in ObsidianMainOverlay instance."));
		Inventory = CreateWidget<UObsidianInventory>(this, InventoryClass);
		Inventory->SetWidgetController(InventoryItemsWidgetController);
		
		RightSideContainer_Overlay->AddChildToOverlay(Inventory);
		Inventory->OnWidgetDestroyedDelegate.AddLambda([this]()
			{
				Inventory = nullptr;

				if(Overlay_GameTabsMenu && Overlay_GameTabsMenu->Inventory_GameTabButton)
				{
					Overlay_GameTabsMenu->Inventory_GameTabButton->bIsCorrespondingTabOpen = false;
				}
			
				if(ensure(InventoryItemsWidgetController))
				{
					InventoryItemsWidgetController->RemoveItemUIElements(EObsidianPanelOwner::Inventory);
					InventoryItemsWidgetController->RemoveItemUIElements(EObsidianPanelOwner::Equipment);
				}
				
				MoveDroppedItemDescOverlay(false);
			});
		
		InventoryItemsWidgetController->OnInventoryOpen();
		MoveDroppedItemDescOverlay(true);
	}
	else
	{
		Inventory->RemoveFromParent();
		Inventory = nullptr;
		Overlay_GameTabsMenu->OnInventoryTabStatusChangeDelegate.Broadcast(false);
		
		MoveDroppedItemDescOverlay(false);
	}
}

void UObsidianMainOverlay::TogglePassiveSkillTree()
{
	if(IsPassiveSkillTreeOpen() == false)
	{
		checkf(PassiveSkillTreeClass, TEXT("Tried to create widget without valid widget class in UObsidianMainOverlay::TogglePassiveSkillTree, fill it in ObsidianMainOverlay instance."));
		PassiveSkillTree = CreateWidget<UObsidianPassiveSkillTree>(this, PassiveSkillTreeClass);

		PassiveSkillTree_Overlay->AddChildToOverlay(PassiveSkillTree);
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
		PassiveSkillTree = nullptr;
		Overlay_GameTabsMenu->OnPassiveSkillTreeTabStatusChangeDelegate.Broadcast(false);
	}
}

void UObsidianMainOverlay::TogglePlayerStash(const bool bShowStash)
{
	if(bShowStash && IsPlayerStashOpen() == false)
	{
		if(IsCharacterStatusOpen())
		{
			ToggleCharacterStatus();
		}
		
		if(IsInventoryOpen() == false)
		{
			ToggleInventory();
		}

		if(InventoryItemsWidgetController == nullptr)
		{
			InventoryItemsWidgetController = UObsidianUIFunctionLibrary::GetInventoryItemsWidgetController(this);
		}
		check(InventoryItemsWidgetController);
		
		checkf(PlayerStashClass, TEXT("Tried to create widget without valid widget class in UObsidianMainOverlay::TogglePlayerStash, fill it in ObsidianMainOverlay instance."));
		PlayerStash = CreateWidget<UObsidianPlayerStashWidget>(this, PlayerStashClass);
		PlayerStash->SetWidgetController(InventoryItemsWidgetController);
		
		LeftSideContainer_Overlay->AddChildToOverlay(PlayerStash);
		PlayerStash->OnWidgetDestroyedDelegate.AddLambda([this]()
			{
				PlayerStash = nullptr;
			
				if(ensure(InventoryItemsWidgetController))
				{
					InventoryItemsWidgetController->RemoveItemUIElements(EObsidianPanelOwner::PlayerStash);
				}
			});
		
		InventoryItemsWidgetController->OnPlayerStashOpen();
	}
	else if(bShowStash == false && IsPlayerStashOpen())
	{
		InventoryItemsWidgetController->RegisterCurrentStashTab(FGameplayTag::EmptyTag);
		PlayerStash->CloseStash();
		PlayerStash = nullptr;
		
		if(IsInventoryOpen())
		{
			ToggleInventory();
		}
	}
}

void UObsidianMainOverlay::AddItemDescriptionToOverlay(UObsidianItemDescriptionBase* ItemDescription) const
{
	if(ItemDescription)
	{
		DroppedItemDesc_Overlay->AddChildToOverlay(ItemDescription);
	}
}

void UObsidianMainOverlay::HandleStackingUIData(const FObsidianEffectUIDataWidgetRow Row, const FObsidianEffectUIStackingData StackingData)
{
	if(StackingInfoWidgetsMap.Contains(Row.EffectTag))
	{
		StackingInfoWidgetsMap[Row.EffectTag]->UpdateStackingInfoWidget(StackingData.EffectStackCount);
		return;
	}

	checkf(Row.StackingDurationalEffectWidget, TEXT("Tried to create widget without valid widget class in UObsidianMainOverlay::HandleStackingUIData, fill it in ObsidianMainOverlay instance."));
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
		checkf(Row.SimpleEffectWidget, TEXT("Tried to create widget without valid widget class in UObsidianMainOverlay::HandleUIData, fill it in ObsidianMainOverlay instance."));
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
		checkf(Row.DurationalEffectWidget, TEXT("Tried to create widget without valid widget class in UObsidianMainOverlay::HandleUIData, fill it in ObsidianMainOverlay instance."));
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
		checkf(RegularEnemyOverlayHealthBarClass, TEXT("Tried to create widget without valid widget class in UObsidianMainOverlay::HandleRegularOverlayBar, fill it in ObsidianMainOverlay instance."));
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
		checkf(BossEnemyOverlayHealthBarClass, TEXT("Tried to create widget without valid widget class in UObsidianMainOverlay::HandleBossOverlayBar, fill it in ObsidianMainOverlay instance."));
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

void UObsidianMainOverlay::UpdatePassiveSkillPointsNotification(float NewSkillPoints)
{
	if(PassiveSkillPointsNotification && NewSkillPoints <= 0)
	{
		PassiveSkillPointsNotification->OnSkillPointsNotificationPressedDelegate.RemoveAll(this);
		PassiveSkillPointsNotification->RemoveFromParent();
		PassiveSkillPointsNotification = nullptr;
		return;
	}
	
	if(PassiveSkillPointsNotification)
	{
		ensureMsgf(NewSkillPoints > 1, TEXT("PassiveSkillPointsNotification is added to viewport but NewSkillPoints is less than 2, why?"));
		
		PassiveSkillPointsNotification->SetSkillPointsCount(NewSkillPoints);
		return;
	}
	
	if(NewSkillPoints > 0)
	{
		checkf(PassiveSkillPointsNotificationClass, TEXT("PassiveSkillPointsNotificationClass is not set in UObsidianMainOverlay."));
		PassiveSkillPointsNotification = CreateWidget<UObsidianSkillPointsNotification>(this, PassiveSkillPointsNotificationClass);
		PassiveSkillPointsNotification->SetSkillPointsCount(NewSkillPoints);
		PassiveSkillPointsNotification->OnSkillPointsNotificationPressedDelegate.AddUObject(this, &ThisClass::TogglePassiveSkillTree);
		PassiveSkillPoints_WrapBox->AddChildToWrapBox(PassiveSkillPointsNotification);
	}
}

void UObsidianMainOverlay::UpdateAscensionSkillPointsNotification(float NewSkillPoints)
{
	if(AscensionSkillPointsNotification && NewSkillPoints <= 0)
	{
		AscensionSkillPointsNotification->RemoveFromParent();
		AscensionSkillPointsNotification = nullptr;
		return;
	}
	
	if(AscensionSkillPointsNotification)
	{
		ensureMsgf(NewSkillPoints > 1, TEXT("AscensionSkillPointsNotification is added to viewport but NewSkillPoints is less than 2, why?"));
		
		AscensionSkillPointsNotification->SetSkillPointsCount(NewSkillPoints);
		return;
	}
	
	if(NewSkillPoints > 0)
	{
		checkf(AscensionSkillPointsNotificationClass, TEXT("AscensionSkillPointsNotificationClass is not set in UObsidianMainOverlay."));
		AscensionSkillPointsNotification = CreateWidget<UObsidianSkillPointsNotification>(this, AscensionSkillPointsNotificationClass);
		AscensionSkillPointsNotification->SetSkillPointsCount(NewSkillPoints);
		PassiveSkillPoints_WrapBox->AddChildToWrapBox(AscensionSkillPointsNotification);
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

void UObsidianMainOverlay::MoveDroppedItemDescOverlay(const bool bInventoryOpen)
{
	if(bInventoryOpen)
	{
		if(Inventory == nullptr)
		{
			return;
		}

		const float InventoryWidth = Inventory->GetWindowWidth();
		if(UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(DroppedItemDesc_Overlay))
		{
			CanvasSlot->SetPosition(FVector2D(-InventoryWidth, -50.0f)); //TODO(intrxx) Hard coded for now, kinda feeling the need to change it
		}
	}
	else
	{
		if(UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(DroppedItemDesc_Overlay))
		{
			CanvasSlot->SetPosition(FVector2D(0.0f, -50.0f)); //TODO(intrxx) Hard coded for now, kinda feeling the need to change it
		}
	}
}



