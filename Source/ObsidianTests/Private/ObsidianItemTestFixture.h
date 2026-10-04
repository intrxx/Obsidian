// Copyright 2026 out of sCope team - intrxx

#pragma once

#include <CoreMinimal.h>

#if WITH_DEV_AUTOMATION_TESTS

#include <Components/ActorTestSpawner.h>
#include <GameplayTagContainer.h>

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

class AObsidianHero;
class AObsidianPlayerController;
class AObsidianPlayerState;
class UObsidianEquipmentComponent;
class UObsidianInventoryComponent;
class UObsidianInventoryItemDefinition;
class UObsidianInventoryItemInstance;
class UObsidianPlayerStashComponent;

/**
 * Everything the Inventory, Equipment and Player Stash need to work, created in a transient game world:
 * a Player Controller that owns the three components, a Player State with the Ability System Component and a Hero.
 *
 * The world has authority (it is standalone), so the components behave the same way they do on the server.
 * A new environment should be created for every test, so the tests never share state.
 */
struct FObsidianItemTestEnvironment
{
	FObsidianItemTestEnvironment();

	UObsidianInventoryComponent& Inventory() const;
	UObsidianEquipmentComponent& Equipment() const;
	UObsidianPlayerStashComponent& Stash() const;

	/** Adds Gameplay Tag to the Player's Ability System Component, e.g. to block actions on one of the components. */
	void AddOwnerTag(const FGameplayTag& Tag) const;
	void RemoveOwnerTag(const FGameplayTag& Tag) const;

	void SetHeroLevel(const uint8 HeroLevel) const;

	/** Stash Tabs that the Player Stash is configured with, both are 12x10 grids. */
	static FGameplayTag PersonalStashTab();
	static FGameplayTag SharedStashTab();
	static FObsidianItemPosition PersonalStashPosition(const int32 X, const int32 Y);
	static FObsidianItemPosition SharedStashPosition(const int32 X, const int32 Y);

	/** Generated Data of a Normal rarity item with given number of stacks and no requirements. */
	static FObsidianItemGeneratedData MakeItemData(const int32 StackCount = 1);

	/** Current stacks of an item, 0 for nullptr. */
	static int32 Stacks(const UObsidianInventoryItemInstance* Instance);

	/**
	 * Creates an Item Instance that is not held by any of the components, the same as an item that is being dragged.
	 * It is made by adding the item to the Inventory and removing it right away.
	 */
	UObsidianInventoryItemInstance* MakeHeldItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef,
		const int32 StackCount = 1) const;

	FActorTestSpawner Spawner;
	AObsidianPlayerController* PlayerController = nullptr;
	AObsidianPlayerState* PlayerState = nullptr;
	AObsidianHero* Hero = nullptr;
};

#endif // WITH_DEV_AUTOMATION_TESTS
