// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/ItemDrop/ObsidianItemManagerSubsystem.h"

// ~ Core

// ~ Project
#include "InventoryItems/ItemDrop/ObsidianTreasureList.h"
#include "InventoryItems/Items/ObsidianDroppableItem.h"
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"

void UObsidianItemManagerSubsystem::RequestDroppingItems(TArray<FObsidianItemToDrop>&& ItemsToDrop) const
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	//TODO(intrxx) Apply delay after every drop?
	for (const FObsidianItemToDrop& Item : ItemsToDrop)
	{
		if (const TSubclassOf<UObsidianInventoryItemDefinition>& ItemToDrop = Item.ItemDefinitionClass)
		{
			AObsidianDroppableItem* DroppableItem = World->SpawnActorDeferred<AObsidianDroppableItem>(AObsidianDroppableItem::StaticClass(), Item.DropTransform);

			FObsidianItemGeneratedData GeneratedData = FObsidianItemGeneratedData(Item.DropStacks, Item.DropRarity, Item.DropAffixes);
			FObsidianItemGeneratedNameData NewNameData;
			NewNameData.MagicItemDisplayNameAddition = Item.DropMagicItemDisplayNameAddition;
			NewNameData.RareItemDisplayNameAddition = Item.DropRareItemDisplayNameAddition;
			GeneratedData.NameData = NewNameData;

			DroppableItem->InitializeItem(ItemToDrop, GeneratedData);
			DroppableItem->FinishSpawning(Item.DropTransform);
		}
	}
}
