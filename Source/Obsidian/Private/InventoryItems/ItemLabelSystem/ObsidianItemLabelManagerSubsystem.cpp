// Copyright 2026 out of sCope - intrxx

#include "InventoryItems/ItemLabelSystem/ObsidianItemLabelManagerSubsystem.h"

#include "InventoryItems/Items/ObsidianDroppableItem.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "UI/InventoryItems/ObsidianItemLabelOverlay.h"
#include "UI/InventoryItems/Items/ObsidianItemLabel.h"

DECLARE_CYCLE_STAT(TEXT("ItemLabelManager"), STAT_ItemLabelManager, STATGROUP_Tickables);

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
	
}

void UObsidianItemLabelManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UObsidianItemLabelManagerSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateLabelAnchors();
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
	if (const APawn* Pawn = ObsidianPC->GetPawn())
	{
		const float SpeedSquared = Pawn->GetVelocity().SizeSquared();
		if (SpeedSquared < KINDA_SMALL_NUMBER) // Don't do anything if the Player isn't actually moving
		{
			return;
		}
	}
	
	for (const FObsidianItemLabelData& LabelData : ItemLabelsData)
	{
		if (LabelData.IsValid())
		{
			const FVector OwningItemWorldPosition = LabelData.OwningItemActor->GetActorLocation();

			FVector2D OutInitialScreenPosition;
			const bool bSuccess = UGameplayStatics::ProjectWorldToScreen(ObsidianPC, OwningItemWorldPosition,
				OutInitialScreenPosition);
			if (bSuccess == false)
			{
				return;
			}

			LabelData.CanvasPanelSlot->SetPosition(OutInitialScreenPosition);
		}
	}
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
	
	const FVector OwningItemWorldPosition = ItemLabelInfo.OwningItemActor->GetActorLocation();

	FVector2D OutInitialScreenPosition;
	const bool bSuccess = UGameplayStatics::ProjectWorldToScreen(OwningPC.Get(), OwningItemWorldPosition,
		OutInitialScreenPosition);
	if (bSuccess == false)
	{
		return;
	}
	
	if (UCanvasPanelSlot* CanvasPanelSlot = ItemLabelOverlay->AddItemLabelToOverlay(ItemLabelInfo.ItemLabelWidget,
		OutInitialScreenPosition))
	{
		FObsidianItemLabelData NewLabelData = FObsidianItemLabelData(ItemLabelInfo);
		NewLabelData.CanvasPanelSlot = CanvasPanelSlot;
		ItemLabelsData.Add(NewLabelData);
	}
	
}
