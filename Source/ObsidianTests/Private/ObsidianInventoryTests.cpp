// Copyright 2026 out of sCope team - intrxx

#include <CoreMinimal.h>

#if WITH_DEV_AUTOMATION_TESTS

#include <CQTest.h>

#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "ObsidianItemTestDefinitions.h"
#include "ObsidianItemTestFixture.h"

/**
 * Tests of UObsidianInventoryComponent, run them from Session Frontend -> Automation -> Obsidian.Items.Inventory.
 * The Inventory used here is the native default, a 12x5 grid.
 */
TEST_CLASS(ObsidianInventoryTests, "Obsidian.Items.Inventory")
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
	 * Adding Item Definitions.
	 */

	TEST_METHOD(AddItemDefinition_ToEmptyInventory_PlacesItemAtFirstSlot)
	{
		UObsidianInventoryComponent& Inventory = Env->Inventory();

		const FObsidianItemOperationResult Result = Inventory.AddItemDefinition(UObsidianTestItemDef_Large::StaticClass(),
			Env->MakeItemData());

		ASSERT_THAT(IsTrue(Result.bActionSuccessful));
		ASSERT_THAT(IsNotNull(Result.AffectedInstance));
		ASSERT_THAT(AreEqual(0, Result.StacksLeft));
		ASSERT_THAT(AreEqual(1, Inventory.GetAllItems().Num()));
		ASSERT_THAT(IsTrue(Inventory.GetItemInstanceAtLocation(FIntPoint(0, 0)) == Result.AffectedInstance));
		ASSERT_THAT(AreEqual(1, Env->Stacks(Result.AffectedInstance)));
		ASSERT_THAT(IsTrue(Result.AffectedInstance->GetItemDef() == UObsidianTestItemDef_Large::StaticClass()));
	}

	TEST_METHOD(AddItemDefinition_SetsItemPositionOnInventoryGrid)
	{
		const FObsidianItemOperationResult Result = Env->Inventory().AddItemDefinitionToSpecifiedSlot(
			UObsidianTestItemDef_Large::StaticClass(), FIntPoint(3, 2), Env->MakeItemData());

		ASSERT_THAT(IsNotNull(Result.AffectedInstance));
		const FObsidianItemPosition Position = Result.AffectedInstance->GetItemCurrentPosition();
		ASSERT_THAT(IsTrue(Position.IsOnInventoryGrid()));
		ASSERT_THAT(IsTrue(Position.GetItemGridPosition() == FIntPoint(3, 2)));
	}

	TEST_METHOD(AddItemDefinition_SecondItem_IsPlacedNextToTheFirstOne)
	{
		UObsidianInventoryComponent& Inventory = Env->Inventory();

		Inventory.AddItemDefinition(UObsidianTestItemDef_Large::StaticClass(), Env->MakeItemData());
		const FObsidianItemOperationResult Result = Inventory.AddItemDefinition(UObsidianTestItemDef_Large::StaticClass(),
			Env->MakeItemData());

		ASSERT_THAT(IsTrue(Result.bActionSuccessful));
		ASSERT_THAT(AreEqual(2, Inventory.GetAllItems().Num()));
		// The first item is 2x2 at [0, 0], so the first slot that fits another 2x2 item is [2, 0].
		ASSERT_THAT(IsTrue(Inventory.GetItemInstanceAtLocation(FIntPoint(2, 0)) == Result.AffectedInstance));
	}

	TEST_METHOD(AddItemDefinitionToSpecifiedSlot_MarksWholeItemSpanAsTaken)
	{
		UObsidianInventoryComponent& Inventory = Env->Inventory();

		Inventory.AddItemDefinitionToSpecifiedSlot(UObsidianTestItemDef_Large::StaticClass(), FIntPoint(0, 0), Env->MakeItemData());

		const FIntPoint SingleSlot(1, 1);
		ASSERT_THAT(IsFalse(Inventory.CheckSpecifiedPosition(SingleSlot, FIntPoint(0, 0))));
		ASSERT_THAT(IsFalse(Inventory.CheckSpecifiedPosition(SingleSlot, FIntPoint(1, 0))));
		ASSERT_THAT(IsFalse(Inventory.CheckSpecifiedPosition(SingleSlot, FIntPoint(0, 1))));
		ASSERT_THAT(IsFalse(Inventory.CheckSpecifiedPosition(SingleSlot, FIntPoint(1, 1))));
		ASSERT_THAT(IsTrue(Inventory.CheckSpecifiedPosition(SingleSlot, FIntPoint(2, 0))));
		ASSERT_THAT(IsTrue(Inventory.CheckSpecifiedPosition(SingleSlot, FIntPoint(0, 2))));
	}

	TEST_METHOD(AddItemDefinitionToSpecifiedSlot_TakenSlot_Fails)
	{
		UObsidianInventoryComponent& Inventory = Env->Inventory();

		Inventory.AddItemDefinitionToSpecifiedSlot(UObsidianTestItemDef_Large::StaticClass(), FIntPoint(0, 0), Env->MakeItemData());
		// Overlaps the first item with its top left square only.
		const FObsidianItemOperationResult Result = Inventory.AddItemDefinitionToSpecifiedSlot(
			UObsidianTestItemDef_Large::StaticClass(), FIntPoint(1, 1), Env->MakeItemData());

		ASSERT_THAT(IsFalse(Result.bActionSuccessful));
		ASSERT_THAT(AreEqual(1, Result.StacksLeft));
		ASSERT_THAT(AreEqual(1, Inventory.GetAllItems().Num()));
	}

	TEST_METHOD(AddItemDefinitionToSpecifiedSlot_ItemStickingOutOfGrid_Fails)
	{
		UObsidianInventoryComponent& Inventory = Env->Inventory();
		const FIntPoint LastSlot(Inventory.GetInventoryGridWidth() - 1, Inventory.GetInventoryGridHeight() - 1);

		const FObsidianItemOperationResult LargeResult = Inventory.AddItemDefinitionToSpecifiedSlot(
			UObsidianTestItemDef_Large::StaticClass(), LastSlot, Env->MakeItemData());
		const FObsidianItemOperationResult SmallResult = Inventory.AddItemDefinitionToSpecifiedSlot(
			UObsidianTestItemDef_Stackable::StaticClass(), LastSlot, Env->MakeItemData());

		ASSERT_THAT(IsFalse(LargeResult.bActionSuccessful));
		ASSERT_THAT(IsTrue(SmallResult.bActionSuccessful));
		ASSERT_THAT(AreEqual(1, Inventory.GetAllItems().Num()));
	}

	TEST_METHOD(AddItemDefinition_FullInventory_Fails)
	{
		UObsidianInventoryComponent& Inventory = Env->Inventory();

		// 2x2 items can only fill full pairs of rows, the last row of a 12x5 grid stays free.
		const int32 LargeItemsThatFit = (Inventory.GetInventoryGridWidth() / 2) * (Inventory.GetInventoryGridHeight() / 2);
		for(int32 i = 0; i < LargeItemsThatFit; ++i)
		{
			ASSERT_THAT(IsTrue(Inventory.AddItemDefinition(UObsidianTestItemDef_Large::StaticClass(), Env->MakeItemData()).bActionSuccessful));
		}

		ASSERT_THAT(IsFalse(Inventory.CanFitItemDefinition(UObsidianTestItemDef_Large::StaticClass())));
		ASSERT_THAT(IsTrue(Inventory.CanFitItemDefinition(UObsidianTestItemDef_Stackable::StaticClass())));

		const FObsidianItemOperationResult Result = Inventory.AddItemDefinition(UObsidianTestItemDef_Large::StaticClass(),
			Env->MakeItemData());

		ASSERT_THAT(IsFalse(Result.bActionSuccessful));
		ASSERT_THAT(AreEqual(LargeItemsThatFit, Inventory.GetAllItems().Num()));
		ASSERT_THAT(AreEqual(LargeItemsThatFit, Inventory.GetTotalItemCountByDefinition(UObsidianTestItemDef_Large::StaticClass())));
	}

	/**
	 * Stacks.
	 */

	TEST_METHOD(AddItemDefinition_Stackable_AddsStacksToExistingItem)
	{
		UObsidianInventoryComponent& Inventory = Env->Inventory();

		const FObsidianItemOperationResult FirstResult = Inventory.AddItemDefinition(UObsidianTestItemDef_Stackable::StaticClass(),
			Env->MakeItemData(4));
		const FObsidianItemOperationResult SecondResult = Inventory.AddItemDefinition(UObsidianTestItemDef_Stackable::StaticClass(),
			Env->MakeItemData(3));

		ASSERT_THAT(IsTrue(SecondResult.bActionSuccessful));
		ASSERT_THAT(AreEqual(0, SecondResult.StacksLeft));
		ASSERT_THAT(AreEqual(1, Inventory.GetAllItems().Num()));
		ASSERT_THAT(IsTrue(SecondResult.AffectedInstance == FirstResult.AffectedInstance));
		ASSERT_THAT(AreEqual(7, Env->Stacks(FirstResult.AffectedInstance)));
	}

	TEST_METHOD(AddItemDefinition_Stackable_OverflowGoesToNewItem)
	{
		UObsidianInventoryComponent& Inventory = Env->Inventory();

		const FObsidianItemOperationResult FirstResult = Inventory.AddItemDefinition(UObsidianTestItemDef_Stackable::StaticClass(),
			Env->MakeItemData(8));
		const FObsidianItemOperationResult SecondResult = Inventory.AddItemDefinition(UObsidianTestItemDef_Stackable::StaticClass(),
			Env->MakeItemData(5));

		ASSERT_THAT(IsTrue(SecondResult.bActionSuccessful));
		ASSERT_THAT(AreEqual(2, Inventory.GetAllItems().Num()));
		ASSERT_THAT(AreEqual(UObsidianTestItemDef_Stackable::MaxStacks, Env->Stacks(FirstResult.AffectedInstance)));
		ASSERT_THAT(IsTrue(SecondResult.AffectedInstance != FirstResult.AffectedInstance));
		ASSERT_THAT(AreEqual(3, Env->Stacks(SecondResult.AffectedInstance)));
		ASSERT_THAT(AreEqual(13, Inventory.FindAllStacksForGivenItem(UObsidianTestItemDef_Stackable::StaticClass())));
	}

	TEST_METHOD(AddItemDefinition_LimitedStackable_NeverExceedsTheLimit)
	{
		UObsidianInventoryComponent& Inventory = Env->Inventory();
		const TSubclassOf<UObsidianInventoryItemDefinition> LimitedDef = UObsidianTestItemDef_LimitedStackable::StaticClass();

		Inventory.AddItemDefinition(LimitedDef, Env->MakeItemData(5));
		// Only 3 more stacks can be held, the rest stays with whoever tried to add them.
		const FObsidianItemOperationResult OverLimitResult = Inventory.AddItemDefinition(LimitedDef, Env->MakeItemData(5));

		ASSERT_THAT(AreEqual(2, OverLimitResult.StacksLeft));
		ASSERT_THAT(AreEqual(UObsidianTestItemDef_LimitedStackable::LimitStacks, Inventory.FindAllStacksForGivenItem(LimitedDef)));

		const FObsidianItemOperationResult AtLimitResult = Inventory.AddItemDefinition(LimitedDef, Env->MakeItemData(1));

		ASSERT_THAT(IsFalse(AtLimitResult.bActionSuccessful));
		ASSERT_THAT(AreEqual(1, AtLimitResult.StacksLeft));
		ASSERT_THAT(AreEqual(UObsidianTestItemDef_LimitedStackable::LimitStacks, Inventory.FindAllStacksForGivenItem(LimitedDef)));
	}

	TEST_METHOD(AddItemDefinition_LimitedStackable_LimitHoldsAcrossMultipleItems)
	{
		UObsidianInventoryComponent& Inventory = Env->Inventory();
		const TSubclassOf<UObsidianInventoryItemDefinition> LimitedDef = UObsidianTestItemDef_LimitedStackable::StaticClass();

		// Two items with room for 3 stacks each, but only 4 more stacks can be held.
		Inventory.AddItemDefinitionToSpecifiedSlot(LimitedDef, FIntPoint(0, 0), Env->MakeItemData(2));
		Inventory.AddItemDefinitionToSpecifiedSlot(LimitedDef, FIntPoint(1, 0), Env->MakeItemData(2));
		Inventory.AddItemDefinition(LimitedDef, Env->MakeItemData(5));

		ASSERT_THAT(AreEqual(UObsidianTestItemDef_LimitedStackable::LimitStacks, Inventory.FindAllStacksForGivenItem(LimitedDef)));
	}

	TEST_METHOD(TryAddingStacksToSpecificSlotWithInstance_WholeItemFits_MovesAllStacks)
	{
		UObsidianInventoryComponent& Inventory = Env->Inventory();
		const TSubclassOf<UObsidianInventoryItemDefinition> StackableDef = UObsidianTestItemDef_Stackable::StaticClass();

		UObsidianInventoryItemInstance* Target = Inventory.AddItemDefinitionToSpecifiedSlot(StackableDef, FIntPoint(0, 0),
			Env->MakeItemData(4)).AffectedInstance;
		UObsidianInventoryItemInstance* HeldItem = Env->MakeHeldItem(StackableDef, 3);

		const FObsidianAddingStacksResult Result = Inventory.TryAddingStacksToSpecificSlotWithInstance(HeldItem, FIntPoint(0, 0));

		ASSERT_THAT(IsTrue(Result.AddingStacksResult == EObsidianAddingStacksResultType::ASR_WholeItemAsStacksAdded));
		ASSERT_THAT(AreEqual(3, Result.AddedStacks));
		ASSERT_THAT(AreEqual(0, Result.StacksLeft));
		ASSERT_THAT(IsTrue(Result.LastAddedToInstance == Target));
		ASSERT_THAT(AreEqual(7, Env->Stacks(Target)));
		ASSERT_THAT(AreEqual(0, Env->Stacks(HeldItem)));
	}

	TEST_METHOD(TryAddingStacksToSpecificSlotWithInstance_NotEnoughRoom_MovesOnlyWhatFits)
	{
		UObsidianInventoryComponent& Inventory = Env->Inventory();
		const TSubclassOf<UObsidianInventoryItemDefinition> StackableDef = UObsidianTestItemDef_Stackable::StaticClass();

		UObsidianInventoryItemInstance* Target = Inventory.AddItemDefinitionToSpecifiedSlot(StackableDef, FIntPoint(0, 0),
			Env->MakeItemData(8)).AffectedInstance;
		UObsidianInventoryItemInstance* HeldItem = Env->MakeHeldItem(StackableDef, 5);

		const FObsidianAddingStacksResult Result = Inventory.TryAddingStacksToSpecificSlotWithInstance(HeldItem, FIntPoint(0, 0));

		ASSERT_THAT(IsTrue(Result.AddingStacksResult == EObsidianAddingStacksResultType::ASR_SomeOfTheStacksAdded));
		ASSERT_THAT(AreEqual(2, Result.AddedStacks));
		ASSERT_THAT(AreEqual(3, Result.StacksLeft));
		ASSERT_THAT(AreEqual(UObsidianTestItemDef_Stackable::MaxStacks, Env->Stacks(Target)));
		ASSERT_THAT(AreEqual(3, Env->Stacks(HeldItem)));
	}

	TEST_METHOD(TryAddingStacksToSpecificSlotWithInstance_StackOverride_MovesRequestedAmount)
	{
		UObsidianInventoryComponent& Inventory = Env->Inventory();
		const TSubclassOf<UObsidianInventoryItemDefinition> StackableDef = UObsidianTestItemDef_Stackable::StaticClass();

		UObsidianInventoryItemInstance* Target = Inventory.AddItemDefinitionToSpecifiedSlot(StackableDef, FIntPoint(0, 0),
			Env->MakeItemData(4)).AffectedInstance;
		UObsidianInventoryItemInstance* HeldItem = Env->MakeHeldItem(StackableDef, 3);

		const FObsidianAddingStacksResult Result = Inventory.TryAddingStacksToSpecificSlotWithInstance(HeldItem, FIntPoint(0, 0), 1);

		ASSERT_THAT(IsTrue(Result.AddingStacksResult == EObsidianAddingStacksResultType::ASR_SomeOfTheStacksAdded));
		ASSERT_THAT(AreEqual(1, Result.AddedStacks));
		ASSERT_THAT(AreEqual(5, Env->Stacks(Target)));
		ASSERT_THAT(AreEqual(2, Env->Stacks(HeldItem)));
	}

	TEST_METHOD(TryAddingStacksToSpecificSlotWithInstance_DifferentItem_AddsNothing)
	{
		UObsidianInventoryComponent& Inventory = Env->Inventory();

		UObsidianInventoryItemInstance* Target = Inventory.AddItemDefinitionToSpecifiedSlot(
			UObsidianTestItemDef_LimitedStackable::StaticClass(), FIntPoint(0, 0), Env->MakeItemData(2)).AffectedInstance;
		UObsidianInventoryItemInstance* HeldItem = Env->MakeHeldItem(UObsidianTestItemDef_Stackable::StaticClass(), 3);

		const FObsidianAddingStacksResult Result = Inventory.TryAddingStacksToSpecificSlotWithInstance(HeldItem, FIntPoint(0, 0));

		ASSERT_THAT(IsTrue(Result.AddingStacksResult == EObsidianAddingStacksResultType::ASR_NoStacksAdded));
		ASSERT_THAT(AreEqual(2, Env->Stacks(Target)));
		ASSERT_THAT(AreEqual(3, Env->Stacks(HeldItem)));
	}

	TEST_METHOD(TryAddingStacksToSpecificSlotWithItemDef_NotEnoughRoom_AddsOnlyWhatFits)
	{
		UObsidianInventoryComponent& Inventory = Env->Inventory();
		const TSubclassOf<UObsidianInventoryItemDefinition> StackableDef = UObsidianTestItemDef_Stackable::StaticClass();

		UObsidianInventoryItemInstance* Target = Inventory.AddItemDefinitionToSpecifiedSlot(StackableDef, FIntPoint(0, 0),
			Env->MakeItemData(8)).AffectedInstance;

		const FObsidianAddingStacksResult Result = Inventory.TryAddingStacksToSpecificSlotWithItemDef(StackableDef, 5, FIntPoint(0, 0));

		ASSERT_THAT(IsTrue(Result.AddingStacksResult == EObsidianAddingStacksResultType::ASR_SomeOfTheStacksAdded));
		ASSERT_THAT(AreEqual(2, Result.AddedStacks));
		ASSERT_THAT(AreEqual(3, Result.StacksLeft));
		ASSERT_THAT(AreEqual(UObsidianTestItemDef_Stackable::MaxStacks, Env->Stacks(Target)));
	}

	TEST_METHOD(TakeOutFromItemInstance_ValidAmount_TakesStacksOut)
	{
		UObsidianInventoryComponent& Inventory = Env->Inventory();

		UObsidianInventoryItemInstance* Item = Inventory.AddItemDefinition(UObsidianTestItemDef_Stackable::StaticClass(),
			Env->MakeItemData(6)).AffectedInstance;

		const FObsidianItemOperationResult Result = Inventory.TakeOutFromItemInstance(Item, 2);

		ASSERT_THAT(IsTrue(Result.bActionSuccessful));
		ASSERT_THAT(AreEqual(2, Result.StacksLeft)); // For taking out, StacksLeft is the number of stacks that were taken.
		ASSERT_THAT(IsTrue(Result.AffectedInstance == Item));
		ASSERT_THAT(AreEqual(4, Env->Stacks(Item)));
	}

	TEST_METHOD(TakeOutFromItemInstance_InvalidAmount_ChangesNothing)
	{
		UObsidianInventoryComponent& Inventory = Env->Inventory();

		UObsidianInventoryItemInstance* Item = Inventory.AddItemDefinition(UObsidianTestItemDef_Stackable::StaticClass(),
			Env->MakeItemData(6)).AffectedInstance;

		// Taking nothing, a negative amount, the whole item or more than the item has would duplicate or void stacks.
		for(const int32 InvalidAmount : {0, -1, 6, 7})
		{
			const FObsidianItemOperationResult Result = Inventory.TakeOutFromItemInstance(Item, InvalidAmount);

			ASSERT_THAT(IsFalse(Result.bActionSuccessful));
			ASSERT_THAT(AreEqual(6, Env->Stacks(Item)));
		}
	}

	/**
	 * Item Instances.
	 */

	TEST_METHOD(RemoveItemInstance_FreesTheSpaceItemTook)
	{
		UObsidianInventoryComponent& Inventory = Env->Inventory();

		UObsidianInventoryItemInstance* Item = Inventory.AddItemDefinitionToSpecifiedSlot(UObsidianTestItemDef_Large::StaticClass(),
			FIntPoint(0, 0), Env->MakeItemData()).AffectedInstance;

		const FObsidianItemOperationResult Result = Inventory.RemoveItemInstance(Item);

		ASSERT_THAT(IsTrue(Result.bActionSuccessful));
		ASSERT_THAT(IsTrue(Result.AffectedInstance == Item));
		ASSERT_THAT(AreEqual(0, Inventory.GetAllItems().Num()));
		ASSERT_THAT(IsNull(Inventory.GetItemInstanceAtLocation(FIntPoint(0, 0))));
		ASSERT_THAT(IsTrue(Inventory.CheckSpecifiedPosition(FIntPoint(2, 2), FIntPoint(0, 0))));
		ASSERT_THAT(IsFalse(Item->GetItemCurrentPosition().IsOnInventoryGrid()));
	}

	TEST_METHOD(AddItemInstance_PlacesTheSameInstance)
	{
		UObsidianInventoryComponent& Inventory = Env->Inventory();
		UObsidianInventoryItemInstance* HeldItem = Env->MakeHeldItem(UObsidianTestItemDef_Large::StaticClass());

		const FObsidianItemOperationResult Result = Inventory.AddItemInstance(HeldItem);

		ASSERT_THAT(IsTrue(Result.bActionSuccessful));
		ASSERT_THAT(IsTrue(Result.AffectedInstance == HeldItem));
		ASSERT_THAT(AreEqual(1, Inventory.GetAllItems().Num()));
		ASSERT_THAT(IsTrue(Inventory.GetItemInstanceAtLocation(FIntPoint(0, 0)) == HeldItem));
	}

	TEST_METHOD(AddItemInstance_Stackable_MergesIntoExistingItem)
	{
		UObsidianInventoryComponent& Inventory = Env->Inventory();
		const TSubclassOf<UObsidianInventoryItemDefinition> StackableDef = UObsidianTestItemDef_Stackable::StaticClass();

		UObsidianInventoryItemInstance* Existing = Inventory.AddItemDefinitionToSpecifiedSlot(StackableDef, FIntPoint(0, 0),
			Env->MakeItemData(4)).AffectedInstance;
		UObsidianInventoryItemInstance* HeldItem = Env->MakeHeldItem(StackableDef, 3);

		const FObsidianItemOperationResult Result = Inventory.AddItemInstance(HeldItem);

		ASSERT_THAT(IsTrue(Result.bActionSuccessful));
		ASSERT_THAT(AreEqual(0, Result.StacksLeft));
		ASSERT_THAT(IsTrue(Result.AffectedInstance == Existing));
		ASSERT_THAT(AreEqual(1, Inventory.GetAllItems().Num()));
		ASSERT_THAT(AreEqual(7, Env->Stacks(Existing)));
	}

	TEST_METHOD(AddItemInstanceToSpecificSlot_PlacesInstanceAtSlot)
	{
		UObsidianInventoryComponent& Inventory = Env->Inventory();
		UObsidianInventoryItemInstance* HeldItem = Env->MakeHeldItem(UObsidianTestItemDef_Large::StaticClass());

		const FObsidianItemOperationResult Result = Inventory.AddItemInstanceToSpecificSlot(HeldItem, FIntPoint(3, 2));

		ASSERT_THAT(IsTrue(Result.bActionSuccessful));
		ASSERT_THAT(IsTrue(Result.AffectedInstance == HeldItem));
		ASSERT_THAT(IsTrue(Inventory.GetItemInstanceAtLocation(FIntPoint(3, 2)) == HeldItem));
		ASSERT_THAT(IsTrue(HeldItem->GetItemCurrentPosition().GetItemGridPosition() == FIntPoint(3, 2)));
	}

	TEST_METHOD(AddItemInstanceToSpecificSlot_TakenSlot_Fails)
	{
		UObsidianInventoryComponent& Inventory = Env->Inventory();

		Inventory.AddItemDefinitionToSpecifiedSlot(UObsidianTestItemDef_Large::StaticClass(), FIntPoint(0, 0), Env->MakeItemData());
		UObsidianInventoryItemInstance* HeldItem = Env->MakeHeldItem(UObsidianTestItemDef_Large::StaticClass());

		const FObsidianItemOperationResult Result = Inventory.AddItemInstanceToSpecificSlot(HeldItem, FIntPoint(1, 1));

		ASSERT_THAT(IsFalse(Result.bActionSuccessful));
		ASSERT_THAT(AreEqual(1, Inventory.GetAllItems().Num()));
	}

	TEST_METHOD(AddItemInstanceToSpecificSlot_StackOverride_SplitsTheItem)
	{
		UObsidianInventoryComponent& Inventory = Env->Inventory();
		UObsidianInventoryItemInstance* HeldItem = Env->MakeHeldItem(UObsidianTestItemDef_Stackable::StaticClass(), 6);

		const FObsidianItemOperationResult Result = Inventory.AddItemInstanceToSpecificSlot(HeldItem, FIntPoint(0, 0), 2);

		// Not the whole item was added, so the action is not "successful" and the held item keeps the rest.
		ASSERT_THAT(IsFalse(Result.bActionSuccessful));
		ASSERT_THAT(AreEqual(4, Result.StacksLeft));
		ASSERT_THAT(IsNotNull(Result.AffectedInstance));
		ASSERT_THAT(IsTrue(Result.AffectedInstance != HeldItem));
		ASSERT_THAT(AreEqual(2, Env->Stacks(Result.AffectedInstance)));
		ASSERT_THAT(AreEqual(4, Env->Stacks(HeldItem)));
		ASSERT_THAT(AreEqual(1, Inventory.GetAllItems().Num()));
		ASSERT_THAT(IsTrue(Inventory.GetItemInstanceAtLocation(FIntPoint(0, 0)) == Result.AffectedInstance));
	}

	/**
	 * Replacing.
	 */

	TEST_METHOD(CanReplaceItemAtSpecificSlot_ReplacedItemSpaceCountsAsFree)
	{
		UObsidianInventoryComponent& Inventory = Env->Inventory();

		Inventory.AddItemDefinitionToSpecifiedSlot(UObsidianTestItemDef_Stackable::StaticClass(), FIntPoint(0, 0), Env->MakeItemData());

		// 2x2 item takes the place of the 1x1 item it replaces.
		ASSERT_THAT(IsTrue(Inventory.CanReplaceItemAtSpecificSlotWithDef(FIntPoint(0, 0), FIntPoint(0, 0),
			UObsidianTestItemDef_Large::StaticClass())));

		// Another item in the way of the 2x2 item.
		Inventory.AddItemDefinitionToSpecifiedSlot(UObsidianTestItemDef_LimitedStackable::StaticClass(), FIntPoint(1, 1), Env->MakeItemData());

		ASSERT_THAT(IsFalse(Inventory.CanReplaceItemAtSpecificSlotWithDef(FIntPoint(0, 0), FIntPoint(0, 0),
			UObsidianTestItemDef_Large::StaticClass())));
	}

	TEST_METHOD(CanReplaceItemAtSpecificSlot_NoItemToReplace_ReturnsFalse)
	{
		ASSERT_THAT(IsFalse(Env->Inventory().CanReplaceItemAtSpecificSlotWithDef(FIntPoint(0, 0), FIntPoint(0, 0),
			UObsidianTestItemDef_Large::StaticClass())));
	}

	/**
	 * Blocking.
	 */

	TEST_METHOD(BlockActionsTag_BlocksEveryChangeOfTheInventory)
	{
		UObsidianInventoryComponent& Inventory = Env->Inventory();

		UObsidianInventoryItemInstance* Item = Inventory.AddItemDefinition(UObsidianTestItemDef_Stackable::StaticClass(),
			Env->MakeItemData(6)).AffectedInstance;
		UObsidianInventoryItemInstance* HeldItem = Env->MakeHeldItem(UObsidianTestItemDef_Large::StaticClass());

		Env->AddOwnerTag(ObsidianGameplayTags::Inventory_BlockActions);

		ASSERT_THAT(IsFalse(Inventory.CanOwnerModifyInventoryState()));
		ASSERT_THAT(IsFalse(Inventory.AddItemDefinition(UObsidianTestItemDef_Large::StaticClass(), Env->MakeItemData()).bActionSuccessful));
		ASSERT_THAT(IsFalse(Inventory.AddItemDefinitionToSpecifiedSlot(UObsidianTestItemDef_Large::StaticClass(), FIntPoint(4, 0),
			Env->MakeItemData()).bActionSuccessful));
		ASSERT_THAT(IsFalse(Inventory.AddItemInstance(HeldItem).bActionSuccessful));
		ASSERT_THAT(IsFalse(Inventory.AddItemInstanceToSpecificSlot(HeldItem, FIntPoint(4, 0)).bActionSuccessful));
		ASSERT_THAT(IsFalse(Inventory.TakeOutFromItemInstance(Item, 2).bActionSuccessful));
		ASSERT_THAT(IsFalse(Inventory.RemoveItemInstance(Item).bActionSuccessful));
		ASSERT_THAT(AreEqual(1, Inventory.GetAllItems().Num()));
		ASSERT_THAT(AreEqual(6, Env->Stacks(Item)));

		Env->RemoveOwnerTag(ObsidianGameplayTags::Inventory_BlockActions);

		ASSERT_THAT(IsTrue(Inventory.CanOwnerModifyInventoryState()));
		ASSERT_THAT(IsTrue(Inventory.RemoveItemInstance(Item).bActionSuccessful));
	}
};

#endif // WITH_DEV_AUTOMATION_TESTS
