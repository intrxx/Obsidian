# Obsidian Inventory Items System

Here you can find a [youtube video](https://www.youtube.com/watch?v=_QPb8MBalo4) showcasing the System.

You can also find a testable packaged game [here](https://github.com/intrxx/Obsidian/releases/tag/Grid_Inventory_System_v0.1.0).

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
> &nbsp;&nbsp; 2.2.1 [ObsidianItemSlot_Inventory](#obsidianinventoryslot) \
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
### 1.1 ObsidianInventoryComponent ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/Inventory/ObsidianInventoryComponent.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/Inventory/ObsidianInventoryComponent.cpp))

Heart of the backend. Provides all the functionality to interact with the Grid, adding Items to first available slot, adding Items to specific slot, adding Stacks, removing Stacks etc.

<a name="obsidianinventorygrid"></a>
### 1.2 ObsidianInventoryGrid ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/Inventory/ObsidianInventoryGridItemList.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/Inventory/ObsidianInventoryGridItemList.cpp))

The ```FFastArraySerializer``` structure that holds Iventory Items. It also holds the Map which maps Items in the inventory to their origin location of the Grid.

<a name="obsidianinventoryitemdef"></a>
### 1.3 ObsidianInventoryItemDefinition ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ObsidianInventoryItemDefinition.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ObsidianInventoryItemDefinition.cpp))

Constant config of the Item, can contain various ```UObsidianInventoryItemFragment```'s that extend its functionality.

![Item Definition](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/InventoryItems/itemdefinition.jpg)

<a name="obsidianinventoryiteminstance"></a>
### 1.4 ObsidianInventoryItemInstance ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ObsidianInventoryItemInstance.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ObsidianInventoryItemInstance.cpp))

Instance of an ```ObsidianInventoryItemDefinition```. When a Player obtains an item, this is the actual thing that they receive.

<a name="obsidianinventoryitemfragment"></a>
### 1.5 ObsidianInventoryItemFragment ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ObsidianInventoryItemFragment.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ObsidianInventoryItemFragment.cpp))

Represents a fragment of an ```ObsidianInventoryItemDefinition```, can be treated like a component to an item.

<a name="obsidianfragment_stacks"></a>
#### 1.5.1 OInventoryItemFragment_Stacks ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/Fragments/OInventoryItemFragment_Stacks.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/Fragments/OInventoryItemFragment_Stacks.cpp))

Component for Items that provided Stacks, this includes: Current Stack Count, Max Stack Count, Inventory Limit Stack Count.

![Item Stacks Fragment](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/InventoryItems/stacksfrag.jpg)

<a name="obsidiangameplaytagstack"></a>
#### 1.5.1.1 ObsidianGameplayTagStack ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ObsidianGameplayTagStack.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ObsidianGameplayTagStack.cpp))

The ```FFastArraySerializer``` structure that holds stack counts and mappings to coresponding Gameplay Tag.

<a name="obsidianframgent_appearance"></a>
#### 1.5.2 OInventoryItemFragment_Appearance ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/Fragments/OInventoryItemFragment_Appearance.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/Fragments/OInventoryItemFragment_Appearance.cpp))

Component for Items that provides visual aspects like Static Mesh (for displaying on the ground), Skeletal Mesh (for character to wear/hold), Image etc. 

![Item Stacks Appearance](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/InventoryItems/appearancefrag.jpg)

<a name="ui"></a>
## 2. User Interface

<a name="obsidianinventorywidgetcontroller"></a>
### 2.1 ObsidianInventoryWidgetController ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/WidgetControllers/ObsidianInventoryWidgetController.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/WidgetControllers/ObsidianInventoryWidgetController.cpp))

Heart of the UI. Handles any data provided by ```ObsidianInventoryComponent``` and broadcasts it to the Player's UI.
It also handles any requests like adding items/removing item to and from Inventory and pushes it further to ```ObsidianInventoryComponent```.

<a name="obsidianinventory"></a>
### 2.2 ObsidianInventory ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/Inventory/ObsidianInventory.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/Inventory/ObsidianInventory.cpp))

Inventory Widget that holds the Grid, in the future it will also contain Equipment slots.

![Inventory Widget](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/InventoryItems/inventorywidget.jpg)

<a name="obsidianinventoryslot"></a>
#### 2.2.1 ObsidianItemSlot_Inventory ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/Inventory/SubWidgets/ObsidianItemSlot_Inventory.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/Inventory/SubWidgets/ObsidianItemSlot_Inventory.cpp))

Individual Slot on the Inventory Grid displayed in ```ObsidianInventory```.

<a name="obsidianitem"></a>
### 2.3 ObsidianItem ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/Inventory/ObsidianItem.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/Inventory/ObsidianItem.cpp))

Visual representation of an Inventory Item on the Inventory Grid.

![Item Widget](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/InventoryItems/inventoryitem.jpg)

<a name="obsidianitemworldname"></a>
### 2.4 ObsidianItemWorldName ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/Inventory/ObsidianItemWorldName.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/Inventory/ObsidianItemWorldName.cpp))

Item's name above its Mesh in the World added to ```ObsidianDroppableItem```. This needs some work as just adding the Widget to the Widget Component
and displaying it in the screen space isn't enough.

![Item World Name](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/InventoryItems/itemworldname.jpg)

<a name="obsidianitemdesc"></a>
### 2.4 ObsidianItemDescriptionBase ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/Inventory/ObsidianItemDescriptionBase.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/Inventory/ObsidianItemDescriptionBase.cpp))

While hovering over the ```ObsidianItemWorldName```, ```ObsidianDroppableItem``` or the ```ObsidianItem``` in the Inventory, the ```ObsidianItemDescription``` is displayed.
It contains information such as: Item Name, Item Desc, Hints, Stack Sizes in case of stackable item and in the feature it will display Item Stats.

![Item Desc](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/InventoryItems/itemdesc.jpg)

<a name="obsidiandraggeditem"></a>
### 2.5 ObsidianDraggedItem ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/Inventory/ObsidianDraggedItem.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/Inventory/ObsidianDraggedItem.cpp))

Representation of Item that is being carried around on the Player's cursor.

![Dragged Item](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/InventoryItems/draggeditem.jpg)

<a name="gameplay"></a>
## 3. Gameplay

<a name="droppableitem"></a>
### 3.1 AObsidianDroppableItem ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/Gameplay/InventoryItems/ObsidianDroppableItem.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/Gameplay/InventoryItems/ObsidianDroppableItem.cpp))

Item that lays on the ground, can contain ```ObsidianInventoryItemDefinition``` or ```ObsidianInventoryItemInstance```. 

![Dropped Item](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/InventoryItems/droppeditem.jpg)

<a name="debugging"></a>
## 4. Debugging

<a name="gdc_inventoryitems"></a>
### 4.1 GameplayDebuggerCategory_InventoryItems ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/Debugging/GameplayDebuggerCategory_InventoryItems.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/Debugging/GameplayDebuggerCategory_InventoryItems.cpp))

Gameplay Debugger Category that contains number of items owned and a list of held items with name, size, stacks and location on the grid.
In the feature I plan to paint the ```InventoryStateMap``` to represent the actual state of the Grid. 

![Inventory System Debugging](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/InventoryItems/InventoryItems_Debugger.jpg)

<a name="rest"></a>
## 5. The Rest

<a name="obsidianherocomponent"></a>
### 5.1 ObsidianHeroComponent ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/CharacterComponents/ObsidianHeroComponent.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/CharacterComponents/ObsidianHeroComponent.cpp))

Used for dragging logic and Server RPC's to pickup items.

<a name="obsidianplayercontroller"></a>
### 5.2 ObsidianPlayerController ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/Characters/Player/ObsidianPlayerController.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/Characters/Player/ObsidianPlayerController.cpp))

```ObsidianInventoryComponent``` is created on Player Controller.

<a name="obsidianmainoverlay"></a>
### 5.3 ObsidianMainOverlay ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/MainOverlay/ObsidianMainOverlay.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/MainOverlay/ObsidianMainOverlay.cpp))

Handles creating ```ObsidianInventory``` Widget.

<a name="obsidianpickableinterface"></a>
### 5.4 ObsidianPickableInterface ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/InventoryItems/ObsidianPickableInterface.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/InventoryItems/ObsidianPickableInterface.cpp))

Interface that gives the functionality to hold ```PickupContent``` which can store ```ObsidianInventoryItemDefinition``` or ```ObsidianInventoryItemInstance```.
It also provides functions to interact with held ```PickupConent```.
