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
class UAbilitySystemComponent;
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
	virtual bool RequiresAuthority() const { return true; }

	/** Called every frame the menu is opened, even if the tab is not the active one. */
	virtual void Tick(const FObsidianDebugMenuContext& InContext, const float InDeltaTime)
	{}

	/** Called every frame the tab is the active one, Context is guaranteed to have a valid World. */
	virtual void Draw(const FObsidianDebugMenuContext& InContext) = 0;
};

/**
 * Core - World info, time control and level travel.
 */
class FObsidianDebugTab_Game : public FObsidianDebugMenuTab
{
public:
	//~ Begin FObsidianDebugMenuTab Interface
	virtual FName GetTabName() const override
	{
		return TEXT("Game");
	}

	virtual void Draw(const FObsidianDebugMenuContext& InContext) override;
	//~ End FObsidianDebugMenuTab Interface

private:
	void GatherMaps();

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
	//~ Begin FObsidianDebugMenuTab Interface
	virtual FName GetTabName() const override
	{
		return TEXT("Player");
	}

	virtual void Tick(const FObsidianDebugMenuContext& InContext, const float InDeltaTime) override;
	virtual void Draw(const FObsidianDebugMenuContext& InContext) override;
	//~ End FObsidianDebugMenuTab Interface

private:
	static void RestoreResources(const FObsidianDebugMenuContext& InContext);

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
	//~ Begin FObsidianDebugMenuTab Interface
	virtual FName GetTabName() const override
	{
		return TEXT("Rendering");
	}

	virtual bool RequiresAuthority() const override { return false; }

	virtual void Draw(const FObsidianDebugMenuContext& InContext) override;
	//~ End FObsidianDebugMenuTab Interface
};

/**
 * Core - console command runner with history and presets.
 */
class FObsidianDebugTab_Console : public FObsidianDebugMenuTab
{
public:
	//~ Begin FObsidianDebugMenuTab Interface
	virtual FName GetTabName() const override
	{
		return TEXT("Console");
	}

	virtual bool RequiresAuthority() const override { return false; }

	virtual void Draw(const FObsidianDebugMenuContext& InContext) override;
	//~ End FObsidianDebugMenuTab Interface

private:
	void RunCommand(const FObsidianDebugMenuContext& InContext, const FString& InCommand);

	FString CommandInput;
	TArray<FString> CommandHistory;
};

/**
 * Obsidian - spawning Items, either rolled the same way the enemies drop them or crafted by hand.
 */
class FObsidianDebugTab_Items : public FObsidianDebugMenuTab
{
public:
	FObsidianDebugTab_Items();

	virtual ~FObsidianDebugTab_Items() override;

	//~ Begin FObsidianDebugMenuTab Interface
	virtual FName GetTabName() const override
	{
		return TEXT("Items");
	}

	virtual void Draw(const FObsidianDebugMenuContext& InContext) override;
	//~ End FObsidianDebugMenuTab Interface

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

	/** Currency items (Item.Category.Currency) have their own section, so they are not listed in the Item Crafting. */
	static bool IsCurrencyItem(const UClass* InItemDefClass);

	/** Fills everything but the transform of the Item To Drop, returns false if the item could not be constructed. */
	static bool ConstructCraftedItem(UObsidianItemDropComponent* InDropComponent, const FCraftingRequest& InRequest,
		FObsidianItemToDrop& OutItemToDrop);

	/**
	 * Crafted items obey the same rules the dropped ones do (affix limits of the rarity, affixes that fit the item, ...),
	 * on top of that item generation asserts on setups that the game never produces. Returns the reason the request
	 * breaks any of these, empty string if the item can be crafted.
	 */
	static FString GetCraftingBlocker(const FCraftingRequest& InRequest);

	/**
	 * Returns the reason the affix cannot be hand-picked for the requested item next to the other picked affixes,
	 * empty string if it can. Picked Affixes of the request are ignored in favor of the provided ones.
	 */
	static FString GetAffixBlocker(const FCraftingRequest& InRequest, const FAffixEntry& InAffix, TConstArrayView<FAffixEntry> InOtherPickedAffixes);

	/** Affixes can only be rolled if they have a tier available for the level of the item. */
	static bool CanAffixRollAtItemLevel(const FObsidianDynamicItemAffix& InAffix, const int32 InItemLevel);
	static bool IsAffixCompatibleWithItem(const FObsidianDynamicItemAffix& InAffix, const UObsidianInventoryItemDefinition* InItemDefault);

	void DrawRandomDrops(const FObsidianDebugMenuContext& InContext);
	void DrawItemCrafting(const FObsidianDebugMenuContext& InContext);
	void DrawAffixPicker(const UObsidianInventoryItemDefinition* InItemDefault);
	void DrawCurrency(const FObsidianDebugMenuContext& InContext);

	void RollRandomDrops(const FObsidianDebugMenuContext& InContext);
	void CraftItems(const FObsidianDebugMenuContext& InContext);
	void SpawnCurrency(const FObsidianDebugMenuContext& InContext);

	/** Drops the items near the Player or adds them to its Inventory, transforms of the items need to be already set. */
	void DeliverItems(const FObsidianDebugMenuContext& InContext, TArray<FObsidianItemToDrop>&& InItems, const FString& InItemName,
		const EDestination InDestination) const;

	/** Affixes can only be hand-picked for the items that get them generated and have any hand-pickable affix that fits them. */
	bool CanHandPickAffixesForItem(const UClass* InItemDefClass) const;

	/** Lists only the items that can be crafted with the chosen Affix Mode in the Item Picker. */
	void UpdateItemPickerFilter();

	/** Gathers the state of the menu into the request to craft given item. */
	FCraftingRequest MakeCraftingRequest(UClass* InItemDefClass) const;

	/**
	 * Crafts every item with every rarity and affix mode on a range of item levels and verifies that the results obey
	 * the crafting rules, then rolls the Random Drops and delivers a sample of the crafted items to the World and the
	 * Inventory. Handler of the "obsidian.DebugMenu.TestItemCrafting" command.
	 */
	void RunCraftingSelfTest(const TArray<FString>& InArgs, UWorld* InWorld);
	void CraftingSelfTest(const FObsidianDebugMenuContext& InContext, const int32 InIterations);

	void GatherAffixes();
	void GatherItemCategories();

	/** Creates the transient Drop Component used to run the regular item generation code for the Player. */
	UObsidianItemDropComponent* CreateDropComponent(const FObsidianDebugMenuContext& InContext) const;

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
	//~ Begin FObsidianDebugMenuTab Interface
	virtual FName GetTabName() const override
	{
		return TEXT("GAS");
	}

	virtual void Draw(const FObsidianDebugMenuContext& InContext) override;
	//~ End FObsidianDebugMenuTab Interface

private:
	/** Draws the picker of the actor to debug, returns its Ability System Component. */
	UAbilitySystemComponent* DrawTargetPicker(const FObsidianDebugMenuContext& InContext);

	void DrawAttributes(const FObsidianDebugMenuContext& InContext, UAbilitySystemComponent* InASC);
	void DrawAbilities(const FObsidianDebugMenuContext& InContext, UAbilitySystemComponent* InASC);
	void DrawEffects(const FObsidianDebugMenuContext& InContext, UAbilitySystemComponent* InASC);
	void DrawTags(const FObsidianDebugMenuContext& InContext, UAbilitySystemComponent* InASC);

	void GatherInputTags();

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
	FObsidianDebugTab_AI();

	virtual ~FObsidianDebugTab_AI() override;

	//~ Begin FObsidianDebugMenuTab Interface
	virtual FName GetTabName() const override
	{
		return TEXT("AI");
	}

	virtual void Tick(const FObsidianDebugMenuContext& InContext, const float InDeltaTime) override;
	virtual void Draw(const FObsidianDebugMenuContext& InContext) override;
	//~ End FObsidianDebugMenuTab Interface

private:
	/** Scripted action of the behavior trace, run on every traced enemy once the trace reaches its time. */
	struct FBehaviorTraceAction
	{
		enum class EType : uint8
		{
			/** Teleports the enemies in front of the Player, Value is the distance. */
			Bring,
			/** Deals damage to the enemies, Value is the percent of their Max Health. */
			Damage,
			/** Teleports the Player to the Location, used to get the Enemy entities hydrated and dehydrated. */
			MovePlayer
		};

		EType Type = EType::Bring;
		float Time = 0.0f;
		float Value = 0.0f;
		FVector2D Location = FVector2D::ZeroVector;
		bool bDone = false;
	};

	struct FBehaviorTracedEnemy
	{
		TWeakObjectPtr<AObsidianEnemy> Enemy;
		TWeakObjectPtr<UAbilitySystemComponent> ASC;
		FString Name;
		FDelegateHandle AbilityActivatedHandle;
		FDelegateHandle AbilityEndedHandle;
		FDelegateHandle AbilityFailedHandle;
		bool bDeathLogged = false;
		bool bGoneLogged = false;
	};

	/**
	 * Logs what the enemies do (used Abilities, movement, distance to the Player) in a form that can be compared between
	 * two runs of the game, so a change in the way the AI is driven can be verified to not change its behavior.
	 */
	struct FBehaviorTrace
	{
		TWeakObjectPtr<UWorld> World;
		/** Only the enemies with this in their class name are traced, every enemy if empty. */
		FString ClassFilter;
		/** Destroys the enemies that are not traced when the trace starts, so they do not interfere. */
		bool bSolo = false;
		bool bQuitWhenDone = false;
		float Duration = 60.0f;
		float SampleInterval = 0.5f;
		TArray<FBehaviorTraceAction> Actions;

		double GiveUpTime = 0.0;
		double StartWorldTime = -1.0;
		double NextSampleTime = 0.0;
		bool bMassEntitiesSeen = false;
		TArray<FBehaviorTracedEnemy> Enemies;
	};

	/**
	 * Enemies assume to be fully set up when they get possessed, so spawning the ones that are not (like the base classes)
	 * crashes the game. Returns what is missing in the setup of the enemy, empty string if it is safe to spawn.
	 */
	static FString GetSpawnBlocker(const AObsidianEnemy* InEnemyDefault);

	static void SetAILogicPaused(const AObsidianEnemy* InEnemy, const bool bInPaused);

	void DrawSpawning(const FObsidianDebugMenuContext& InContext);
	void DrawSpawnedEnemies(const FObsidianDebugMenuContext& InContext);

	void SpawnEnemies(const FObsidianDebugMenuContext& InContext);

	void RunBehaviorTrace(const TArray<FString>& InArgs, UWorld* InWorld);
	/** Returns false once the trace is finished. */
	bool TickBehaviorTrace();
	void StartBehaviorTrace(const FObsidianDebugMenuContext& InContext);
	void FinishBehaviorTrace();
	void TrackNewEnemies(const FObsidianDebugMenuContext& InContext, const double InTraceTime);
	void RunBehaviorTraceAction(const FObsidianDebugMenuContext& InContext, const FBehaviorTraceAction& InAction, const double InTraceTime);
	void LogBehaviorTraceSamples(const FObsidianDebugMenuContext& InContext, const double InTraceTime);

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

	FAutoConsoleCommandWithWorldAndArgs BehaviorTraceCommand;
	FTSTicker::FDelegateHandle BehaviorTraceTickerHandle;
	TUniquePtr<FBehaviorTrace> BehaviorTrace;
};

#endif // WITH_OBSIDIAN_DEBUG_MENU
