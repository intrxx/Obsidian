// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/ItemDrop/ObsidianItemManagerSubsystem.h"

// ~ Core

// ~ Project
#include "InventoryItems/ItemDrop/ObsidianTreasureList.h"
#include "InventoryItems/Items/ObsidianDroppableItem.h"
#include "ObsidianTypes/ObsidianItemTypes.h"

void UObsidianItemManagerSubsystem::RequestDroppingItems(TArray<FObsidianDropItem>&& ItemsToDrop) const
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	//TODO(intrxx) Apply delay after every drop?
	for (const FObsidianDropItem& RolledItem : ItemsToDrop)
	{
		if (const TSubclassOf<UObsidianInventoryItemDefinition>& ItemToDrop = RolledItem.SoftTreasureItemDefinitionClass.Get())
		{
			AObsidianDroppableItem* Item = World->SpawnActorDeferred<AObsidianDroppableItem>(AObsidianDroppableItem::StaticClass(), RolledItem.DropTransform);
			Item->InitializeItem(ItemToDrop, RolledItem.DropStacks);
			Item->FinishSpawning(RolledItem.DropTransform);
		}
	}
}
