# Obsidian Inventory Items System

Here you can find a [youtube video](https://www.youtube.com/watch?v=_QPb8MBalo4) showcasing the System.

<a name="table-of-contents"></a>
## Class Structure Overview

> 1. [Backend](#backend) \
> &nbsp; 1.1 [ObsidianInventoryComponent](#obsidianinventorycomp) \
> &nbsp; 1.2 [ObsidianInventoryGrid](#obsidianinventorygrid) \
> &nbsp; 1.3 [ObsidianInventoryItemDefinition](#obsidianinventoryitemdef) \
> &nbsp; 1.4 [ObsidianInventoryItemInstance](#obsidianinventoryiteminstance) \
> &nbsp; 1.5 [ObsidianInventoryItemFragment](#obsidianinventoryitemfragment) \
> &nbsp;&nbsp; 1.5.1 [OInventoryItemFragment_Stacks](#obsidianfragment_stacks) \
> &nbsp;&nbsp;&nbsp; 1.5.1.1 [ObsidianGameplayTagStack](#obsidiangameplaytagstack) \
> &nbsp;&nbsp; 1.5.2 [OInventoryItemFragment_Appearance](#obsidianframgent_appearance)
> 2. [User Interface](#ui) \
> &nbsp; 2.1 [ObsidianInventoryWidgetController](#obsidianinventorywidgetcontroller) \
> &nbsp; 2.2 [ObsidianInventory](#obsidianinventory) \
> &nbsp;&nbsp; 2.2.1 [ObsidianInventorySlot](#obsidianinventoryslot) \
> &nbsp; 2.3 [ObsidianItem](#obsidianitem) \
> &nbsp; 2.4 [ObsidianGroundItemDesc](#obsidiangrounditemdesc) \
> &nbsp; 2.5 [ObsidianDraggedItem](#obsidiandraggeditem)
> 3. [The Rest](#rest) \
> &nbsp; 3.1 [ObsidianHeroComponent](#obsidianherocomponent) \
> &nbsp; 3.2 [ObsidianPlayerController](#obsidianplayercontroller) \
> &nbsp; 3.3 [ObsidianMainOverlay](#obsidianmainoverlay) \
> &nbsp; 3.4 [ObsidianPickableInterface](#obsidianpickableinterface)


<a name="backend"></a>
## 1. Backend

<a name="obsidianinventorycomp"></a>
### 1.1 ObsidianInventoryComponent

Heart of the backend.

Repo Links:
- [ObsidianInventoryComponent.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ObsidianInventoryComponent.h)
- [ObsidianInventoryComponent.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ObsidianInventoryComponent.cpp)

<a name="obsidianinventorygrid"></a>
### 1.2 ObsidianInventoryGrid

Repo Links:
- [ObsidianInventoryGrid.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ObsidianInventoryGrid.h)
- [ObsidianInventoryGrid.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ObsidianInventoryGrid.cpp)

<a name="obsidianinventoryitemdef"></a>
### 1.3 ObsidianInventoryItemDefinition

Repo Links:
- [ObsidianInventoryItemDefinition.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ObsidianInventoryItemDefinition.h)
- [ObsidianInventoryItemDefinition.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ObsidianInventoryItemDefinition.cpp)

<a name="obsidianinventoryiteminstance"></a>
### 1.4 ObsidianInventoryItemInstance

Repo Links:
- [ObsidianInventoryItemInstance.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ObsidianInventoryItemInstance.h)
- [ObsidianInventoryItemInstance.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ObsidianInventoryItemInstance.cpp)

<a name="obsidianinventoryitemfragment"></a>
### 1.5 ObsidianInventoryItemFragment

Repo Links:
- [ObsidianInventoryItemFragment.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ObsidianInventoryItemFragment.h)
- [ObsidianInventoryItemFragment.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ObsidianInventoryItemFragment.cpp)

<a name="obsidianfragment_stacks"></a>
#### 1.5.1 OInventoryItemFragment_Stacks

Repo Links:
- [OInventoryItemFragment_Stacks.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/Fragments/OInventoryItemFragment_Stacks.h)
- [OInventoryItemFragment_Stacks.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/Fragments/OInventoryItemFragment_Stacks.cpp)

<a name="obsidiangameplaytagstack"></a>
#### 1.5.1.1 ObsidianGameplayTagStack

Repo Links:
- [ObsidianGameplayTagStack.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/Core/ObsidianGameplayTagStack.h)
- [ObsidianGameplayTagStack.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/Core/ObsidianGameplayTagStack.cpp)

<a name="obsidianframgent_appearance"></a>
#### 1.5.2 OInventoryItemFragment_Appearance

Repo Links:
- [OInventoryItemFragment_Appearance.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/Fragments/OInventoryItemFragment_Appearance.h)
- [OInventoryItemFragment_Appearance.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/Fragments/OInventoryItemFragment_Appearance.cpp)

<a name="ui"></a>
## 2. User Interface

<a name="obsidianinventorywidgetcontroller"></a>
### 2.1 ObsidianInventoryWidgetController

Heart of the UI. Sort of data provider for ObsidianInventory Widget.

Repo Links:
- [ObsidianInventoryWidgetController.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/WidgetControllers/ObsidianInventoryWidgetController.h)
- [ObsidianInventoryWidgetController.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/WidgetControllers/ObsidianInventoryWidgetController.cpp)

<a name="obsidianinventory"></a>
### 2.2 ObsidianInventory

Repo Links:
- [ObsidianInventory.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/Inventory/ObsidianInventory.h)
- [ObsidianInventory.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/Inventory/ObsidianInventory.cpp)

<a name="obsidianinventoryslot"></a>
#### 2.2.1 ObsidianInventorySlot

Repo Links:
- [ObsidianInventorySlot.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/Inventory/SubWidgets/ObsidianInventorySlot.h)
- [ObsidianInventorySlot.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/Inventory/SubWidgets/ObsidianInventorySlot.cpp)

<a name="obsidianitem"></a>
### 2.3 ObsidianItem

Repo Links:
- [ObsidianItem.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/Inventory/ObsidianItem.h)
- [ObsidianItem.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/Inventory/ObsidianItem.cpp)

<a name="obsidiangrounditemdesc"></a>
### 2.4 ObsidianGroundItemDesc

Repo Links:
- [ObsidianGroundItemDesc.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/Inventory/ObsidianGroundItemDesc.h)
- [ObsidianGroundItemDesc.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/Inventory/ObsidianGroundItemDesc.cpp)

<a name="obsidiandraggeditem"></a>
### 2.5 ObsidianDraggedItem

Repo Links:
- [ObsidianDraggedItem.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/Inventory/ObsidianDraggedItem.h)
- [ObsidianDraggedItem.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/Inventory/ObsidianDraggedItem.cpp)

<a name="rest"></a>
## 3. The Rest

<a name="obsidianherocomponent"></a>
### 3.1 ObsidianHeroComponent

Used for dragging items.

Repo Links:
- [ObsidianHeroComponent.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/CharacterComponents/ObsidianHeroComponent.h)
- [ObsidianHeroComponent.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/CharacterComponents/ObsidianHeroComponent.cpp)

<a name="obsidianplayercontroller"></a>
### 3.2 ObsidianPlayerController

InventoryComponent is on PC.

Repo Links:
- [ObsidianPlayerController.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/Characters/Player/ObsidianPlayerController.h)
- [ObsidianPlayerController.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/Characters/Player/ObsidianPlayerController.cpp)

<a name="obsidianmainoverlay"></a>
### 3.3 ObsidianMainOverlay

Setup for creating ObsidianInventory is here.

Repo Links:
- [ObsidianMainOverlay.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/MainOverlay/ObsidianMainOverlay.h)
- [ObsidianMainOverlay.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/MainOverlay/ObsidianMainOverlay.cpp)

<a name="obsidianpickableinterface"></a>
### 3.4 ObsidianPickableInterface

Repo Links:
- [ObsidianPickableInterface.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ObsidianPickableInterface.h)
- [ObsidianPickableInterface.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ObsidianPickableInterface.cpp)




