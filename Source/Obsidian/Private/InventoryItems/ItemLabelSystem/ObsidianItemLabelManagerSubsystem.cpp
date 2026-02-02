// Copyright 2026 out of sCope - intrxx

#include "InventoryItems/ItemLabelSystem/ObsidianItemLabelManagerSubsystem.h"

UObsidianItemLabelManagerSubsystem::UObsidianItemLabelManagerSubsystem()
{
}

void UObsidianItemLabelManagerSubsystem::InitializeItemLabelManager(UObsidianItemLabelOverlay* InItemLabelOverlay)
{
	ItemLabelOverlay = InItemLabelOverlay;

	//TODO(intrxx) Gather any items in the world and init it? Or init it in item's Begin Play?
}

void UObsidianItemLabelManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogTemp, Display, TEXT("Hi from UObsidianItemLabelManagerSubsystem"));
}

void UObsidianItemLabelManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}
