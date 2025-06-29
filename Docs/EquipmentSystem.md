# Obsidian Equipment System

Here you can find a [youtube video](https://www.youtube.com/watch?v=Fop-aGk-9MU) showcasing the System.

You can also find a testable packaged game [here](https://github.com/intrxx/Obsidian/releases/tag/Equipment_System_v0.1.0).

*Most of the classes used in the Equipment System is cross used by described previously [Inventory System](https://github.com/intrxx/Obsidian/blob/main/Docs/InventoryItemsSystem.md).
I won't be mentioning these classes here, they serve pretty much the same role for these systems.*

![Equipment Showcase](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/EquipmentSystem/EquipmentSystemShowcase.jpg)

<a name="table-of-contents"></a>
## Class Structure Overview

> 1. [Backend](#1-backend) \
> &nbsp; 1.1 [ObsidianEquipmentComponent](#11-obsidianequipmentcomponent-h--cpp) \
> &nbsp; 1.2 [ObsidianEquipmentList](#12-obsidianequipmentlist-h--cpp) 
> 2. [User Interface](#2-user-interface) \
> &nbsp; 2.1 [ObsidianEquipmentPanel](#21-obsidianequipmentpanel-h--cpp) \
> &nbsp;&nbsp; 2.2.1 [ObsidianItemSlot_Equipment](#221-obsidianitemslot_equipment-h--cpp) \
> &nbsp; 2.2 [ObsidianSlotBlockadeItem](#22-obsidianslotblockadeitem-h--cpp) 
> 3. [Gameplay](#3-gameplay) \
> &nbsp; 3.1 [ObsidianSpawnedEquipmentPiece](#31-obsidianspawnedequipmentpiece-h--cpp) 
> 4. [Debugging](#4-debugging) \
> &nbsp; 4.1 [GameplayDebuggerCategory_Equipment](#41-gameplaydebuggercategory_equipment-h--cpp)
> 5. [The Rest](#5-the-rest) \
> &nbsp; 5.1 [ObsidianPlayerController](#52-obsidianplayercontroller-h--cpp)

## 1. Backend

### 1.1 ObsidianEquipmentComponent ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/Equipment/ObsidianEquipmentComponent.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/Equipment/ObsidianEquipmentComponent.cpp))

Heart of the backend. Provides all the functionality to interact with Equipment Slots, equipping items, automatic equipping if nothing is equipped at slot, spawn actor on Hero etc.

### 1.2 ObsidianEquipmentList ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/Equipment/ObsidianEquipmentList.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/Equipment/ObsidianEquipmentList.cpp))

The ```FFastArraySerializer``` structure that holds Equipped Items. It also holds the definition and state of all equipment slots.

## 2. User Interface

### 2.1 ObsidianEquipmentPanel ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/Inventory/ObsidianEquipmentPanel.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/Inventory/ObsidianEquipmentPanel.cpp))

Widget that manages equipment slots in form of ```ObsidianItemSlot_Equipment```.

#### 2.2.1 ObsidianItemSlot_Equipment ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/Inventory/Slots/ObsidianItemSlot_Equipment.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/Inventory/Slots/ObsidianItemSlot_Equipment.cpp))

Individual Slot managed in ```ObsidianEquipmentPanel``` displayed in ```ObsidianInventory```, supports different kinds of highlight.

### 2.2 ObsidianSlotBlockadeItem ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/Inventory/Slots/ObsidianSlotBlockadeItem.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/Inventory/Slots/ObsidianSlotBlockadeItem.cpp))

Special ```ObsidianItem``` meant to block the other slot if specific item has that property e.g. Two-Handed Axe.

## 3. Gameplay

### 3.1 ObsidianSpawnedEquipmentPiece ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/Equipment/ObsidianSpawnedEquipmentPiece.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/Equipment/ObsidianSpawnedEquipmentPiece.cpp))

The actual actor that gets spawned on the Hero if one is specified in ```OInventoryItemFragment_Equippable```.

## 4. Debugging

### 4.1 GameplayDebuggerCategory_Equipment ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/Debugging/GameplayDebuggerCategory_Equipment.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/Debugging/GameplayDebuggerCategory_Equipment.cpp))

Gameplay Debugger Category that contains number of Items equipped and its Name, Item Definition Class, Currently
Equipped Slot, Spawned Actors and Owning Ability Sets.

![Equipment System Debugging](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/EquipmentSystem/equipmentdebugger.jpg)

## 5. The Rest

### 5.2 ObsidianPlayerController ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/Characters/Player/ObsidianPlayerController.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/Characters/Player/ObsidianPlayerController.cpp))

```ObsidianEquipmentComponent``` lives on Player Controller.