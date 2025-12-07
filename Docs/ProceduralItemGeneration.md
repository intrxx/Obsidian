# Procedural Item Generation

Here you can find a [youtube video](https://www.youtube.com/watch?v=HcLxNVmMXsk) showcasing the System.

You can also find a testable packaged game [here](https://github.com/intrxx/Obsidian/releases/tag/Procedural_Item_Generation_Showcase_v0.1.0).

<a name="table-of-contents"></a>
## Class Structure Overview

> 1. [Backend](#1-backend) \
> &nbsp; 1.1 [ObsidianDropComponent](#11-obsidiandropcomponent-h--cpp) \
> &nbsp; 1.2 [ObsidianItemDropManagerSubsystem](#12-obsidianitemdropmanagersubsystem-h--cpp) \
> &nbsp; 1.3 [ObsidianItemAffixStack](#12-obsidianitemaffixstack-h--cpp) 
> 2. [Item Data](#2-item-data) \
> &nbsp; 2.1 [ObsidianItemDataDeveloperSettings](#12-obsidianitemdatadevelopersettings-h--cpp) \
> &nbsp; 2.2 [ObsidianItemDataLoaderSubsystem](#12-obsidianitemdataloadersubsystem-h--cpp) \
> &nbsp; 2.3 [ObsidianItemDataConfig](#12-obsidianitemdataconfig-h--cpp) \
> &nbsp; 2.4 [ObsidianTreasureList](#12-obsidiantreasurelist-h--cpp) \
> &nbsp; 2.5 [ObsidianAffixList](#12-obsidianaffixlist-h--cpp) \
> &nbsp; 2.6 [ObsidianAffixAbilitySet](#12-obsidianaffixabilityset-h--cpp) \
> &nbsp; 2.7 [ObsidianItemTypes](#12-obsidianitemtypes-h--cpp) 
> 3. [Gameplay](#3-gameplay) \
> &nbsp; 3.1 [ObsidianItemSpawner](#31-obsidianitemspawner-h--cpp) 
> 4. [The Rest](#4-the-rest) \
> &nbsp; 4.1 [ObsidianEnemy](#52-obisidianenemy-h--cpp)

## 1. Backend

### 1.1 ObsidianDropComponent ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ItemDrop/ObsidianItemDropComponent.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ItemDrop/ObsidianItemDropComponent.cpp))

Drop Component is the heart of the system, if an entity wishes to drop items it needs to add the **ObsidianDropComponent** and call ```DropItem``` in desired place. The default behaviour of Drop Item is to take the default set of common items specified in **ObsidianItemDataDeveloperSettings** and roll from them, the user can always specified additional Trasure Lists on the component instance in order to draw items from it.

### 1.2 ObsidianItemDropManagerSubsystem ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ItemDrop/ObsidianItemDropManagerSubsystem.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ItemDrop/ObsidianItemDropManagerSubsystem.cpp))

The task of this subsystem is contruct ADroppableItems items from FObsidianItemToDrop structure and drop it into the world for the Player.

### 1.3 ObsidianItemAffixStack ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ItemAffixes/ObsidianItemAffixStack.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ItemAffixes/ObsidianItemAffixStack.cpp))

The ```FFastArraySerializer``` Affix structure that handes affixes on the item instance.

## 2. Item Data

### 2.1 ObsidianItemDataDeveloperSettings ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ItemDrop/ObsidianItemDataDeveloperSettings.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ItemDrop/ObsidianItemDataDeveloperSettings.cpp))

A place to specify common data and settings for Procedural Item Generation, such as (but not exclusive to):
- Common item definitions to drop
- Common item affixes definitions
- Default values for item/affix drop requirements
- Item names

### 2.2 ObsidianItemDataLoaderSubsystem ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ItemDrop/ObsidianItemDataLoaderSubsystem.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ItemDrop/ObsidianItemDataLoaderSubsystem.cpp))

System responsible for loading data for the drop system such as Soft Item Definition Pointers.

### 2.3 ObsidianItemDataConfig ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ItemDrop/ObsidianItemDataConfig.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ItemDrop/ObsidianItemDropComponent.cpp))

Data Asset that holds Item Data settings.

### 2.4 ObsidianTreasureList ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ItemDrop/ObsidianItemDropComponent.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ItemDrop/ObsidianItemDataConfig.cpp))

Data Asset that holds Items Data to controcut ItemToDrop from.

### 2.5 ObsidianAffixList ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ItemAffixes/ObsidianAffixList.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ItemAffixes/ObsidianAffixList.cpp))

Data Asset that holds item affixes data to contruct ```FObsidianActiveItemAffix``` from.

### 2.6 ObsidianAffixAbilitySet ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ItemAffixes/ObsidianAffixAbilitySet.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ItemAffixes/ObsidianAffixAbilitySet.cpp))

Data Asset which holds gameplay effects and abilities from Items that can be given to the Player.

### 2.6 ObsidianItemTypes ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/ObsidianTypes/ItemTypes/ObsidianItemTypes.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/ObsidianTypes/ItemTypes/ObsidianItemTypes.cpp))

Place that variouse structures are defined in.

## 3. Gameplay

### 3.1 ObsidianItemSpawner ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/Items/ObsidianItemSpawner.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/Items/ObsidianItemSpawner.cpp))

Actor with ```UObsidianDropComponent``` that can be placed in the level and drop items after fufilling some condition.

## 4. The Rest 

### 4.1 ObsidianEnemy ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/Characters/Enemies/ObsidianEnemy.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/Characters/Enemies/ObsidianEnemy.cpp))

This Enemy class has ```UObsidianDropComponent``` added to and is the most common source of dropped items in Obsidian.
