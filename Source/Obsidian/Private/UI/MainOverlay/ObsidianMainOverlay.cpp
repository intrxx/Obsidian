// Copyright 2026 out of sCope team - intrxx

#include "UI/MainOverlay/ObsidianMainOverlay.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "Components/VerticalBox.h"
#include "Components/WrapBox.h"

#include "CharacterComponents/ObsidianEnemyOverlayBarComponent.h"
#include "CharacterComponents/ObsidianPlayerInputManager.h"
#include "Core/FunctionLibraries/ObsidianUIFunctionLibrary.h"
#include "Obsidian/ObsidianLogCategories.h"
#include "ObsidianTypes/ObsidianUITypes.h"
#include "UI/CharacterStatus/ObsidianCharacterStatus.h"
#include "UI/GameTabsMenu/ObsidianOverlayGameTabsMenu.h"
#include "UI/GameTabsMenu/Subwidgets/ObsidianGameTabButton.h"
#include "UI/InventoryItems/Items/ObsidianItemDescriptionBase.h"
#include "UI/InventoryItems/Items/ObsidianItemLabel.h"
#include "UI/InventoryItems/ObsidianInventory.h"
#include "UI/InventoryItems/ObsidianPlayerStashWidget.h"
#include "UI/MainOverlay/SkillPoints/ObsidianSkillPointsNotification.h"
#include "UI/MainOverlay/Subwidgets/ObsidianDurationalEffectInfo.h"
#include "UI/MainOverlay/Subwidgets/OStackingDurationalEffectInfo.h"
#include "UI/PassiveSkillTree/ObsidianPassiveSkillTree.h"
#include "UI/ProgressBars/ObsidianOverlayBossEnemyBar.h"
#include "UI/ProgressBars/ObsidianOverlayExperienceBar.h"
#include "UI/ProgressBars/ObsidianOverlayStaminaBar.h"
#include "UI/ProgressBars/ProgressGlobe/ObsidianProgressGlobe_Health.h"
#include "UI/ProgressBars/ProgressGlobe/ObsidianProgressGlobe_Mana.h"
#include "UI/ProgressBars/UObsidianOverlayEnemyBar.h"
#include "UI/WidgetControllers/ObCharacterStatusWidgetController.h"
#include "UI/WidgetControllers/ObInventoryItemsWidgetController.h"
#include "UI/WidgetControllers/ObMainOverlayWidgetController.h"


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
	StaminaProgressBar->SetWidgetController(WidgetController);
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

void UObsidianMainOverlay::TogglePlayerStash(const bool bInShowStash)
{
	if(bInShowStash && IsPlayerStashOpen() == false)
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
	else if(bInShowStash == false && IsPlayerStashOpen())
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

void UObsidianMainOverlay::AddItemDescriptionToOverlay(UObsidianItemDescriptionBase* InItemDescription) const
{
	if(InItemDescription)
	{
		DroppedItemDesc_Overlay->AddChildToOverlay(InItemDescription);
	}
}

UCanvasPanelSlot* UObsidianMainOverlay::AddItemLabelToOverlay(UObsidianItemLabel* InItemLabelWidget,
	const FVector2D& InAtPosition)
{
	if (InItemLabelWidget == nullptr)
	{
		UE_LOG(ObLogUIMainOverlay, Error, TEXT("Passed ItemLabelWidget is invalid in [%hs]."), __FUNCTION__);
		return nullptr;
	}
	
	if (ItemLabels_CanvasPanel)
	{
		UCanvasPanelSlot* CanvasSlot = ItemLabels_CanvasPanel->AddChildToCanvas(InItemLabelWidget);
		CanvasSlot->SetAutoSize(true);
		CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		CanvasSlot->SetPosition(InAtPosition);

		return CanvasSlot;
	}
	return nullptr;
}

UCanvasPanelSlot* UObsidianMainOverlay::AddItemLabelToOverlayDebug(UUserWidget* InItemLabelWidget,
	const FVector2D& InAtPosition)
{
	if (ItemLabels_CanvasPanel)
	{
		UCanvasPanelSlot* CanvasSlot = ItemLabels_CanvasPanel->AddChildToCanvas(InItemLabelWidget);
		CanvasSlot->SetAutoSize(true);
		CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		CanvasSlot->SetPosition(InAtPosition);

		return CanvasSlot;
	}
	return nullptr;
}

void UObsidianMainOverlay::SetItemLabelsVisibility(ESlateVisibility InVisibility)
{
	if (ItemLabels_CanvasPanel)
	{
		ItemLabels_CanvasPanel->SetVisibility(InVisibility);
	}
}

void UObsidianMainOverlay::ForceItemLabelsPrepass()
{
	if (ItemLabels_CanvasPanel)
	{
		ItemLabels_CanvasPanel->ForceLayoutPrepass();
	}
}

void UObsidianMainOverlay::HandleStackingUIData(const FObsidianEffectUIDataWidgetRow InRow, const FObsidianEffectUIStackingData InStackingData)
{
	if(StackingInfoWidgetsMap.Contains(InRow.EffectTag))
	{
		StackingInfoWidgetsMap[InRow.EffectTag]->UpdateStackingInfoWidget(InStackingData.EffectStackCount);
		return;
	}

	checkf(InRow.StackingDurationalEffectWidget, TEXT("Tried to create widget without valid widget class in UObsidianMainOverlay::HandleStackingUIData, fill it in ObsidianMainOverlay instance."));
	UOStackingDurationalEffectInfo* StackingInfoWidget = CreateWidget<UOStackingDurationalEffectInfo>(OwningPlayerController, InRow.StackingDurationalEffectWidget);
	StackingInfoWidgetsMap.Add(InRow.EffectTag, StackingInfoWidget);
	
	StackingInfoWidget->InitDurationalStackingEffectInfo(InRow.EffectName, InRow.EffectDesc, InRow.EffectImage, InRow.EffectDuration, InStackingData);

	FObsidianProgressBarEffectFillImage FillImage;
	if(HealthProgressGlobe->GetEffectFillImageForTag(/* OUT */ FillImage, InRow.EffectTag))
	{
		HealthProgressGlobe->SetProgressGlobeStyle(FillImage.ProgressBarFillImage);
		EffectFillImages.Add(FillImage);
	}
	
	StackingInfoWidget->OnStackingInfoWidgetTerminatedDelegate.AddLambda([InRow, this](UOStackingDurationalEffectInfo* InWidgetToDestroy)
		{
			DestroyStackingInfoWidget(InWidgetToDestroy);
			HandleEffectFillImageRemoval(InRow.EffectTag);
		});

	switch(InRow.EffectClassification)
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

void UObsidianMainOverlay::HandleUIData(const FObsidianEffectUIDataWidgetRow InRow)
{
	if(InRow.InfoWidgetType == EObsidianInfoWidgetType::EIWT_SimpleEffectInfo)
	{
		checkf(InRow.SimpleEffectWidget, TEXT("Tried to create widget without valid widget class in UObsidianMainOverlay::HandleUIData, fill it in ObsidianMainOverlay instance."));
		UObsidianEffectInfoBase* InfoWidget = CreateWidget<UObsidianEffectInfoBase>(OwningPlayerController, InRow.SimpleEffectWidget);
		InfoWidget->InitEffectInfo(InRow.EffectName, InRow.EffectDesc, InRow.EffectImage, InRow.EffectTag);
		
		switch(InRow.EffectClassification)
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

		if(InRow.bAuraEffect)
		{
			AuraUIInfoArray.AddUnique(InfoWidget);
		}

		return;
	}
	
	if(InRow.InfoWidgetType == EObsidianInfoWidgetType::EIWT_DurationalEffectInfo)
	{
		checkf(InRow.DurationalEffectWidget, TEXT("Tried to create widget without valid widget class in UObsidianMainOverlay::HandleUIData, fill it in ObsidianMainOverlay instance."));
		UObsidianDurationalEffectInfo* DurationalInfoWidget = CreateWidget<UObsidianDurationalEffectInfo>(OwningPlayerController, InRow.DurationalEffectWidget);
		DurationalInfoWidget->InitDurationalEffectInfo(InRow.EffectName, InRow.EffectDesc, InRow.EffectImage, InRow.EffectDuration);

		FObsidianProgressBarEffectFillImage FillImage;
		if(HealthProgressGlobe->GetEffectFillImageForTag(/* OUT */FillImage, InRow.EffectTag))
		{
			HealthProgressGlobe->SetProgressGlobeStyle(FillImage.ProgressBarFillImage);
			EffectFillImages.Add(FillImage);
			
			DurationalInfoWidget->OnDurationalInfoWidgetTerminatedDelegate.AddLambda([InRow, this](UObsidianDurationalEffectInfo* InWidgetToDestroy)
				{
					HandleEffectFillImageRemoval(InRow.EffectTag);
				});
		}

		switch(InRow.EffectClassification)
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

void UObsidianMainOverlay::HandleRegularOverlayBar(AActor* InTargetActor, bool bInDisplayBar)
{
	if(bInDisplayBar)
	{
		checkf(RegularEnemyOverlayHealthBarClass, TEXT("Tried to create widget without valid widget class in UObsidianMainOverlay::HandleRegularOverlayBar, fill it in ObsidianMainOverlay instance."));
		RegularEnemyOverlayHealthBar = CreateWidget<UObsidianOverlayEnemyBar>(OwningPlayerController, RegularEnemyOverlayHealthBarClass);
		
		UObsidianEnemyOverlayBarComponent* EnemyOverlayBarComponent = UObsidianEnemyOverlayBarComponent::FindEnemyOverlayComponent(InTargetActor);
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

void UObsidianMainOverlay::HandleBossOverlayBar(AActor* InTargetActor, bool bInDisplayBar)
{
	if(bInDisplayBar)
	{
		checkf(BossEnemyOverlayHealthBarClass, TEXT("Tried to create widget without valid widget class in UObsidianMainOverlay::HandleBossOverlayBar, fill it in ObsidianMainOverlay instance."));
		BossEnemyOverlayHealthBar = CreateWidget<UObsidianOverlayBossEnemyBar>(OwningPlayerController, BossEnemyOverlayHealthBarClass);
		
		UObsidianEnemyOverlayBarComponent* EnemyOverlayBarComponent = UObsidianEnemyOverlayBarComponent::FindEnemyOverlayComponent(InTargetActor);
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

void UObsidianMainOverlay::UpdatePassiveSkillPointsNotification(float InNewSkillPoints)
{
	if(PassiveSkillPointsNotification && InNewSkillPoints <= 0)
	{
		PassiveSkillPointsNotification->OnSkillPointsNotificationPressedDelegate.RemoveAll(this);
		PassiveSkillPointsNotification->RemoveFromParent();
		PassiveSkillPointsNotification = nullptr;
		return;
	}
	
	if(PassiveSkillPointsNotification)
	{
		ensureMsgf(InNewSkillPoints > 1, TEXT("PassiveSkillPointsNotification is added to viewport but NewSkillPoints is less than 2, why?"));
		
		PassiveSkillPointsNotification->SetSkillPointsCount(InNewSkillPoints);
		return;
	}
	
	if(InNewSkillPoints > 0)
	{
		checkf(PassiveSkillPointsNotificationClass, TEXT("PassiveSkillPointsNotificationClass is not set in UObsidianMainOverlay."));
		PassiveSkillPointsNotification = CreateWidget<UObsidianSkillPointsNotification>(this, PassiveSkillPointsNotificationClass);
		PassiveSkillPointsNotification->SetSkillPointsCount(InNewSkillPoints);
		PassiveSkillPointsNotification->OnSkillPointsNotificationPressedDelegate.AddUObject(this, &ThisClass::TogglePassiveSkillTree);
		PassiveSkillPoints_WrapBox->AddChildToWrapBox(PassiveSkillPointsNotification);
	}
}

void UObsidianMainOverlay::UpdateAscensionSkillPointsNotification(float InNewSkillPoints)
{
	if(AscensionSkillPointsNotification && InNewSkillPoints <= 0)
	{
		AscensionSkillPointsNotification->RemoveFromParent();
		AscensionSkillPointsNotification = nullptr;
		return;
	}
	
	if(AscensionSkillPointsNotification)
	{
		ensureMsgf(InNewSkillPoints > 1, TEXT("AscensionSkillPointsNotification is added to viewport but NewSkillPoints is less than 2, why?"));
		
		AscensionSkillPointsNotification->SetSkillPointsCount(InNewSkillPoints);
		return;
	}
	
	if(InNewSkillPoints > 0)
	{
		checkf(AscensionSkillPointsNotificationClass, TEXT("AscensionSkillPointsNotificationClass is not set in UObsidianMainOverlay."));
		AscensionSkillPointsNotification = CreateWidget<UObsidianSkillPointsNotification>(this, AscensionSkillPointsNotificationClass);
		AscensionSkillPointsNotification->SetSkillPointsCount(InNewSkillPoints);
		PassiveSkillPoints_WrapBox->AddChildToWrapBox(AscensionSkillPointsNotification);
	}
}

void UObsidianMainOverlay::DestroyStackingInfoWidget(UOStackingDurationalEffectInfo* InWidgetToDestroy)
{
	if(const FGameplayTag* Key = StackingInfoWidgetsMap.FindKey(InWidgetToDestroy))
	{
		StackingInfoWidgetsMap.Remove(*Key);
	}
}

void UObsidianMainOverlay::HandleEffectFillImageRemoval(const FGameplayTag& InEffectTag)
{
	if(!EffectFillImages.IsEmpty())
	{
		for(int i = 0; i < EffectFillImages.Num(); i++)
		{
			if(EffectFillImages[i].EffectTag == InEffectTag)
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

void UObsidianMainOverlay::DestroyAuraInfoWidget(const FGameplayTag InWidgetToDestroyWithTag)
{
	if(AuraUIInfoArray.IsEmpty())
	{
		return;
	}

	for(UObsidianEffectInfoBase* Widget : AuraUIInfoArray)
	{
		if(Widget->UIEffectTag == InWidgetToDestroyWithTag)
		{
			Widget->RemoveAuraInfoWidget();
		}
	}
}

void UObsidianMainOverlay::MoveDroppedItemDescOverlay(const bool bInInventoryOpen)
{
	if(bInInventoryOpen)
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



