// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "InventoryItems/ObsidianInventoryItemDefinition.h"

#include "ObsidianItemTestDefinitions.generated.h"

/**
 * Item Definitions used by automation tests (see ObsidianInventoryTests.cpp, ObsidianEquipmentTests.cpp and
 * ObsidianPlayerStashTests.cpp).
 *
 * Tests need Item Definitions that never change, so they are built here in code instead of relying on content.
 * They live in the ObsidianTests editor module, so they are never part of the game, HideDropdown keeps them out of
 * the editor pickers.
 */

/** Stackable 1x1 item, up to 10 stacks on a single item, no limit of stacks held by the Player. */
UCLASS(HideDropdown, NotBlueprintable)
class UObsidianTestItemDef_Stackable : public UObsidianInventoryItemDefinition
{
	GENERATED_BODY()

public:
	UObsidianTestItemDef_Stackable();

	static constexpr int32 MaxStacks = 10;
};

/** Stackable 1x1 item, up to 5 stacks on a single item, the Player can only hold 8 stacks. */
UCLASS(HideDropdown, NotBlueprintable)
class UObsidianTestItemDef_LimitedStackable : public UObsidianInventoryItemDefinition
{
	GENERATED_BODY()

public:
	UObsidianTestItemDef_LimitedStackable();

	static constexpr int32 MaxStacks = 5;
	static constexpr int32 LimitStacks = 8;
};

/** Non-stackable 2x2 item that can't be equipped. */
UCLASS(HideDropdown, NotBlueprintable)
class UObsidianTestItemDef_Large : public UObsidianInventoryItemDefinition
{
	GENERATED_BODY()

public:
	UObsidianTestItemDef_Large();
};

/** Equippable 2x2 Helmet. */
UCLASS(HideDropdown, NotBlueprintable)
class UObsidianTestItemDef_Helmet : public UObsidianInventoryItemDefinition
{
	GENERATED_BODY()

public:
	UObsidianTestItemDef_Helmet();
};

/** Equippable 1x3 One-Handed Sword. */
UCLASS(HideDropdown, NotBlueprintable)
class UObsidianTestItemDef_OneHandSword : public UObsidianInventoryItemDefinition
{
	GENERATED_BODY()

public:
	UObsidianTestItemDef_OneHandSword();
};

/** Equippable 2x4 Two-Handed Sword, needs both weapon slots. */
UCLASS(HideDropdown, NotBlueprintable)
class UObsidianTestItemDef_TwoHandSword : public UObsidianInventoryItemDefinition
{
	GENERATED_BODY()

public:
	UObsidianTestItemDef_TwoHandSword();
};

/** Equippable 2x2 Shield. */
UCLASS(HideDropdown, NotBlueprintable)
class UObsidianTestItemDef_Shield : public UObsidianInventoryItemDefinition
{
	GENERATED_BODY()

public:
	UObsidianTestItemDef_Shield();
};
