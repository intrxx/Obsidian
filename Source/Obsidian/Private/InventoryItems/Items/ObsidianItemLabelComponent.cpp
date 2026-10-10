// Copyright 2026 out of sCope - intrxx

#include "InventoryItems/Items/ObsidianItemLabelComponent.h"

#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "InventoryItems/ItemLabelSystem/ObsidianItemLabelManagerSubsystem.h"
#include "InventoryItems/Items/ObsidianDroppableItem.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"


UObsidianItemLabelComponent::UObsidianItemLabelComponent(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UObsidianItemLabelComponent::SetItemOwner(AObsidianDroppableItem* InOwningItemActor)
{
	WeakOwningItemActor = InOwningItemActor;
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

void UObsidianItemLabelComponent::EndPlay(const EEndPlayReason::Type InEndPlayReason)
{
	if (InEndPlayReason == EEndPlayReason::Type::Destroyed)
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
	
	Super::EndPlay(InEndPlayReason);
}

void UObsidianItemLabelComponent::HandleLabelMouseHover(const bool bInMouseEnter)
{
	if (AObsidianDroppableItem* OwningDroppableItem = WeakOwningItemActor.Get())
	{
		OwningDroppableItem->OnItemMouseHover(bInMouseEnter);
	}
}

void UObsidianItemLabelComponent::HandleLabelMouseButtonDown(const int32 InPlayerIndex,
	const FObsidianItemInteractionFlags& InInteractionFlags)
{
	if (AObsidianDroppableItem* OwningDroppableItem = WeakOwningItemActor.Get())
	{
		OwningDroppableItem->OnItemMouseButtonDown(InPlayerIndex, InInteractionFlags);
	}
}


