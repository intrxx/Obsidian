# Obsidian Project

The idea behind this project is to create an extendable platform for building an ARPG game.
By "platform," I mean designing and implementing various systems common to any ARPG title,
which can then be used as a foundation to build upon. This project also serves as a learning
endeavor for me, allowing me to explore and develop my skills while working and thinking
about my beloved game genre.

The end goal is to take the fundation and create my own game on it which I've already started designing.

## What have I done so far?

As of now, I've already developed some fundamental systems and have slowly started to
build some gameplay content on top of them.

### These includs:
- **Grid Inventory System/Inventory Items** - Grid Inventory System built using ```FFastArraySerializer``` to efficiently store and replicate items.
  Supports items of various sizes, stacks, limits, item descriptions, and networking. </br>
  *A more detailed presentation can be found [here](https://github.com/intrxx/Obsidian/blob/main/Docs/InventoryItemsSystem.md).*

- **Equipment System** - Closly related to **Grid Inventory System** and **Inventory Items**, also using ```FFastArraySerializer``` to store Equipment Items,
  this System provides a way to equip Items in various Equipment Slots, check for equiping conditions, spawn Mesh Actors on owning Player and give Ability Sets (which can include Gameplay Effects and Gameplay Abilities). </br>
  *A more detailed presentation can be found [here](https://github.com/intrxx/Obsidian/blob/main/Docs/EquipmentSystem.md).*

- **Player Stash System** - Just like two previous systems Player Stash Component is also built on ```FFastArraySerializer```.
  It supports configurable Stash Tabs with customizable sizes and capacities, such as Grid Stash Tabs and Slot Stash Tabs.
  This system combines and extends concepts from two previous systems to store and display Stashed Items. </br>
  *A more detailed presentation can be found [here](https://github.com/intrxx/Obsidian/blob/main/Docs/PlayerStashSystem.md).*

- **Buff/Debuff display on Health Bars and Player's HUD** - Gameplay Effects mapped to their descriptions (name, description, type, etc.) 
using Gameplay Tags, which are displayed on the player's HUD. </br>
 *A more detailed presentation can be found [here](https://github.com/intrxx/Obsidian/blob/main/Docs/EffectsUIInfoSystem.md).*
    - **Player/Enemy Health Bars**

- **Damage Numbers** - A Damage Number System with functionality to collect data on damage dealt and adjust the displayed number accordingly - for example,
 changing the color to yellow for critical hits. </br>
*A more detailed presentation can be found [here](https://github.com/intrxx/Obsidian/blob/main/Docs/DamageNumberSystem.md).* 

- **Gameplay Attributes** - A setup for Gameplay Abilities with corresponding components to broadcast and/or handle changes. This setup also includes various ```UGameplayModMagnitudeCalculation```'s.
 An example use case for these calculations is increasing Health when the Strength attribute is increased. </br>
  *A more detailed presentation can be found [here](https://github.com/intrxx/Obsidian/blob/main/Docs/Attributes.md) (TODO).* 

- **Extensive Damage Calculations** - Extensive Damage Calculation classes that includes various damage mitigations,
 damage amplifiers, critical strike and dodge calculations ect. </br>
*A more detailed presentation can be found [here](https://github.com/intrxx/Obsidian/blob/main/Docs/DamageCalculation.md) (TODO).*

- **Advanced Combat Component** (Extended Combat Manager from AGR PRO) - This is an extension of AGR PRO Combat Manager
 that builds upon the functionality to perform various traces. The implemented improvments concerns the usage of this Manager,
 users are able to choose from mapped meshes in anim notifies which heavly alleviates errors in setup and enables users to trace different meshes in one Anim Montage. </br>
 *A more detailed presentation can be found [here](https://github.com/intrxx/Obsidian/blob/main/Docs/AdvancedCombatComponent.md) (TODO).*

- **Enemies** (Melee, Ranged, Mage, Melee Boss with two phases) - "One from each" enemy implementations.
 I've used Blackboard/Behavioural Trees, custom Decorators, Services, Tasks and EQS'es and integrated Gameplay Ability System with it. </br>
*A more detailed presentation can be found [here](https://github.com/intrxx/Obsidian/blob/main/Docs/Enemies.md) (TODO).*

- **Gameplay Abilities for Enemies and Hero** - Various Gameplay Abilities. </br>
*A more detailed presentation can be found [here](https://github.com/intrxx/Obsidian/blob/main/Docs/Abilities_Root.md) (TODO).*

## What am I planning to implement?

- **Item stats**

- **Character Passive Skill Tree**

- **PoE like Abilities in form of items**

    - **Exensions for Abilities**

- **In-Editor proceduraly generated Levels**

- **Various Mage, Warrior and Assassin Abilities**

## Can I download and build the project?

Although the whole code is available on the github repo, there are a lot of ignored assets referenced
in the project so it will probably be difficult to get the project to compile.
I am planning to create a "pure" version of the project so anyone can download and build it.

## Can I contribute?

I would be more than happy to accept any contributions, but the above mentioned difficulties in building
the project would need solving first.
