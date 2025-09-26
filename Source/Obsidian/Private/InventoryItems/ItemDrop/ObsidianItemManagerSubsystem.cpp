// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/ItemDrop/ObsidianItemManagerSubsystem.h"

// ~ Core
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

// ~ Project
#include "InventoryItems/Fragments/OInventoryItemFragment_Affixes.h"
#include "InventoryItems/ItemDrop/ObsidianTreasureList.h"
#include "InventoryItems/Items/ObsidianDroppableItem.h"
#include "ObsidianTypes/ObsidianItemTypes.h"

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
		ItemsToDropPaths.Add(RolledItem.SoftTreasureItemDefinitionClass.ToSoftObjectPath());
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
				if (const TSubclassOf<UObsidianInventoryItemDefinition>& ItemToDrop = RolledItem.SoftTreasureItemDefinitionClass.Get())
				{
					uint8 StacksToDrop = 1;
					if (const UObsidianInventoryItemDefinition* DefaultObject = ItemToDrop.GetDefaultObject())
					{
						StacksToDrop = DefaultObject->IsStackable() == true ? RolledItem.GetRandomStackSizeToDropAdjusted(TreasureQuality) : StacksToDrop;
						
						if (UOInventoryItemFragment_Affixes* AffixFragment = DefaultObject->ShouldBeGeneratedAtDrop_GetAffixObject())
						{
							const EObsidianAffixGenerationType GenerationType = AffixFragment->GetGenerationType();
							UE_LOG(LogTemp, Error, TEXT("Implement Generating Random Affixes."));
							
							//TODO Roll Affixes
							// Don't worry about soft Gameplay Effects as they can be loaded by Items in Begin Play

							// // Temp
							// FObsidianDynamicItemAffix TempAffix;
							// TempAffix.AffixDescription = FText::FromString(TEXT("My super duper Temp Affix Desc."));
							// TempAffix.AffixTag = ObsidianGameplayTags::Item_Affix_Attribute_Dexterity;
							// TempAffix.AffixType = EObsidianAffixType::Prefix;
							// TempAffix.AffixTier = 2;
							// TempAffix.TempAffixMagnitude = 6;
							// AffixFragment->AddItemAffixes({TempAffix}, ObsidianGameplayTags::Item_Rarity_Magic);
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
