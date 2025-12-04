# Procedural Item Generation

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
> &nbsp; 2.4 [ObsidianTreasureList](#12-obsidiantreasurelist-h--cpp) 
> &nbsp; 2.5 [ObsidianAffixList](#12-obsidianaffixlist-h--cpp) \
> &nbsp; 2.6 [ObsidianAffixAbilitySet](#12-obsidianaffixabilityset-h--cpp) \
> &nbsp; 2.7 [ObsidianItemTypes](#12-obsidianitemtypes-h--cpp) 
> 3. [Gameplay](#3-gameplay) \
> &nbsp; 3.1 [ObsidianItemSpawner](#31-obsidianitemspawner-h--cpp) 
> 4. [The Rest](#4-the-rest) \
> &nbsp; 4.1 [ObsidianEnemy](#52-obisidianenemy-h--cpp)

## 1. Backend

### 1.1 ObsidianDropComponent ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ItemDrop/ObsidianItemDropComponent.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ItemDrop/ObsidianItemDropComponent.cpp))

### 1.2 ObsidianItemDropManagerSubsystem ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ItemDrop/ObsidianItemDropManagerSubsystem.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ItemDrop/ObsidianItemDropManagerSubsystem.cpp))

### 1.3 ObsidianItemAffixStack ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ItemAffixes/ObsidianItemAffixStack.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ItemAffixes/ObsidianItemAffixStack.cpp))

## 2. Item Data

### 2.1 ObsidianItemDataDeveloperSettings ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ItemDrop/ObsidianItemDataDeveloperSettings.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ItemDrop/ObsidianItemDataDeveloperSettings.cpp))

### 2.2 ObsidianItemDataLoaderSubsystem ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ItemDrop/ObsidianItemDataLoaderSubsystem.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ItemDrop/ObsidianItemDataLoaderSubsystem.cpp))

### 2.3 ObsidianItemDataConfig ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ItemDrop/ObsidianItemDataConfig.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ItemDrop/ObsidianItemDropComponent.cpp))

### 2.4 ObsidianTreasureList ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ItemDrop/ObsidianItemDropComponent.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ItemDrop/ObsidianItemDataConfig.cpp))

### 2.5 ObsidianAffixList ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ItemAffixes/ObsidianAffixList.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ItemAffixes/ObsidianAffixList.cpp))

### 2.6 ObsidianAffixAbilitySet ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ItemAffixes/ObsidianAffixAbilitySet.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ItemAffixes/ObsidianAffixAbilitySet.cpp))

### 2.6 ObsidianItemTypes ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/ObsidianTypes/ItemTypes/ObsidianItemTypes.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/ObsidianTypes/ItemTypes/ObsidianItemTypes.cpp))

## 3. Gameplay

### 3.1 ObsidianItemSpawner ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/Items/ObsidianItemSpawner.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/Items/ObsidianItemSpawner.cpp))

## 4. The Rest 

### 4.1 ObsidianEnemy ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/Characters/Enemies/ObsidianEnemy.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/Characters/Enemies/ObsidianEnemy.cpp))