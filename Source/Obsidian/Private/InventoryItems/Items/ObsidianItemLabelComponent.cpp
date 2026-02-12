// Copyright 2026 out of sCope - intrxx

#include "InventoryItems/Items/ObsidianItemLabelComponent.h"

#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "InventoryItems/ItemLabelSystem/ObsidianItemLabelManagerSubsystem.h"
#include "InventoryItems/Items/ObsidianDroppableItem.h"

UObsidianItemLabelComponent::UObsidianItemLabelComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UObsidianItemLabelComponent::SetItemOwner(AObsidianDroppableItem* OwningItemActor)
{
	WeakOwningItemActor = OwningItemActor;
}

FVector UObsidianItemLabelComponent::GetOwningItemActorLocation() const
{
	if (const AObsidianDroppableItem* DroppableItem = WeakOwningItemActor.Get())
	{
		return DroppableItem->GetActorLocation();
	}
	return FVector::ZeroVector;
}

FObsidianLabelInitializationData UObsidianItemLabelComponent::GetLabelInitializationData() const
{
	FObsidianLabelInitializationData InitializationData;
	
	if (const AObsidianDroppableItem* DroppableItem = WeakOwningItemActor.Get())
	{
		if(const TSubclassOf<UObsidianInventoryItemDefinition> PickupItemDef =
			DroppableItem->GetPickupTemplateFromPickupContent().ItemDef)
		{
			if(const UObsidianInventoryItemDefinition* DefaultItem = PickupItemDef.GetDefaultObject())
			{
				if(const UOInventoryItemFragment_Appearance* Appearance = Cast<UOInventoryItemFragment_Appearance>(
					DefaultItem->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
				{
					InitializationData.ItemName = Appearance->GetItemDisplayName();
				}
			}
		}
		else if(const UObsidianInventoryItemInstance* ItemInstance =
			DroppableItem->GetPickupInstanceFromPickupContent().Item)
		{
			InitializationData.ItemName = ItemInstance->GetItemDisplayName();
		}
	}
	
	return InitializationData;
}

void UObsidianItemLabelComponent::RegisterLabelComponent()
{
	const UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return;
	}
	
	if (UObsidianItemLabelManagerSubsystem* ItemLabelSubsystem = World->GetSubsystem<UObsidianItemLabelManagerSubsystem>())
	{
		RegisteredLabelID = ItemLabelSubsystem->RegisterItemLabel(this);
		check(RegisteredLabelID.IsValid());
	}
}

void UObsidianItemLabelComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EndPlayReason == EEndPlayReason::Type::Destroyed)
	{
		if (const UWorld* World = GetWorld())
		{
			if (UObsidianItemLabelManagerSubsystem* ItemLabelSubsystem = World->GetSubsystem<UObsidianItemLabelManagerSubsystem>())
			{
				check(RegisteredLabelID.IsValid());
				ItemLabelSubsystem->UnregisterItemLabel(RegisteredLabelID);
			}
		}
	}
	
	Super::EndPlay(EndPlayReason);
}

void UObsidianItemLabelComponent::HandleLabelMouseHover(const bool bMouseEnter)
{
	if (AObsidianDroppableItem* OwningDroppableItem = WeakOwningItemActor.Get())
	{
		OwningDroppableItem->OnItemMouseHover(bMouseEnter);
	}
}

void UObsidianItemLabelComponent::HandleLabelMouseButtonDown(const int32 PlayerIndex,
	const FObsidianItemInteractionFlags& InteractionFlags)
{
	if (AObsidianDroppableItem* OwningDroppableItem = WeakOwningItemActor.Get())
	{
		OwningDroppableItem->OnItemMouseButtonDown(PlayerIndex, InteractionFlags);
	}
}


