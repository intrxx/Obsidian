// Copyright 2026 out of sCope - intrxx

#include "InventoryItems/ItemLabelSystem/ObsidianItemLabelManagerSubsystem.h"

#include <Components/CanvasPanelSlot.h>
#include <Blueprint/WidgetLayoutLibrary.h>

#include "Characters/Player/ObsidianPlayerController.h"
#include "InventoryItems/ItemDrop/ObsidianItemDataDeveloperSettings.h"
#include "InventoryItems/Items/ObsidianItemLabelComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/InventoryItems/ObsidianItemLabelOverlay.h"
#include "UI/InventoryItems/Items/ObsidianItemLabel.h"

DECLARE_CYCLE_STAT(TEXT("ItemLabelManager"), STAT_ItemLabelManager, STATGROUP_Tickables);
DEFINE_LOG_CATEGORY(LogItemLabelManager)

// ~ Start of FObsidianItemLabelData

bool FObsidianItemLabelData::IsValid() const
{
	return ItemLabelWidget && CanvasPanelSlot && SourceLabelComponent;
}

// ~ End of FObsidianItemLabelData

UObsidianItemLabelManagerSubsystem::UObsidianItemLabelManagerSubsystem()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> ItemLabelClassFinder(TEXT("/Game/Obsidian/UI/GameplayUserInterface/Inventory/Items/WBP_ItemWorldName.WBP_ItemWorldName_C"));
	if (ItemLabelClassFinder.Succeeded())
	{
		ItemLabelClass = ItemLabelClassFinder.Class;
	}
#if !UE_BUILD_SHIPPING
	else
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("ItemLabelClassFinder cannot find the default ItemLabelClass for WorldItemNameWidgetComp in AObsidianDroppableItem's constructor"
													   " previously located in /Game/Obsidian/UI/GameplayUserInterface/Inventory/Items/WBP_ItemWorldName.WBP_ItemWorldName_C")), ELogVerbosity::Error);
	}
#endif
}

void UObsidianItemLabelManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	OnViewportResizeDelegateHandle = FViewport::ViewportResizedEvent.AddUObject(this, &ThisClass::HandleViewportResize);
	
	if (const UObsidianItemDataDeveloperSettings* ItemDataSettings = GetDefault<UObsidianItemDataDeveloperSettings>())
	{
		ItemLabelGroundZOffset = ItemDataSettings->DefaultItemLabelGroundZOffset;
		LabelAdjustmentSmoothSpeed = ItemDataSettings->LabelAdjustmentSmoothSpeed;
	}
}

void UObsidianItemLabelManagerSubsystem::Deinitialize()
{
	if (OnViewportResizeDelegateHandle.IsValid())
	{
		FViewport::ViewportResizedEvent.Remove(OnViewportResizeDelegateHandle);
	}
	
	Super::Deinitialize();
}

void UObsidianItemLabelManagerSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bLabelOverlayVisible)
	{
		SolveLabelLayout();
		UpdateLabelAnchors(DeltaTime);
	}
}

TStatId UObsidianItemLabelManagerSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UItemLabelManagerSubsystem, STATGROUP_Tickables);
}

void UObsidianItemLabelManagerSubsystem::UpdateLabelAnchors(float DeltaTime)
{
	if (OwningPC.IsValid() == false || ItemLabelOverlay == nullptr)
	{
		return;
	}

	const AObsidianPlayerController* ObsidianPC = OwningPC.Get();

	if (bLayoutDirty == false)
	{
		if (const APawn* Pawn = ObsidianPC->GetPawn())
		{
			const float SpeedSquared = Pawn->GetVelocity().SizeSquared();
			if (SpeedSquared < KINDA_SMALL_NUMBER) // Don't do anything if the Player isn't actually moving
			{
				return;
			}
		}
	}
	
	for (TTuple<FGuid, FObsidianItemLabelData>& Pair : ItemLabelsDataMap)
	{
		FObsidianItemLabelData& LabelData = Pair.Value;
		if (LabelData.IsValid() && LabelData.bVisible)
		{
			FVector2D OutUpdatedAnchorScreenPosition;
			bool bSuccess = UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(ObsidianPC,
				LabelData.LabelAdjustedWorldPosition, OutUpdatedAnchorScreenPosition, false);
			if (bSuccess == false)
			{
				UE_LOG(LogItemLabelManager, Warning, TEXT("Label outside of viewport?"));
				continue;
			}
			
			const FVector2D NewLabelPosition = LabelData.LabelAnchorPosition + LabelData.LabelSolvedPositionOffset;
			if (IsOutsideCurrentViewport(NewLabelPosition))
			{
				UE_LOG(LogTemp, Warning, TEXT("[%s] is outside viewport."),
					*LabelData.SourceLabelComponent->GetLabelInitializationData().ItemName.ToString());
				//TODO(intrxx) Release widget?
				continue;
			}
			
			LabelData.CanvasPanelSlot->SetPosition(NewLabelPosition);
		}
	}

	bLayoutDirty = false;
}

void UObsidianItemLabelManagerSubsystem::SolveLabelLayout()
{
	if (bLayoutDirty == false)
	{
		return;
	}

	AObsidianPlayerController* OwningOPC = OwningPC.IsValid()
		? OwningPC.Get()
		: Cast<AObsidianPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (OwningOPC == nullptr)
	{
		return;
	}

	UE_LOG(LogItemLabelManager, Warning, TEXT("Solving Layout!"));

	TArray<FObsidianItemLabelData*> CandidateLabels;
	CandidateLabels.Reserve(ItemLabelsDataMap.Num());

	FVector CamLoc = OwningOPC->PlayerCameraManager
		? OwningOPC->PlayerCameraManager->GetCameraLocation()
		: FVector::Zero();

	for (TTuple<FGuid, FObsidianItemLabelData>& Pair : ItemLabelsDataMap)
	{
		FObsidianItemLabelData& LabelData = Pair.Value;
		CandidateLabels.Add(&LabelData);
	}
	
	CandidateLabels.Sort([](const FObsidianItemLabelData& DataA, const FObsidianItemLabelData& DataB)
		{
			if (DataA.Priority != DataB.Priority)
			{
				return DataA.Priority > DataB.Priority;
			}

			const float AY = DataA.LabelSolvedPosition.Y;
			const float BY = DataB.LabelSolvedPosition.Y;
			if (!FMath::IsNearlyEqual(AY, BY))
			{
				return AY < BY;
			}
		
			return DataA.Priority > DataB.Priority;
		});
	
	for (int32 i = 1; i < CandidateLabels.Num(); ++i)
	{
		FObsidianItemLabelData* Current = CandidateLabels[i];
		FObsidianItemLabelData* Previous = CandidateLabels[i - 1];
		
		if (/** CheckHorizontalOverlap(Current, Previous) && */ CheckVerticalOverlap(*Current, *Previous))
		{
			const float NewY = Previous->LabelSolvedPosition.Y + Previous->LabelSize.Y;

			Current->LabelSolvedPosition.Y = NewY;
			Current->LabelSolvedPositionOffset.Y = Current->LabelSolvedPosition.Y - Current->LabelAnchorPosition.Y;
		}
	}
}

bool UObsidianItemLabelManagerSubsystem::IsOutsideCurrentViewport(const FVector2D& ViewportPosition)
{
	if (const UWorld* World = GetWorld())
	{
		const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(World);
		const float DPIScale = UWidgetLayoutLibrary::GetViewportScale(World);
		
		constexpr float AdditionalBufferArea = 1.2;
		const FVector2D ViewportSizeScaled = (ViewportSize / DPIScale) * AdditionalBufferArea;
		
		const bool bIsOnScreen =
			ViewportPosition.X >= -250.0f &&
			ViewportPosition.X <= ViewportSizeScaled.X &&
			ViewportPosition.Y >= -250.0f &&
			ViewportPosition.Y <= ViewportSizeScaled.Y;
		return !bIsOnScreen;
	}
	return false;
}

bool UObsidianItemLabelManagerSubsystem::CheckVerticalOverlap(const FObsidianItemLabelData& LabelA,
                                                              const FObsidianItemLabelData& LabelB)
{
	const float LabelABottom = LabelA.LabelSolvedPosition.Y;
	const float LabelATop = LabelABottom - LabelA.LabelSize.Y;
	
	const float LabelBBottom = LabelB.LabelSolvedPosition.Y;
	const float LabelBTop = LabelBBottom - LabelB.LabelSize.Y;

	return !(LabelABottom <= LabelBTop || LabelBBottom <= LabelATop);
}

bool UObsidianItemLabelManagerSubsystem::CheckHorizontalOverlap(const FObsidianItemLabelData& LabelA,
	const FObsidianItemLabelData& LabelB)
{
	const float LabelAHalfSizeX = LabelA.LabelSize.X / 2;
	const float LabelALeft = LabelA.LabelSolvedPosition.X - LabelAHalfSizeX;
	const float LabelARight = LabelA.LabelSolvedPosition.X + LabelAHalfSizeX;

	const float LabelBHalfSizeX = LabelB.LabelSize.X / 2;
	const float LabelBLeft = LabelB.LabelSolvedPosition.X - LabelBHalfSizeX;
	const float LabelBRight = LabelB.LabelSolvedPosition.X + LabelBHalfSizeX;

	return !(LabelARight <= LabelBLeft || LabelBRight <= LabelALeft);
}

void UObsidianItemLabelManagerSubsystem::InitializeItemLabelManager(UObsidianItemLabelOverlay* InItemLabelOverlay,
                                                                    AObsidianPlayerController* InObsidianPC)
{
	ItemLabelOverlay = InItemLabelOverlay;
	OwningPC = InObsidianPC;
	
	//TODO(intrxx) Gather any items in the world and init it? Or init it in item's Begin Play?
}

FGuid UObsidianItemLabelManagerSubsystem::RegisterItemLabel(UObsidianItemLabelComponent* SourceLabelComponent)
{
	if (OwningPC.IsValid() == false || ItemLabelOverlay == nullptr || SourceLabelComponent == nullptr)
	{
		return FGuid();
	}
	
	FVector OwningItemWorldPosition = SourceLabelComponent->GetOwningItemActorLocation();
	OwningItemWorldPosition.Z += ItemLabelGroundZOffset;

	FVector2D OutInitialScreenPosition;
	const bool bVisibleOnScreen = UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(OwningPC.Get(),
				OwningItemWorldPosition, OutInitialScreenPosition, false);

	FObsidianItemLabelData NewLabelData;
	NewLabelData.LabelAdjustedWorldPosition = OwningItemWorldPosition;
	NewLabelData.LabelAnchorPosition = OutInitialScreenPosition;
	NewLabelData.LabelID = FGuid::NewGuid();
	NewLabelData.SourceLabelComponent = SourceLabelComponent;
	NewLabelData.Priority = /**TODO(Switch to it after implementing Prio) InitializationData.Priority; */ FMath::RandRange(0, 8);
	NewLabelData.LabelSolvedPosition = OutInitialScreenPosition;
	NewLabelData.bVisible = bVisibleOnScreen;
	
	if (bVisibleOnScreen)
	{
		const FObsidianLabelInitializationData InitializationData = SourceLabelComponent->GetLabelInitializationData();
		UObsidianItemLabel* ItemLabel = AcquireWidget(NewLabelData.LabelID);
		ItemLabel->SetItemName(InitializationData.ItemName);
		ItemLabel->SetVisibility(ESlateVisibility::Visible);
		NewLabelData.ItemLabelWidget = ItemLabel;

		if (UCanvasPanelSlot* CanvasPanelSlot = ItemLabelOverlay->AddItemLabelToOverlay(ItemLabel,
			OutInitialScreenPosition))
		{
			UE_LOG(LogItemLabelManager, Display, TEXT("New Label Pos: %s"), *OutInitialScreenPosition.ToString());
			
			NewLabelData.LabelSize = CanvasPanelSlot->GetSize();
			NewLabelData.CanvasPanelSlot = CanvasPanelSlot;

			//TODO(intrxx) Check if the Layout needs solving first? Item Can be outside any cluster
			bLayoutDirty = true;
		}
	}
	else
	{
		UE_LOG(LogItemLabelManager, Display, TEXT("Item Label [%s] wasn't projected onto screen,"
											" will be added as invisible data to use later."),
											*SourceLabelComponent->GetLabelInitializationData().ItemName.ToString());
	}
	
	ItemLabelsDataMap.Add(NewLabelData.LabelID, MoveTemp(NewLabelData));
	return NewLabelData.LabelID;
}

void UObsidianItemLabelManagerSubsystem::UnregisterItemLabel(const FGuid& LabelID)
{
	if (const FObsidianItemLabelData* FoundLabel = ItemLabelsDataMap.Find(LabelID))
	{
		ReleaseWidget(FoundLabel->ItemLabelWidget);
		ItemLabelsDataMap.Remove(LabelID);
	}
}

void UObsidianItemLabelManagerSubsystem::ToggleItemLabelHighlight(const bool bHighlight)
{
	if (ItemLabelOverlay == nullptr)
	{
		UE_LOG(LogItemLabelManager, Error, TEXT("ItemLabelOverlay is invalid in [%hs]."), __FUNCTION__);
		return;
	}
	
	if (bHighlight)
	{
		bLayoutDirty = true;
		ItemLabelOverlay->SetVisibility(ESlateVisibility::Visible);
		UE_LOG(LogItemLabelManager, Display, TEXT("Toggling Highlight on!"));
	}
	else
	{
		ItemLabelOverlay->SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(LogItemLabelManager, Display, TEXT("Toggling Highlight off!"));
	}

	bLabelOverlayVisible = bHighlight;
}

void UObsidianItemLabelManagerSubsystem::HandleViewportResize(FViewport* Viewport, uint32 /** unused */)
{
	bLayoutDirty = true;
}

UObsidianItemLabel* UObsidianItemLabelManagerSubsystem::AcquireWidget(const FGuid& ForID)
{
	for (UObsidianItemLabel* ExistingWidget : LabelWidgetPool)
	{
		if (ExistingWidget && ExistingWidget->IsInUse() == false)
		{
			//TODO(intrxx) Decide if should bind here if cleared in ReleaseWidget
			// ExistingWidget->OnItemLabelMouseHoverDelegate.AddUObject(this, &ThisClass::HandleLabelHovered);
			// ExistingWidget->OnItemLabelMouseButtonDownDelegate.AddUObject(this, &ThisClass::HandleLabelPressed);
			ExistingWidget->MarkInUse(true, ForID);
			return ExistingWidget;
		}
	}

	if (ItemLabelClass == nullptr)
	{
		UE_LOG(LogItemLabelManager, Error, TEXT("Could not create new Label Widget, Widget class is invalid"
										  " in [%hs]"), __FUNCTION__);
		return nullptr;
	}

	AObsidianPlayerController* OwningOPC = OwningPC.IsValid()
		? OwningPC.Get()
		: Cast<AObsidianPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (OwningOPC == nullptr)
	{
		UE_LOG(LogItemLabelManager, Error, TEXT("Could not create new Label Widget, Obsidian PC is invalid"
										  " in [%hs]"), __FUNCTION__);
		return nullptr;
	}
	
	if (UObsidianItemLabel* NewItemLabel = CreateWidget<UObsidianItemLabel>(OwningOPC, ItemLabelClass))
	{
		NewItemLabel->OnItemLabelMouseHoverDelegate.AddUObject(this, &ThisClass::HandleLabelHovered);
		NewItemLabel->OnItemLabelMouseButtonDownDelegate.AddUObject(this, &ThisClass::HandleLabelPressed);
		NewItemLabel->MarkInUse(true, ForID);
		LabelWidgetPool.Add(NewItemLabel);
		return NewItemLabel;
	}
	
	UE_LOG(LogItemLabelManager, Error, TEXT("Creating new Item Widget failed in [%hs]"), __FUNCTION__);
	return nullptr;
}

void UObsidianItemLabelManagerSubsystem::ReleaseWidget(UObsidianItemLabel* LabelWidget)
{
	if (LabelWidget == nullptr)
	{
		UE_LOG(LogItemLabelManager, Error, TEXT("Passed Item Label Widget to release is invalid in [%hs]"),
			__FUNCTION__);
		return;
	}

	//TODO(intrxx) Decide if clear it now and bind in AcquireWidget
	// LabelWidget->OnItemLabelMouseHoverDelegate.Clear();
	// LabelWidget->OnItemLabelMouseButtonDownDelegate.Clear();
	LabelWidget->MarkInUse(false);
	LabelWidget->SetVisibility(ESlateVisibility::Collapsed);

	//TODO(intrxx) Reset Label content here?
}

void UObsidianItemLabelManagerSubsystem::HandleLabelHovered(const bool bEnter, const FGuid& LabelID)
{
	if (const FObsidianItemLabelData* FoundLabel = ItemLabelsDataMap.Find(LabelID))
	{
		if (UObsidianItemLabelComponent* LabelComponent = FoundLabel->SourceLabelComponent)
		{
			LabelComponent->HandleLabelMouseHover(bEnter);
		}
	}
}

void UObsidianItemLabelManagerSubsystem::HandleLabelPressed(const int32 PlayerIndex,
	const FObsidianItemInteractionFlags& InteractionFlags, const FGuid& LabelID)
{
	if (const FObsidianItemLabelData* FoundLabel = ItemLabelsDataMap.Find(LabelID))
	{
		if (UObsidianItemLabelComponent* LabelComponent = FoundLabel->SourceLabelComponent)
		{
			LabelComponent->HandleLabelMouseButtonDown(PlayerIndex, InteractionFlags);
		}
	}
}
