// Copyright 2026 out of sCope - intrxx

#include "InventoryItems/ItemLabelSystem/ObsidianItemLabelManagerSubsystem.h"

#include <Components/CanvasPanelSlot.h>
#include <Blueprint/WidgetLayoutLibrary.h>
#include <SceneView.h>

#include "Characters/Player/ObsidianPlayerController.h"
#include "InventoryItems/ItemDrop/ObsidianItemDataDeveloperSettings.h"
#include "InventoryItems/Items/ObsidianItemLabelComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/InventoryItems/Items/ObsidianItemLabel.h"
#include "UI/MainOverlay/ObsidianMainOverlay.h"
#include "Obsidian/ObsidianLogCategories.h"

DECLARE_CYCLE_STAT(TEXT("ItemLabelManager"), STAT_ItemLabelManager, STATGROUP_Tickables);
namespace ObsidianItemLabelLayout
{
	/** Gap kept between two labels, in canvas units. */
	constexpr double LabelPadding = 2.0;

	/** Labels whose anchor is further than this outside the viewport are released back to the pool, in canvas units. */
	constexpr double OffscreenMargin = 150.0;

	/** Slack for the overlap test, so a label resting exactly against a blocker isn't pushed again. */
	constexpr double OverlapTolerance = 0.01;
	
	double FindFreeCenter(const TArray<FBox2D, TInlineAllocator<16>>& Blockers, const double StartCenter,
		const double HalfExtent, const double Direction, const int32 Axis)
	{
		double Center = StartCenter;

		for (int32 Pass = 0; Pass <= Blockers.Num(); ++Pass)
		{
			bool bPushed = false;
			for (const FBox2D& Blocker : Blockers)
			{
				const double BlockerMin = Blocker.Min[Axis] - LabelPadding;
				const double BlockerMax = Blocker.Max[Axis] + LabelPadding;
				if (Center + HalfExtent > BlockerMin + OverlapTolerance && Center - HalfExtent < BlockerMax - OverlapTolerance)
				{
					Center = Direction < 0.0 ? BlockerMin - HalfExtent : BlockerMax + HalfExtent;
					bPushed = true;
				}
			}

			if (bPushed == false)
			{
				break;
			}
		}
		return Center;
	}

	void GatherBlockers(const TArray<FBox2D>& Occupied, const double Center, const double HalfExtent, const int32 Axis,
		TArray<FBox2D, TInlineAllocator<16>>& OutBlockers)
	{
		OutBlockers.Reset();
		const double RangeMin = Center - HalfExtent - LabelPadding;
		const double RangeMax = Center + HalfExtent + LabelPadding;
		for (const FBox2D& TakenArea : Occupied)
		{
			if (TakenArea.Max[Axis] > RangeMin && TakenArea.Min[Axis] < RangeMax)
			{
				OutBlockers.Add(TakenArea);
			}
		}
	}
	
	double PickPushOffset(const double NegativeDistance, const double PositiveDistance, const bool bNegativeFits,
		const bool bPositiveFits, const double PreviousOffset, const double SideSwitchThreshold)
	{
		if (bNegativeFits != bPositiveFits)
		{
			return bNegativeFits ? -NegativeDistance : PositiveDistance;
		}

		const double NegativeCost = NegativeDistance - (PreviousOffset < 0.0 ? SideSwitchThreshold : 0.0);
		const double PositiveCost = PositiveDistance - (PreviousOffset > 0.0 ? SideSwitchThreshold : 0.0);
		return NegativeCost <= PositiveCost ? -NegativeDistance : PositiveDistance;
	}
}

// ~ Start of FObsidianItemLabelData

bool FObsidianItemLabelData::IsValid() const
{
	return ItemLabelWidget && CanvasPanelSlot && SourceLabelComponent;
}

void FObsidianItemLabelData::ResetLabelData()
{
	CanvasPanelSlot = nullptr;
	ItemLabelWidget = nullptr;
	LabelSize = FVector2D::Zero();
	LabelSolvedPositionOffset = FVector2D::Zero();
	LabelDisplayedPositionOffset = FVector2D::Zero();

	bVisible = false;
	bSnapToSolvedPosition = true;
}

// ~ Start of FObsidianLabelManagerLateTickFunction

void FObsidianLabelManagerLateTickFunction::ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread,
	const FGraphEventRef& MyCompletionGraphEvent)
{
	if (Target && IsValid(Target))
	{
		FScopeCycleCounterUObject TargetScope = FScopeCycleCounterUObject(Target);
		Target->PostWorkTick(DeltaTime);
	}
}

FString FObsidianLabelManagerLateTickFunction::DiagnosticMessage()
{
	if (Target)
	{
		return Target->GetFullName() + TEXT("Late Tick");
	}
	return FTickFunction::DiagnosticMessage();
}

// ~ End of FObsidianLabelManagerLateTickFunction

UObsidianItemLabelManagerSubsystem::UObsidianItemLabelManagerSubsystem()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> ItemLabelClassFinder(TEXT("/Game/Obsidian/UI/GameplayUserInterface/Inventory/Items/WBP_ItemLabel.WBP_ItemLabel_C"));
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

bool UObsidianItemLabelManagerSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UObsidianItemLabelManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (const UObsidianItemDataDeveloperSettings* ItemDataSettings = GetDefault<UObsidianItemDataDeveloperSettings>())
	{
		ItemLabelGroundZOffset = ItemDataSettings->DefaultItemLabelGroundZOffset;
		LabelAdjustmentSmoothSpeed = ItemDataSettings->LabelAdjustmentSmoothSpeed;
	}

	if (const UWorld* World = GetWorld())
	{
		LateTickFunction.Target = this;
		LateTickFunction.TickGroup = TG_PostUpdateWork;
		LateTickFunction.bCanEverTick = true;
		LateTickFunction.bStartWithTickEnabled = true;
		LateTickFunction.SetTickFunctionEnable(true);
		LateTickFunction.RegisterTickFunction(World->PersistentLevel);
	}
}

void UObsidianItemLabelManagerSubsystem::Deinitialize()
{
	LateTickFunction.UnRegisterTickFunction();

	Super::Deinitialize();
}

void UObsidianItemLabelManagerSubsystem::PostWorkTick(float DeltaTime)
{
	if (bLabelOverlayVisible)
	{
		UpdateLabels(DeltaTime);
	}
}

void UObsidianItemLabelManagerSubsystem::UpdateLabels(float DeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_ItemLabelManager);

	if (OwningPC.IsValid() == false || MainOverlay == nullptr || ItemLabelsDataMap.IsEmpty())
	{
		return;
	}

	TArray<FObsidianItemLabelData*> LabelsToSolve;
	LabelsToSolve.Reserve(ItemLabelsDataMap.Num());

	FBox2D ViewportArea(ForceInit);
	UpdateLabelAnchors(LabelsToSolve, ViewportArea);
	SolveLabelLayout(LabelsToSolve, ViewportArea);

	for (FObsidianItemLabelData* Label : LabelsToSolve)
	{
		if (Label->bSnapToSolvedPosition)
		{
			Label->LabelDisplayedPositionOffset = Label->LabelSolvedPositionOffset;
			Label->bSnapToSolvedPosition = Label->LabelSize.IsNearlyZero();
		}
		else
		{
			Label->LabelDisplayedPositionOffset = FMath::Vector2DInterpTo(Label->LabelDisplayedPositionOffset,
				Label->LabelSolvedPositionOffset, DeltaTime, LabelAdjustmentSmoothSpeed);
		}

		Label->LabelSolvedPosition = Label->LabelAnchorPosition + Label->LabelDisplayedPositionOffset;
		Label->CanvasPanelSlot->SetPosition(Label->LabelSolvedPosition);
	}
}

void UObsidianItemLabelManagerSubsystem::UpdateLabelAnchors(TArray<FObsidianItemLabelData*>& OutLabelsToSolve,
	FBox2D& OutViewportArea)
{
	const ULocalPlayer* LocalPlayer = OwningPC->GetLocalPlayer();
	if (LocalPlayer == nullptr || LocalPlayer->ViewportClient == nullptr)
	{
		return;
	}

	FSceneViewProjectionData ProjectionData;
	if (LocalPlayer->GetProjectionData(LocalPlayer->ViewportClient->Viewport, ProjectionData) == false)
	{
		return;
	}

	const double DPIScale = UWidgetLayoutLibrary::GetViewportScale(GetWorld());
	if (DPIScale <= 0.0)
	{
		return;
	}

	const FMatrix ViewProjectionMatrix = ProjectionData.ComputeViewProjectionMatrix();
	const FIntRect ViewRect = ProjectionData.GetConstrainedViewRect();

	using namespace ObsidianItemLabelLayout;
	OutViewportArea = FBox2D(FVector2D(ViewRect.Min) / DPIScale, FVector2D(ViewRect.Max) / DPIScale);
	const FBox2D VisibleArea = OutViewportArea.ExpandBy(OffscreenMargin);

	for (TTuple<FGuid, FObsidianItemLabelData>& Pair : ItemLabelsDataMap)
	{
		FObsidianItemLabelData& LabelData = Pair.Value;

		FVector2D AnchorPixelPosition;
		const bool bOnScreen = IsValid(LabelData.SourceLabelComponent)
			&& FSceneView::ProjectWorldToScreen(LabelData.LabelAdjustedWorldPosition, ViewRect, ViewProjectionMatrix, AnchorPixelPosition)
			&& VisibleArea.IsInside(AnchorPixelPosition / DPIScale);
		if (bOnScreen == false)
		{
			DeactivateLabel(LabelData);
			continue;
		}

		LabelData.LabelAnchorPosition = AnchorPixelPosition / DPIScale;

		if (LabelData.bVisible == false && ActivateLabel(LabelData) == false)
		{
			continue;
		}

		LabelData.LabelSize = LabelData.ItemLabelWidget->GetDesiredSize();
		OutLabelsToSolve.Add(&LabelData);
	}
}

void UObsidianItemLabelManagerSubsystem::SolveLabelLayout(TArray<FObsidianItemLabelData*>& LabelsToSolve,
	const FBox2D& ViewportArea)
{
	using namespace ObsidianItemLabelLayout;

	LabelsToSolve.Sort([](const FObsidianItemLabelData& DataA, const FObsidianItemLabelData& DataB)
		{
			if (DataA.Priority != DataB.Priority)
			{
				return DataA.Priority > DataB.Priority;
			}
			return DataA.RegistrationIndex < DataB.RegistrationIndex;
		});

	TArray<FBox2D> Occupied;
	Occupied.Reserve(LabelsToSolve.Num());

	TArray<FBox2D, TInlineAllocator<16>> Blockers;

	for (FObsidianItemLabelData* Label : LabelsToSolve)
	{
		const FVector2D Anchor = Label->LabelAnchorPosition;
		const FVector2D HalfSize = Label->LabelSize * 0.5;
		if (HalfSize.IsNearlyZero())
		{
			Label->LabelSolvedPositionOffset = FVector2D::Zero();
			continue;
		}

		const double SideSwitchThreshold = Label->LabelSize.Y;
		const FVector2D PreviousOffset = Label->LabelSolvedPositionOffset;

		GatherBlockers(Occupied, Anchor.X, HalfSize.X, 0, Blockers);
		const double UpDistance = Anchor.Y - FindFreeCenter(Blockers, Anchor.Y, HalfSize.Y, -1.0, 1);
		const double DownDistance = FindFreeCenter(Blockers, Anchor.Y, HalfSize.Y, 1.0, 1) - Anchor.Y;

		const double VerticalFitMargin = PreviousOffset.X != 0.0 ? SideSwitchThreshold : 0.0;
		const bool bUpFits = UpDistance <= 0.0
			|| Anchor.Y - UpDistance - HalfSize.Y >= ViewportArea.Min.Y + VerticalFitMargin;
		const bool bDownFits = DownDistance <= 0.0
			|| Anchor.Y + DownDistance + HalfSize.Y <= ViewportArea.Max.Y - VerticalFitMargin;

		if (bUpFits || bDownFits)
		{
			Label->LabelSolvedPositionOffset = FVector2D(0.0, PickPushOffset(UpDistance, DownDistance, bUpFits,
				bDownFits, PreviousOffset.Y, SideSwitchThreshold));
		}
		else
		{
			// The vertical stack would leave the viewport on both ends, place the label next to it instead.
			GatherBlockers(Occupied, Anchor.Y, HalfSize.Y, 1, Blockers);
			const double LeftDistance = Anchor.X - FindFreeCenter(Blockers, Anchor.X, HalfSize.X, -1.0, 0);
			const double RightDistance = FindFreeCenter(Blockers, Anchor.X, HalfSize.X, 1.0, 0) - Anchor.X;

			const bool bLeftFits = Anchor.X - LeftDistance - HalfSize.X >= ViewportArea.Min.X;
			const bool bRightFits = Anchor.X + RightDistance + HalfSize.X <= ViewportArea.Max.X;

			Label->LabelSolvedPositionOffset = FVector2D(PickPushOffset(LeftDistance, RightDistance, bLeftFits,
				bRightFits, PreviousOffset.X, SideSwitchThreshold), 0.0);
		}

		const FVector2D SolvedCenter = Anchor + Label->LabelSolvedPositionOffset;
		Occupied.Add(FBox2D(SolvedCenter - HalfSize, SolvedCenter + HalfSize));
	}
}

bool UObsidianItemLabelManagerSubsystem::ActivateLabel(FObsidianItemLabelData& LabelData)
{
	UObsidianItemLabel* LabelWidget = AcquireWidget(LabelData.LabelID);
	if (LabelWidget == nullptr)
	{
		return false;
	}

	UCanvasPanelSlot* CanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(LabelWidget);
	if (CanvasPanelSlot == nullptr)
	{
		CanvasPanelSlot = MainOverlay->AddItemLabelToOverlay(LabelWidget, LabelData.LabelAnchorPosition);
	}
	if (CanvasPanelSlot == nullptr)
	{
		ReleaseWidget(LabelWidget);
		return false;
	}

	LabelWidget->SetItemName(LabelData.SourceLabelComponent->GetLabelInitializationData().ItemName);
	LabelWidget->SetVisibility(ESlateVisibility::Visible);

	// The solver needs the size right now, without the prepass it would only be available after the next Slate tick.
	LabelWidget->ForceLayoutPrepass();

	LabelData.ItemLabelWidget = LabelWidget;
	LabelData.CanvasPanelSlot = CanvasPanelSlot;
	LabelData.LabelSolvedPositionOffset = FVector2D::Zero();
	LabelData.LabelDisplayedPositionOffset = FVector2D::Zero();
	LabelData.bVisible = true;
	LabelData.bSnapToSolvedPosition = true;
	return true;
}

void UObsidianItemLabelManagerSubsystem::DeactivateLabel(FObsidianItemLabelData& LabelData)
{
	if (LabelData.ItemLabelWidget)
	{
		ReleaseWidget(LabelData.ItemLabelWidget);
	}
	LabelData.ResetLabelData();
}

void UObsidianItemLabelManagerSubsystem::InitializeItemLabelManager(UObsidianMainOverlay* InItemLabelOverlay,
                                                                    AObsidianPlayerController* InObsidianPC)
{
	if (MainOverlay != InItemLabelOverlay)
	{
		for (TTuple<FGuid, FObsidianItemLabelData>& Pair : ItemLabelsDataMap)
		{
			DeactivateLabel(Pair.Value);
		}
		LabelWidgetPool.Reset();
	}

	MainOverlay = InItemLabelOverlay;
	OwningPC = InObsidianPC;
}

FGuid UObsidianItemLabelManagerSubsystem::RegisterItemLabel(UObsidianItemLabelComponent* SourceLabelComponent)
{
	if (SourceLabelComponent == nullptr)
	{
		return FGuid();
	}

	FVector OwningItemWorldPosition = SourceLabelComponent->GetOwningItemActorLocation();
	OwningItemWorldPosition.Z += ItemLabelGroundZOffset;

	FObsidianItemLabelData NewLabelData;
	NewLabelData.LabelAdjustedWorldPosition = OwningItemWorldPosition;
	NewLabelData.LabelID = FGuid::NewGuid();
	NewLabelData.SourceLabelComponent = SourceLabelComponent;
	NewLabelData.Priority = /**TODO(Switch to it after implementing Prio) InitializationData.Priority; */ FMath::RandRange(0, 8);
	NewLabelData.RegistrationIndex = NextRegistrationIndex++;

	const FGuid NewLabelID = NewLabelData.LabelID;
	ItemLabelsDataMap.Add(NewLabelID, MoveTemp(NewLabelData));
	return NewLabelID;
}

void UObsidianItemLabelManagerSubsystem::UnregisterItemLabel(const FGuid& LabelID)
{
	if (FObsidianItemLabelData* FoundLabel = ItemLabelsDataMap.Find(LabelID))
	{
		DeactivateLabel(*FoundLabel);
		ItemLabelsDataMap.Remove(LabelID);
	}
}

void UObsidianItemLabelManagerSubsystem::ToggleItemLabelHighlight(const bool bHighlight)
{
	if (MainOverlay == nullptr)
	{
		UE_LOG(ObLogItemLabels, Error, TEXT("ItemLabelOverlay is invalid in [%hs]."), __FUNCTION__);
		return;
	}

	if (bHighlight)
	{
		for (TTuple<FGuid, FObsidianItemLabelData>& Pair : ItemLabelsDataMap)
		{
			Pair.Value.bSnapToSolvedPosition = true;
		}

		MainOverlay->SetItemLabelsVisibility(ESlateVisibility::Visible);
		UE_LOG(ObLogItemLabels, Verbose, TEXT("Toggling Highlight on!"));
	}
	else
	{
		MainOverlay->SetItemLabelsVisibility(ESlateVisibility::Collapsed);
		UE_LOG(ObLogItemLabels, Verbose, TEXT("Toggling Highlight off!"));
	}

	bLabelOverlayVisible = bHighlight;
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
		UE_LOG(ObLogItemLabels, Error, TEXT("Could not create new Label Widget, Widget class is invalid"
										  " in [%hs]"), __FUNCTION__);
		return nullptr;
	}

	AObsidianPlayerController* OwningOPC = OwningPC.IsValid()
		? OwningPC.Get()
		: Cast<AObsidianPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (OwningOPC == nullptr)
	{
		UE_LOG(ObLogItemLabels, Error, TEXT("Could not create new Label Widget, Obsidian PC is invalid"
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

	UE_LOG(ObLogItemLabels, Error, TEXT("Creating new Item Widget failed in [%hs]"), __FUNCTION__);
	return nullptr;
}

void UObsidianItemLabelManagerSubsystem::ReleaseWidget(UObsidianItemLabel* LabelWidget)
{
	if (LabelWidget == nullptr)
	{
		UE_LOG(ObLogItemLabels, Error, TEXT("Passed Item Label Widget to release is invalid in [%hs]"),
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
