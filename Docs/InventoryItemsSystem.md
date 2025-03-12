# Obsidian Inventory Items System

Here you can find a [youtube video](https://www.youtube.com/watch?v=_QPb8MBalo4) showcasing the System.

![Inventory Showcase](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/InventoryItems/InventoryItems_Inventory.jpg)

## Usage

With this system, you can pick up Items that have a different grid sizes, store it in Grid Inventory, add stacks to items, take out and distribute stacks and limit the Item in the Inventory to some amount of stacks.

Key Binds / Navigation:
- **LMB** on Item on the ground (with closed Inventory) - Automatically adds the item to the first available slot in the Inventory,
- **LMB** on Item on the ground (with opened Inventory) - Grabs the Item onto the Player's cursor,
- **LMB + Left CTRL** on Item on the ground (with opened Inventory) - Automatically adds the item to the first available slot in the Inventory,
- **LMB** on the Inventory Slot (with Item on the cursor) - If the item fits, and is not over stacks limit it will be added at pressed location,
- **Shift + LBM** on Inventory Slot/Inventory Item (with Item on the cursor) - If Item is stackable and (in the case of clicking on another item) clicked Item is "the same" Item, 1 stack will be added to the Item in the Inventory,
- **Shift + LMB** on Item in the Inventory (without Item on the cursor) - If the clicked Item is stackable and contains more than 1 stack, window will show that will give the ability to take some amout of stacks from Item,
- **LMB** on Item in the Inventory (without Item on the cursor) - Grabs the Item onto the Player's cursor,
- **LMB** on Item in the Inventory (with Item on the cursor) - If carried Item fits in the clicked Item's location, carried Item will replace the Item in the Inventory.


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
> &nbsp; 2.4 [ObsidianItemWorldName](#obsidianitemworldname) \
> &nbsp; 2.5 [ObsidianItemDescription](#obsidianitemdesc) \
> &nbsp; 2.6 [ObsidianDraggedItem](#obsidiandraggeditem)
> 3. [Gameplay](#gameplay) \
> &nbsp; 3.1 [ObsidianDroppableItem](#droppableitem)
> 4. [Debugging](#debugging) \
> &nbsp; 4.1 [GameplayDebuggerCategory_InventoryItems](#gdc_inventoryitems)
> 5. [The Rest](#rest) \
> &nbsp; 5.1 [ObsidianHeroComponent](#obsidianherocomponent) \
> &nbsp; 5.2 [ObsidianPlayerController](#obsidianplayercontroller) \
> &nbsp; 5.3 [ObsidianMainOverlay](#obsidianmainoverlay) \
> &nbsp; 5.4 [ObsidianPickableInterface](#obsidianpickableinterface)


<a name="backend"></a>
## 1. Backend

<a name="obsidianinventorycomp"></a>
### 1.1 ObsidianInventoryComponent

Heart of the backend. Provides all the functionality to interact with the Grid, adding Items to first available slot, adding Items to specific slot, adding Stacks, removing Stacks etc.

Repo Links:
- [ObsidianInventoryComponent.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/Inventory/ObsidianInventoryComponent.h)
- [ObsidianInventoryComponent.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/Inventory/ObsidianInventoryComponent.cpp)

<a name="obsidianinventorygrid"></a>
### 1.2 ObsidianInventoryGrid

The ```FFastArraySerializer``` structure that holds Iventory Items. It also holds the Map which maps Items in the inventory to their origin location of the Grid.

Repo Links:
- [ObsidianInventoryGrid.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/Inventory/ObsidianInventoryGrid.h)
- [ObsidianInventoryGrid.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/Inventory/ObsidianInventoryGrid.cpp)

<a name="obsidianinventoryitemdef"></a>
### 1.3 ObsidianInventoryItemDefinition

Constant config of the Item, can contain various ```UObsidianInventoryItemFragment```'s that extend its functionality.

![Item Definition](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/InventoryItems/itemdefinition.jpg)

Repo Links:
- [ObsidianInventoryItemDefinition.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ObsidianInventoryItemDefinition.h)
- [ObsidianInventoryItemDefinition.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ObsidianInventoryItemDefinition.cpp)

<a name="obsidianinventoryiteminstance"></a>
### 1.4 ObsidianInventoryItemInstance

Instance of an ```ObsidianInventoryItemDefinition```. When a Player obtains an item, this is the actual thing that they receive.

Repo Links:
- [ObsidianInventoryItemInstance.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ObsidianInventoryItemInstance.h)
- [ObsidianInventoryItemInstance.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ObsidianInventoryItemInstance.cpp)

<a name="obsidianinventoryitemfragment"></a>
### 1.5 ObsidianInventoryItemFragment

Represents a fragment of an ```ObsidianInventoryItemDefinition```, can be treated like a component to an item.

Repo Links:
- [ObsidianInventoryItemFragment.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ObsidianInventoryItemFragment.h)
- [ObsidianInventoryItemFragment.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ObsidianInventoryItemFragment.cpp)

<a name="obsidianfragment_stacks"></a>
#### 1.5.1 OInventoryItemFragment_Stacks

Component for Items that provided Stacks, this includes: Current Stack Count, Max Stack Count, Inventory Limit Stack Count.

![Item Stacks Fragment](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/InventoryItems/stacksfrag.jpg)

Repo Links:
- [OInventoryItemFragment_Stacks.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/Fragments/OInventoryItemFragment_Stacks.h)
- [OInventoryItemFragment_Stacks.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/Fragments/OInventoryItemFragment_Stacks.cpp)

<a name="obsidiangameplaytagstack"></a>
#### 1.5.1.1 ObsidianGameplayTagStack

The ```FFastArraySerializer``` structure that holds stack counts and mappings to coresponding Gameplay Tag.

Repo Links:
- [ObsidianGameplayTagStack.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ObsidianGameplayTagStack.h)
- [ObsidianGameplayTagStack.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ObsidianGameplayTagStack.cpp)

<a name="obsidianframgent_appearance"></a>
#### 1.5.2 OInventoryItemFragment_Appearance

Component for Items that provides visual aspects like Static Mesh (for displaying on the ground), Skeletal Mesh (for character to wear/hold), Image etc. 

![Item Stacks Appearance](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/InventoryItems/appearancefrag.jpg)

Repo Links:
- [OInventoryItemFragment_Appearance.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/Fragments/OInventoryItemFragment_Appearance.h)
- [OInventoryItemFragment_Appearance.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/Fragments/OInventoryItemFragment_Appearance.cpp)

<a name="ui"></a>
## 2. User Interface

<a name="obsidianinventorywidgetcontroller"></a>
### 2.1 ObsidianInventoryWidgetController

Heart of the UI. Handles any data provided by ```ObsidianInventoryComponent``` and broadcasts it to the Player's UI.
It also handles any requests like adding items/removing item to and from Inventory and pushes it further to ```ObsidianInventoryComponent```.

Repo Links:
- [ObsidianInventoryWidgetController.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/WidgetControllers/ObsidianInventoryWidgetController.h)
- [ObsidianInventoryWidgetController.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/WidgetControllers/ObsidianInventoryWidgetController.cpp)

<a name="obsidianinventory"></a>
### 2.2 ObsidianInventory

Inventory Widget that holds the Grid, in the future it will also contain Equipment slots.

![Inventory Widget](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/InventoryItems/inventorywidget.jpg)

Repo Links:
- [ObsidianInventory.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/Inventory/ObsidianInventory.h)
- [ObsidianInventory.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/Inventory/ObsidianInventory.cpp)

<a name="obsidianinventoryslot"></a>
#### 2.2.1 ObsidianInventorySlot

Individual Slot on the Inventory Grid displayed in ```ObsidianInventory```.

Repo Links:
- [ObsidianInventorySlot.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/Inventory/SubWidgets/ObsidianInventorySlot.h)
- [ObsidianInventorySlot.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/Inventory/SubWidgets/ObsidianInventorySlot.cpp)

<a name="obsidianitem"></a>
### 2.3 ObsidianItem

Visual representation of an Inventory Item on the Inventory Grid.

![Item Widget](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/InventoryItems/inventoryitem.jpg)

Repo Links:
- [ObsidianItem.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/Inventory/ObsidianItem.h)
- [ObsidianItem.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/Inventory/ObsidianItem.cpp)

<a name="obsidianitemworldname"></a>
### 2.4 ObsidianItemWorldName

Item's name above its Mesh in the World added to ```ObsidianDroppableItem```. This needs some work as just adding the Widget to the Widget Component
and displaying it in the screen space isn't enough.

![Item World Name](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/InventoryItems/itemworldname.jpg)

Repo Links:
- [ObsidianItemWorldName.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/Inventory/ObsidianItemWorldName.h)
- [ObsidianItemWorldName.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/Inventory/ObsidianItemWorldName.cpp)

<a name="obsidianitemdesc"></a>
### 2.4 ObsidianItemDescriptionBase

While hovering over the ```ObsidianItemWorldName```, ```ObsidianDroppableItem``` or the ```ObsidianItem``` in the Inventory, the ```ObsidianItemDescription``` is displayed.
It contains information such as: Item Name, Item Desc, Hints, Stack Sizes in case of stackable item and in the feature it will display Item Stats.

![Item Desc](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/InventoryItems/itemdesc.jpg)

Repo Links:
- [ObsidianItemDescriptionBase.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/Inventory/ObsidianItemDescriptionBase.h)
- [ObsidianItemDescriptionBase.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/Inventory/ObsidianItemDescriptionBase.cpp)

<a name="obsidiandraggeditem"></a>
### 2.5 ObsidianDraggedItem

Representation of Item that is being carried around on the Player's cursor.

![Dragged Item](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/InventoryItems/draggeditem.jpg)

Repo Links:
- [ObsidianDraggedItem.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/Inventory/ObsidianDraggedItem.h)
- [ObsidianDraggedItem.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/Inventory/ObsidianDraggedItem.cpp)

<a name="gameplay"></a>
## 3. Gameplay

<a name="droppableitem"></a>
### 3.1 AObsidianDroppableItem

Item that lays on the ground, can contain ```ObsidianInventoryItemDefinition``` or ```ObsidianInventoryItemInstance```. 

![Dropped Item](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/InventoryItems/droppeditem.jpg)

Repo Links:
- [ObsidianDroppableItem.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/Gameplay/InventoryItems/ObsidianDroppableItem.h)
- [ObsidianDroppableItem.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/Gameplay/InventoryItems/ObsidianDroppableItem.cpp)

<a name="debugging"></a>
## 4. Debugging

<a name="gdc_inventoryitems"></a>
### 4.1 GameplayDebuggerCategory_InventoryItems

Gameplay Debugger Category that contains number of items owned and a list of held items with name, size, stacks and location on the grid.
In the feature I plan to paint the ```InventoryStateMap``` to represent the actual state of the Grid. 

![Inventory System Debugging](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/InventoryItems/InventoryItems_Debugger.jpg)

Repo Links:
- [GameplayDebuggerCategory_InventoryItems.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/Debugging/GameplayDebuggerCategory_InventoryItems.h)
- [GameplayDebuggerCategory_InventoryItems.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/Debugging/GameplayDebuggerCategory_InventoryItems.cpp)

<a name="rest"></a>
## 5. The Rest

<a name="obsidianherocomponent"></a>
### 5.1 ObsidianHeroComponent

Used for dragging logic and Server RPC's to pickup items.

Repo Links:
- [ObsidianHeroComponent.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/CharacterComponents/ObsidianHeroComponent.h)
- [ObsidianHeroComponent.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/CharacterComponents/ObsidianHeroComponent.cpp)

<a name="obsidianplayercontroller"></a>
### 5.2 ObsidianPlayerController

```ObsidianInventoryComponent``` is created on Player Controller.

Repo Links:
- [ObsidianPlayerController.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/Characters/Player/ObsidianPlayerController.h)
- [ObsidianPlayerController.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/Characters/Player/ObsidianPlayerController.cpp)

<a name="obsidianmainoverlay"></a>
### 5.3 ObsidianMainOverlay

Handles creating ```ObsidianInventory``` Widget.

Repo Links:
- [ObsidianMainOverlay.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/MainOverlay/ObsidianMainOverlay.h)
- [ObsidianMainOverlay.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/MainOverlay/ObsidianMainOverlay.cpp)

<a name="obsidianpickableinterface"></a>
### 5.4 ObsidianPickableInterface

Interface that gives the functionality to hold ```PickupContent``` which can store ```ObsidianInventoryItemDefinition``` or ```ObsidianInventoryItemInstance```.
It also provides functions to interact with held ```PickupConent```.

Repo Links:
- [ObsidianPickableInterface.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ObsidianPickableInterface.h)
- [ObsidianPickableInterface.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ObsidianPickableInterface.cpp)




