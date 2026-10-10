// Copyright 2026 out of sCope team - intrxx

#include "ObsidianItemTestDefinitions.h"

#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Equippable.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Stacks.h"
#include "Obsidian/ObsidianGameplayTags.h"


namespace ObsidianItemTestDefinitions
{
	/**
	 * Fragments are configured through EditDefaultsOnly properties that are not accessible from here, reflection
	 * lets the tests set them without opening up the Fragments just for testing.
	 */
	template <typename ValueType>
	void SetFragmentProperty(UObject* Fragment, const FName PropertyName, const ValueType& Value)
	{
		const FProperty* Property = FindFProperty<FProperty>(Fragment->GetClass(), PropertyName);
		checkf(Property, TEXT("Property [%s] was not found on [%s], was it renamed?"), *PropertyName.ToString(),
			*GetNameSafe(Fragment->GetClass()));
		checkf(Property->GetElementSize() == sizeof(ValueType), TEXT("Property [%s] on [%s] is of a different type than expected."),
			*PropertyName.ToString(), *GetNameSafe(Fragment->GetClass()));

		*Property->ContainerPtrToValuePtr<ValueType>(Fragment) = Value;
	}

	void AddAppearance(UObsidianInventoryItemDefinition* Definition, const EObsidianInventoryItemGridSize GridSize)
	{
		UOInventoryItemFragment_Appearance* Appearance = Definition->CreateDefaultSubobject<UOInventoryItemFragment_Appearance>(
			TEXT("Appearance"));
		SetFragmentProperty(Appearance, TEXT("InventoryItemGridSizeDesc"), GridSize);
		Definition->ItemFragments.Add(Appearance);
	}

	/** LimitStacks of 0 means that there is no limit, same as leaving the limit out in the editor. */
	void AddStacks(UObsidianInventoryItemDefinition* Definition, const int32 MaxStacks, const int32 LimitStacks)
	{
		TMap<FGameplayTag, int32> StackNumbers;
		StackNumbers.Add(ObsidianGameplayTags::Item::StackCount::Max, MaxStacks);
		if(LimitStacks > 0)
		{
			StackNumbers.Add(ObsidianGameplayTags::Item::StackCount::Limit, LimitStacks);
		}

		UOInventoryItemFragment_Stacks* Stacks = Definition->CreateDefaultSubobject<UOInventoryItemFragment_Stacks>(TEXT("Stacks"));
		SetFragmentProperty(Stacks, TEXT("bStackable"), true);
		SetFragmentProperty(Stacks, TEXT("InventoryItemStackNumbers"), StackNumbers);
		Definition->ItemFragments.Add(Stacks);
	}

	void AddEquippable(UObsidianInventoryItemDefinition* Definition)
	{
		Definition->ItemFragments.Add(Definition->CreateDefaultSubobject<UOInventoryItemFragment_Equippable>(TEXT("Equippable")));
	}
}

UObsidianTestItemDef_Stackable::UObsidianTestItemDef_Stackable()
{
	DebugName = TEXT("Test Stackable");
	ItemCategory = ObsidianGameplayTags::Item::Category::Currency::Resource;
	bStartsIdentified = true;

	ObsidianItemTestDefinitions::AddAppearance(this, EObsidianInventoryItemGridSize::IIGS_SingleSquare);
	ObsidianItemTestDefinitions::AddStacks(this, MaxStacks, 0);
}

UObsidianTestItemDef_LimitedStackable::UObsidianTestItemDef_LimitedStackable()
{
	DebugName = TEXT("Test Limited Stackable");
	ItemCategory = ObsidianGameplayTags::Item::Category::Currency::Resource;
	bStartsIdentified = true;

	ObsidianItemTestDefinitions::AddAppearance(this, EObsidianInventoryItemGridSize::IIGS_SingleSquare);
	ObsidianItemTestDefinitions::AddStacks(this, MaxStacks, LimitStacks);
}

UObsidianTestItemDef_Large::UObsidianTestItemDef_Large()
{
	DebugName = TEXT("Test Large");
	ItemCategory = ObsidianGameplayTags::Item::Category::Currency::Functional;
	bStartsIdentified = true;

	ObsidianItemTestDefinitions::AddAppearance(this, EObsidianInventoryItemGridSize::IIGS_FourSquares_Square);
}

UObsidianTestItemDef_Helmet::UObsidianTestItemDef_Helmet()
{
	DebugName = TEXT("Test Helmet");
	ItemCategory = ObsidianGameplayTags::Item::Category::Equipment::Armor::Helmet;

	ObsidianItemTestDefinitions::AddAppearance(this, EObsidianInventoryItemGridSize::IIGS_FourSquares_Square);
	ObsidianItemTestDefinitions::AddEquippable(this);
}

UObsidianTestItemDef_OneHandSword::UObsidianTestItemDef_OneHandSword()
{
	DebugName = TEXT("Test One-Hand Sword");
	ItemCategory = ObsidianGameplayTags::Item::Category::Equipment::Weapon::Melee::OneHand::Sword;

	ObsidianItemTestDefinitions::AddAppearance(this, EObsidianInventoryItemGridSize::IIGS_ThreeSquares_Vertical);
	ObsidianItemTestDefinitions::AddEquippable(this);
}

UObsidianTestItemDef_TwoHandSword::UObsidianTestItemDef_TwoHandSword()
{
	DebugName = TEXT("Test Two-Hand Sword");
	ItemCategory = ObsidianGameplayTags::Item::Category::Equipment::Weapon::Melee::TwoHand::Sword;

	ObsidianItemTestDefinitions::AddAppearance(this, EObsidianInventoryItemGridSize::IIGS_EightSquares_VerticalRectangle);
	ObsidianItemTestDefinitions::AddEquippable(this);
}

UObsidianTestItemDef_Shield::UObsidianTestItemDef_Shield()
{
	DebugName = TEXT("Test Shield");
	ItemCategory = ObsidianGameplayTags::Item::Category::Equipment::Offhand::Shield;

	ObsidianItemTestDefinitions::AddAppearance(this, EObsidianInventoryItemGridSize::IIGS_FourSquares_Square);
	ObsidianItemTestDefinitions::AddEquippable(this);
}
