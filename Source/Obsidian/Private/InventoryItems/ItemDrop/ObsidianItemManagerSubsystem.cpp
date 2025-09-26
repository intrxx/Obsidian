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

void UObsidianItemManagerSubsystem::RequestDroppingItemsAsync(TArray<FObsidianDropItem>&& ItemsToDrop, const uint8 TreasureQuality) const
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

	//TODO(intrxx) dont async load it here, async load it in other place, ItemDropComponent should have these loaded to roll affixes and then this manager can just drop it with some delay or something.
	FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
	StreamableManager.RequestAsyncLoad(ItemsToDropPaths, FStreamableDelegate::CreateLambda([WeakWorld, ItemsToDrop = MoveTemp(ItemsToDrop), TreasureQuality]()
		{
			if (WeakWorld.IsValid() == false)
			{
				return;
			}
		
			for (const FObsidianDropItem& RolledItem : ItemsToDrop)
			{
				if (const TSubclassOf<UObsidianInventoryItemDefinition>& ItemToDrop = RolledItem.SoftTreasureItemDefinitionClass.Get())
				{
					uint8 StacksToDrop = 1;
					if (const UObsidianInventoryItemDefinition* DefaultObject = ItemToDrop.GetDefaultObject())
					{
						if (UOInventoryItemFragment_Affixes* AffixFragment = DefaultObject->GetAffixFragment_Mutable())
						{
							AffixFragment->RandomiseStaticAffixValues();
							
							switch (AffixFragment->GetGenerationType())
							{
								case EObsidianAffixGenerationType::NoGeneration:
									{} break;
								case EObsidianAffixGenerationType::DefaultGeneration:
									{
										
									} break;
								case EObsidianAffixGenerationType::FullGeneration:
									{
										
									} break;
									
									default:
										{} break;
							}
							
							//TODO Roll Affixes
							// Don't worry about soft Gameplay Effects as they can be loaded by Items in Begin Play

							// // Temp
							// FObsidianDynamicItemAffix TempAffix;
							// TempAffix.AffixDescription = FText::FromString(TEXT("My super duper Temp Affix Desc."));
							// TempAffix.AffixTag = ObsidianGameplayTags::Item_Affix_Attribute_Dexterity;
							// TempAffix.AffixType = EObsidianAffixType::Prefix;
							// TempAffix.AffixTier = 2;
							// TempAffix.TempAffixMagnitude = 6;
							// AffixFragment->InitializeDynamicAffixes({TempAffix}, ObsidianGameplayTags::Item_Rarity_Magic);
						}

						StacksToDrop = DefaultObject->IsStackable() == true ? RolledItem.DropStacks : StacksToDrop;
					}
					
					AObsidianDroppableItem* Item = WeakWorld.Get()->SpawnActorDeferred<AObsidianDroppableItem>(AObsidianDroppableItem::StaticClass(), RolledItem.DropTransform);
					Item->InitializeItem(ItemToDrop, StacksToDrop);
					Item->FinishSpawning(RolledItem.DropTransform);
				}
			}
		}));
}
