// Copyright 2026 out of sCope team - intrxx

#pragma once

#if WITH_OBSIDIAN_DEBUG_MENU

#include "Containers/Ticker.h"
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "HAL/IConsoleManager.h"
#include "Templates/SubclassOf.h"
#include "UObject/StrongObjectPtr.h"

#include "Debug/ObsidianDebugMenuTypes.h"
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

struct FObsidianItemToDrop;

class AObsidianEnemy;
class UObsidianInventoryItemDefinition;
class UObsidianItemDropComponent;

/**
 * Base class of the Debug Menu tabs.
 */
class FObsidianDebugMenuTab
{
public:
	virtual ~FObsidianDebugMenuTab() = default;

	virtual FName GetTabName() const = 0;

	/** Tabs that change gameplay state are disabled when the chosen World has no authority. */
	virtual bool RequiresAuthority() const
	{
		return true;
	}

	/** Called every frame the menu is opened, even if the tab is not the active one. */
	virtual void Tick(const FObsidianDebugMenuContext& Context, const float DeltaTime)
	{}

	/** Called every frame the tab is the active one, Context is guaranteed to have a valid World. */
	virtual void Draw(const FObsidianDebugMenuContext& Context) = 0;
};

/**
 * Core - World info, time control and level travel.
 */
class FObsidianDebugTab_Game : public FObsidianDebugMenuTab
{
public:
	virtual FName GetTabName() const override
	{
		return TEXT("Game");
	}

	virtual void Draw(const FObsidianDebugMenuContext& Context) override;

private:
	void GatherMaps();

private:
	FObsidianDebugComboBox MapComboBox;
	TArray<FString> MapPackageNames;
	TArray<FString> MapNames;
	int32 SelectedMapIndex = 0;
	bool bGatheredMaps = false;

	float SmoothedFrameTime = 0.0f;
};

/**
 * Core - resources, progression and teleporting of the chosen Player.
 */
class FObsidianDebugTab_Player : public FObsidianDebugMenuTab
{
public:
	virtual FName GetTabName() const override
	{
		return TEXT("Player");
	}

	virtual void Tick(const FObsidianDebugMenuContext& Context, const float DeltaTime) override;
	virtual void Draw(const FObsidianDebugMenuContext& Context) override;

private:
	static void RestoreResources(const FObsidianDebugMenuContext& Context);

private:
	bool bKeepResourcesFull = false;
	float ExperienceToAdd = 100.0f;
	FVector TeleportLocation = FVector::ZeroVector;
	FVector SavedLocation = FVector::ZeroVector;
	bool bHasSavedLocation = false;
};

/**
 * Core - stats, view modes, show flags and a few of the most useful rendering console variables.
 */
class FObsidianDebugTab_Rendering : public FObsidianDebugMenuTab
{
public:
	virtual FName GetTabName() const override
	{
		return TEXT("Rendering");
	}

	virtual bool RequiresAuthority() const override
	{
		return false;
	}

	virtual void Draw(const FObsidianDebugMenuContext& Context) override;
};

/**
 * Core - console command runner with history and presets.
 */
class FObsidianDebugTab_Console : public FObsidianDebugMenuTab
{
public:
	virtual FName GetTabName() const override
	{
		return TEXT("Console");
	}

	virtual bool RequiresAuthority() const override
	{
		return false;
	}

	virtual void Draw(const FObsidianDebugMenuContext& Context) override;

private:
	void RunCommand(const FObsidianDebugMenuContext& Context, const FString& Command);

private:
	FString CommandInput;
	TArray<FString> CommandHistory;
};

/**
 * Obsidian - spawning Items, either rolled the same way the enemies drop them or crafted by hand.
 */
class FObsidianDebugTab_Items : public FObsidianDebugMenuTab
{
public:
	virtual FName GetTabName() const override
	{
		return TEXT("Items");
	}

	FObsidianDebugTab_Items();
	virtual ~FObsidianDebugTab_Items() override;

	virtual void Draw(const FObsidianDebugMenuContext& Context) override;

private:
	enum class EAffixMode : uint8
	{
		/** Affixes are rolled the same way they are when the item gets dropped. */
		Generated = 0,
		/** Only the affixes picked in the menu are added. */
		HandPicked,
		/** Item is left without any affixes. */
		None
	};

	enum class EDestination : uint8
	{
		Ground = 0,
		Inventory
	};

	/** Dynamic Affix that can be hand-picked, gathered from the Common Affix Lists. */
	struct FAffixEntry
	{
		FObsidianDynamicItemAffix Affix;
		EObsidianAffixType AffixType = EObsidianAffixType::None;
	};

	void DrawRandomDrops(const FObsidianDebugMenuContext& Context);
	void DrawItemCrafting(const FObsidianDebugMenuContext& Context);
	void DrawAffixPicker(const UObsidianInventoryItemDefinition* ItemDefault);
	void DrawCurrency(const FObsidianDebugMenuContext& Context);

	void RollRandomDrops(const FObsidianDebugMenuContext& Context);
	void CraftItems(const FObsidianDebugMenuContext& Context);
	void SpawnCurrency(const FObsidianDebugMenuContext& Context);

	/** Drops the items near the Player or adds them to its Inventory, transforms of the items need to be already set. */
	void DeliverItems(const FObsidianDebugMenuContext& Context, TArray<FObsidianItemToDrop>&& Items, const FString& ItemName,
		const EDestination Destination) const;

	/** Currency items (Item.Category.Currency) have their own section, so they are not listed in the Item Crafting. */
	static bool IsCurrencyItem(const UClass* ItemDefClass);

	/** Affixes can only be hand-picked for the items that get them generated and have any hand-pickable affix that fits them. */
	bool CanHandPickAffixesForItem(const UClass* ItemDefClass) const;

	/** Lists only the items that can be crafted with the chosen Affix Mode in the Item Picker. */
	void UpdateItemPickerFilter();

	/** Everything that describes the item to craft, filled from the menu or by the self-test. */
	struct FCraftingRequest
	{
		const UObsidianInventoryItemDefinition* GetItemDefault() const;

		/** Rarity the item is going to have, with the "Item Default" option already resolved. */
		EObsidianItemRarity GetRarity() const;
		int32 GetItemLevel() const;

		TSubclassOf<UObsidianInventoryItemDefinition> ItemDef;

		/** None stands for the default rarity of the Item Definition. */
		EObsidianItemRarity ChosenRarity = EObsidianItemRarity::None;
		int32 ItemLevel = 1;
		int32 Stacks = 1;

		EAffixMode AffixMode = EAffixMode::Generated;
		TArray<FAffixEntry> PickedAffixes;
		bool bIncludeStaticAffixes = true;
		bool bApplyMagicMultiplier = false;
	};

	/** Gathers the state of the menu into the request to craft given item. */
	FCraftingRequest MakeCraftingRequest(UClass* ItemDefClass) const;

	/** Fills everything but the transform of the Item To Drop, returns false if the item could not be constructed. */
	static bool ConstructCraftedItem(UObsidianItemDropComponent* DropComponent, const FCraftingRequest& Request,
		FObsidianItemToDrop& OutItemToDrop);

	/**
	 * Crafted items obey the same rules the dropped ones do (affix limits of the rarity, affixes that fit the item, ...),
	 * on top of that item generation asserts on setups that the game never produces. Returns the reason the request
	 * breaks any of these, empty string if the item can be crafted.
	 */
	static FString GetCraftingBlocker(const FCraftingRequest& Request);

	/**
	 * Returns the reason the affix cannot be hand-picked for the requested item next to the other picked affixes,
	 * empty string if it can. Picked Affixes of the request are ignored in favor of the provided ones.
	 */
	static FString GetAffixBlocker(const FCraftingRequest& Request, const FAffixEntry& Affix, TConstArrayView<FAffixEntry> OtherPickedAffixes);

	/**
	 * Crafts every item with every rarity and affix mode on a range of item levels and verifies that the results obey
	 * the crafting rules, then rolls the Random Drops and delivers a sample of the crafted items to the World and the
	 * Inventory. Handler of the "obsidian.DebugMenu.TestItemCrafting" command.
	 */
	void RunCraftingSelfTest(const TArray<FString>& Args, UWorld* World);
	void CraftingSelfTest(const FObsidianDebugMenuContext& Context, const int32 Iterations);

	void GatherAffixes();
	void GatherItemCategories();

	/** Creates the transient Drop Component used to run the regular item generation code for the Player. */
	UObsidianItemDropComponent* CreateDropComponent(const FObsidianDebugMenuContext& Context) const;

	/** Affixes can only be rolled if they have a tier available for the level of the item. */
	static bool CanAffixRollAtItemLevel(const FObsidianDynamicItemAffix& Affix, const int32 ItemLevel);
	static bool IsAffixCompatibleWithItem(const FObsidianDynamicItemAffix& Affix, const UObsidianInventoryItemDefinition* ItemDefault);

private:
	/**
	 * Random Drops.
	 */

	FObsidianDebugComboBox EntityRarityComboBox;
	int32 EntityRarityIndex = 1;
	int32 EntityLevel = 1;
	int32 DropTimes = 1;
	float DropRadius = 150.0f;

	bool bLimitCategory = false;
	FObsidianDebugComboBox CategoryComboBox;
	TArray<FGameplayTag> ItemCategories;
	TArray<FString> ItemCategoryNames;
	int32 CategoryIndex = 0;

	/**
	 * Item Crafting.
	 */

	FObsidianDebugClassPicker ItemPicker;
	/** Strong, as nothing else references the loaded Blueprint class, so it would get garbage collected while it is still chosen. */
	TStrongObjectPtr<UClass> SelectedItemDef;

	FObsidianDebugComboBox ItemRarityComboBox;
	int32 ItemRarityIndex = 0;
	int32 ItemLevel = 1;
	int32 ItemStacks = 1;
	int32 ItemCount = 1;
	bool bApplyMagicMultiplier = false;

	FObsidianDebugComboBox AffixModeComboBox;
	int32 AffixModeIndex = 0;

	/** Affix Mode the Item Picker is currently filtered for, INDEX_NONE until it gets filtered for the first time. */
	int32 ItemPickerAffixModeIndex = INDEX_NONE;

	FObsidianDebugComboBox DestinationComboBox;
	int32 DestinationIndex = 0;

	/**
	 * Currency.
	 */

	FObsidianDebugClassPicker CurrencyPicker;
	int32 CurrencyStacks = 10;
	int32 CurrencyCount = 1;

	FObsidianDebugComboBox CurrencyDestinationComboBox;
	int32 CurrencyDestinationIndex = 1;

	/**
	 * Affix picking.
	 */

	TArray<FAffixEntry> AllAffixes;
	bool bGatheredAffixes = false;

	FObsidianDebugComboBox AffixTypeComboBox;
	int32 AffixTypeIndex = 0;
	bool bIncludeStaticAffixes = true;

	FObsidianDebugComboBox AffixComboBox;
	int32 AffixIndex = 0;

	TArray<FAffixEntry> PickedAffixes;

	FAutoConsoleCommandWithWorldAndArgs CraftingSelfTestCommand;
	FTSTicker::FDelegateHandle CraftingSelfTestTickerHandle;
};

/**
 * Obsidian - Attributes, Abilities, Effects and Tags of any Ability System Component in the World.
 */
class FObsidianDebugTab_GAS : public FObsidianDebugMenuTab
{
public:
	virtual FName GetTabName() const override
	{
		return TEXT("GAS");
	}

	virtual void Draw(const FObsidianDebugMenuContext& Context) override;

private:
	/** Draws the picker of the actor to debug, returns its Ability System Component. */
	UAbilitySystemComponent* DrawTargetPicker(const FObsidianDebugMenuContext& Context);

	void DrawAttributes(const FObsidianDebugMenuContext& Context, UAbilitySystemComponent* ASC);
	void DrawAbilities(const FObsidianDebugMenuContext& Context, UAbilitySystemComponent* ASC);
	void DrawEffects(const FObsidianDebugMenuContext& Context, UAbilitySystemComponent* ASC);
	void DrawTags(const FObsidianDebugMenuContext& Context, UAbilitySystemComponent* ASC);

	void GatherInputTags();

private:
	FObsidianDebugComboBox TargetComboBox;
	TWeakObjectPtr<AActor> TargetActor;

	/**
	 * Attributes.
	 */

	FString AttributeFilter;
	float AttributeAmount = 10.0f;
	bool bApplyAsInstantEffect = true;
	FObsidianDebugClassPicker AttributeSetPicker;

	/**
	 * Abilities.
	 */

	FObsidianDebugClassPicker AbilityPicker;
	int32 AbilityLevel = 1;

	FObsidianDebugComboBox InputTagComboBox;
	TArray<FGameplayTag> InputTags;
	TArray<FString> InputTagNames;
	int32 InputTagIndex = 0;

	/**
	 * Effects.
	 */

	FObsidianDebugClassPicker EffectPicker;
	float EffectLevel = 1.0f;

	/**
	 * Tags.
	 */

	FString TagInput;
};

/**
 * Obsidian - spawning and managing AI Characters.
 */
class FObsidianDebugTab_AI : public FObsidianDebugMenuTab
{
public:
	virtual FName GetTabName() const override
	{
		return TEXT("AI");
	}

	virtual void Tick(const FObsidianDebugMenuContext& Context, const float DeltaTime) override;
	virtual void Draw(const FObsidianDebugMenuContext& Context) override;

private:
	void DrawSpawning(const FObsidianDebugMenuContext& Context);
	void DrawSpawnedEnemies(const FObsidianDebugMenuContext& Context);

	void SpawnEnemies(const FObsidianDebugMenuContext& Context);

	/**
	 * Enemies assume to be fully set up when they get possessed, so spawning the ones that are not (like the base classes)
	 * crashes the game. Returns what is missing in the setup of the enemy, empty string if it is safe to spawn.
	 */
	static FString GetSpawnBlocker(const AObsidianEnemy* EnemyDefault);

	static void SetAILogicPaused(const AObsidianEnemy* Enemy, const bool bPaused);

private:
	FObsidianDebugClassPicker EnemyPicker;
	/** Strong, as nothing else references the loaded Blueprint class, so it would get garbage collected while it is still chosen. */
	TStrongObjectPtr<UClass> SelectedEnemyClass;
	int32 SpawnCount = 1;
	float SpawnDistance = 600.0f;
	float SpawnSpreadRadius = 200.0f;

	bool bOverrideLevel = false;
	int32 LevelOverride = 1;

	bool bOverrideRarity = false;
	FObsidianDebugComboBox RarityComboBox;
	int32 RarityIndex = 1;

	bool bFreezeAI = false;
	bool bCreditPlayerForKills = true;
};

#endif // WITH_OBSIDIAN_DEBUG_MENU
