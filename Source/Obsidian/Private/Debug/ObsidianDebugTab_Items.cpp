// Copyright 2026 out of sCope team - intrxx

#include "Debug/ObsidianDebugMenuTabs.h"

#if WITH_OBSIDIAN_DEBUG_MENU

#include <Algo/RandomShuffle.h>
#include <Engine/Engine.h>
#include <Engine/GameInstance.h>
#include <GameFramework/Pawn.h>
#include <GameplayTagsManager.h>
#include <SlateIM.h>

#include "Characters/Player/ObsidianPlayerController.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Affixes.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Equippable.h"
#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "InventoryItems/ItemAffixes/ObsidianAffixList.h"
#include "InventoryItems/ItemDrop/ObsidianItemDataConfig.h"
#include "InventoryItems/ItemDrop/ObsidianItemDataDeveloperSettings.h"
#include "InventoryItems/ItemDrop/ObsidianItemDataLoaderSubsystem.h"
#include "InventoryItems/ItemDrop/ObsidianItemDropComponent.h"
#include "InventoryItems/ItemDrop/ObsidianItemDropManagerSubsystem.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianItemsFunctionLibrary.h"
#include "Obsidian/ObsidianLogCategories.h"

namespace ObsidianDebugItems
{
	struct FItemRarityOption
	{
		const TCHAR* Name;
		EObsidianItemRarity Rarity;
	};

	/** None stands for the default rarity of the Item Definition. */
	const FItemRarityOption ItemRarityOptions[] =
	{
		{TEXT("Item Default"), EObsidianItemRarity::None},
		{TEXT("Normal"), EObsidianItemRarity::Normal},
		{TEXT("Magic"), EObsidianItemRarity::Magic},
		{TEXT("Rare"), EObsidianItemRarity::Rare},
		{TEXT("Unique"), EObsidianItemRarity::Unique},
		{TEXT("Set"), EObsidianItemRarity::Set}
	};

	struct FAffixTypeOption
	{
		const TCHAR* Name;
		EObsidianAffixType AffixType;
	};

	/** None stands for every type of affix. */
	const FAffixTypeOption AffixTypeOptions[] =
	{
		{TEXT("All"), EObsidianAffixType::None},
		{TEXT("Prefix"), EObsidianAffixType::Prefix},
		{TEXT("Suffix"), EObsidianAffixType::Suffix},
		{TEXT("Implicit"), EObsidianAffixType::Implicit},
		{TEXT("Skill Implicit"), EObsidianAffixType::SkillImplicit}
	};

	const TCHAR* GetAffixTypeName(const EObsidianAffixType AffixType)
	{
		for (const FAffixTypeOption& Option : AffixTypeOptions)
		{
			if (Option.AffixType == AffixType && AffixType != EObsidianAffixType::None)
			{
				return Option.Name;
			}
		}
		return TEXT("Affix");
	}

	template<typename OptionType, int32 Count>
	TArray<FString> GetOptionNames(const OptionType (&Options)[Count])
	{
		TArray<FString> Names;
		for (const OptionType& Option : Options)
		{
			Names.Add(Option.Name);
		}
		return Names;
	}

	FString GetAffixDisplayName(const FObsidianDynamicItemAffix& Affix, const EObsidianAffixType AffixType)
	{
		FString TagName = Affix.AffixTag.ToString();
		TagName.RemoveFromStart(TEXT("Item.Affix."));

		FString DisplayName = FString::Printf(TEXT("[%s] %s"), GetAffixTypeName(AffixType), *TagName);
		if (Affix.AffixItemNameAddition.IsEmpty() == false)
		{
			DisplayName += FString::Printf(TEXT(" - %s"), *Affix.AffixItemNameAddition);
		}
		return DisplayName;
	}

	/** Mirrors the conversion done by the Item Drop Manager when the item gets dropped. */
	FObsidianItemGeneratedData MakeGeneratedData(const FObsidianItemToDrop& ItemToDrop)
	{
		FObsidianItemGeneratedData GeneratedData;
		GeneratedData.SetStackCount(ItemToDrop.DropStacks);
		GeneratedData.ItemLevel = ItemToDrop.DropItemLevel;
		GeneratedData.ItemRarity = ItemToDrop.DropRarity;
		GeneratedData.ItemAffixes = ItemToDrop.DropAffixes;
		GeneratedData.ItemEquippingRequirements = ItemToDrop.DropItemRequirements;
		GeneratedData.NameData = FObsidianItemGeneratedNameData(ItemToDrop.DropRareItemDisplayNameAddition,
			ItemToDrop.DropMagicItemDisplayNameAddition);
		return GeneratedData;
	}

	int32 GetMaxItemLevel()
	{
		const UObsidianItemDataDeveloperSettings* ItemDataSettings = GetDefault<UObsidianItemDataDeveloperSettings>();
		return ItemDataSettings ? ItemDataSettings->MaxTreasureQuality : 90;
	}

	/** Mirrors the candidates gathering of FObsidianRareItemNameGenerationData, which asserts if there are none. */
	bool CanGenerateRareItemName(const int32 ItemLevel, const FGameplayTag& ItemCategory)
	{
		const UObsidianItemDataDeveloperSettings* ItemDataSettings = GetDefault<UObsidianItemDataDeveloperSettings>();
		const UObsidianItemDataConfig* ItemDataConfig = ItemDataSettings ? ItemDataSettings->ItemDataConfig.Get() : nullptr;
		if (ItemDataConfig == nullptr || ItemCategory.IsValid() == false)
		{
			// No name is generated in these cases.
			return true;
		}

		const auto HasNameForLevel = [ItemLevel](const TArray<FObsidianRareItemNameAddition>& NameAdditions)
		{
			return NameAdditions.ContainsByPredicate([ItemLevel](const FObsidianRareItemNameAddition& NameAddition)
				{
					return NameAddition.ItemLevelRange.X <= ItemLevel && NameAddition.ItemNameAdditions.IsEmpty() == false;
				});
		};

		const FObsidianRareItemNameGenerationData& NameData = ItemDataConfig->RareItemNameGenerationData;
		const bool bHasSuffix = NameData.SuffixNameAdditions.ContainsByPredicate(
			[&HasNameForLevel, &ItemCategory](const FObsidianRareItemSuffixNameAddition& SuffixAddition)
			{
				return SuffixAddition.ForItemCategories.HasTagExact(ItemCategory) && HasNameForLevel(SuffixAddition.ItemNameAdditions);
			});

		return bHasSuffix && HasNameForLevel(NameData.PrefixNameAdditions);
	}

	/** Lowest item level at which any of the tiers can be rolled, INDEX_NONE if there are no tiers at all. */
	int32 GetMinItemLevelOfTiers(const FObsidianAffixValues& AffixValues)
	{
		int32 MinItemLevel = INDEX_NONE;
		for (const FObsidianAffixValueRange& AffixRange : AffixValues.PossibleAffixRanges)
		{
			const int32 TierItemLevel = AffixRange.AffixTier.MinItemLevelRequirement;
			MinItemLevel = MinItemLevel == INDEX_NONE ? TierItemLevel : FMath::Min(MinItemLevel, TierItemLevel);
		}
		return MinItemLevel;
	}

	/** Lowest item level at which the affix has a tier to roll, INDEX_NONE if it has no tiers at all. */
	int32 GetMinItemLevelOfStaticAffix(const FObsidianStaticItemAffix& Affix)
	{
		return GetMinItemLevelOfTiers(Affix.AffixValuesDefinition);
	}

	/** Lowest item level at which the affix can be rolled, INDEX_NONE if it has no tiers at all. */
	int32 GetMinItemLevelOfDynamicAffix(const FObsidianDynamicItemAffix& Affix)
	{
		const int32 MinItemLevelOfTiers = GetMinItemLevelOfTiers(Affix.AffixValuesDefinition);
		return MinItemLevelOfTiers == INDEX_NONE ? INDEX_NONE : FMath::Max<int32>(MinItemLevelOfTiers, Affix.MinItemLevelRequirement);
	}

	FString GetRarityName(const EObsidianItemRarity Rarity)
	{
		return StaticEnum<EObsidianItemRarity>()->GetNameStringByValue(static_cast<int64>(Rarity));
	}

	/** Rarities the items are generated with, the rest (Unique, Set) is only ever made from its own Item Definitions. */
	bool IsGeneratedRarity(const EObsidianItemRarity Rarity)
	{
		return Rarity == EObsidianItemRarity::Normal || Rarity == EObsidianItemRarity::Magic || Rarity == EObsidianItemRarity::Rare;
	}

	/**
	 * How many affixes of given type an item of given rarity can have, the same limits the dropped items are rolled with.
	 * These are only set up for the generated rarities, the settings log an error when asked for any other.
	 */
	int32 GetMaxAffixCountOfType(const EObsidianItemRarity Rarity, const EObsidianAffixType AffixType)
	{
		const UObsidianItemDataDeveloperSettings* ItemDataSettings = GetDefault<UObsidianItemDataDeveloperSettings>();
		if (ItemDataSettings == nullptr)
		{
			return 0;
		}

		switch (AffixType)
		{
			case EObsidianAffixType::Prefix:
				return IsGeneratedRarity(Rarity) ? ItemDataSettings->GetMaxPrefixCountForRarity(Rarity) : 0;
			case EObsidianAffixType::Suffix:
				return IsGeneratedRarity(Rarity) ? ItemDataSettings->GetMaxSuffixCountForRarity(Rarity) : 0;
			case EObsidianAffixType::Implicit:
				return ItemDataSettings->DefaultMaxImplicitCount;
			case EObsidianAffixType::SkillImplicit:
				return 1; // By design there can be only one Skill Implicit.
			default:
				return 0;
		}
	}

	int32 GetMaxPrefixAndSuffixCount(const EObsidianItemRarity Rarity)
	{
		const UObsidianItemDataDeveloperSettings* ItemDataSettings = GetDefault<UObsidianItemDataDeveloperSettings>();
		return ItemDataSettings && IsGeneratedRarity(Rarity) ? ItemDataSettings->GetMaxAffixCountForRarity(Rarity) : 0;
	}

	/**
	 * Item Definitions are named with a suffix that describes them, e.g. ID_BasicAxe_OneHand_TEWA. Its first letter says
	 * if the item is Static (predefined) or a Template (meant to be generated), the following ones are the initials of
	 * the category folders the item lives in (Equipment, Weapons, Axe).
	 *
	 * Returns the name of the item with the suffix spelled out, e.g. "ID_BasicAxe_OneHand_TEWA  [Template, Equipment,
	 * Weapons, Axe]". The categories are read from the folders, so new ones need no changes here. The plain name is
	 * returned for the items that do not follow the convention.
	 */
	FString GetItemDefinitionDisplayName(const FSoftClassPath& ItemDefPath)
	{
		FString ItemName = ItemDefPath.GetAssetName();
		ItemName.RemoveFromEnd(TEXT("_C"));

		int32 SuffixSeparatorIndex = INDEX_NONE;
		if (ItemName.FindLastChar(TEXT('_'), SuffixSeparatorIndex) == false)
		{
			return ItemName;
		}

		// Every part of the suffix starts with an uppercase letter, e.g. "TEABa" is T, E, A and Ba.
		TArray<FString> SuffixParts;
		for (int32 i = SuffixSeparatorIndex + 1; i < ItemName.Len(); ++i)
		{
			if (FChar::IsUpper(ItemName[i]))
			{
				SuffixParts.AddDefaulted();
			}
			else if (SuffixParts.IsEmpty() || FChar::IsLower(ItemName[i]) == false)
			{
				return ItemName;
			}
			SuffixParts.Last().AppendChar(ItemName[i]);
		}

		TArray<FString> Folders;
		ItemDefPath.GetLongPackageName().ParseIntoArray(Folders, TEXT("/"));
		Folders.Pop(); // The last one is the name of the asset.

		const int32 StaticFolderIndex = Folders.IndexOfByKey(FString(TEXT("ItemStaticDefinitions")));
		const int32 TemplateFolderIndex = Folders.IndexOfByKey(FString(TEXT("ItemTemplateDefinitions")));
		const int32 RootFolderIndex = FMath::Max(StaticFolderIndex, TemplateFolderIndex);
		if (SuffixParts.IsEmpty() || RootFolderIndex == INDEX_NONE || SuffixParts[0] != (StaticFolderIndex != INDEX_NONE ? TEXT("S") : TEXT("T")))
		{
			return ItemName;
		}

		TArray<FString> SuffixNames;
		SuffixNames.Add(StaticFolderIndex != INDEX_NONE ? TEXT("Static") : TEXT("Template"));
		for (int32 i = 1; i < SuffixParts.Num(); ++i)
		{
			const int32 FolderIndex = RootFolderIndex + i;
			if (Folders.IsValidIndex(FolderIndex) == false || Folders[FolderIndex].StartsWith(SuffixParts[i].Left(1), ESearchCase::CaseSensitive) == false)
			{
				return ItemName;
			}
			SuffixNames.Add(Folders[FolderIndex]);
		}

		return FString::Printf(TEXT("%s  [%s]"), *ItemName, *FString::Join(SuffixNames, TEXT(", ")));
	}

	const UOInventoryItemFragment_Affixes* GetAffixFragment(const UObsidianInventoryItemDefinition* ItemDefault)
	{
		return ItemDefault ? Cast<UOInventoryItemFragment_Affixes>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Affixes::StaticClass())) : nullptr;
	}

	/**
	 * Number of Prefixes and Suffixes the item generation has to roll from for given item, gathered the same way it does it.
	 * Returns INDEX_NONE if the Item Data Config is not loaded, so it is not known.
	 */
	int32 CountRollablePrefixesAndSuffixes(const UObsidianInventoryItemDefinition* ItemDefault, const int32 ItemLevel)
	{
		const UObsidianItemDataDeveloperSettings* ItemDataSettings = GetDefault<UObsidianItemDataDeveloperSettings>();
		const UObsidianItemDataConfig* ItemDataConfig = ItemDataSettings ? ItemDataSettings->ItemDataConfig.Get() : nullptr;
		if (ItemDataConfig == nullptr || ItemDefault == nullptr)
		{
			return INDEX_NONE;
		}

		int32 Count = 0;
		for (const UObsidianAffixList* AffixList : ItemDataConfig->CommonAffixLists)
		{
			if (AffixList == nullptr)
			{
				continue;
			}

			for (const FObsidianAffixClass& AffixClass : AffixList->ReadAllAffixClasses())
			{
				if (AffixClass.AffixClassType == EObsidianAffixType::Prefix || AffixClass.AffixClassType == EObsidianAffixType::Suffix)
				{
					Count += AffixClass.GetAllAffixesUpToQualityForCategory(ItemLevel, ItemDefault->GetItemCategoryTag(),
						ItemDefault->GetItemBaseTypeTag()).Num();
				}
			}
		}
		return Count;
	}
}

FObsidianDebugTab_Items::FObsidianDebugTab_Items()
	: CraftingSelfTestCommand(TEXT("obsidian.DebugMenu.TestItemCrafting"),
		TEXT("Crafts every item with every rarity and affix mode and verifies the results, then rolls the Random Drops, see the log for the summary."
			" Usage: obsidian.DebugMenu.TestItemCrafting [Iterations] [quit], quit closes the game once the test is done."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateRaw(this, &FObsidianDebugTab_Items::RunCraftingSelfTest))
{
	ItemPicker.ClassFilter = [this](const UClass* ItemDefClass)
		{
			if (IsCurrencyItem(ItemDefClass))
			{
				return false;
			}

			const bool bHandPickingAffixes = static_cast<EAffixMode>(ItemPickerAffixModeIndex) == EAffixMode::HandPicked;
			return bHandPickingAffixes == false || CanHandPickAffixesForItem(ItemDefClass);
		};
	CurrencyPicker.ClassFilter = &FObsidianDebugTab_Items::IsCurrencyItem;

	ItemPicker.DisplayNameProvider = &ObsidianDebugItems::GetItemDefinitionDisplayName;
	CurrencyPicker.DisplayNameProvider = &ObsidianDebugItems::GetItemDefinitionDisplayName;
}

FObsidianDebugTab_Items::~FObsidianDebugTab_Items()
{
	FTSTicker::RemoveTicker(CraftingSelfTestTickerHandle);
}

const UObsidianInventoryItemDefinition* FObsidianDebugTab_Items::FCraftingRequest::GetItemDefault() const
{
	return ItemDef.GetDefaultObject();
}

EObsidianItemRarity FObsidianDebugTab_Items::FCraftingRequest::GetRarity() const
{
	if (ChosenRarity != EObsidianItemRarity::None)
	{
		return ChosenRarity;
	}

	const UObsidianInventoryItemDefinition* ItemDefault = GetItemDefault();
	if (ItemDefault == nullptr)
	{
		return EObsidianItemRarity::None;
	}

	// Items that get their affixes generated usually have no default rarity as it is rolled when they drop, the base item is Normal.
	const EObsidianItemRarity DefaultRarity = ItemDefault->GetItemDefaultRarity();
	const UOInventoryItemFragment_Affixes* AffixFragment = ObsidianDebugItems::GetAffixFragment(ItemDefault);
	const bool bGeneratesAffixes = AffixFragment && AffixFragment->GetGenerationType() != EObsidianAffixGenerationType::NoGeneration;
	return DefaultRarity == EObsidianItemRarity::None && bGeneratesAffixes ? EObsidianItemRarity::Normal : DefaultRarity;
}

int32 FObsidianDebugTab_Items::FCraftingRequest::GetItemLevel() const
{
	return FMath::Clamp(ItemLevel, 1, ObsidianDebugItems::GetMaxItemLevel());
}

FObsidianDebugTab_Items::FCraftingRequest FObsidianDebugTab_Items::MakeCraftingRequest(UClass* ItemDefClass) const
{
	using namespace ObsidianDebugItems;

	const int32 LastItemRarityIndex = static_cast<int32>(UE_ARRAY_COUNT(ItemRarityOptions)) - 1;

	FCraftingRequest Request;
	Request.ItemDef = ItemDefClass;
	Request.ChosenRarity = ItemRarityOptions[FMath::Clamp(ItemRarityIndex, 0, LastItemRarityIndex)].Rarity;
	Request.ItemLevel = ItemLevel;
	Request.Stacks = ItemStacks;
	Request.AffixMode = static_cast<EAffixMode>(FMath::Clamp(AffixModeIndex, 0, static_cast<int32>(EAffixMode::None)));
	Request.PickedAffixes = PickedAffixes;
	Request.bIncludeStaticAffixes = bIncludeStaticAffixes;
	Request.bApplyMagicMultiplier = bApplyMagicMultiplier;
	return Request;
}

bool FObsidianDebugTab_Items::IsCurrencyItem(const UClass* ItemDefClass)
{
	const UObsidianInventoryItemDefinition* ItemDefault = ItemDefClass ? Cast<UObsidianInventoryItemDefinition>(ItemDefClass->GetDefaultObject()) : nullptr;
	if (ItemDefault == nullptr)
	{
		return false;
	}

	// Compared by name as the tag matching functions ensure on categories that are no longer registered, which some definitions still use.
	const FString CategoryName = ItemDefault->GetItemCategoryTag().ToString();
	return CategoryName == TEXT("Item.Category.Currency") || CategoryName.StartsWith(TEXT("Item.Category.Currency."));
}

bool FObsidianDebugTab_Items::CanHandPickAffixesForItem(const UClass* ItemDefClass) const
{
	const UObsidianInventoryItemDefinition* ItemDefault = ItemDefClass ? Cast<UObsidianInventoryItemDefinition>(ItemDefClass->GetDefaultObject()) : nullptr;
	const UOInventoryItemFragment_Affixes* AffixFragment = ObsidianDebugItems::GetAffixFragment(ItemDefault);
	if (AffixFragment == nullptr || AffixFragment->GetGenerationType() == EObsidianAffixGenerationType::NoGeneration)
	{
		return false;
	}

	return AllAffixes.ContainsByPredicate([ItemDefault](const FAffixEntry& AffixEntry)
		{
			return IsAffixCompatibleWithItem(AffixEntry.Affix, ItemDefault);
		});
}

void FObsidianDebugTab_Items::UpdateItemPickerFilter()
{
	if (ItemPickerAffixModeIndex == AffixModeIndex)
	{
		return;
	}
	ItemPickerAffixModeIndex = AffixModeIndex;

	if (bGatheredAffixes == false)
	{
		GatherAffixes();
	}

	// Keeps the chosen item if it is still listed, falls back to the first one otherwise.
	ItemPicker.Refresh(UObsidianInventoryItemDefinition::StaticClass());
	SelectedItemDef.Reset(ItemPicker.LoadSelectedClass());
}

void FObsidianDebugTab_Items::Draw(const FObsidianDebugMenuContext& Context)
{
	if (Context.Pawn == nullptr)
	{
		ObsidianDebugUI::WarningText(TEXT("Chosen Player has no Pawn, items are spawned around it."));
		return;
	}

	SlateIM::BeginHorizontalStack();
	{
		ObsidianDebugUI::Label(TEXT("Drop Radius"));
		SlateIM::SetToolTip(TEXT("Radius around the Player in which the items are dropped."));
		SlateIM::MinWidth(120.0f);
		SlateIM::SpinBox(DropRadius, {.Min = 0.0f, .Max = 5000.0f});
	}
	SlateIM::EndHorizontalStack();

	DrawRandomDrops(Context);
	DrawItemCrafting(Context);
	DrawCurrency(Context);
}

void FObsidianDebugTab_Items::DrawCurrency(const FObsidianDebugMenuContext& Context)
{
	ObsidianDebugUI::Section(TEXT("Currency"));

	SlateIM::BeginHorizontalStack();
	{
		ObsidianDebugUI::Label(TEXT("Currency"));
		CurrencyPicker.Draw(UObsidianInventoryItemDefinition::StaticClass());
	}
	SlateIM::EndHorizontalStack();

	SlateIM::BeginHorizontalStack();
	{
		ObsidianDebugUI::Label(TEXT("Stacks"));
		SlateIM::SetToolTip(TEXT("Stacks of a single spawned item, ignored by the currency that does not stack."));
		SlateIM::MinWidth(120.0f);
		SlateIM::SpinBox(CurrencyStacks, {.Min = 1, .Max = 255});
	}
	SlateIM::EndHorizontalStack();

	SlateIM::BeginHorizontalStack();
	{
		ObsidianDebugUI::Label(TEXT("Count"));
		SlateIM::SetToolTip(TEXT("Number of items to spawn, each one of them with the given amount of stacks."));
		SlateIM::MinWidth(120.0f);
		SlateIM::SpinBox(CurrencyCount, {.Min = 1, .Max = 50});
	}
	SlateIM::EndHorizontalStack();

	SlateIM::BeginHorizontalStack();
	{
		ObsidianDebugUI::Label(TEXT("Destination"));
		SlateIM::MinWidth(200.0f);
		CurrencyDestinationComboBox.Draw({TEXT("Drop Near Player"), TEXT("Add To Inventory")}, CurrencyDestinationIndex);
	}
	SlateIM::EndHorizontalStack();

	if (SlateIM::Button(TEXT("Spawn Currency")))
	{
		SpawnCurrency(Context);
	}
}

void FObsidianDebugTab_Items::DrawRandomDrops(const FObsidianDebugMenuContext& Context)
{
	ObsidianDebugUI::Section(TEXT("Random Drops"));
	SlateIM::Text(TEXT("Rolls the items the same way an entity of given rarity and level would drop them."),
		{.Color = FStyleColors::AccentGray});

	SlateIM::BeginHorizontalStack();
	{
		ObsidianDebugUI::Label(TEXT("Entity Rarity"));

		// None is not a valid rarity to drop the items with, so it is skipped.
		TArray<FString> EntityRarityNames;
		const UEnum* EntityRarityEnum = StaticEnum<EObsidianEntityRarity>();
		for (int32 i = 1; i < EntityRarityEnum->NumEnums() - 1; ++i)
		{
			EntityRarityNames.Add(EntityRarityEnum->GetDisplayNameTextByIndex(i).ToString());
		}

		SlateIM::MinWidth(200.0f);
		EntityRarityComboBox.Draw(EntityRarityNames, EntityRarityIndex);
	}
	SlateIM::EndHorizontalStack();

	SlateIM::BeginHorizontalStack();
	{
		ObsidianDebugUI::Label(TEXT("Entity Level"));
		SlateIM::MinWidth(120.0f);
		SlateIM::SpinBox(EntityLevel, {.Min = 1, .Max = ObsidianDebugItems::GetMaxItemLevel()});
	}
	SlateIM::EndHorizontalStack();

	SlateIM::BeginHorizontalStack();
	{
		ObsidianDebugUI::Label(TEXT("Times To Roll"));
		SlateIM::SetToolTip(TEXT("How many times the whole drop is rolled, each one can result in multiple items."));
		SlateIM::MinWidth(120.0f);
		SlateIM::SpinBox(DropTimes, {.Min = 1, .Max = 100});
	}
	SlateIM::EndHorizontalStack();

	SlateIM::BeginHorizontalStack();
	{
		SlateIM::VAlign(VAlign_Center);
		SlateIM::MinWidth(150.0f);
		SlateIM::SetToolTip(TEXT("Limits the common Treasure Classes to given category the same way the Item Drop Component does it,"
			" which only uses the classes of exactly the resulting Treasure Quality (Entity Level + bonus of the Entity Rarity)."));
		SlateIM::CheckBox(bLimitCategory, {.Label = TEXT("Limit Category")});

		if (bLimitCategory)
		{
			if (ItemCategories.IsEmpty())
			{
				GatherItemCategories();
			}

			SlateIM::MinWidth(320.0f);
			CategoryComboBox.Draw(ItemCategoryNames, CategoryIndex, true);
		}
	}
	SlateIM::EndHorizontalStack();

	if (SlateIM::Button(TEXT("Roll Drops")))
	{
		RollRandomDrops(Context);
	}
}

void FObsidianDebugTab_Items::DrawItemCrafting(const FObsidianDebugMenuContext& Context)
{
	using namespace ObsidianDebugItems;

	ObsidianDebugUI::Section(TEXT("Item Crafting"));

	UpdateItemPickerFilter();

	SlateIM::BeginHorizontalStack();
	{
		ObsidianDebugUI::Label(TEXT("Item"));
		SlateIM::SetToolTip(TEXT("With the hand-picked affixes, only the items that can have them are listed."));
		if (ItemPicker.Draw(UObsidianInventoryItemDefinition::StaticClass()))
		{
			SelectedItemDef.Reset(ItemPicker.LoadSelectedClass());
		}
	}
	SlateIM::EndHorizontalStack();

	const UObsidianInventoryItemDefinition* ItemDefault = SelectedItemDef.IsValid() ?
		GetDefault<UObsidianInventoryItemDefinition>(SelectedItemDef.Get()) : nullptr;
	if (ItemDefault)
	{
		const UEnum* ItemRarityEnum = StaticEnum<EObsidianItemRarity>();
		SlateIM::Text(FString::Printf(TEXT("Category: %s  |  Base Type: %s  |  Default Rarity: %s%s%s"),
			*ItemDefault->GetItemCategoryTag().ToString(), *ItemDefault->GetItemBaseTypeTag().ToString(),
			*ItemRarityEnum->GetNameStringByValue(static_cast<int64>(ItemDefault->GetItemDefaultRarity())),
			ItemDefault->IsStackable() ? TEXT("  |  Stackable") : TEXT(""),
			ItemDefault->IsEquippable() ? TEXT("  |  Equippable") : TEXT("")), {.Color = FStyleColors::AccentGray});
	}

	SlateIM::BeginHorizontalStack();
	{
		ObsidianDebugUI::Label(TEXT("Rarity"));
		SlateIM::MinWidth(200.0f);
		ItemRarityComboBox.Draw(GetOptionNames(ItemRarityOptions), ItemRarityIndex);
	}
	SlateIM::EndHorizontalStack();

	SlateIM::BeginHorizontalStack();
	{
		ObsidianDebugUI::Label(TEXT("Item Level"));
		SlateIM::MinWidth(120.0f);
		SlateIM::SpinBox(ItemLevel, {.Min = 1, .Max = GetMaxItemLevel()});
	}
	SlateIM::EndHorizontalStack();

	SlateIM::BeginHorizontalStack();
	{
		ObsidianDebugUI::Label(TEXT("Stacks"));
		SlateIM::SetToolTip(TEXT("Used only by the stackable items."));
		SlateIM::MinWidth(120.0f);
		SlateIM::SpinBox(ItemStacks, {.Min = 1, .Max = 255});
	}
	SlateIM::EndHorizontalStack();

	SlateIM::BeginHorizontalStack();
	{
		ObsidianDebugUI::Label(TEXT("Count"));
		SlateIM::SetToolTip(TEXT("Number of items to craft, each one of them is generated separately."));
		SlateIM::MinWidth(120.0f);
		SlateIM::SpinBox(ItemCount, {.Min = 1, .Max = 50});
	}
	SlateIM::EndHorizontalStack();

	SlateIM::BeginHorizontalStack();
	{
		ObsidianDebugUI::Label(TEXT("Affixes"));
		SlateIM::MinWidth(320.0f);
		AffixModeComboBox.Draw({TEXT("Generated (same as dropped)"), TEXT("Hand-picked"), TEXT("None (Normal items only)")}, AffixModeIndex);
	}
	SlateIM::EndHorizontalStack();

	if (static_cast<EAffixMode>(AffixModeIndex) == EAffixMode::HandPicked)
	{
		DrawAffixPicker(ItemDefault);
	}

	SlateIM::SetToolTip(TEXT("Rolls the affix values with the multiplier of Magic items, has no effect on other rarities."));
	SlateIM::CheckBox(bApplyMagicMultiplier, {.Label = TEXT("Apply Magic Affix Multiplier")});

	SlateIM::BeginHorizontalStack();
	{
		ObsidianDebugUI::Label(TEXT("Destination"));
		SlateIM::MinWidth(200.0f);
		DestinationComboBox.Draw({TEXT("Drop Near Player"), TEXT("Add To Inventory")}, DestinationIndex);
	}
	SlateIM::EndHorizontalStack();

	const FString CraftingBlocker = GetCraftingBlocker(MakeCraftingRequest(SelectedItemDef.Get()));
	if (SlateIM::Button(TEXT("Craft Item"), {.bEnabled = CraftingBlocker.IsEmpty()}))
	{
		CraftItems(Context);
	}

	if (CraftingBlocker.IsEmpty() == false)
	{
		SlateIM::HAlign(HAlign_Fill);
		ObsidianDebugUI::WarningText(CraftingBlocker);
	}
}

void FObsidianDebugTab_Items::DrawAffixPicker(const UObsidianInventoryItemDefinition* ItemDefault)
{
	using namespace ObsidianDebugItems;

	if (bGatheredAffixes == false)
	{
		GatherAffixes();
	}

	FCraftingRequest Request = MakeCraftingRequest(SelectedItemDef.Get());
	const EObsidianItemRarity Rarity = Request.GetRarity();

	SlateIM::Padding(FMargin(16.0f, 4.0f, 0.0f, 4.0f));
	SlateIM::HAlign(HAlign_Fill);
	SlateIM::BeginVerticalStack();
	{
		SlateIM::BeginHorizontalStack();
		{
			ObsidianDebugUI::Label(TEXT("Affix Type"), 134.0f);
			SlateIM::MinWidth(160.0f);
			AffixTypeComboBox.Draw(GetOptionNames(AffixTypeOptions), AffixTypeIndex);

			SlateIM::SetToolTip(TEXT("Gathers the affixes again, use it after changing the Affix Lists."));
			if (SlateIM::Button(TEXT("Refresh")))
			{
				GatherAffixes();
			}
		}
		SlateIM::EndHorizontalStack();

		const int32 LastAffixTypeIndex = static_cast<int32>(UE_ARRAY_COUNT(AffixTypeOptions)) - 1;
		const EObsidianAffixType AffixTypeFilter = AffixTypeOptions[FMath::Clamp(AffixTypeIndex, 0, LastAffixTypeIndex)].AffixType;

		// Only the affixes that could be rolled on the chosen item with the chosen item level are listed.
		TArray<int32> FilteredAffixIndices;
		TArray<FString> FilteredAffixNames;
		for (int32 i = 0; i < AllAffixes.Num(); ++i)
		{
			const FAffixEntry& AffixEntry = AllAffixes[i];
			if (AffixTypeFilter != EObsidianAffixType::None && AffixEntry.AffixType != AffixTypeFilter)
			{
				continue;
			}

			if (CanAffixRollAtItemLevel(AffixEntry.Affix, Request.GetItemLevel()) == false
				|| (ItemDefault && IsAffixCompatibleWithItem(AffixEntry.Affix, ItemDefault) == false))
			{
				continue;
			}

			FilteredAffixIndices.Add(i);
			FilteredAffixNames.Add(GetAffixDisplayName(AffixEntry.Affix, AffixEntry.AffixType));
		}

		FString AddingBlocker;
		SlateIM::BeginHorizontalStack();
		{
			ObsidianDebugUI::Label(TEXT("Affix"), 134.0f);
			SlateIM::MinWidth(420.0f);
			AffixComboBox.Draw(FilteredAffixNames, AffixIndex, true);

			const FAffixEntry* AffixToAdd = FilteredAffixIndices.IsValidIndex(AffixIndex) ? &AllAffixes[FilteredAffixIndices[AffixIndex]] : nullptr;
			if (AffixToAdd)
			{
				AddingBlocker = GetAffixBlocker(Request, *AffixToAdd, PickedAffixes);
			}

			if (SlateIM::Button(TEXT("Add"), {.bEnabled = AffixToAdd != nullptr && AddingBlocker.IsEmpty()}) && AffixToAdd)
			{
				// Copied as the entry lives in an array that can be regathered.
				const FAffixEntry AffixEntryToAdd = *AffixToAdd;
				PickedAffixes.Add(AffixEntryToAdd);
			}
			if (SlateIM::Button(TEXT("Clear"), {.bEnabled = PickedAffixes.IsEmpty() == false}))
			{
				PickedAffixes.Reset();
			}
		}
		SlateIM::EndHorizontalStack();

		// Texts wrap to the width they are given, so they are stretched to not end up as narrow as the text they showed before.
		if (AddingBlocker.IsEmpty() == false)
		{
			SlateIM::HAlign(HAlign_Fill);
			SlateIM::Text(FString::Printf(TEXT("Cannot add the chosen affix. %s"), *AddingBlocker), {.Color = FStyleColors::AccentGray});
		}

		// The same limits the dropped items are rolled with.
		if (ItemDefault)
		{
			const auto CountPickedAffixes = [this](const EObsidianAffixType AffixType)
				{
					return PickedAffixes.FilterByPredicate([AffixType](const FAffixEntry& PickedAffix)
						{
							return PickedAffix.AffixType == AffixType;
						}).Num();
				};

			const UOInventoryItemFragment_Affixes* AffixFragment = GetAffixFragment(ItemDefault);
			const bool bGivesStaticImplicit = bIncludeStaticAffixes && AffixFragment && AffixFragment->HasImplicitAffix()
				&& static_cast<bool>(AffixFragment->GetStaticImplicitAffix());

			SlateIM::HAlign(HAlign_Fill);
			SlateIM::Text(FString::Printf(TEXT("%s item  |  Prefixes: %d / %d  |  Suffixes: %d / %d  |  Implicit: %d / %d  |  Skill Implicit: %d / %d"),
				*GetRarityName(Rarity),
				CountPickedAffixes(EObsidianAffixType::Prefix), GetMaxAffixCountOfType(Rarity, EObsidianAffixType::Prefix),
				CountPickedAffixes(EObsidianAffixType::Suffix), GetMaxAffixCountOfType(Rarity, EObsidianAffixType::Suffix),
				CountPickedAffixes(EObsidianAffixType::Implicit) + (bGivesStaticImplicit ? 1 : 0), GetMaxAffixCountOfType(Rarity, EObsidianAffixType::Implicit),
				CountPickedAffixes(EObsidianAffixType::SkillImplicit), GetMaxAffixCountOfType(Rarity, EObsidianAffixType::SkillImplicit)),
				{.Color = FStyleColors::AccentGray});
		}

		if (PickedAffixes.IsEmpty())
		{
			SlateIM::Text(TEXT("No affixes picked."), {.Color = FStyleColors::AccentGray});
		}

		int32 AffixIndexToRemove = INDEX_NONE;
		for (int32 i = 0; i < PickedAffixes.Num(); ++i)
		{
			const FAffixEntry& PickedAffix = PickedAffixes[i];

			// One button per affix, texts next to the buttons end up wrapped to the width of the text they showed before.
			SlateIM::SetToolTip(TEXT("Removes the affix from the picked ones."));
			if (SlateIM::Button(FString::Printf(TEXT("Remove  %s"), *GetAffixDisplayName(PickedAffix.Affix, PickedAffix.AffixType))))
			{
				AffixIndexToRemove = i;
			}

			// The picked affixes can stop fitting the item when the item, its rarity or its level gets changed.
			const FString PickedAffixBlocker = GetAffixBlocker(Request, PickedAffix, MakeArrayView(PickedAffixes.GetData(), i));
			if (PickedAffixBlocker.IsEmpty() == false)
			{
				SlateIM::HAlign(HAlign_Fill);
				SlateIM::Padding(FMargin(8.0f, 0.0f, 0.0f, 4.0f));
				ObsidianDebugUI::WarningText(PickedAffixBlocker);
			}
		}

		if (PickedAffixes.IsValidIndex(AffixIndexToRemove))
		{
			PickedAffixes.RemoveAt(AffixIndexToRemove);
		}

		SlateIM::SetToolTip(TEXT("Adds the Primary Item Affixes and the Implicit specified in the Item Definition on top of the picked ones."));
		SlateIM::CheckBox(bIncludeStaticAffixes, {.Label = TEXT("Include Affixes Of The Item Definition")});
	}
	SlateIM::EndVerticalStack();
}

void FObsidianDebugTab_Items::RollRandomDrops(const FObsidianDebugMenuContext& Context)
{
	UObsidianItemDropComponent* DropComponent = CreateDropComponent(Context);
	if (DropComponent == nullptr)
	{
		return;
	}

	if (bLimitCategory && ItemCategories.IsValidIndex(CategoryIndex))
	{
		DropComponent->bLimitCommonTreasureCategory = true;
		DropComponent->LimitCommonTreasureCategoryTag = ItemCategories[CategoryIndex];
	}

	// First entry of the enum (None) is skipped in the Combo Box.
	const EObsidianEntityRarity EntityRarity = static_cast<EObsidianEntityRarity>(EntityRarityIndex + 1);
	for (int32 i = 0; i < DropTimes; ++i)
	{
		DropComponent->DropItems(EntityRarity, static_cast<uint8>(EntityLevel));
	}

	DropComponent->DestroyComponent();

	Context.Notify(FString::Printf(TEXT("Rolled drops [%d] time(s) for [%s] entity of level [%d], see the log for the rolled items."), DropTimes,
		*StaticEnum<EObsidianEntityRarity>()->GetNameStringByValue(static_cast<int64>(EntityRarity)), EntityLevel));
}

void FObsidianDebugTab_Items::CraftItems(const FObsidianDebugMenuContext& Context)
{
	UClass* ItemDefClass = ItemPicker.LoadSelectedClass();
	if (ItemDefClass == nullptr)
	{
		Context.Notify(TEXT("Could not craft the item, chosen Item Definition is invalid."));
		return;
	}
	SelectedItemDef.Reset(ItemDefClass);

	const FCraftingRequest Request = MakeCraftingRequest(ItemDefClass);
	const FString CraftingBlocker = GetCraftingBlocker(Request);
	if (CraftingBlocker.IsEmpty() == false)
	{
		Context.Notify(FString::Printf(TEXT("Could not craft the item. %s"), *CraftingBlocker));
		return;
	}

	UObsidianItemDropComponent* DropComponent = CreateDropComponent(Context);
	if (DropComponent == nullptr)
	{
		return;
	}

	TArray<FObsidianItemToDrop> CraftedItems;
	for (int32 i = 0; i < ItemCount; ++i)
	{
		FObsidianItemToDrop CraftedItem;
		if (ConstructCraftedItem(DropComponent, Request, CraftedItem) == false)
		{
			break;
		}

		CraftedItem.DropTransform = DropComponent->GetDropTransformAligned(Context.Pawn);
		CraftedItems.Add(MoveTemp(CraftedItem));
	}

	DropComponent->DestroyComponent();

	DeliverItems(Context, MoveTemp(CraftedItems), ObsidianDebugUI::GetCleanClassName(ItemDefClass), static_cast<EDestination>(DestinationIndex));
}

void FObsidianDebugTab_Items::SpawnCurrency(const FObsidianDebugMenuContext& Context)
{
	const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = CurrencyPicker.LoadSelectedClass();
	const UObsidianInventoryItemDefinition* ItemDefault = ItemDef.GetDefaultObject();
	if (ItemDefault == nullptr)
	{
		Context.Notify(TEXT("Could not spawn the currency, chosen Item Definition is invalid."));
		return;
	}

	UObsidianItemDropComponent* DropComponent = CreateDropComponent(Context);
	if (DropComponent == nullptr)
	{
		return;
	}

	// Currency is spawned as defined, with nothing but the stacks to set.
	TArray<FObsidianItemToDrop> CurrencyItems;
	for (int32 i = 0; i < CurrencyCount; ++i)
	{
		FObsidianItemToDrop CurrencyItem;
		CurrencyItem.ItemDefinitionClass = ItemDef;
		CurrencyItem.DropRarity = ItemDefault->GetItemDefaultRarity();
		CurrencyItem.DropStacks = ItemDefault->IsStackable() ? static_cast<uint8>(FMath::Clamp(CurrencyStacks, 1, 255)) : 1;
		CurrencyItem.DropTransform = DropComponent->GetDropTransformAligned(Context.Pawn);
		CurrencyItems.Add(MoveTemp(CurrencyItem));
	}

	DropComponent->DestroyComponent();

	DeliverItems(Context, MoveTemp(CurrencyItems), ObsidianDebugUI::GetCleanClassName(ItemDef.Get()),
		static_cast<EDestination>(CurrencyDestinationIndex));
}

void FObsidianDebugTab_Items::DeliverItems(const FObsidianDebugMenuContext& Context, TArray<FObsidianItemToDrop>&& Items,
	const FString& ItemName, const EDestination Destination) const
{
	TArray<FObsidianItemToDrop> CraftedItems = MoveTemp(Items);
	const int32 CraftedItemsCount = CraftedItems.Num();

	if (Destination == EDestination::Inventory)
	{
		const AObsidianPlayerController* ObsidianPC = Context.GetObsidianPC();
		UObsidianInventoryComponent* InventoryComponent = ObsidianPC ? ObsidianPC->GetInventoryComponent() : nullptr;
		if (InventoryComponent == nullptr)
		{
			Context.Notify(TEXT("Could not add the item to the Inventory, chosen Player has no Inventory Component."));
			return;
		}

		int32 AddedItemsCount = 0;
		for (const FObsidianItemToDrop& CraftedItem : CraftedItems)
		{
			if (InventoryComponent->AddItemDefinition(CraftedItem.ItemDefinitionClass, ObsidianDebugItems::MakeGeneratedData(CraftedItem)))
			{
				++AddedItemsCount;
			}
		}

		Context.Notify(FString::Printf(TEXT("Added [%d] out of [%d] [%s] to the Inventory."), AddedItemsCount,
			CraftedItemsCount, *ItemName));
		return;
	}

	if (const UObsidianItemDropManagerSubsystem* DropManager = Context.World->GetSubsystem<UObsidianItemDropManagerSubsystem>())
	{
		DropManager->RequestDroppingItems(MoveTemp(CraftedItems));
		Context.Notify(FString::Printf(TEXT("Dropped [%d] [%s]."), CraftedItemsCount, *ItemName));
	}
}

bool FObsidianDebugTab_Items::ConstructCraftedItem(UObsidianItemDropComponent* DropComponent, const FCraftingRequest& Request,
	FObsidianItemToDrop& OutItemToDrop)
{
	using namespace ObsidianDebugItems;

	const UObsidianInventoryItemDefinition* ItemDefault = Request.GetItemDefault();
	if (ItemDefault == nullptr || DropComponent == nullptr || DropComponent->CachedItemDataLoader == nullptr)
	{
		return false;
	}

	OutItemToDrop.ItemDefinitionClass = Request.ItemDef;
	OutItemToDrop.DropItemLevel = static_cast<int8>(Request.GetItemLevel());
	OutItemToDrop.DropRarity = Request.GetRarity();
	OutItemToDrop.bShouldApplyMultiplier = Request.bApplyMagicMultiplier && OutItemToDrop.DropRarity == EObsidianItemRarity::Magic;
	OutItemToDrop.DropStacks = ItemDefault->IsStackable() ? static_cast<uint8>(FMath::Clamp(Request.Stacks, 1, 255)) : 1;

	if (Request.AffixMode == EAffixMode::Generated)
	{
		DropComponent->ConstructItem(OutItemToDrop);
		return true;
	}

	// The rest mirrors UObsidianItemDropComponent::ConstructItem, but with the affixes picked by hand instead of rolled.

	const UOInventoryItemFragment_Affixes* AffixFragment = GetAffixFragment(ItemDefault);
	if (AffixFragment == nullptr)
	{
		// There is nothing more to generate for the items without the Affix Fragment.
		return true;
	}

	if (Request.AffixMode == EAffixMode::HandPicked)
	{
		if (Request.bIncludeStaticAffixes)
		{
			DropComponent->TryToGivePrimaryItemAffix(OutItemToDrop, AffixFragment);
			DropComponent->TryToGiveStaticImplicit(OutItemToDrop, AffixFragment);
		}

		for (const FAffixEntry& PickedAffix : Request.PickedAffixes)
		{
			// Requests are validated before they get here, but initializing the affix asserts if there is no tier to roll.
			if (CanAffixRollAtItemLevel(PickedAffix.Affix, OutItemToDrop.DropItemLevel) == false
				|| OutItemToDrop.DropAffixes.Contains(PickedAffix.Affix))
			{
				UE_LOG(ObLogDebugMenu, Warning, TEXT("Skipped affix [%s], item level is too low for it or the item already has it."),
					*PickedAffix.Affix.AffixTag.ToString());
				continue;
			}

			FObsidianActiveItemAffix ActiveAffix;
			ActiveAffix.InitializeWithDynamic(PickedAffix.Affix, OutItemToDrop.DropItemLevel, OutItemToDrop.bShouldApplyMultiplier);
			// Type stored on the affix itself is only refreshed when its Affix List gets saved, the class it comes from is always right.
			ActiveAffix.AffixType = PickedAffix.AffixType;
			OutItemToDrop.DropAffixes.Add(ActiveAffix);
		}
	}

	if (OutItemToDrop.DropRarity == EObsidianItemRarity::Rare)
	{
		OutItemToDrop.DropRareItemDisplayNameAddition = DropComponent->CachedItemDataLoader->GetRandomRareItemNameAddition(
			OutItemToDrop.DropItemLevel, ItemDefault->GetItemCategoryTag());
	}
	else if (OutItemToDrop.bShouldApplyMultiplier)
	{
		OutItemToDrop.DropMagicItemDisplayNameAddition = DropComponent->CachedItemDataLoader->GetAffixMultiplierMagicItemNameAddition();
	}

	const UOInventoryItemFragment_Equippable* EquippableFragment = Cast<UOInventoryItemFragment_Equippable>(
		ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Equippable::StaticClass()));
	if (EquippableFragment)
	{
		FObsidianItemRequirements Requirements = EquippableFragment->GetItemDefaultEquippingRequirements();
		if (UObsidianItemsFunctionLibrary::HasEquippingRequirements(Requirements))
		{
			DropComponent->AdjustItemRequirementsBasedOnAddedAffixes(Requirements, OutItemToDrop);
			OutItemToDrop.DropItemRequirements = Requirements;
		}
	}

	return true;
}

FString FObsidianDebugTab_Items::GetCraftingBlocker(const FCraftingRequest& Request)
{
	using namespace ObsidianDebugItems;

	const UObsidianInventoryItemDefinition* ItemDefault = Request.GetItemDefault();
	if (ItemDefault == nullptr)
	{
		return FString();
	}

	FCraftingRequest DefaultRarityRequest;
	DefaultRarityRequest.ItemDef = Request.ItemDef;
	const EObsidianItemRarity DefaultRarity = DefaultRarityRequest.GetRarity();
	const EObsidianItemRarity ItemRarity = Request.GetRarity();
	const int32 ItemLevel = Request.GetItemLevel();
	const EAffixMode AffixMode = Request.AffixMode;

	const UOInventoryItemFragment_Affixes* AffixFragment = GetAffixFragment(ItemDefault);
	if (AffixFragment == nullptr)
	{
		// Neither affixes nor names are generated for the items without the Affix Fragment.
		if (ItemRarity != DefaultRarity)
		{
			return FString::Printf(TEXT("Item has no Affix Fragment, so it can only be crafted with its default rarity [%s]."),
				*GetRarityName(DefaultRarity));
		}
		if (AffixMode == EAffixMode::HandPicked && Request.PickedAffixes.IsEmpty() == false)
		{
			return TEXT("Item has no Affix Fragment, so it cannot have any affixes.");
		}
		return FString();
	}

	const EObsidianAffixGenerationType GenerationType = AffixFragment->GetGenerationType();
	if (GenerationType == EObsidianAffixGenerationType::NoGeneration)
	{
		// Unique, Set and other hand-made items are dropped exactly as their Item Definition describes them.
		if (ItemRarity != DefaultRarity)
		{
			return FString::Printf(TEXT("Affixes of this item are set in its Item Definition (No Generation), so it can only be crafted"
				" with its default rarity [%s]."), *GetRarityName(DefaultRarity));
		}
		if (AffixMode != EAffixMode::Generated)
		{
			return TEXT("Affixes of this item are set in its Item Definition (No Generation), so they cannot be hand-picked or left out.");
		}
	}
	else
	{
		if (ItemRarity != DefaultRarity && IsGeneratedRarity(ItemRarity) == false)
		{
			return FString::Printf(TEXT("[%s] items are only made from their own Item Definitions, this item can be Normal, Magic or Rare."),
				*GetRarityName(ItemRarity));
		}

		if (AffixMode == EAffixMode::None && ItemRarity != EObsidianItemRarity::Normal)
		{
			return FString::Printf(TEXT("Only Normal items come without affixes, [%s] item needs them."), *GetRarityName(ItemRarity));
		}

		const bool bNeedsPrefixOrSuffix = ItemRarity == EObsidianItemRarity::Magic || ItemRarity == EObsidianItemRarity::Rare;
		if (AffixMode == EAffixMode::Generated && bNeedsPrefixOrSuffix && CountRollablePrefixesAndSuffixes(ItemDefault, ItemLevel) == 0)
		{
			return FString::Printf(TEXT("None of the Affix Lists has a Prefix or Suffix for [%s] at item level [%d], so the generated item"
				" would be [%s] without having any."), *ItemDefault->GetItemCategoryTag().ToString(), ItemLevel, *GetRarityName(ItemRarity));
		}
	}

	if (AffixMode == EAffixMode::HandPicked)
	{
		int32 PickedPrefixesAndSuffixes = 0;
		for (int32 i = 0; i < Request.PickedAffixes.Num(); ++i)
		{
			const FAffixEntry& PickedAffix = Request.PickedAffixes[i];
			const FString AffixBlocker = GetAffixBlocker(Request, PickedAffix, MakeArrayView(Request.PickedAffixes.GetData(), i));
			if (AffixBlocker.IsEmpty() == false)
			{
				return FString::Printf(TEXT("%s: %s"), *GetAffixDisplayName(PickedAffix.Affix, PickedAffix.AffixType), *AffixBlocker);
			}

			if (PickedAffix.AffixType == EObsidianAffixType::Prefix || PickedAffix.AffixType == EObsidianAffixType::Suffix)
			{
				++PickedPrefixesAndSuffixes;
			}
		}

		if (PickedPrefixesAndSuffixes == 0 && (ItemRarity == EObsidianItemRarity::Magic || ItemRarity == EObsidianItemRarity::Rare))
		{
			return FString::Printf(TEXT("[%s] item needs at least one Prefix or Suffix, without them it would be a Normal item."),
				*GetRarityName(ItemRarity));
		}
	}

	if (ItemRarity == EObsidianItemRarity::Rare && CanGenerateRareItemName(ItemLevel, ItemDefault->GetItemCategoryTag()) == false)
	{
		return FString::Printf(TEXT("There are no Rare item names for [%s] at item level [%d], so the item cannot be Rare."),
			*ItemDefault->GetItemCategoryTag().ToString(), ItemLevel);
	}

	// Gather the affixes of the Item Definition that are going to be given to the item, these have no level check on their own.
	TArray<FObsidianStaticItemAffix> StaticAffixes;
	if (AffixMode == EAffixMode::Generated || (AffixMode == EAffixMode::HandPicked && Request.bIncludeStaticAffixes))
	{
		if (AffixFragment->HasPrimaryItemAffix())
		{
			StaticAffixes.Append(AffixFragment->GetPrimaryItemAffixes());
		}

		const bool bGivesStaticImplicit = AffixMode == EAffixMode::HandPicked || GenerationType != EObsidianAffixGenerationType::FullGeneration;
		if (bGivesStaticImplicit && AffixFragment->HasImplicitAffix())
		{
			StaticAffixes.Add(AffixFragment->GetStaticImplicitAffix());
		}
	}
	if (AffixMode == EAffixMode::Generated && GenerationType == EObsidianAffixGenerationType::NoGeneration)
	{
		StaticAffixes.Add(AffixFragment->GetStaticSkillImplicitAffix());
		StaticAffixes.Append(AffixFragment->GetStaticAffixes());
	}

	for (const FObsidianStaticItemAffix& StaticAffix : StaticAffixes)
	{
		if (!StaticAffix)
		{
			continue;
		}

		const int32 MinItemLevel = GetMinItemLevelOfStaticAffix(StaticAffix);
		if (MinItemLevel == INDEX_NONE)
		{
			return FString::Printf(TEXT("Affix [%s] of the Item Definition has no value ranges to roll."), *StaticAffix.AffixTag.ToString());
		}
		if (MinItemLevel > ItemLevel)
		{
			return FString::Printf(TEXT("Affix [%s] of the Item Definition needs the item level to be at least [%d]."),
				*StaticAffix.AffixTag.ToString(), MinItemLevel);
		}
	}

	return FString();
}

FString FObsidianDebugTab_Items::GetAffixBlocker(const FCraftingRequest& Request, const FAffixEntry& Affix,
	TConstArrayView<FAffixEntry> OtherPickedAffixes)
{
	using namespace ObsidianDebugItems;

	const UObsidianInventoryItemDefinition* ItemDefault = Request.GetItemDefault();
	if (ItemDefault == nullptr)
	{
		// Nothing to validate the affix against until the item is chosen.
		return FString();
	}

	const UOInventoryItemFragment_Affixes* AffixFragment = GetAffixFragment(ItemDefault);
	if (AffixFragment == nullptr)
	{
		return TEXT("Item has no Affix Fragment, so it cannot have any affixes.");
	}
	if (AffixFragment->GetGenerationType() == EObsidianAffixGenerationType::NoGeneration)
	{
		return TEXT("Affixes of this item are set in its Item Definition (No Generation).");
	}

	const EObsidianAffixType AffixType = Affix.AffixType;
	const bool bPrefixOrSuffix = AffixType == EObsidianAffixType::Prefix || AffixType == EObsidianAffixType::Suffix;
	if (bPrefixOrSuffix == false && AffixType != EObsidianAffixType::Implicit && AffixType != EObsidianAffixType::SkillImplicit)
	{
		return TEXT("Only Prefixes, Suffixes, Implicits and Skill Implicits can be hand-picked.");
	}

	if (IsAffixCompatibleWithItem(Affix.Affix, ItemDefault) == false)
	{
		return FString::Printf(TEXT("It cannot be rolled on [%s] items%s."), *ItemDefault->GetItemCategoryTag().ToString(),
			Affix.Affix.bOverride_HasBaseTypeRequirements ? TEXT(" of this base type") : TEXT(""));
	}

	const int32 ItemLevel = Request.GetItemLevel();
	if (CanAffixRollAtItemLevel(Affix.Affix, ItemLevel) == false)
	{
		const int32 MinItemLevel = GetMinItemLevelOfDynamicAffix(Affix.Affix);
		return MinItemLevel == INDEX_NONE ? FString(TEXT("It has no value ranges to roll.")) :
			FString::Printf(TEXT("It needs the item level to be at least [%d]."), MinItemLevel);
	}

	// Affixes are identified by their tag, the item can only have one of each.
	const bool bAlreadyPicked = OtherPickedAffixes.ContainsByPredicate([&Affix](const FAffixEntry& OtherAffix)
		{
			return OtherAffix.Affix == Affix.Affix;
		});
	if (bAlreadyPicked)
	{
		return TEXT("Item already has this affix.");
	}

	const bool bGivesStaticAffixes = Request.bIncludeStaticAffixes;
	const FObsidianStaticItemAffix StaticImplicit = bGivesStaticAffixes && AffixFragment->HasImplicitAffix() ?
		AffixFragment->GetStaticImplicitAffix() : FObsidianStaticItemAffix();
	if (bGivesStaticAffixes)
	{
		const bool bIsPrimaryAffix = AffixFragment->HasPrimaryItemAffix() && AffixFragment->GetPrimaryItemAffixes().ContainsByPredicate(
			[&Affix](const FObsidianStaticItemAffix& PrimaryAffix)
			{
				return PrimaryAffix == Affix.Affix;
			});
		if (bIsPrimaryAffix || (StaticImplicit && StaticImplicit == Affix.Affix))
		{
			return TEXT("Item already has this affix from its Item Definition.");
		}
	}

	const auto CountOtherAffixes = [&OtherPickedAffixes](const EObsidianAffixType OfType)
		{
			int32 Count = 0;
			for (const FAffixEntry& OtherAffix : OtherPickedAffixes)
			{
				Count += OtherAffix.AffixType == OfType ? 1 : 0;
			}
			return Count;
		};

	// The same limits the dropped items are rolled with.
	const EObsidianItemRarity ItemRarity = Request.GetRarity();
	const int32 MaxCountOfType = GetMaxAffixCountOfType(ItemRarity, AffixType);
	int32 CountOfType = CountOtherAffixes(AffixType);
	if (AffixType == EObsidianAffixType::Implicit && StaticImplicit)
	{
		++CountOfType;
	}

	if (CountOfType >= MaxCountOfType)
	{
		return MaxCountOfType == 0 ?
			FString::Printf(TEXT("[%s] item cannot have a %s."), *GetRarityName(ItemRarity), GetAffixTypeName(AffixType)) :
			FString::Printf(TEXT("[%s] item can have at most [%d] %s affix(es)."), *GetRarityName(ItemRarity), MaxCountOfType, GetAffixTypeName(AffixType));
	}

	if (bPrefixOrSuffix)
	{
		const int32 MaxPrefixAndSuffixCount = GetMaxPrefixAndSuffixCount(ItemRarity);
		if (CountOtherAffixes(EObsidianAffixType::Prefix) + CountOtherAffixes(EObsidianAffixType::Suffix) >= MaxPrefixAndSuffixCount)
		{
			return FString::Printf(TEXT("[%s] item can have at most [%d] Prefixes and Suffixes in total."), *GetRarityName(ItemRarity),
				MaxPrefixAndSuffixCount);
		}
	}

	return FString();
}

void FObsidianDebugTab_Items::GatherAffixes()
{
	AllAffixes.Reset();
	bGatheredAffixes = true;

	const UObsidianItemDataDeveloperSettings* ItemDataSettings = GetDefault<UObsidianItemDataDeveloperSettings>();
	const UObsidianItemDataConfig* ItemDataConfig = ItemDataSettings ? ItemDataSettings->ItemDataConfig.LoadSynchronous() : nullptr;
	if (ItemDataConfig == nullptr)
	{
		UE_LOG(ObLogDebugMenu, Warning, TEXT("Could not gather the affixes, Item Data Config is not set in the Item Data settings."));
		return;
	}

	for (const UObsidianAffixList* AffixList : ItemDataConfig->CommonAffixLists)
	{
		if (AffixList == nullptr)
		{
			continue;
		}

		for (const FObsidianAffixClass& AffixClass : AffixList->ReadAllAffixClasses())
		{
			for (const FObsidianDynamicItemAffix& Affix : AffixClass.ItemAffixList)
			{
				if (Affix)
				{
					AllAffixes.Add({Affix, AffixClass.AffixClassType});
				}
			}
		}
	}
}

void FObsidianDebugTab_Items::GatherItemCategories()
{
	ItemCategories.Reset();
	ItemCategoryNames.Reset();

	const FGameplayTag CategoryRootTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Item.Category")), false);
	if (CategoryRootTag.IsValid() == false)
	{
		return;
	}

	const FGameplayTagContainer CategoryTags = UGameplayTagsManager::Get().RequestGameplayTagChildren(CategoryRootTag);
	for (const FGameplayTag& CategoryTag : CategoryTags)
	{
		FString CategoryName = CategoryTag.ToString();
		CategoryName.RemoveFromStart(TEXT("Item.Category."));

		ItemCategories.Add(CategoryTag);
		ItemCategoryNames.Add(MoveTemp(CategoryName));
	}
}

UObsidianItemDropComponent* FObsidianDebugTab_Items::CreateDropComponent(const FObsidianDebugMenuContext& Context) const
{
	if (Context.Pawn == nullptr || Context.World == nullptr)
	{
		return nullptr;
	}

	const UGameInstance* GameInstance = Context.World->GetGameInstance();
	UObsidianItemDataLoaderSubsystem* ItemDataLoader = GameInstance ? GameInstance->GetSubsystem<UObsidianItemDataLoaderSubsystem>() : nullptr;
	if (ItemDataLoader == nullptr)
	{
		Context.Notify(TEXT("Could not spawn the items, Item Data Loader Subsystem is not available."));
		return nullptr;
	}

	// The component is never registered, it only lends its item generation code with the Pawn being the dropping actor.
	UObsidianItemDropComponent* DropComponent = NewObject<UObsidianItemDropComponent>(Context.Pawn);
	DropComponent->CachedItemDataLoader = ItemDataLoader;
	DropComponent->ItemDropRadius = DropRadius;
	return DropComponent;
}

bool FObsidianDebugTab_Items::CanAffixRollAtItemLevel(const FObsidianDynamicItemAffix& Affix, const int32 ItemLevel)
{
	if (Affix.MinItemLevelRequirement > ItemLevel)
	{
		return false;
	}

	for (const FObsidianAffixValueRange& AffixRange : Affix.AffixValuesDefinition.PossibleAffixRanges)
	{
		if (AffixRange.AffixTier.MinItemLevelRequirement <= ItemLevel)
		{
			return true;
		}
	}
	return false;
}

bool FObsidianDebugTab_Items::IsAffixCompatibleWithItem(const FObsidianDynamicItemAffix& Affix, const UObsidianInventoryItemDefinition* ItemDefault)
{
	if (ItemDefault == nullptr || Affix.AcceptedItemCategories.HasTagExact(ItemDefault->GetItemCategoryTag()) == false)
	{
		return false;
	}

	return Affix.bOverride_HasBaseTypeRequirements == false || Affix.RequiredItemBaseType.HasTagExact(ItemDefault->GetItemBaseTypeTag());
}

void FObsidianDebugTab_Items::RunCraftingSelfTest(const TArray<FString>& Args, UWorld* World)
{
	if (CraftingSelfTestTickerHandle.IsValid())
	{
		UE_LOG(ObLogDebugMenu, Warning, TEXT("Item crafting self-test is already waiting to be run."));
		return;
	}

	int32 Iterations = 10;
	bool bQuitWhenDone = false;
	for (const FString& Arg : Args)
	{
		if (Arg.Equals(TEXT("quit"), ESearchCase::IgnoreCase))
		{
			bQuitWhenDone = true;
		}
		else if (Arg.IsNumeric())
		{
			Iterations = FMath::Clamp(FCString::Atoi(*Arg), 1, 1000);
		}
	}

	// The command can be run before the game is ready (e.g. with -ExecCmds), so it waits for the Player and the item data.
	const double GiveUpTime = FPlatformTime::Seconds() + 120.0;
	CraftingSelfTestTickerHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda(
		[this, Iterations, bQuitWhenDone, GiveUpTime](float DeltaTime)
		{
			FObsidianDebugMenuContext Context;
			if (GEngine)
			{
				for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
				{
					UWorld* GameWorld = WorldContext.World();
					const bool bGameWorld = WorldContext.WorldType == EWorldType::Game || WorldContext.WorldType == EWorldType::PIE;
					if (GameWorld == nullptr || bGameWorld == false || GameWorld->GetNetMode() == NM_Client || GameWorld->HasBegunPlay() == false)
					{
						continue;
					}

					APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GameWorld);
					if (LocalPlayerController && LocalPlayerController->GetPawn())
					{
						Context.World = GameWorld;
						Context.PlayerController = LocalPlayerController;
						Context.Pawn = LocalPlayerController->GetPawn();
						break;
					}
				}
			}

			const UObsidianItemDataDeveloperSettings* ItemDataSettings = GetDefault<UObsidianItemDataDeveloperSettings>();
			const bool bItemDataLoaded = ItemDataSettings && ItemDataSettings->ItemDataConfig.Get() != nullptr;
			const bool bReady = Context.Pawn != nullptr && bItemDataLoaded;
			if (bReady == false && FPlatformTime::Seconds() < GiveUpTime)
			{
				return true;
			}

			CraftingSelfTestTickerHandle.Reset();
			if (bReady)
			{
				CraftingSelfTest(Context, Iterations);
			}
			else
			{
				UE_LOG(ObLogDebugMenu, Error, TEXT("Item crafting self-test was not run, there is no game World with a local Player"
					" that has a Pawn or the Item Data Config is not loaded."));
			}

			if (bQuitWhenDone)
			{
				// Gives the delivered items a few seconds to live in the World, so anything that would break on them does it.
				FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([](float DeltaTime)
					{
						FPlatformMisc::RequestExit(false);
						return false;
					}), 5.0f);
			}
			return false;
		}), 0.5f);
}

void FObsidianDebugTab_Items::CraftingSelfTest(const FObsidianDebugMenuContext& Context, const int32 Iterations)
{
	using namespace ObsidianDebugItems;

	UObsidianItemDropComponent* DropComponent = CreateDropComponent(Context);
	if (DropComponent == nullptr)
	{
		return;
	}

	GatherAffixes();
	// Every item is tested no matter what the menu is set to, the Item Picker gets filtered for the menu again the next time it is drawn.
	ItemPickerAffixModeIndex = static_cast<int32>(EAffixMode::Generated);
	ItemPicker.Refresh(UObsidianInventoryItemDefinition::StaticClass());
	ItemPickerAffixModeIndex = INDEX_NONE;

	struct FRarityStats
	{
		int32 CraftedItems = 0;
		int32 Prefixes = 0;
		int32 Suffixes = 0;
		int32 MaxPrefixes = 0;
		int32 MaxSuffixes = 0;
	};
	TMap<EObsidianItemRarity, FRarityStats> GeneratedStats;

	// One of each kind of the crafted items is also dropped or added to the Inventory once the test is done.
	TArray<FObsidianItemToDrop> ItemsToDrop;
	TArray<FObsidianItemToDrop> ItemsToAddToInventory;

	int32 ValidRequests = 0;
	int32 BlockedRequests = 0;
	int32 CraftedItems = 0;
	int32 Failures = 0;

	const auto CountAffixes = [](const FObsidianItemToDrop& Item, const EObsidianAffixType AffixType)
		{
			int32 Count = 0;
			for (const FObsidianActiveItemAffix& Affix : Item.DropAffixes)
			{
				Count += Affix.AffixType == AffixType ? 1 : 0;
			}
			return Count;
		};

	// Reports the crafted item that breaks the rules the dropped items are generated with.
	const auto VerifyItem = [&Failures, &CountAffixes](const FCraftingRequest& Request, const FObsidianItemToDrop& Item, const FString& RequestName)
		{
			const auto Fail = [&Failures, &RequestName](const FString& Reason)
				{
					++Failures;
					UE_LOG(ObLogDebugMenu, Error, TEXT("Crafting self-test: %s -> %s"), *RequestName, *Reason);
				};

			const EObsidianItemRarity Rarity = Item.DropRarity;
			const int32 Prefixes = CountAffixes(Item, EObsidianAffixType::Prefix);
			const int32 Suffixes = CountAffixes(Item, EObsidianAffixType::Suffix);
			if (Prefixes > GetMaxAffixCountOfType(Rarity, EObsidianAffixType::Prefix))
			{
				Fail(FString::Printf(TEXT("has [%d] Prefixes, more than its rarity allows."), Prefixes));
			}
			if (Suffixes > GetMaxAffixCountOfType(Rarity, EObsidianAffixType::Suffix))
			{
				Fail(FString::Printf(TEXT("has [%d] Suffixes, more than its rarity allows."), Suffixes));
			}
			if (Prefixes + Suffixes > GetMaxPrefixAndSuffixCount(Rarity))
			{
				Fail(FString::Printf(TEXT("has [%d] Prefixes and Suffixes, more than its rarity allows."), Prefixes + Suffixes));
			}
			if ((Rarity == EObsidianItemRarity::Magic || Rarity == EObsidianItemRarity::Rare) && Prefixes + Suffixes == 0
				&& GetAffixFragment(Request.GetItemDefault()))
			{
				Fail(TEXT("is Magic or Rare but has neither a Prefix nor a Suffix."));
			}
			if (CountAffixes(Item, EObsidianAffixType::Implicit) > GetMaxAffixCountOfType(Rarity, EObsidianAffixType::Implicit))
			{
				Fail(TEXT("has too many Implicits."));
			}
			if (CountAffixes(Item, EObsidianAffixType::SkillImplicit) > GetMaxAffixCountOfType(Rarity, EObsidianAffixType::SkillImplicit))
			{
				Fail(TEXT("has too many Skill Implicits."));
			}

			for (int32 i = 0; i < Item.DropAffixes.Num(); ++i)
			{
				const FObsidianActiveItemAffix& Affix = Item.DropAffixes[i];
				if (Affix.CurrentAffixValue.IsValid() == false)
				{
					Fail(FString::Printf(TEXT("affix [%s] has no rolled values."), *Affix.AffixTag.ToString()));
				}
				if (Affix.AffixType == EObsidianAffixType::None)
				{
					Fail(FString::Printf(TEXT("affix [%s] has no type."), *Affix.AffixTag.ToString()));
				}
				for (int32 j = 0; j < i; ++j)
				{
					if (Item.DropAffixes[j].AffixTag == Affix.AffixTag)
					{
						Fail(FString::Printf(TEXT("has affix [%s] more than once."), *Affix.AffixTag.ToString()));
					}
				}
			}
		};

	const int32 MaxItemLevel = GetMaxItemLevel();
	const int32 ItemLevels[] = {1, 5, 15, 30, 45, 60, MaxItemLevel};
	const EAffixMode AffixModes[] = {EAffixMode::Generated, EAffixMode::HandPicked, EAffixMode::None};

	UE_LOG(ObLogDebugMenu, Display, TEXT("Crafting self-test: started for [%d] items and [%d] hand-pickable affixes, [%d] iteration(s) each."),
		ItemPicker.GetClassPaths().Num(), AllAffixes.Num(), Iterations);

	// Affixes are identified by their tags, so the number of the unique ones is what limits how many of them an item can have.
	for (const FAffixTypeOption& AffixTypeOption : AffixTypeOptions)
	{
		if (AffixTypeOption.AffixType == EObsidianAffixType::None)
		{
			continue;
		}

		int32 AffixCount = 0;
		int32 AffixesWithStaleType = 0;
		TSet<FGameplayTag> UniqueAffixTags;
		for (const FAffixEntry& AffixEntry : AllAffixes)
		{
			if (AffixEntry.AffixType == AffixTypeOption.AffixType)
			{
				++AffixCount;
				AffixesWithStaleType += AffixEntry.Affix.AffixType != AffixEntry.AffixType ? 1 : 0;
				UniqueAffixTags.Add(AffixEntry.Affix.AffixTag);
			}
		}
		UE_LOG(ObLogDebugMenu, Display, TEXT("Crafting self-test: [%s] affixes: [%d] entries, [%d] unique tags, [%d] with a type that differs from their class."),
			AffixTypeOption.Name, AffixCount, UniqueAffixTags.Num(), AffixesWithStaleType);
	}

	for (const FSoftClassPath& ItemDefPath : ItemPicker.GetClassPaths())
	{
		UClass* ItemDefClass = ItemDefPath.TryLoadClass<UObsidianInventoryItemDefinition>();
		const UObsidianInventoryItemDefinition* ItemDefault = ItemDefClass ? GetDefault<UObsidianInventoryItemDefinition>(ItemDefClass) : nullptr;
		if (ItemDefault == nullptr)
		{
			continue;
		}

		const FString ItemName = ObsidianDebugUI::GetCleanClassName(ItemDefClass);
		const UOInventoryItemFragment_Affixes* AffixFragment = GetAffixFragment(ItemDefault);
		const bool bGeneratesAffixes = AffixFragment && AffixFragment->GetGenerationType() != EObsidianAffixGenerationType::NoGeneration;

		for (const FItemRarityOption& RarityOption : ItemRarityOptions)
		{
			for (const int32 TestedItemLevel : ItemLevels)
			{
				for (const EAffixMode AffixMode : AffixModes)
				{
					for (int32 Iteration = 0; Iteration < Iterations; ++Iteration)
					{
						FCraftingRequest Request;
						Request.ItemDef = ItemDefClass;
						Request.ChosenRarity = RarityOption.Rarity;
						Request.ItemLevel = TestedItemLevel;
						Request.AffixMode = AffixMode;
						Request.bIncludeStaticAffixes = FMath::RandBool();
						Request.bApplyMagicMultiplier = FMath::RandBool();

						if (AffixMode == EAffixMode::HandPicked)
						{
							// Picks random affixes the same way the menu lets them be picked, so only the ones that fit the item.
							TArray<FAffixEntry> ShuffledAffixes = AllAffixes;
							Algo::RandomShuffle(ShuffledAffixes);
							for (const FAffixEntry& AffixEntry : ShuffledAffixes)
							{
								if (FMath::FRand() < 0.7f && GetAffixBlocker(Request, AffixEntry, Request.PickedAffixes).IsEmpty())
								{
									Request.PickedAffixes.Add(AffixEntry);
								}
							}
						}

						const FString RequestName = FString::Printf(TEXT("[%s] as [%s], item level [%d], affix mode [%d], [%d] picked affix(es)"),
							*ItemName, RarityOption.Name, TestedItemLevel, static_cast<int32>(AffixMode), Request.PickedAffixes.Num());

						const FString CraftingBlocker = GetCraftingBlocker(Request);
						if (CraftingBlocker.IsEmpty() == false)
						{
							++BlockedRequests;
							UE_LOG(ObLogDebugMenu, Verbose, TEXT("Crafting self-test: %s is blocked. %s"), *RequestName, *CraftingBlocker);
							break; // Only the picked affixes differ between the iterations, and these are valid by construction.
						}

						// Logged before crafting, so the request is known if the item generation asserts.
						UE_LOG(ObLogDebugMenu, Log, TEXT("Crafting self-test: crafting %s."), *RequestName);
						++ValidRequests;

						FObsidianItemToDrop CraftedItem;
						if (ConstructCraftedItem(DropComponent, Request, CraftedItem) == false)
						{
							++Failures;
							UE_LOG(ObLogDebugMenu, Error, TEXT("Crafting self-test: %s -> could not be constructed."), *RequestName);
							continue;
						}
						++CraftedItems;

						VerifyItem(Request, CraftedItem, RequestName);

						if (Iteration == 0 && TestedItemLevel == MaxItemLevel)
						{
							FObsidianItemToDrop ItemToDeliver = CraftedItem;
							ItemToDeliver.DropTransform = DropComponent->GetDropTransformAligned(Context.Pawn);
							(ItemsToDrop.Num() <= ItemsToAddToInventory.Num() ? ItemsToDrop : ItemsToAddToInventory).Add(MoveTemp(ItemToDeliver));
						}

						if (AffixMode == EAffixMode::Generated && bGeneratesAffixes)
						{
							FRarityStats& Stats = GeneratedStats.FindOrAdd(CraftedItem.DropRarity);
							const int32 Prefixes = CountAffixes(CraftedItem, EObsidianAffixType::Prefix);
							const int32 Suffixes = CountAffixes(CraftedItem, EObsidianAffixType::Suffix);
							++Stats.CraftedItems;
							Stats.Prefixes += Prefixes;
							Stats.Suffixes += Suffixes;
							Stats.MaxPrefixes = FMath::Max(Stats.MaxPrefixes, Prefixes);
							Stats.MaxSuffixes = FMath::Max(Stats.MaxSuffixes, Suffixes);
						}

						// Runs the crafted item through the same code the Inventory and the item descriptions use.
						UObsidianInventoryItemInstance* Instance = UObsidianItemsFunctionLibrary::CreateItemInstanceFromDefinition(
							Context.PlayerController, Request.ItemDef, MakeGeneratedData(CraftedItem), FObsidianItemPosition());
						FObsidianItemStats ItemStats;
						UObsidianItemsFunctionLibrary::GetItemStats(Context.GetObsidianPC(), Instance, ItemStats);
						Instance->SetIdentified(true);
						UObsidianItemsFunctionLibrary::GetItemStats(Context.GetObsidianPC(), Instance, ItemStats);
						if (Instance->GetAllItemAffixes().Num() != CraftedItem.DropAffixes.Num())
						{
							++Failures;
							UE_LOG(ObLogDebugMenu, Error, TEXT("Crafting self-test: %s -> Item Instance has [%d] affixes instead of [%d]."),
								*RequestName, Instance->GetAllItemAffixes().Num(), CraftedItem.DropAffixes.Num());
						}
					}
				}
			}
		}

		// Requests that break the rules need to be blocked, here a Normal item with a Prefix or Suffix and too many affixes of a Magic item.
		if (bGeneratesAffixes)
		{
			TArray<FAffixEntry> FittingPrefixesAndSuffixes;
			for (const FAffixEntry& AffixEntry : AllAffixes)
			{
				const bool bPrefixOrSuffix = AffixEntry.AffixType == EObsidianAffixType::Prefix || AffixEntry.AffixType == EObsidianAffixType::Suffix;
				if (bPrefixOrSuffix && IsAffixCompatibleWithItem(AffixEntry.Affix, ItemDefault) && CanAffixRollAtItemLevel(AffixEntry.Affix, MaxItemLevel)
					&& FittingPrefixesAndSuffixes.ContainsByPredicate([&AffixEntry](const FAffixEntry& Other){ return Other.Affix == AffixEntry.Affix; }) == false)
				{
					FittingPrefixesAndSuffixes.Add(AffixEntry);
				}
			}

			FCraftingRequest Request;
			Request.ItemDef = ItemDefClass;
			Request.ItemLevel = MaxItemLevel;
			Request.AffixMode = EAffixMode::HandPicked;

			if (FittingPrefixesAndSuffixes.Num() >= 1)
			{
				Request.ChosenRarity = EObsidianItemRarity::Normal;
				Request.PickedAffixes = {FittingPrefixesAndSuffixes[0]};
				if (GetCraftingBlocker(Request).IsEmpty())
				{
					++Failures;
					UE_LOG(ObLogDebugMenu, Error, TEXT("Crafting self-test: Normal [%s] with a Prefix or Suffix is not blocked."), *ItemName);
				}
			}

			const int32 TooManyForMagic = GetMaxPrefixAndSuffixCount(EObsidianItemRarity::Magic) + 1;
			if (FittingPrefixesAndSuffixes.Num() >= TooManyForMagic)
			{
				Request.ChosenRarity = EObsidianItemRarity::Magic;
				Request.PickedAffixes = TArray<FAffixEntry>(FittingPrefixesAndSuffixes.GetData(), TooManyForMagic);
				if (GetCraftingBlocker(Request).IsEmpty())
				{
					++Failures;
					UE_LOG(ObLogDebugMenu, Error, TEXT("Crafting self-test: Magic [%s] with [%d] Prefixes and Suffixes is not blocked."),
						*ItemName, TooManyForMagic);
				}
			}

			for (const EObsidianItemRarity BlockedRarity : {EObsidianItemRarity::Magic, EObsidianItemRarity::Rare})
			{
				Request.ChosenRarity = BlockedRarity;
				Request.PickedAffixes.Reset();
				Request.AffixMode = EAffixMode::None;
				if (ItemDefault->GetItemDefaultRarity() != BlockedRarity && GetCraftingBlocker(Request).IsEmpty())
				{
					++Failures;
					UE_LOG(ObLogDebugMenu, Error, TEXT("Crafting self-test: [%s] [%s] without affixes is not blocked."),
						*GetRarityName(BlockedRarity), *ItemName);
				}
			}
		}
	}

	// Random Drops run the whole drop flow, from picking the Treasure Classes to spawning the items.
	const UEnum* EntityRarityEnum = StaticEnum<EObsidianEntityRarity>();
	int32 RolledDrops = 0;
	for (int32 EntityRarityValue = 1; EntityRarityValue < EntityRarityEnum->NumEnums() - 1; ++EntityRarityValue)
	{
		for (const int32 TestedEntityLevel : ItemLevels)
		{
			UE_LOG(ObLogDebugMenu, Log, TEXT("Crafting self-test: rolling drops of [%s] entity of level [%d]."),
				*EntityRarityEnum->GetNameStringByIndex(EntityRarityValue), TestedEntityLevel);

			for (int32 Iteration = 0; Iteration < FMath::Min(Iterations, 3); ++Iteration)
			{
				DropComponent->DropItems(static_cast<EObsidianEntityRarity>(EntityRarityEnum->GetValueByIndex(EntityRarityValue)),
					static_cast<uint8>(TestedEntityLevel));
				++RolledDrops;
			}
		}
	}
	UE_LOG(ObLogDebugMenu, Display, TEXT("Crafting self-test: rolled the Random Drops [%d] time(s)."), RolledDrops);

	DropComponent->DestroyComponent();

	DeliverItems(Context, MoveTemp(ItemsToDrop), TEXT("self-test items"), EDestination::Ground);
	DeliverItems(Context, MoveTemp(ItemsToAddToInventory), TEXT("self-test items"), EDestination::Inventory);

	for (const TPair<EObsidianItemRarity, FRarityStats>& StatsPair : GeneratedStats)
	{
		const FRarityStats& Stats = StatsPair.Value;
		UE_LOG(ObLogDebugMenu, Display, TEXT("Crafting self-test: generated [%d] [%s] items, Prefixes avg [%.2f] max [%d], Suffixes avg [%.2f] max [%d]."),
			Stats.CraftedItems, *GetRarityName(StatsPair.Key),
			static_cast<float>(Stats.Prefixes) / FMath::Max(Stats.CraftedItems, 1), Stats.MaxPrefixes,
			static_cast<float>(Stats.Suffixes) / FMath::Max(Stats.CraftedItems, 1), Stats.MaxSuffixes);
	}

	Context.Notify(FString::Printf(TEXT("Crafting self-test: finished with [%d] failure(s). Crafted [%d] items from [%d] valid requests, [%d] requests were blocked."),
		Failures, CraftedItems, ValidRequests, BlockedRequests));
}

#endif // WITH_OBSIDIAN_DEBUG_MENU
