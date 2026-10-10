// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/ItemDrop/ObsidianItemDropManagerSubsystem.h"

#include "InventoryItems/ItemDrop/ObsidianTreasureList.h"
#include "InventoryItems/Items/ObsidianDroppableItem.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"


void UObsidianItemDropManagerSubsystem::RequestDroppingItems(TArray<FObsidianItemToDrop>&& InItemsToDrop) const
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	//TODO(intrxx) Apply delay after every drop?
	for (const FObsidianItemToDrop& Item : InItemsToDrop)
	{
		if (const TSubclassOf<UObsidianInventoryItemDefinition>& ItemToDrop = Item.ItemDefinitionClass)
		{
			AObsidianDroppableItem* DroppableItem = World->SpawnActorDeferred<AObsidianDroppableItem>(
				AObsidianDroppableItem::StaticClass(), Item.DropTransform);

			FObsidianItemGeneratedData GeneratedData;
			GeneratedData.SetStackCount(Item.DropStacks);
			GeneratedData.ItemLevel = Item.DropItemLevel;
			GeneratedData.ItemRarity = Item.DropRarity;
			GeneratedData.ItemAffixes = Item.DropAffixes;
			GeneratedData.ItemEquippingRequirements = Item.DropItemRequirements;
			
			FObsidianItemGeneratedNameData NewNameData;
			NewNameData.MagicItemDisplayNameAddition = Item.DropMagicItemDisplayNameAddition;
			NewNameData.RareItemDisplayNameAddition = Item.DropRareItemDisplayNameAddition;
			GeneratedData.NameData = NewNameData;

			DroppableItem->InitializeItem(ItemToDrop, GeneratedData);
			DroppableItem->FinishSpawning(Item.DropTransform);
		}
	}
}
