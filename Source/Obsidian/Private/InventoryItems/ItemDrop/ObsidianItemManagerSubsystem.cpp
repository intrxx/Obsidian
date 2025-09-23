// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/ItemDrop/ObsidianItemManagerSubsystem.h"

// ~ Core
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

// ~ Project
#include "InventoryItems/ItemDrop/ObsidianTreasureList.h"
#include "InventoryItems/Items/ObsidianDroppableItem.h"

void UObsidianItemManagerSubsystem::RequestDroppingItemsAsync(TArray<FObsidianDropItem>&& ItemsToDrop, TArray<FTransform>&& DropLocations, const uint8 TreasureQuality) const
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	TWeakObjectPtr<UWorld> WeakWorld(World);
	
	TArray<FSoftObjectPath> ItemsToDropPaths;
	for (const FObsidianDropItem& RolledItem : ItemsToDrop)
	{
		ItemsToDropPaths.Add(RolledItem.TreasureItemDefinitionClass.ToSoftObjectPath());
	}

	check(ItemsToDrop.Num() == DropLocations.Num());
	FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
	StreamableManager.RequestAsyncLoad(ItemsToDropPaths, FStreamableDelegate::CreateLambda([WeakWorld, ItemsToDrop = MoveTemp(ItemsToDrop), DropLocations = MoveTemp(DropLocations), TreasureQuality]()
		{
			if (WeakWorld.IsValid() == false)
			{
				return;
			}
		
			uint16 DropIndex = 0;
			for (const FObsidianDropItem& RolledItem : ItemsToDrop)
			{
				if (const TSubclassOf<UObsidianInventoryItemDefinition>& ItemToDrop = RolledItem.TreasureItemDefinitionClass.Get())
				{
					uint8 StacksToDrop = 1;
					if (const UObsidianInventoryItemDefinition* DefaultObject = ItemToDrop.GetDefaultObject())
					{
						StacksToDrop = DefaultObject->IsStackable() == true ? RolledItem.GetRandomStackSizeToDropAdjusted(TreasureQuality) : StacksToDrop;

						if (DefaultObject->IsEquippable())
						{
							//TODO Roll Affixes
							// Don't worry about soft Gameplay Effects as they can be loaded by Items in Begin Play
						}
					}
					
					AObsidianDroppableItem* Item = WeakWorld.Get()->SpawnActorDeferred<AObsidianDroppableItem>(AObsidianDroppableItem::StaticClass(), DropLocations[DropIndex]);
					Item->InitializeItem(ItemToDrop, StacksToDrop);
					Item->FinishSpawning(DropLocations[DropIndex]);
					DropIndex++;
				}
			}
		}));
}
