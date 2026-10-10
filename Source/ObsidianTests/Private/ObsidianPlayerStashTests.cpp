// Copyright 2026 out of sCope team - intrxx

#include "CoreMinimal.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "CQTest.h"

#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/PlayerStash/ObsidianPlayerStashComponent.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "ObsidianItemTestDefinitions.h"
#include "ObsidianItemTestFixture.h"


/**
 * Tests of UObsidianPlayerStashComponent, run them from Session Frontend -> Automation -> Obsidian.Items.PlayerStash.
 * The Stash used here has two 12x10 grid tabs, a personal and a shared one, see FObsidianItemTestEnvironment.
 */
TEST_CLASS(ObsidianPlayerStashTests, "Obsidian.Items.PlayerStash")
{
	TUniquePtr<FObsidianItemTestEnvironment> Env;

	BEFORE_EACH()
	{
		Env = MakeUnique<FObsidianItemTestEnvironment>();
	}

	AFTER_EACH()
	{
		Env.Reset();
	}

	/**
	 * Stash Tabs.
	 */

	TEST_METHOD(StashTabs_AreCreatedFromConfig)
	{
		UObsidianPlayerStashComponent& Stash = Env->Stash();

		ASSERT_THAT(AreEqual(2, Stash.GetAllStashTabs().Num()));
		ASSERT_THAT(IsNotNull(Stash.GetStashTabForTag(Env->PersonalStashTab())));
		ASSERT_THAT(IsNotNull(Stash.GetStashTabForTag(Env->SharedStashTab())));
		ASSERT_THAT(IsNull(Stash.GetStashTabForTag(ObsidianGameplayTags::StashTab_Grid_3)));
	}

	TEST_METHOD(AddItemDefinition_ItemIsOnlyInTheTabItWasAddedTo)
	{
		UObsidianPlayerStashComponent& Stash = Env->Stash();

		const FObsidianItemOperationResult Result = Stash.AddItemDefinition(UObsidianTestItemDef_Large::StaticClass(),
			Env->PersonalStashTab(), Env->MakeItemData());

		ASSERT_THAT(IsTrue(Result.bActionSuccessful));
		ASSERT_THAT(IsNotNull(Result.AffectedInstance));
		ASSERT_THAT(AreEqual(1, Stash.GetAllItems().Num()));
		ASSERT_THAT(AreEqual(1, Stash.GetAllItemsFromStashTab(Env->PersonalStashTab()).Num()));
		ASSERT_THAT(AreEqual(0, Stash.GetAllItemsFromStashTab(Env->SharedStashTab()).Num()));

		const FObsidianItemPosition Position = Result.AffectedInstance->GetItemCurrentPosition();
		ASSERT_THAT(IsTrue(Position.IsOnStash()));
		ASSERT_THAT(IsTrue(Position.GetOwningStashTabTag() == Env->PersonalStashTab()));
		ASSERT_THAT(IsTrue(Stash.GetItemInstanceFromTabAtPosition(Position) == Result.AffectedInstance));
	}

	TEST_METHOD(AddItemDefinition_PersonalAndSharedItemsAreKeptApart)
	{
		UObsidianPlayerStashComponent& Stash = Env->Stash();

		UObsidianInventoryItemInstance* PersonalItem = Stash.AddItemDefinition(UObsidianTestItemDef_Large::StaticClass(),
			Env->PersonalStashTab(), Env->MakeItemData()).AffectedInstance;
		UObsidianInventoryItemInstance* SharedItem = Stash.AddItemDefinition(UObsidianTestItemDef_Large::StaticClass(),
			Env->SharedStashTab(), Env->MakeItemData()).AffectedInstance;

		// Personal items are saved with the Hero, shared ones with the Shared Stash, so they must not get mixed up.
		ASSERT_THAT(AreEqual(2, Stash.GetAllItems().Num()));
		ASSERT_THAT(AreEqual(1, Stash.GetAllPersonalItems().Num()));
		ASSERT_THAT(IsTrue(Stash.GetAllPersonalItems()[0] == PersonalItem));
		ASSERT_THAT(AreEqual(1, Stash.GetAllSharedItems().Num()));
		ASSERT_THAT(IsTrue(Stash.GetAllSharedItems()[0] == SharedItem));
	}

	TEST_METHOD(AddItemDefinition_UnknownStashTab_Fails)
	{
		UObsidianPlayerStashComponent& Stash = Env->Stash();

		const FObsidianItemOperationResult Result = Stash.AddItemDefinition(UObsidianTestItemDef_Large::StaticClass(),
			ObsidianGameplayTags::StashTab_Grid_3, Env->MakeItemData());

		ASSERT_THAT(IsFalse(Result.bActionSuccessful));
		ASSERT_THAT(AreEqual(0, Stash.GetAllItems().Num()));
	}

	/**
	 * Placing items.
	 */

	TEST_METHOD(AddItemDefinitionToSpecifiedSlot_PlacesItemAtPosition)
	{
		UObsidianPlayerStashComponent& Stash = Env->Stash();
		const FObsidianItemPosition Position = Env->PersonalStashPosition(2, 3);

		const FObsidianItemOperationResult Result = Stash.AddItemDefinitionToSpecifiedSlot(UObsidianTestItemDef_Large::StaticClass(),
			Position, Env->MakeItemData());

		ASSERT_THAT(IsTrue(Result.bActionSuccessful));
		ASSERT_THAT(IsTrue(Stash.GetItemInstanceFromTabAtPosition(Position) == Result.AffectedInstance));
		ASSERT_THAT(IsTrue(Result.AffectedInstance->GetItemCurrentPosition() == Position));
		// The same position in the other tab is not affected.
		ASSERT_THAT(IsNull(Stash.GetItemInstanceFromTabAtPosition(Env->SharedStashPosition(2, 3))));
	}

	TEST_METHOD(AddItemDefinitionToSpecifiedSlot_TakenPosition_Fails)
	{
		UObsidianPlayerStashComponent& Stash = Env->Stash();

		Stash.AddItemDefinitionToSpecifiedSlot(UObsidianTestItemDef_Large::StaticClass(), Env->PersonalStashPosition(0, 0), Env->MakeItemData());
		// Overlaps the first item with its top left square only.
		const FObsidianItemOperationResult Result = Stash.AddItemDefinitionToSpecifiedSlot(UObsidianTestItemDef_Large::StaticClass(),
			Env->PersonalStashPosition(1, 1), Env->MakeItemData());

		ASSERT_THAT(IsFalse(Result.bActionSuccessful));
		ASSERT_THAT(AreEqual(1, Stash.GetAllItems().Num()));
	}

	TEST_METHOD(AddItemDefinitionToSpecifiedSlot_SamePositionInOtherTab_Succeeds)
	{
		UObsidianPlayerStashComponent& Stash = Env->Stash();

		Stash.AddItemDefinitionToSpecifiedSlot(UObsidianTestItemDef_Large::StaticClass(), Env->PersonalStashPosition(0, 0), Env->MakeItemData());
		const FObsidianItemOperationResult Result = Stash.AddItemDefinitionToSpecifiedSlot(UObsidianTestItemDef_Large::StaticClass(),
			Env->SharedStashPosition(0, 0), Env->MakeItemData());

		ASSERT_THAT(IsTrue(Result.bActionSuccessful));
		ASSERT_THAT(AreEqual(2, Stash.GetAllItems().Num()));
	}

	TEST_METHOD(CheckSpecifiedPosition_RespectsItemSpanAndGridBounds)
	{
		UObsidianPlayerStashComponent& Stash = Env->Stash();
		const FGameplayTag Category = ObsidianGameplayTags::Item_Category_Currency_Functional;
		const FGameplayTag NoBaseType = FGameplayTag::EmptyTag;

		Stash.AddItemDefinitionToSpecifiedSlot(UObsidianTestItemDef_Large::StaticClass(), Env->PersonalStashPosition(0, 0), Env->MakeItemData());

		ASSERT_THAT(IsFalse(Stash.CheckSpecifiedPosition(Env->PersonalStashPosition(1, 1), Category, NoBaseType, FIntPoint(1, 1))));
		ASSERT_THAT(IsTrue(Stash.CheckSpecifiedPosition(Env->PersonalStashPosition(2, 0), Category, NoBaseType, FIntPoint(1, 1))));
		// The stash tab grid is 12x10, a 2x2 item does not fit in its last slot.
		ASSERT_THAT(IsTrue(Stash.CheckSpecifiedPosition(Env->PersonalStashPosition(11, 9), Category, NoBaseType, FIntPoint(1, 1))));
		ASSERT_THAT(IsFalse(Stash.CheckSpecifiedPosition(Env->PersonalStashPosition(11, 9), Category, NoBaseType, FIntPoint(2, 2))));
	}

	TEST_METHOD(RemoveItemInstance_FreesTheSpaceItemTook)
	{
		UObsidianPlayerStashComponent& Stash = Env->Stash();
		const FObsidianItemPosition Position = Env->PersonalStashPosition(0, 0);

		UObsidianInventoryItemInstance* Item = Stash.AddItemDefinitionToSpecifiedSlot(UObsidianTestItemDef_Large::StaticClass(),
			Position, Env->MakeItemData()).AffectedInstance;

		const FObsidianItemOperationResult Result = Stash.RemoveItemInstance(Item);

		ASSERT_THAT(IsTrue(Result.bActionSuccessful));
		ASSERT_THAT(IsTrue(Result.AffectedInstance == Item));
		ASSERT_THAT(AreEqual(0, Stash.GetAllItems().Num()));
		ASSERT_THAT(IsNull(Stash.GetItemInstanceFromTabAtPosition(Position)));
		ASSERT_THAT(IsTrue(Stash.CanFitInstanceInStashTab(FIntPoint(2, 2), Item->GetItemCategoryTag(), Item->GetItemBaseTypeTag(),
			Env->PersonalStashTab())));
		ASSERT_THAT(IsFalse(Item->GetItemCurrentPosition().IsOnStash()));
	}

	/**
	 * Item Instances.
	 */

	TEST_METHOD(AddItemInstance_MovesItemFromInventoryToStash)
	{
		UObsidianPlayerStashComponent& Stash = Env->Stash();
		UObsidianInventoryComponent& Inventory = Env->Inventory();

		UObsidianInventoryItemInstance* Item = Inventory.AddItemDefinition(UObsidianTestItemDef_Large::StaticClass(),
			Env->MakeItemData()).AffectedInstance;

		// The same steps the Item Manager takes when transferring an item to the Stash.
		ASSERT_THAT(IsTrue(Inventory.RemoveItemInstance(Item).bActionSuccessful));
		const FObsidianItemOperationResult Result = Stash.AddItemInstance(Item, Env->SharedStashTab());

		ASSERT_THAT(IsTrue(Result.bActionSuccessful));
		ASSERT_THAT(IsTrue(Result.AffectedInstance == Item));
		ASSERT_THAT(AreEqual(0, Inventory.GetAllItems().Num()));
		ASSERT_THAT(AreEqual(1, Stash.GetAllItemsFromStashTab(Env->SharedStashTab()).Num()));
		ASSERT_THAT(IsTrue(Item->GetItemCurrentPosition().IsOnStash()));
		ASSERT_THAT(IsTrue(Item->GetItemCurrentPosition().GetOwningStashTabTag() == Env->SharedStashTab()));
	}

	TEST_METHOD(RemoveItemInstance_MovesItemFromStashToInventory)
	{
		UObsidianPlayerStashComponent& Stash = Env->Stash();
		UObsidianInventoryComponent& Inventory = Env->Inventory();

		UObsidianInventoryItemInstance* Item = Stash.AddItemDefinition(UObsidianTestItemDef_Large::StaticClass(),
			Env->PersonalStashTab(), Env->MakeItemData()).AffectedInstance;

		// The same steps the Item Manager takes when transferring an item to the Inventory.
		ASSERT_THAT(IsTrue(Stash.RemoveItemInstance(Item).bActionSuccessful));
		ASSERT_THAT(IsTrue(Inventory.AddItemInstance(Item).bActionSuccessful));

		ASSERT_THAT(AreEqual(0, Stash.GetAllItems().Num()));
		ASSERT_THAT(AreEqual(1, Inventory.GetAllItems().Num()));
		ASSERT_THAT(IsTrue(Item->GetItemCurrentPosition().IsOnInventoryGrid()));
	}

	TEST_METHOD(AddItemInstanceToSpecificSlot_PlacesInstanceAtPosition)
	{
		UObsidianPlayerStashComponent& Stash = Env->Stash();
		UObsidianInventoryItemInstance* HeldItem = Env->MakeHeldItem(UObsidianTestItemDef_Large::StaticClass());
		const FObsidianItemPosition Position = Env->SharedStashPosition(4, 6);

		const FObsidianItemOperationResult Result = Stash.AddItemInstanceToSpecificSlot(HeldItem, Position);

		ASSERT_THAT(IsTrue(Result.bActionSuccessful));
		ASSERT_THAT(IsTrue(Result.AffectedInstance == HeldItem));
		ASSERT_THAT(IsTrue(Stash.GetItemInstanceFromTabAtPosition(Position) == HeldItem));
		ASSERT_THAT(IsTrue(HeldItem->GetItemCurrentPosition() == Position));
	}

	TEST_METHOD(AddItemInstanceToSpecificSlot_StackOverride_SplitsTheItem)
	{
		UObsidianPlayerStashComponent& Stash = Env->Stash();
		UObsidianInventoryItemInstance* HeldItem = Env->MakeHeldItem(UObsidianTestItemDef_Stackable::StaticClass(), 6);
		const FObsidianItemPosition Position = Env->PersonalStashPosition(0, 0);

		const FObsidianItemOperationResult Result = Stash.AddItemInstanceToSpecificSlot(HeldItem, Position, 2);

		// Not the whole item was added, so the action is not "successful" and the held item keeps the rest.
		ASSERT_THAT(IsFalse(Result.bActionSuccessful));
		ASSERT_THAT(AreEqual(4, Result.StacksLeft));
		ASSERT_THAT(IsNotNull(Result.AffectedInstance));
		ASSERT_THAT(IsTrue(Result.AffectedInstance != HeldItem));
		ASSERT_THAT(AreEqual(2, Env->Stacks(Result.AffectedInstance)));
		ASSERT_THAT(AreEqual(4, Env->Stacks(HeldItem)));
		ASSERT_THAT(IsTrue(Stash.GetItemInstanceFromTabAtPosition(Position) == Result.AffectedInstance));
	}

	/**
	 * Stacks.
	 */

	TEST_METHOD(AddItemDefinition_Stackable_AddsStacksToExistingItemInTheSameTab)
	{
		UObsidianPlayerStashComponent& Stash = Env->Stash();
		const TSubclassOf<UObsidianInventoryItemDefinition> StackableDef = UObsidianTestItemDef_Stackable::StaticClass();

		UObsidianInventoryItemInstance* Existing = Stash.AddItemDefinition(StackableDef, Env->PersonalStashTab(),
			Env->MakeItemData(4)).AffectedInstance;
		const FObsidianItemOperationResult Result = Stash.AddItemDefinition(StackableDef, Env->PersonalStashTab(), Env->MakeItemData(3));

		ASSERT_THAT(IsTrue(Result.bActionSuccessful));
		ASSERT_THAT(AreEqual(0, Result.StacksLeft));
		ASSERT_THAT(AreEqual(1, Stash.GetAllItems().Num()));
		ASSERT_THAT(AreEqual(7, Env->Stacks(Existing)));
	}

	TEST_METHOD(AddItemDefinition_Stackable_OverflowGoesToNewItem)
	{
		UObsidianPlayerStashComponent& Stash = Env->Stash();
		const TSubclassOf<UObsidianInventoryItemDefinition> StackableDef = UObsidianTestItemDef_Stackable::StaticClass();

		UObsidianInventoryItemInstance* Existing = Stash.AddItemDefinition(StackableDef, Env->PersonalStashTab(),
			Env->MakeItemData(8)).AffectedInstance;
		const FObsidianItemOperationResult Result = Stash.AddItemDefinition(StackableDef, Env->PersonalStashTab(), Env->MakeItemData(5));

		ASSERT_THAT(IsTrue(Result.bActionSuccessful));
		ASSERT_THAT(AreEqual(2, Stash.GetAllItems().Num()));
		ASSERT_THAT(AreEqual(UObsidianTestItemDef_Stackable::MaxStacks, Env->Stacks(Existing)));
		ASSERT_THAT(IsTrue(Result.AffectedInstance != Existing));
		ASSERT_THAT(AreEqual(3, Env->Stacks(Result.AffectedInstance)));
	}

	TEST_METHOD(TryAddingStacksToSpecificSlotWithInstance_NotEnoughRoom_MovesOnlyWhatFits)
	{
		UObsidianPlayerStashComponent& Stash = Env->Stash();
		const TSubclassOf<UObsidianInventoryItemDefinition> StackableDef = UObsidianTestItemDef_Stackable::StaticClass();
		const FObsidianItemPosition Position = Env->PersonalStashPosition(0, 0);

		UObsidianInventoryItemInstance* Target = Stash.AddItemDefinitionToSpecifiedSlot(StackableDef, Position,
			Env->MakeItemData(8)).AffectedInstance;
		UObsidianInventoryItemInstance* HeldItem = Env->MakeHeldItem(StackableDef, 5);

		const FObsidianAddingStacksResult Result = Stash.TryAddingStacksToSpecificSlotWithInstance(HeldItem, Position);

		ASSERT_THAT(IsTrue(Result.AddingStacksResult == EObsidianAddingStacksResultType::ASR_SomeOfTheStacksAdded));
		ASSERT_THAT(AreEqual(2, Result.AddedStacks));
		ASSERT_THAT(AreEqual(3, Result.StacksLeft));
		ASSERT_THAT(IsTrue(Result.LastAddedToInstance == Target));
		ASSERT_THAT(AreEqual(UObsidianTestItemDef_Stackable::MaxStacks, Env->Stacks(Target)));
		ASSERT_THAT(AreEqual(3, Env->Stacks(HeldItem)));
	}

	TEST_METHOD(TryAddingStacksToSpecificSlotWithInstance_WholeItemFits_MovesAllStacks)
	{
		UObsidianPlayerStashComponent& Stash = Env->Stash();
		const TSubclassOf<UObsidianInventoryItemDefinition> StackableDef = UObsidianTestItemDef_Stackable::StaticClass();
		const FObsidianItemPosition Position = Env->PersonalStashPosition(0, 0);

		UObsidianInventoryItemInstance* Target = Stash.AddItemDefinitionToSpecifiedSlot(StackableDef, Position,
			Env->MakeItemData(4)).AffectedInstance;
		UObsidianInventoryItemInstance* HeldItem = Env->MakeHeldItem(StackableDef, 3);

		const FObsidianAddingStacksResult Result = Stash.TryAddingStacksToSpecificSlotWithInstance(HeldItem, Position);

		ASSERT_THAT(IsTrue(Result.AddingStacksResult == EObsidianAddingStacksResultType::ASR_WholeItemAsStacksAdded));
		ASSERT_THAT(AreEqual(3, Result.AddedStacks));
		ASSERT_THAT(AreEqual(0, Result.StacksLeft));
		ASSERT_THAT(AreEqual(7, Env->Stacks(Target)));
	}

	TEST_METHOD(TryAddingStacksToSpecificSlotWithItemDef_DifferentItem_AddsNothing)
	{
		UObsidianPlayerStashComponent& Stash = Env->Stash();
		const FObsidianItemPosition Position = Env->PersonalStashPosition(0, 0);

		UObsidianInventoryItemInstance* Target = Stash.AddItemDefinitionToSpecifiedSlot(
			UObsidianTestItemDef_LimitedStackable::StaticClass(), Position, Env->MakeItemData(2)).AffectedInstance;

		const FObsidianAddingStacksResult Result = Stash.TryAddingStacksToSpecificSlotWithItemDef(
			UObsidianTestItemDef_Stackable::StaticClass(), 3, Position);

		ASSERT_THAT(IsTrue(Result.AddingStacksResult == EObsidianAddingStacksResultType::ASR_NoStacksAdded));
		ASSERT_THAT(AreEqual(3, Result.StacksLeft));
		ASSERT_THAT(AreEqual(2, Env->Stacks(Target)));
	}

	TEST_METHOD(TakeOutFromItemInstance_ValidAmount_TakesStacksOut)
	{
		UObsidianPlayerStashComponent& Stash = Env->Stash();

		UObsidianInventoryItemInstance* Item = Stash.AddItemDefinition(UObsidianTestItemDef_Stackable::StaticClass(),
			Env->PersonalStashTab(), Env->MakeItemData(6)).AffectedInstance;

		const FObsidianItemOperationResult Result = Stash.TakeOutFromItemInstance(Item, 2);

		ASSERT_THAT(IsTrue(Result.bActionSuccessful));
		ASSERT_THAT(AreEqual(2, Result.StacksLeft)); // For taking out, StacksLeft is the number of stacks that were taken.
		ASSERT_THAT(AreEqual(4, Env->Stacks(Item)));
	}

	TEST_METHOD(TakeOutFromItemInstance_InvalidAmount_ChangesNothing)
	{
		UObsidianPlayerStashComponent& Stash = Env->Stash();

		UObsidianInventoryItemInstance* Item = Stash.AddItemDefinition(UObsidianTestItemDef_Stackable::StaticClass(),
			Env->PersonalStashTab(), Env->MakeItemData(6)).AffectedInstance;

		// Taking nothing, a negative amount, the whole item or more than the item has would duplicate or void stacks.
		for(const int32 InvalidAmount : {0, -1, 6, 7})
		{
			const FObsidianItemOperationResult Result = Stash.TakeOutFromItemInstance(Item, InvalidAmount);

			ASSERT_THAT(IsFalse(Result.bActionSuccessful));
			ASSERT_THAT(AreEqual(6, Env->Stacks(Item)));
		}
	}

	/**
	 * Blocking.
	 */

	TEST_METHOD(BlockActionsTag_BlocksEveryChangeOfTheStash)
	{
		UObsidianPlayerStashComponent& Stash = Env->Stash();

		UObsidianInventoryItemInstance* Item = Stash.AddItemDefinition(UObsidianTestItemDef_Stackable::StaticClass(),
			Env->PersonalStashTab(), Env->MakeItemData(6)).AffectedInstance;
		UObsidianInventoryItemInstance* HeldItem = Env->MakeHeldItem(UObsidianTestItemDef_Large::StaticClass());

		Env->AddOwnerTag(ObsidianGameplayTags::PlayerStash_BlockActions);

		ASSERT_THAT(IsFalse(Stash.CanOwnerModifyPlayerStashState()));
		ASSERT_THAT(IsFalse(Stash.AddItemDefinition(UObsidianTestItemDef_Large::StaticClass(), Env->PersonalStashTab(),
			Env->MakeItemData()).bActionSuccessful));
		ASSERT_THAT(IsFalse(Stash.AddItemDefinitionToSpecifiedSlot(UObsidianTestItemDef_Large::StaticClass(),
			Env->PersonalStashPosition(4, 0), Env->MakeItemData()).bActionSuccessful));
		ASSERT_THAT(IsFalse(Stash.AddItemInstance(HeldItem, Env->PersonalStashTab()).bActionSuccessful));
		ASSERT_THAT(IsFalse(Stash.AddItemInstanceToSpecificSlot(HeldItem, Env->PersonalStashPosition(4, 0)).bActionSuccessful));
		ASSERT_THAT(IsFalse(Stash.TakeOutFromItemInstance(Item, 2).bActionSuccessful));
		ASSERT_THAT(IsFalse(Stash.RemoveItemInstance(Item).bActionSuccessful));
		ASSERT_THAT(AreEqual(1, Stash.GetAllItems().Num()));
		ASSERT_THAT(AreEqual(6, Env->Stacks(Item)));

		Env->RemoveOwnerTag(ObsidianGameplayTags::PlayerStash_BlockActions);

		ASSERT_THAT(IsTrue(Stash.CanOwnerModifyPlayerStashState()));
		ASSERT_THAT(IsTrue(Stash.RemoveItemInstance(Item).bActionSuccessful));
	}

	TEST_METHOD(BlockActionsTag_OfTheStash_DoesNotBlockTheInventory)
	{
		Env->AddOwnerTag(ObsidianGameplayTags::PlayerStash_BlockActions);

		ASSERT_THAT(IsTrue(Env->Inventory().AddItemDefinition(UObsidianTestItemDef_Large::StaticClass(), Env->MakeItemData()).bActionSuccessful));
	}
};

#endif // WITH_DEV_AUTOMATION_TESTS
