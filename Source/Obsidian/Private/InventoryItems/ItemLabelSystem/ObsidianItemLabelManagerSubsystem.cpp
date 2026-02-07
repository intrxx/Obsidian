// Copyright 2026 out of sCope - intrxx

#include "InventoryItems/ItemLabelSystem/ObsidianItemLabelManagerSubsystem.h"

#include <Components/CanvasPanelSlot.h>
#include <Blueprint/WidgetLayoutLibrary.h>

#include "InventoryItems/Items/ObsidianDroppableItem.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "InventoryItems/ItemDrop/ObsidianItemDataDeveloperSettings.h"
#include "UI/InventoryItems/ObsidianItemLabelOverlay.h"
#include "UI/InventoryItems/Items/ObsidianItemLabel.h"

DECLARE_CYCLE_STAT(TEXT("ItemLabelManager"), STAT_ItemLabelManager, STATGROUP_Tickables);
DEFINE_LOG_CATEGORY(LogItemLabelManager)

// ~ Start of FObsidianItemLabelInfo

bool FObsidianItemLabelInfo::IsValid() const
{
	return ItemLabelWidget && OwningItemActor;
}

// ~ Start of FObsidianItemLabelData

FObsidianItemLabelData::FObsidianItemLabelData(const FObsidianItemLabelInfo& ItemLabelInfo)
	: ItemLabelWidget(ItemLabelInfo.ItemLabelWidget)
	, OwningItemActor(ItemLabelInfo.OwningItemActor)
	, Priority(ItemLabelInfo.Priority)
{
}

bool FObsidianItemLabelData::IsValid() const
{
	return ItemLabelWidget && CanvasPanelSlot && OwningItemActor;
}

// ~ End of FObsidianItemLabelData

UObsidianItemLabelManagerSubsystem::UObsidianItemLabelManagerSubsystem()
{
}

void UObsidianItemLabelManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	OnViewportResizeDelegateHandle = FViewport::ViewportResizedEvent.AddUObject(this, &ThisClass::HandleViewportResize);
	
	if (const UObsidianItemDataDeveloperSettings* ItemDataSettings = GetDefault<UObsidianItemDataDeveloperSettings>())
	{
		ItemLabelGroundZOffset = ItemDataSettings->DefaultItemLabelGroundZOffset;
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
		UpdateLabelAnchors();
	}
}

TStatId UObsidianItemLabelManagerSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UItemLabelManagerSubsystem, STATGROUP_Tickables);
}

void UObsidianItemLabelManagerSubsystem::UpdateLabelAnchors()
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
	
	for (FObsidianItemLabelData& LabelData : ItemLabelsData)
	{
		if (LabelData.IsValid() && LabelData.bVisible)
		{
			FVector2D OutUpdatedAnchorScreenPosition;
			const bool bSuccess = UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(ObsidianPC,
				LabelData.LabelAdjustedWorldPosition, OutUpdatedAnchorScreenPosition, false);
			if (bSuccess == false)
			{
				UE_LOG(LogItemLabelManager, Warning, TEXT("Label outside of viewport?"));
				return;
			}
			
			LabelData.LabelAnchorPosition = OutUpdatedAnchorScreenPosition;

			const FVector2D NewLabelPosition = LabelData.LabelAnchorPosition + LabelData.LabelSolvedPositionOffset;
			LabelData.CanvasPanelSlot->SetPosition(NewLabelPosition);
		}
	}

	bLayoutDirty = false;
}

void UObsidianItemLabelManagerSubsystem::ClusterLabels()
{
}

void UObsidianItemLabelManagerSubsystem::SolveLabelLayout()
{
	if (bLayoutDirty == false)
	{
		return;
	}

	UE_LOG(LogItemLabelManager, Warning, TEXT("Solving Layout!"));
	
	SolveVerticalCluster();
}

void UObsidianItemLabelManagerSubsystem::SolveVerticalCluster()
{
	ItemLabelsData.Sort([](const FObsidianItemLabelData& DataA, const FObsidianItemLabelData& DataB)
		{
			return DataA.LabelSolvedPosition.Y < DataB.LabelSolvedPosition.Y;
		});
	
	for (int32 i = 1; i < ItemLabelsData.Num(); ++i)
	{
		FObsidianItemLabelData& Current = ItemLabelsData[i];
		FObsidianItemLabelData& Previous = ItemLabelsData[i - 1];
		
		if (/** CheckHorizontalOverlap(Current, Previous) && */ CheckVerticalOverlap(Current, Previous))
		{
			const float NewY = Previous.LabelSolvedPosition.Y + Previous.LabelSize.Y;

			Current.LabelSolvedPosition.Y = NewY;
			Current.LabelSolvedPositionOffset.Y = Current.LabelSolvedPosition.Y - Current.LabelAnchorPosition.Y;
		}
	}
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

void UObsidianItemLabelManagerSubsystem::RegisterItemLabel(const FObsidianItemLabelInfo& ItemLabelInfo)
{
	if (OwningPC.IsValid() == false || ItemLabelOverlay == nullptr || ItemLabelInfo.IsValid() == false)
	{
		return;
	}

	UObsidianItemLabel* ItemLabel = ItemLabelInfo.ItemLabelWidget;
	
	FVector OwningItemWorldPosition = ItemLabelInfo.OwningItemActor->GetActorLocation();
	OwningItemWorldPosition.Z += ItemLabelGroundZOffset;
	
	FVector2D OutInitialScreenPosition;
	const bool bSuccess = UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(OwningPC.Get(),
				OwningItemWorldPosition, OutInitialScreenPosition, false);
	if (bSuccess == false)
	{
		return;
	}
	
	if (UCanvasPanelSlot* CanvasPanelSlot = ItemLabelOverlay->AddItemLabelToOverlay(ItemLabel,
		OutInitialScreenPosition))
	{
		UE_LOG(LogItemLabelManager, Display, TEXT("New Label Pos: %s"), *OutInitialScreenPosition.ToString());
		FObsidianItemLabelData NewLabelData = FObsidianItemLabelData(ItemLabelInfo);
		NewLabelData.CanvasPanelSlot = CanvasPanelSlot;
		NewLabelData.LabelSize = CanvasPanelSlot->GetSize();
		NewLabelData.LabelAdjustedWorldPosition = OwningItemWorldPosition;
		NewLabelData.LabelAnchorPosition = OutInitialScreenPosition;
		NewLabelData.LabelSolvedPosition = OutInitialScreenPosition;
		NewLabelData.bVisible = true; //TODO(intrxx) Check if highlight is toggled on first
		ItemLabelsData.Add(NewLabelData);
		
		// Check if the Layout needs solving first? Item Can be outside any cluster
		bLayoutDirty = true;
	}
}

void UObsidianItemLabelManagerSubsystem::UnregisterItemLabel(UObsidianItemLabel* ItemLabel)
{
	if (ItemLabel == nullptr)
	{
		return;
	}
	
	for(auto It = ItemLabelsData.CreateIterator(); It; ++It)
	{
		FObsidianItemLabelData& Entry = *It;
		if(Entry.ItemLabelWidget == ItemLabel)
		{
			ItemLabel->RemoveFromParent();
			It.RemoveCurrent();
		}
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
