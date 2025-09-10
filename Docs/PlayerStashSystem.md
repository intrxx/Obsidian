# Obsidian Player Stash System

Here you can find a [youtube video](https://www.youtube.com/watch?v=jK_RvB3hMKY) showcasing the System.

You can also find a testable packaged game [here](https://github.com/intrxx/Obsidian/releases/tag/PlayerStashSystem_0.2.0).

*This system is closly coupled with [Inventory](https://github.com/intrxx/Obsidian/blob/main/Docs/InventoryItemsSystem.md) and [Equipment](https://github.com/intrxx/Obsidian/blob/main/Docs/EquipmentSystem.md) Systems, press on the links to learn more.*

![Player Stash Showcase](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/PlayerStash/PlayerStashShowcase.jpg)

<a name="table-of-contents"></a>
## Class Structure Overview

> 1. [Backend](#1-backend) \
> &nbsp; 1.1 [ObsidianPlayerStashComponent](#11-obsidianplayerstashcomponent-h--cpp) \
> &nbsp; 1.2 [ObsidianStashItemList](#12-obsidianstashitemlist-h--cpp) \
> &nbsp; 1.3 [ObsidianStashTabsConfig](#13-obsidianstashtabsconfig-h--cpp) \
> &nbsp; 1.4 [ObsidianStashTab](#14-obsidianstashtab-h--cpp) \
> &nbsp;&nbsp; 1.4.1 [ObsidianStashTab_Slots](#141-obsidianstashtab_slots-h--cpp) \
> &nbsp;&nbsp; 1.4.2 [ObsidianStashTab_Grid](#142-obsidianstashtab_grid-h--cpp)
> 2. [User Interface](#2-user-interface) \
> &nbsp; 2.1 [ObsidianPlayerStashWidget](#21-obsidianPlayerstashwidget-h--cpp) \
> &nbsp; 2.2 [ObsidianStashTabWidget](#22-obsidianstashtabwidget-h--cpp) \
> &nbsp;&nbsp; 2.2.1 [ObsidianStashTabWidget_Slots](#221-obsidianstashtabwidget_slots-h--cpp) \
> &nbsp;&nbsp; 2.2.2 [ObsidianStashTabWidget_Grid](#222-obsidianstashtabwidget_grid-h--cpp) \
> &nbsp; 2.3 [ObsidianStashButton](#23-obsidianstashbutton-h--cpp) 
> 3. [Gameplay](#3-gameplay) \
> &nbsp; 3.1 [ObsidianPlayerStash](#31-obsidianplayerstash-h--cpp) 
> 4. [Debugging](#4-debugging) \
> &nbsp; 4.1 [GameplayDebuggerCategory_PlayerStash](#41-gameplaydebuggercategory_playerstash-h--cpp)
> 5. [The Rest](#5-the-rest) \
> &nbsp; 5.1 [ObsidianPlayerController](#52-obsidianplayercontroller-h--cpp)

## 1. Backend

### 1.1 ObsidianPlayerStashComponent ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/PlayerStash/ObsidianPlayerStashComponent.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/PlayerStash/ObsidianPlayerStashComponent.cpp))

Heart of the backend. Provides all the functionality to interact with player Stash, placing items, taking stacks, finding appropriate place for items etc.

### 1.2 ObsidianStashItemList ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/PlayerStash/ObsidianStashItemList.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/PlayerStash/ObsidianStashItemList.cpp))

The ```FFastArraySerializer``` structure that holds Stashed Items. It also hold constructed stash tabs with item maps for faster lookups.

### 1.3 ObsidianStashTabsConfig ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/PlayerStash/ObsidianStashTabsConfig.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/PlayerStash/ObsidianStashTabsConfig.cpp))

Configuration of Stash Tabs in the form of Data Asset that can be asinged on the Player Stash Components, stash tabs are constructed from this config, it can mage it size (Grid Stash Tab) or set up individual item slots (Slots Stash Tab).

![ObsidianStashTabsConfig](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/PlayerStash/stashtabconfig.jpg)

### 1.4 ObsidianStashTab ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/PlayerStash/ObsidianStashTab.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/PlayerStash/ObsidianStashTab.cpp))

Parent class for all stash tabs, provides base functionallity and virtual functions that child Stash Tabs need to override to mark/unmark space in the stash tab or place/take items.

#### 1.4.1 ObsidianStashTab_Slots ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/PlayerStash/Tabs/ObsidianStashTab_Slots.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/PlayerStash/Tabs/ObsidianStashTab_Slots.cpp))

Slots Stash Tab that has the ability to contain slots for certain items and these items only (like Path of Exile's Currency Stash Tab).

#### 1.4.2 ObsidianStashTab_Grid ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/PlayerStash/Tabs/ObsidianStashTab_Grid.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/PlayerStash/Tabs/ObsidianStashTab_Grid.cpp))

Grid Stash Tab, regular Grid (just like Inventory) for storing items with various sizes and types.

## 2. User Interface

### 2.1 ObsidianPlayerStashWidget ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/Inventory/ObsidianPlayerStashWidget.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/Inventory/ObsidianPlayerStashWidget.cpp))

Manages initialization and provides recived data further to partictular stash tabs.

#### 2.2 ObsidianStashTabWidget ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/Inventory/Stash/ObsidianStashTabWidget.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/Inventory/Stash/ObsidianStashTabWidget.cpp))

Base class for Stash Tab Widgets in Obsidian, manages base functinality like showing/hiding stash tabs.

#### 2.2.1 ObsidianStashTabWidget_Slots ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/Inventory/Stash/ObsidianStashTabWidget_Slots.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/Inventory/Stash/ObsidianStashTabWidget_Slots.cpp))

Displayes configured slots, that can hold specific items, handles displaying the Stashed Items.

#### 2.2.2 ObsidianStashTabWidget_Grid ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/Inventory/Stash/ObsidianStashTabWidget_Grid.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/Inventory/Stash/ObsidianStashTabWidget_Grid.cpp))

Grid Stash Tab Widget, holds regular Inventory Grid on which it displays Stashed Items.

#### 2.3 ObsidianStashButton ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/Inventory/Stash/ObsidianStashButton.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/Inventory/Stash/ObsidianStashButton.cpp))

Stash Tab Button provided the functionality to switch between tabs, in the future it will support Player Side Customization.

## 3. Gameplay

### 3.1 ObsidianPlayerStash ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/PlayerStash/ObsidianPlayerStash.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/PlayerStash/ObsidianPlayerStash.cpp))

Player Stash Actor that can be placed in the world and interacted with to display's interacing Player's Stash.

![PlayerStashActor](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/PlayerStash/stashactor.jpg)

## 4. Debugging

### 4.1 GameplayDebuggerCategory_PlayerStash ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/Debugging/GameplayDebuggerCategory_PlayerStash.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/Debugging/GameplayDebuggerCategory_PlayerStash.cpp))

Gameplay Debugger Category that contains number of Items Stashed and its Name, Item Definition Class, Currently
Stashed Location and Stash Tab information. It also displayes a map of taken fields in Grid Stash Tab or needed information about every Slot in Slot Stash Tab.

Grid Stash Tab:

![Player Stash System Debugging - Grid](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/PlayerStash/gridstashdebbuger.jpg)

Slot Stash Tab:

![Player Stash System Debugging - Slot](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/PlayerStash/slotstashdebugger.jpg)

## 5. The Rest

### 5.2 ObsidianPlayerController ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/Characters/Player/ObsidianPlayerController.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/Characters/Player/ObsidianPlayerController.cpp))

```ObsidianPlayerStashComponent``` lives on Player Controller.
