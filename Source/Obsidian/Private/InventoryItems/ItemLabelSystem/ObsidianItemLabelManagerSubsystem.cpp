// Copyright 2026 out of sCope - intrxx

#include "InventoryItems/ItemLabelSystem/ObsidianItemLabelManagerSubsystem.h"

#include <Components/CanvasPanelSlot.h>
#include <Blueprint/WidgetLayoutLibrary.h>

#include "Characters/Player/ObsidianPlayerController.h"
#include "InventoryItems/ItemDrop/ObsidianItemDataDeveloperSettings.h"
#include "InventoryItems/Items/ObsidianItemLabelComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Math/CameraPoseMath.h"
#include "Obsidian/ObsidianGameModule.h"
#include "UI/InventoryItems/ObsidianItemLabelOverlay.h"
#include "UI/InventoryItems/Items/ObsidianItemLabel.h"

DECLARE_CYCLE_STAT(TEXT("ItemLabelManager"), STAT_ItemLabelManager, STATGROUP_Tickables);
DEFINE_LOG_CATEGORY(LogItemLabelManager)

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

	bVisible = false;
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
	
	// ~ Debug help
	static ConstructorHelpers::FClassFinder<UUserWidget> ItemLabelHelperTL(TEXT("/Game/Obsidian/Debug/WBP_ItemLabelDebugHelperTL.WBP_ItemLabelDebugHelperTL_C"));
	if (ItemLabelClassFinder.Succeeded())
	{
		ItemLabelHelperTLClass = ItemLabelHelperTL.Class;
	}
	static ConstructorHelpers::FClassFinder<UUserWidget> ItemLabelHelperBR(TEXT("/Game/Obsidian/Debug/WBP_ItemLabelDebugHelperBR.WBP_ItemLabelDebugHelperBR_C"));
	if (ItemLabelClassFinder.Succeeded())
	{
		ItemLabelHelperBRClass = ItemLabelHelperBR.Class;
	}
	// ~ End of Debug help
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
	if (OnViewportResizeDelegateHandle.IsValid())
	{
		FViewport::ViewportResizedEvent.Remove(OnViewportResizeDelegateHandle);
	}

	LateTickFunction.UnRegisterTickFunction();
	
	Super::Deinitialize();
}

void UObsidianItemLabelManagerSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bLabelOverlayVisible)
	{
		//SolveLabelLayout_1();
		SolveLabelLayout_2();
	}
}

void UObsidianItemLabelManagerSubsystem::PostWorkTick(float DeltaTime)
{
	if (bLabelOverlayVisible)
	{
		UpdateLabelAnchors(DeltaTime);
	}
	//SolveLabelLayout_2();
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

	uint32 UpdatedWidgetsCount = 0;
	bool bShouldRecalculateLayout = false;
	for (TTuple<FGuid, FObsidianItemLabelData>& Pair : ItemLabelsDataMap)
	{
		FObsidianItemLabelData& LabelData = Pair.Value;

		// FCameraPose CameraPose;
		// CameraPose.
		//UE::Cameras::FCameraPoseMath::ProjectWorldToScreen();
		
		// bool bSuccess = UGameplayStatics::ProjectWorldToScreen(ObsidianPC, LabelData.LabelAdjustedWorldPosition,
		// 	OutUpdatedAnchorScreenPosition, false);

		//TODO(intrxx) Projection gets fucky wacky with the camera perspective so solved layout is drifting, specially when moving top/down
		
		FVector2D OutUpdatedAnchorScreenPosition;
		bool bSuccess = UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(ObsidianPC,
			LabelData.LabelAdjustedWorldPosition, OutUpdatedAnchorScreenPosition, false);
		if (bSuccess == false)
		{
			UE_LOG(LogItemLabelManager, Warning, TEXT("Label outside of viewport?"));
			continue;
		}
		
		LabelData.LabelAnchorPosition = OutUpdatedAnchorScreenPosition;
		const FVector2D NewLabelPosition = LabelData.LabelAnchorPosition + LabelData.LabelSolvedPositionOffset;
		if (IsOutsideCurrentViewport(NewLabelPosition))
		{
			if (LabelData.bVisible)
			{
				UE_LOG(LogTemp, Warning, TEXT("[%s] went outside viewport."),
					*LabelData.SourceLabelComponent->GetLabelInitializationData().ItemName.ToString());
				//TODO(intrxx) Deactivate label
				ReleaseWidget(LabelData.ItemLabelWidget);
				LabelData.ResetLabelData();
			}
			continue;
		}
		if (LabelData.bVisible == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("[%s] is back inside the viewport."),
				*LabelData.SourceLabelComponent->GetLabelInitializationData().ItemName.ToString());
			//TODO(intrxx) Activate label

			UObsidianItemLabelComponent* LabelComponent = LabelData.SourceLabelComponent;
			if (LabelComponent == nullptr)
			{
				continue;
			}
				
			const FObsidianLabelInitializationData InitializationData = LabelComponent->GetLabelInitializationData();
			LabelData.ItemLabelWidget = AcquireWidget(LabelData.LabelID);
			LabelData.ItemLabelWidget->SetItemName(InitializationData.ItemName);
			LabelData.ItemLabelWidget->SetVisibility(ESlateVisibility::Visible);
			LabelData.CanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(LabelData.ItemLabelWidget);
			
			LabelData.LabelSize = LabelData.CanvasPanelSlot->GetSize();
				
			LabelData.bVisible = true;
			
			bShouldRecalculateLayout = true;
		}
		
		LabelData.CanvasPanelSlot->SetPosition(NewLabelPosition);
		UpdatedWidgetsCount++;
	}

	//UE_LOG(LogTemp, Warning, TEXT("Updated [%d] widget's positions."), UpdatedWidgetsCount);

	if (bShouldRecalculateLayout)
	{
		MakeLayoutDirty();
		return;
	}
	MakeLayoutClean();
}

void UObsidianItemLabelManagerSubsystem::SolveLabelLayout_1()
{
	//Note(intrxx) I need to solve this every frame due to camera projection (due to the camera being at an angle)
	if (bLayoutDirty == false) 
	{
		return;
	}
	
	if (ItemLabelsDataMap.IsEmpty())
	{
		return;
	}

	//TODO(intrxx) Force this now to get all proper sizes, need to work it out I guess or check how expensive this is,
	// Solve Layout is already being called in another frame but for some reason the Layout isn't prepassed anyway
	ItemLabelOverlay->ForceLayoutPrepass();

	AObsidianPlayerController* OwningOPC = OwningPC.IsValid()
		? OwningPC.Get()
		: Cast<AObsidianPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (OwningOPC == nullptr)
	{
		return;
	}

	UE_LOG(LogItemLabelManager, Warning, TEXT("------ Solving Layout 1! ------"));

	TArray<FObsidianItemLabelData*> CandidateLabels;
	CandidateLabels.Reserve(ItemLabelsDataMap.Num());

	// FVector CamLoc = OwningOPC->PlayerCameraManager
	// 	? OwningOPC->PlayerCameraManager->GetCameraLocation()
	// 	: FVector::Zero();

	for (TTuple<FGuid, FObsidianItemLabelData>& Pair : ItemLabelsDataMap)
	{
		FObsidianItemLabelData& LabelData = Pair.Value;
		if (LabelData.bVisible == false)
		{
			continue;
		}
		
		CandidateLabels.Add(&LabelData);
	}

	UE_LOG(LogTemp, Display, TEXT("Solving Layout for [%d] widgets."), CandidateLabels.Num());
	
	CandidateLabels.Sort([](const FObsidianItemLabelData& DataA, const FObsidianItemLabelData& DataB)
		{
			const uint8 APriority = DataA.Priority;
			const uint8 BPriority = DataB.Priority;
			if (APriority != BPriority)
			{
				return APriority > BPriority;
			}

			const float AY = DataA.LabelSolvedPosition.Y;
			const float BY = DataB.LabelSolvedPosition.Y;
			if (!FMath::IsNearlyEqual(AY, BY))
			{
				return AY < BY;
			}
		
			return DataA.LabelID < DataB.LabelID;
		});

	// ~ Debug
	// int32 index = 1;
	// UE_LOG(LogTemp, Display, TEXT("Solving for widgets in Priority:"));
	// for (const auto& Data : CandidateLabels)
	// {
	// 	UE_LOG(LogTemp, Display, TEXT("%d. [%s]."), index,
	// 		*Data->SourceLabelComponent->GetLabelInitializationData().ItemName.ToString());
	// 	++index;
	// }
	// ~ End of Debug

	TArray<FBox2D> Occupied;
	Occupied.Reserve(CandidateLabels.Num());

	if (CandidateLabels.IsEmpty() == false)
	{
		FObsidianItemLabelData& FirstLabel = *CandidateLabels[0];
		if (FirstLabel.LabelSize.IsZero())
		{
			FirstLabel.LabelSize = FirstLabel.ItemLabelWidget->GetDesiredSize();
		}
		// ~ Debug
		if (FirstLabel.LabelSize.IsZero())
		{
			UE_LOG(LogTemp, Error, TEXT("First Label Size is zero!! [%s]"), *FirstLabel.LabelSize.ToString());
		}
		UE_LOG(LogTemp, Display, TEXT("First Label Size: [%s]"), *FirstLabel.LabelSize.ToString());
		// ~ End of debug
		
		const FVector2D FirstLabelTopLeft = FirstLabel.LabelSolvedPosition;
		const FVector2D FirstLabelBottomRight = FVector2D(
				FirstLabel.LabelSolvedPosition.X + FirstLabel.LabelSize.X,
				FirstLabel.LabelSolvedPosition.Y + FirstLabel.LabelSize.Y);
		Occupied.Add(FBox2D(FirstLabelTopLeft, FirstLabelBottomRight));

		// ~ Debug help
		// UUserWidget* LabelDebugTL = CreateWidget(OwningOPC, ItemLabelHelperTLClass);
		// UUserWidget* LabelDebugBR = CreateWidget(OwningOPC, ItemLabelHelperBRClass);
		// UCanvasPanelSlot* CanvasPanelSlotTL = ItemLabelOverlay->AddItemLabelToOverlayDebug(LabelDebugTL, FirstLabelTopLeft);
		// UCanvasPanelSlot* CanvasPanelSlotBR = ItemLabelOverlay->AddItemLabelToOverlayDebug(LabelDebugBR, FirstLabelBottomRight);
		// ~ End of Debug help
	}
	
	for (int32 i = 1; i < CandidateLabels.Num(); ++i)
	{
		FObsidianItemLabelData& CurrentLabelData = *CandidateLabels[i];
		FObsidianItemLabelData& PreviousLabelData = *CandidateLabels[i - 1];

		UE_LOG(LogTemp, Warning, TEXT("Solving for [%s] label."),
				*CurrentLabelData.SourceLabelComponent->GetLabelInitializationData().ItemName.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Previous Label being: [%s]."),
				*PreviousLabelData.SourceLabelComponent->GetLabelInitializationData().ItemName.ToString());
		
		if (CurrentLabelData.LabelSize.IsZero())
		{
			CurrentLabelData.LabelSize = CurrentLabelData.ItemLabelWidget->GetDesiredSize();
		}
		
		// ~ Debug
		// if (CurrentLabelData.LabelSize.IsZero())
		// {
		// 	UE_LOG(LogTemp, Error, TEXT("Current Label Size is zero!! [%s]"), *CurrentLabelData.LabelSize.ToString());
		// }
		// UE_LOG(LogTemp, Display, TEXT("Current Label Size: [%s]"), *CurrentLabelData.LabelSize.ToString());
		// ~ End of debug

		// Assume label alignment [0, 0] (top left)
		const float PreviousLabelHeight = PreviousLabelData.LabelSize.Y;
		const float PreviousLabelWidth = PreviousLabelData.LabelSize.X;
		const float CurrentLabelHeight = CurrentLabelData.LabelSize.Y;
		const float CurrentLabelWidth = CurrentLabelData.LabelSize.X;
		const FVector2D CurrentLabelTopLeft = CurrentLabelData.LabelSolvedPosition;
		const FVector2D CurrentLabelBottomRight = FVector2D(
				CurrentLabelData.LabelSolvedPosition.X + CurrentLabelWidth,
				CurrentLabelData.LabelSolvedPosition.Y + CurrentLabelHeight);

		// I think I want the offsets to lay the item next to previous one (if it's in some range) so the current location
		// of PreviousLabelData will need to be used, so far the items aren't aligning by if they have enough space for
		// offset 0,0 to work
		static float PlacementOffset = 1.0f;
		const TArray<FVector2D> CandidateOffsets = {
			FVector2D(0.f, 0.f),
			FVector2D(0.f, -PreviousLabelHeight - PlacementOffset),
			FVector2D(0.f, PreviousLabelHeight + PlacementOffset),
			FVector2D(-PreviousLabelWidth - PlacementOffset, PlacementOffset),
			FVector2D(PreviousLabelWidth + PlacementOffset, PlacementOffset),
			FVector2D(-PreviousLabelWidth - PlacementOffset, -PreviousLabelHeight - PlacementOffset),
			FVector2D(-PreviousLabelWidth - PlacementOffset, PreviousLabelHeight + PlacementOffset),
			FVector2D(PreviousLabelWidth + PlacementOffset, -PreviousLabelHeight - PlacementOffset),
			FVector2D(PreviousLabelWidth + PlacementOffset, PreviousLabelHeight + PlacementOffset)};

		bool bPlaced = false;
		for (const FVector2D& Offset : CandidateOffsets)
		{
			const FVector2D BoxTopLeft = CurrentLabelTopLeft + Offset;
			const FVector2D BoxBottomRight = BoxTopLeft + FVector2D(CurrentLabelWidth, CurrentLabelHeight);
			FBox2D LabelRect = FBox2D(BoxTopLeft, BoxBottomRight);

			bool bOverlap = false;
			for (const FBox2D& OccupiedRegion : Occupied)
			{
				if (OccupiedRegion.Intersect(LabelRect))
				{
					UE_LOG(LogTemp, Display, TEXT("[%s] intersects [%s], skipping."), *LabelRect.ToString(), *OccupiedRegion.ToString());
					bOverlap = true;
					break;
				}
			}

			if (bOverlap == false)
			{
				CurrentLabelData.LabelSolvedPosition = CurrentLabelData.LabelSolvedPosition + Offset;
				CurrentLabelData.LabelSolvedPositionOffset = CurrentLabelData.LabelSolvedPosition - CurrentLabelData.LabelAnchorPosition;
				Occupied.Add(LabelRect);
				bPlaced = true;
				break;
			}
		}

		if (bPlaced == false)
		{
			UE_LOG(LogObsidian, Error, TEXT("There is no more room for another Item Label on the screen!"));
			// UUserWidget* LabelDebugTL = CreateWidget(OwningOPC, ItemLabelHelperTLClass);
			// UUserWidget* LabelDebugBR = CreateWidget(OwningOPC, ItemLabelHelperBRClass);
			// UCanvasPanelSlot* CanvasPanelSlotTL = ItemLabelOverlay->AddItemLabelToOverlayDebug(LabelDebugTL, PreviousLabelData.LabelSolvedPosition);
			// UCanvasPanelSlot* CanvasPanelSlotBR = ItemLabelOverlay->AddItemLabelToOverlayDebug(LabelDebugBR, CurrentLabelData.LabelSolvedPosition);
		}
	}

	UE_LOG(LogItemLabelManager, Warning, TEXT("------ End Solving Layout 1! ------"));
}

void UObsidianItemLabelManagerSubsystem::SolveLabelLayout_2()
{
	//TODO(intrxx) I need to solve this every frame due to camera projection (due to the camera being at an angle)
	if (bLayoutDirty == false) 
	{
		return;
	}

	if (ItemLabelsDataMap.IsEmpty())
	{
		return;
	}

	//TODO(intrxx) Force this now to get all proper sizes, need to work it out I guess or check how expensive this is,
	// Solve Layout is already being called in another frame but for some reason the Layout isn't prepassed anyway
	ItemLabelOverlay->ForceLayoutPrepass();

	AObsidianPlayerController* OwningOPC = OwningPC.IsValid()
		? OwningPC.Get()
		: Cast<AObsidianPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (OwningOPC == nullptr)
	{
		return;
	}

	UE_LOG(LogItemLabelManager, Warning, TEXT("------ Solving Layout 2! ------"));

	TArray<FObsidianItemLabelData*> CandidateLabels;
	CandidateLabels.Reserve(ItemLabelsDataMap.Num());

	for (TTuple<FGuid, FObsidianItemLabelData>& Pair : ItemLabelsDataMap)
	{
		FObsidianItemLabelData& LabelData = Pair.Value;
		if (LabelData.bVisible == false || LabelData.IsValid() == false)
		{
			UE_LOG(LogTemp, Display, TEXT("Skipping label."));
			continue;
		}
		
		CandidateLabels.Add(&LabelData);
	}

	UE_LOG(LogTemp, Display, TEXT("Solving Layout for [%d] widgets."), CandidateLabels.Num());
	
	CandidateLabels.Sort([](const FObsidianItemLabelData& DataA, const FObsidianItemLabelData& DataB)
		{
			const uint8 APriority = DataA.Priority;
			const uint8 BPriority = DataB.Priority;
			if (APriority != BPriority)
			{
				return APriority > BPriority;
			}

			const float AY = DataA.LabelAnchorPosition.Y;
			const float BY = DataB.LabelAnchorPosition.Y;
			if (!FMath::IsNearlyEqual(AY, BY))
			{
				return AY < BY;
			}
		
			return DataA.LabelID < DataB.LabelID;
		});

	TArray<FBox2D> Occupied;
    Occupied.Reserve(CandidateLabels.Num());
	
    for (FObsidianItemLabelData* Label : CandidateLabels)
    {
        if (Label->LabelSize.IsZero() && Label->ItemLabelWidget)
        {
            Label->LabelSize = Label->ItemLabelWidget->GetDesiredSize();
        }

        const FVector2D Anchor = Label->LabelAnchorPosition;
        const FVector2D Size = Label->LabelSize;
    	const FVector2D HalfSize = Size * 0.5f;
    	
        bool bPlaced = false;
	    
	    {
		    const FVector2D LabelPreviouslySolvedCenter = Anchor + Label->LabelSolvedPositionOffset;
        	const FVector2D LabelTopLeft = LabelPreviouslySolvedCenter - HalfSize;
        	const FVector2D LabelBottomRight = LabelPreviouslySolvedCenter + HalfSize;
        	const FBox2D Rect = FBox2D(LabelTopLeft, LabelBottomRight);

        	bool bOverlap = false;
        	for (const FBox2D& TakenBoxArea : Occupied)
        	{
        		if (TakenBoxArea.Intersect(Rect))
        		{
        			bOverlap = true;
        			break;
        		}
        	}

        	if (!bOverlap)
        	{
        		Label->LabelSolvedPosition = LabelPreviouslySolvedCenter;
        		Label->LabelSolvedPositionOffset = LabelPreviouslySolvedCenter - Anchor;
        		Occupied.Add(Rect);
        		continue;
        	}
	    }
    	
        FDeterministicRadialEnumerator Enumerator(4, 50);
        Enumerator.Reset();

        FVector2D Offset;
        while (Enumerator.Next(Offset))
        {
        	const FVector2D LabelCenter = Anchor + Offset;
            FVector2D LabelTopLeft = LabelCenter - HalfSize;
            FVector2D LabelBottomRight = LabelCenter + HalfSize;
            FBox2D Rect = FBox2D(LabelTopLeft, LabelBottomRight);

            bool bOverlap = false;

            for (const FBox2D& TakenBoxArea : Occupied)
            {
                if (TakenBoxArea.Intersect(Rect))
                {
                    bOverlap = true;
                    break;
                }
            }

            if (!bOverlap)
            {
                Label->LabelSolvedPosition = LabelCenter;
                Label->LabelSolvedPositionOffset = LabelCenter - Anchor;

                Occupied.Add(Rect);
                bPlaced = true;
                break;
            }
        }
    	
        if (!bPlaced)
        {
        	UE_LOG(LogObsidian, Error, TEXT("No Placement found for [%s]"),
        		*Label->SourceLabelComponent->GetLabelInitializationData().ItemName.ToString());
        	// This problem will occur one way or another, the question is, how to actually drop an Item now?
            Label->bVisible = false;
        }
        else
        {
            Label->bVisible = true;
        }
    }
	
	UE_LOG(LogItemLabelManager, Warning, TEXT("------ End Solving Layout 2! ------"));
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
	return true;
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
	
	const FObsidianLabelInitializationData InitializationData = SourceLabelComponent->GetLabelInitializationData();
	NewLabelData.ItemLabelWidget = AcquireWidget(NewLabelData.LabelID);
	NewLabelData.ItemLabelWidget->SetItemName(InitializationData.ItemName);
	NewLabelData.ItemLabelWidget->SetVisibility(ESlateVisibility::Visible);

	if (UCanvasPanelSlot* CanvasPanelSlot = ItemLabelOverlay->AddItemLabelToOverlay(NewLabelData.ItemLabelWidget,
		OutInitialScreenPosition))
	{
		UE_LOG(LogItemLabelManager, Display, TEXT("New Label Pos: %s"), *OutInitialScreenPosition.ToString());
		
		NewLabelData.CanvasPanelSlot = CanvasPanelSlot;
		
		//TODO(intrxx) Check if the Layout needs solving first? Item Can be outside any cluster.
		if (const UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ThisClass::MakeLayoutDirty));
		}
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
		MakeLayoutDirty();
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
	MakeLayoutDirty();
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

void UObsidianItemLabelManagerSubsystem::MakeLayoutDirty()
{
	bLayoutDirty = true;
}

void UObsidianItemLabelManagerSubsystem::MakeLayoutClean()
{
	bLayoutDirty = false;
}
