// Copyright 2026 out of sCope team - intrxx

#include "CoreMinimal.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "CQTest.h"

#include "InventoryItems/Equipment/ObsidianEquipmentComponent.h"
#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "ObsidianItemTestDefinitions.h"
#include "ObsidianItemTestFixture.h"


/**
 * Tests of UObsidianEquipmentComponent, run them from Session Frontend -> Automation -> Obsidian.Items.Equipment.
 */
TEST_CLASS(ObsidianEquipmentTests, "Obsidian.Items.Equipment")
{
	TUniquePtr<FObsidianItemTestEnvironment> Env;

	FGameplayTag HelmetSlot = ObsidianGameplayTags::Item::Slot::Equipment::Helmet;
	FGameplayTag BootsSlot = ObsidianGameplayTags::Item::Slot::Equipment::Boots;
	FGameplayTag RightHandSlot = ObsidianGameplayTags::Item::Slot::Equipment::Weapon::RightHand;
	FGameplayTag LeftHandSlot = ObsidianGameplayTags::Item::Slot::Equipment::Weapon::LeftHand;

	BEFORE_EACH()
	{
		Env = MakeUnique<FObsidianItemTestEnvironment>();
	}

	AFTER_EACH()
	{
		Env.Reset();
	}

	/**
	 * Equipping.
	 */

	TEST_METHOD(EquipItemToSpecificSlot_Definition_EquipsNewItem)
	{
		UObsidianEquipmentComponent& Equipment = Env->Equipment();

		const FObsidianEquipmentResult Result = Equipment.EquipItemToSpecificSlot(UObsidianTestItemDef_Helmet::StaticClass(),
			HelmetSlot, Env->MakeItemData());

		ASSERT_THAT(IsTrue(Result.bActionSuccessful));
		ASSERT_THAT(IsNotNull(Result.AffectedInstance));
		ASSERT_THAT(IsTrue(Equipment.IsItemEquippedAtSlot(HelmetSlot)));
		ASSERT_THAT(IsTrue(Equipment.GetEquippedInstanceAtSlot(HelmetSlot) == Result.AffectedInstance));
		ASSERT_THAT(AreEqual(1, Equipment.GetAllEquippedItems().Num()));
		ASSERT_THAT(AreEqual(1, Env->Stacks(Result.AffectedInstance)));

		const FObsidianItemPosition Position = Result.AffectedInstance->GetItemCurrentPosition();
		ASSERT_THAT(IsTrue(Position.IsOnEquipmentSlot()));
		ASSERT_THAT(IsTrue(Position.GetItemSlotTag() == HelmetSlot));
	}

	TEST_METHOD(EquipItemToSpecificSlot_Instance_EquipsTheSameInstance)
	{
		UObsidianEquipmentComponent& Equipment = Env->Equipment();
		UObsidianInventoryItemInstance* HeldItem = Env->MakeHeldItem(UObsidianTestItemDef_Helmet::StaticClass());

		const FObsidianEquipmentResult Result = Equipment.EquipItemToSpecificSlot(HeldItem, HelmetSlot);

		ASSERT_THAT(IsTrue(Result.bActionSuccessful));
		ASSERT_THAT(IsTrue(Result.AffectedInstance == HeldItem));
		ASSERT_THAT(IsTrue(Equipment.GetEquippedInstanceAtSlot(HelmetSlot) == HeldItem));
		ASSERT_THAT(IsTrue(HeldItem->GetItemCurrentPosition().GetItemSlotTag() == HelmetSlot));
	}

	TEST_METHOD(EquipItemToSpecificSlot_SlotDoesNotAcceptItem_Fails)
	{
		UObsidianEquipmentComponent& Equipment = Env->Equipment();
		const TSubclassOf<UObsidianInventoryItemDefinition> HelmetDef = UObsidianTestItemDef_Helmet::StaticClass();

		ASSERT_THAT(IsTrue(Equipment.CanEquipTemplate(HelmetDef, BootsSlot, Env->MakeItemData()) == EObsidianEquipCheckResult::CannotEquipToSlot));

		const FObsidianEquipmentResult Result = Equipment.EquipItemToSpecificSlot(HelmetDef, BootsSlot, Env->MakeItemData());

		ASSERT_THAT(IsFalse(Result.bActionSuccessful));
		ASSERT_THAT(IsFalse(Equipment.IsItemEquippedAtSlot(BootsSlot)));
		ASSERT_THAT(AreEqual(0, Equipment.GetAllEquippedItems().Num()));
	}

	TEST_METHOD(EquipItemToSpecificSlot_NotEquippableItem_Fails)
	{
		UObsidianEquipmentComponent& Equipment = Env->Equipment();
		const TSubclassOf<UObsidianInventoryItemDefinition> LargeDef = UObsidianTestItemDef_Large::StaticClass();

		ASSERT_THAT(IsTrue(Equipment.CanEquipTemplate(LargeDef, HelmetSlot, Env->MakeItemData()) == EObsidianEquipCheckResult::ItemUnequippable));
		ASSERT_THAT(IsFalse(Equipment.EquipItemToSpecificSlot(LargeDef, HelmetSlot, Env->MakeItemData()).bActionSuccessful));
		ASSERT_THAT(AreEqual(0, Equipment.GetAllEquippedItems().Num()));
	}

	TEST_METHOD(UnequipItem_FreesTheSlot)
	{
		UObsidianEquipmentComponent& Equipment = Env->Equipment();

		UObsidianInventoryItemInstance* Helmet = Equipment.EquipItemToSpecificSlot(UObsidianTestItemDef_Helmet::StaticClass(),
			HelmetSlot, Env->MakeItemData()).AffectedInstance;

		const FObsidianEquipmentResult Result = Equipment.UnequipItem(Helmet);

		ASSERT_THAT(IsTrue(Result.bActionSuccessful));
		ASSERT_THAT(IsTrue(Result.AffectedInstance == Helmet));
		ASSERT_THAT(IsFalse(Equipment.IsItemEquippedAtSlot(HelmetSlot)));
		ASSERT_THAT(AreEqual(0, Equipment.GetAllEquippedItems().Num()));
		ASSERT_THAT(IsFalse(Helmet->GetItemCurrentPosition().IsOnEquipmentSlot()));
	}

	TEST_METHOD(UnequipItem_ItemCanBeEquippedAgain)
	{
		UObsidianEquipmentComponent& Equipment = Env->Equipment();

		UObsidianInventoryItemInstance* Helmet = Equipment.EquipItemToSpecificSlot(UObsidianTestItemDef_Helmet::StaticClass(),
			HelmetSlot, Env->MakeItemData()).AffectedInstance;
		Equipment.UnequipItem(Helmet);

		ASSERT_THAT(IsTrue(Equipment.EquipItemToSpecificSlot(Helmet, HelmetSlot).bActionSuccessful));
		ASSERT_THAT(IsTrue(Equipment.GetEquippedInstanceAtSlot(HelmetSlot) == Helmet));
	}

	/**
	 * Automatic equipping, used when picking items up.
	 */

	TEST_METHOD(AutomaticallyEquipItem_FreeMatchingSlot_EquipsThere)
	{
		UObsidianEquipmentComponent& Equipment = Env->Equipment();

		const FObsidianEquipmentResult Result = Equipment.AutomaticallyEquipItem(UObsidianTestItemDef_Helmet::StaticClass(),
			Env->MakeItemData());

		ASSERT_THAT(IsTrue(Result.bActionSuccessful));
		ASSERT_THAT(IsTrue(Equipment.GetEquippedInstanceAtSlot(HelmetSlot) == Result.AffectedInstance));
	}

	TEST_METHOD(AutomaticallyEquipItem_SlotTaken_DoesNotReplace)
	{
		UObsidianEquipmentComponent& Equipment = Env->Equipment();

		UObsidianInventoryItemInstance* FirstHelmet = Equipment.AutomaticallyEquipItem(UObsidianTestItemDef_Helmet::StaticClass(),
			Env->MakeItemData()).AffectedInstance;
		const FObsidianEquipmentResult Result = Equipment.AutomaticallyEquipItem(UObsidianTestItemDef_Helmet::StaticClass(),
			Env->MakeItemData());

		ASSERT_THAT(IsFalse(Result.bActionSuccessful));
		ASSERT_THAT(IsTrue(Equipment.GetEquippedInstanceAtSlot(HelmetSlot) == FirstHelmet));
		ASSERT_THAT(AreEqual(1, Equipment.GetAllEquippedItems().Num()));
	}

	TEST_METHOD(AutomaticallyEquipItem_OneHandWeapons_FillRightThenLeftHand)
	{
		UObsidianEquipmentComponent& Equipment = Env->Equipment();
		const TSubclassOf<UObsidianInventoryItemDefinition> SwordDef = UObsidianTestItemDef_OneHandSword::StaticClass();

		UObsidianInventoryItemInstance* FirstSword = Equipment.AutomaticallyEquipItem(SwordDef, Env->MakeItemData()).AffectedInstance;
		UObsidianInventoryItemInstance* SecondSword = Equipment.AutomaticallyEquipItem(SwordDef, Env->MakeItemData()).AffectedInstance;
		const FObsidianEquipmentResult ThirdResult = Equipment.AutomaticallyEquipItem(SwordDef, Env->MakeItemData());

		ASSERT_THAT(IsNotNull(FirstSword));
		ASSERT_THAT(IsNotNull(SecondSword));
		ASSERT_THAT(IsTrue(Equipment.GetEquippedInstanceAtSlot(RightHandSlot) == FirstSword));
		ASSERT_THAT(IsTrue(Equipment.GetEquippedInstanceAtSlot(LeftHandSlot) == SecondSword));
		ASSERT_THAT(IsFalse(ThirdResult.bActionSuccessful));
		ASSERT_THAT(AreEqual(2, Equipment.GetAllEquippedItems().Num()));
	}

	TEST_METHOD(AutomaticallyEquipItem_Instance_EquipsTheSameInstance)
	{
		UObsidianEquipmentComponent& Equipment = Env->Equipment();
		UObsidianInventoryItemInstance* HeldItem = Env->MakeHeldItem(UObsidianTestItemDef_Shield::StaticClass());

		const FObsidianEquipmentResult Result = Equipment.AutomaticallyEquipItem(HeldItem);

		ASSERT_THAT(IsTrue(Result.bActionSuccessful));
		ASSERT_THAT(IsTrue(Result.AffectedInstance == HeldItem));
		ASSERT_THAT(IsTrue(Equipment.GetEquippedInstanceAtSlot(LeftHandSlot) == HeldItem));
	}

	/**
	 * Weapons that share the two hand slots.
	 */

	TEST_METHOD(TwoHandWeapon_WithOtherHandTaken_CanNotBeEquippedDirectly)
	{
		UObsidianEquipmentComponent& Equipment = Env->Equipment();
		const TSubclassOf<UObsidianInventoryItemDefinition> TwoHandDef = UObsidianTestItemDef_TwoHandSword::StaticClass();

		Equipment.EquipItemToSpecificSlot(UObsidianTestItemDef_Shield::StaticClass(), LeftHandSlot, Env->MakeItemData());

		ASSERT_THAT(IsTrue(Equipment.CanEquipTemplate(TwoHandDef, RightHandSlot, Env->MakeItemData())
			== EObsidianEquipCheckResult::UnableToEquip_DoesNotFitWithOtherWeaponType));
		ASSERT_THAT(IsFalse(Equipment.EquipItemToSpecificSlot(TwoHandDef, RightHandSlot, Env->MakeItemData()).bActionSuccessful));
		ASSERT_THAT(IsFalse(Equipment.AutomaticallyEquipItem(TwoHandDef, Env->MakeItemData()).bActionSuccessful));
		ASSERT_THAT(IsFalse(Equipment.IsItemEquippedAtSlot(RightHandSlot)));
	}

	TEST_METHOD(TwoHandWeapon_Equipped_BlocksTheOtherHand)
	{
		UObsidianEquipmentComponent& Equipment = Env->Equipment();
		const TSubclassOf<UObsidianInventoryItemDefinition> ShieldDef = UObsidianTestItemDef_Shield::StaticClass();

		ASSERT_THAT(IsTrue(Equipment.EquipItemToSpecificSlot(UObsidianTestItemDef_TwoHandSword::StaticClass(), RightHandSlot,
			Env->MakeItemData()).bActionSuccessful));

		ASSERT_THAT(IsTrue(Equipment.CanEquipTemplate(ShieldDef, LeftHandSlot, Env->MakeItemData())
			== EObsidianEquipCheckResult::UnableToEquip_DoesNotFitWithOtherWeaponType));
		ASSERT_THAT(IsFalse(Equipment.EquipItemToSpecificSlot(ShieldDef, LeftHandSlot, Env->MakeItemData()).bActionSuccessful));
	}

	TEST_METHOD(OneHandWeapon_FitsWithShieldInOtherHand)
	{
		UObsidianEquipmentComponent& Equipment = Env->Equipment();

		Equipment.EquipItemToSpecificSlot(UObsidianTestItemDef_Shield::StaticClass(), LeftHandSlot, Env->MakeItemData());

		ASSERT_THAT(IsTrue(Equipment.EquipItemToSpecificSlot(UObsidianTestItemDef_OneHandSword::StaticClass(), RightHandSlot,
			Env->MakeItemData()).bActionSuccessful));
		ASSERT_THAT(AreEqual(2, Equipment.GetAllEquippedItems().Num()));
	}

	TEST_METHOD(ReplaceItemAtSpecificSlot_TwoHandWeapon_MovesOtherHandItemToInventory)
	{
		UObsidianEquipmentComponent& Equipment = Env->Equipment();
		UObsidianInventoryComponent& Inventory = Env->Inventory();
		const TSubclassOf<UObsidianInventoryItemDefinition> TwoHandDef = UObsidianTestItemDef_TwoHandSword::StaticClass();

		UObsidianInventoryItemInstance* Shield = Equipment.EquipItemToSpecificSlot(UObsidianTestItemDef_Shield::StaticClass(),
			LeftHandSlot, Env->MakeItemData()).AffectedInstance;

		ASSERT_THAT(IsTrue(Equipment.CanReplaceTemplate(TwoHandDef, RightHandSlot, Env->MakeItemData()) == EObsidianEquipCheckResult::CanEquip));

		const FObsidianEquipmentResult Result = Equipment.ReplaceItemAtSpecificSlot(TwoHandDef, RightHandSlot, Env->MakeItemData());

		ASSERT_THAT(IsTrue(Result.bActionSuccessful));
		ASSERT_THAT(IsTrue(Equipment.GetEquippedInstanceAtSlot(RightHandSlot) == Result.AffectedInstance));
		ASSERT_THAT(IsFalse(Equipment.IsItemEquippedAtSlot(LeftHandSlot)));
		ASSERT_THAT(AreEqual(1, Equipment.GetAllEquippedItems().Num()));
		ASSERT_THAT(AreEqual(1, Inventory.GetAllItems().Num()));
		ASSERT_THAT(IsTrue(Inventory.GetAllItems()[0] == Shield));
		ASSERT_THAT(IsTrue(Shield->GetItemCurrentPosition().IsOnInventoryGrid()));
	}

	TEST_METHOD(ReplaceItemAtSpecificSlot_TwoHandWeapon_FailsWhenInventoryHasNoRoom)
	{
		UObsidianEquipmentComponent& Equipment = Env->Equipment();
		UObsidianInventoryComponent& Inventory = Env->Inventory();
		const TSubclassOf<UObsidianInventoryItemDefinition> TwoHandDef = UObsidianTestItemDef_TwoHandSword::StaticClass();

		UObsidianInventoryItemInstance* Shield = Equipment.EquipItemToSpecificSlot(UObsidianTestItemDef_Shield::StaticClass(),
			LeftHandSlot, Env->MakeItemData()).AffectedInstance;

		// Leaves only the last row free, the 2x2 Shield does not fit there.
		while(Inventory.CanFitItemDefinition(UObsidianTestItemDef_Large::StaticClass()))
		{
			Inventory.AddItemDefinition(UObsidianTestItemDef_Large::StaticClass(), Env->MakeItemData());
		}
		const int32 ItemsInInventory = Inventory.GetAllItems().Num();

		ASSERT_THAT(IsTrue(Equipment.CanReplaceTemplate(TwoHandDef, RightHandSlot, Env->MakeItemData())
			== EObsidianEquipCheckResult::UnableToEquip_NoSufficientInventorySpace));

		const FObsidianEquipmentResult Result = Equipment.ReplaceItemAtSpecificSlot(TwoHandDef, RightHandSlot, Env->MakeItemData());

		ASSERT_THAT(IsFalse(Result.bActionSuccessful));
		ASSERT_THAT(IsTrue(Equipment.GetEquippedInstanceAtSlot(LeftHandSlot) == Shield));
		ASSERT_THAT(IsFalse(Equipment.IsItemEquippedAtSlot(RightHandSlot)));
		ASSERT_THAT(AreEqual(ItemsInInventory, Inventory.GetAllItems().Num()));
	}

	TEST_METHOD(WeaponSwap_MovesWeaponsToSwapSlotsAndBack)
	{
		UObsidianEquipmentComponent& Equipment = Env->Equipment();

		UObsidianInventoryItemInstance* Sword = Equipment.EquipItemToSpecificSlot(UObsidianTestItemDef_OneHandSword::StaticClass(),
			RightHandSlot, Env->MakeItemData()).AffectedInstance;

		Equipment.WeaponSwap();

		ASSERT_THAT(IsFalse(Equipment.IsItemEquippedAtSlot(RightHandSlot)));
		ASSERT_THAT(IsTrue(Sword->GetItemCurrentPosition().GetItemSlotTag() == ObsidianGameplayTags::Item::SwapSlot::Equipment::Weapon::RightHand));
		ASSERT_THAT(AreEqual(1, Equipment.GetAllEquippedItems().Num())); // Swapped out weapons are still held by the Equipment.

		Equipment.WeaponSwap();

		ASSERT_THAT(IsTrue(Equipment.GetEquippedInstanceAtSlot(RightHandSlot) == Sword));
		ASSERT_THAT(IsTrue(Sword->GetItemCurrentPosition().GetItemSlotTag() == RightHandSlot));
	}

	TEST_METHOD(WeaponSwap_SwapsEquippedWeaponWithTheSwappedOutOne)
	{
		UObsidianEquipmentComponent& Equipment = Env->Equipment();
		const TSubclassOf<UObsidianInventoryItemDefinition> SwordDef = UObsidianTestItemDef_OneHandSword::StaticClass();

		UObsidianInventoryItemInstance* FirstSword = Equipment.EquipItemToSpecificSlot(SwordDef, RightHandSlot, Env->MakeItemData()).AffectedInstance;
		Equipment.WeaponSwap();
		UObsidianInventoryItemInstance* SecondSword = Equipment.EquipItemToSpecificSlot(SwordDef, RightHandSlot, Env->MakeItemData()).AffectedInstance;

		ASSERT_THAT(IsNotNull(SecondSword));
		ASSERT_THAT(IsTrue(Equipment.GetEquippedInstanceAtSlot(RightHandSlot) == SecondSword));

		Equipment.WeaponSwap();

		ASSERT_THAT(IsTrue(Equipment.GetEquippedInstanceAtSlot(RightHandSlot) == FirstSword));
		ASSERT_THAT(IsTrue(SecondSword->GetItemCurrentPosition().GetItemSlotTag() == ObsidianGameplayTags::Item::SwapSlot::Equipment::Weapon::RightHand));
		ASSERT_THAT(AreEqual(2, Equipment.GetAllEquippedItems().Num()));
	}

	/**
	 * Requirements.
	 */

	TEST_METHOD(CanEquipTemplate_HeroLevelTooLow_CanNotEquip)
	{
		UObsidianEquipmentComponent& Equipment = Env->Equipment();
		const TSubclassOf<UObsidianInventoryItemDefinition> HelmetDef = UObsidianTestItemDef_Helmet::StaticClass();

		FObsidianItemGeneratedData ItemData = Env->MakeItemData();
		ItemData.ItemEquippingRequirements.RequiredLevel = 10;

		Env->SetHeroLevel(9);
		ASSERT_THAT(IsTrue(Equipment.CanEquipTemplate(HelmetDef, HelmetSlot, ItemData) == EObsidianEquipCheckResult::HeroLevelTooLow));
		ASSERT_THAT(IsFalse(Equipment.EquipItemToSpecificSlot(HelmetDef, HelmetSlot, ItemData).bActionSuccessful));

		Env->SetHeroLevel(10);
		ASSERT_THAT(IsTrue(Equipment.CanEquipTemplate(HelmetDef, HelmetSlot, ItemData) == EObsidianEquipCheckResult::CanEquip));
		ASSERT_THAT(IsTrue(Equipment.EquipItemToSpecificSlot(HelmetDef, HelmetSlot, ItemData).bActionSuccessful));
	}

	TEST_METHOD(CanEquipTemplate_UnidentifiedItem_CanNotEquip)
	{
		UObsidianEquipmentComponent& Equipment = Env->Equipment();
		const TSubclassOf<UObsidianInventoryItemDefinition> HelmetDef = UObsidianTestItemDef_Helmet::StaticClass();

		// Items above the Normal rarity start unidentified unless the Item Definition says otherwise.
		FObsidianItemGeneratedData ItemData = Env->MakeItemData();
		ItemData.ItemRarity = EObsidianItemRarity::Magic;

		ASSERT_THAT(IsTrue(Equipment.CanEquipTemplate(HelmetDef, HelmetSlot, ItemData) == EObsidianEquipCheckResult::ItemUnientified));
		ASSERT_THAT(IsFalse(Equipment.EquipItemToSpecificSlot(HelmetDef, HelmetSlot, ItemData).bActionSuccessful));
	}

	/**
	 * Blocking.
	 */

	TEST_METHOD(BlockActionsTag_BlocksEveryChangeOfTheEquipment)
	{
		UObsidianEquipmentComponent& Equipment = Env->Equipment();
		const TSubclassOf<UObsidianInventoryItemDefinition> SwordDef = UObsidianTestItemDef_OneHandSword::StaticClass();

		UObsidianInventoryItemInstance* Helmet = Equipment.EquipItemToSpecificSlot(UObsidianTestItemDef_Helmet::StaticClass(),
			HelmetSlot, Env->MakeItemData()).AffectedInstance;
		UObsidianInventoryItemInstance* Sword = Equipment.EquipItemToSpecificSlot(SwordDef, RightHandSlot, Env->MakeItemData()).AffectedInstance;

		Env->AddOwnerTag(ObsidianGameplayTags::Equipment::BlockActions);

		ASSERT_THAT(IsFalse(Equipment.CanOwnerModifyEquipmentState()));
		ASSERT_THAT(IsTrue(Equipment.CanEquipTemplate(SwordDef, LeftHandSlot, Env->MakeItemData()) == EObsidianEquipCheckResult::EquipmentActionsBlocked));
		ASSERT_THAT(IsFalse(Equipment.EquipItemToSpecificSlot(SwordDef, LeftHandSlot, Env->MakeItemData()).bActionSuccessful));
		ASSERT_THAT(IsFalse(Equipment.AutomaticallyEquipItem(SwordDef, Env->MakeItemData()).bActionSuccessful));
		ASSERT_THAT(IsFalse(Equipment.UnequipItem(Helmet).bActionSuccessful));

		Equipment.WeaponSwap();

		ASSERT_THAT(IsTrue(Equipment.GetEquippedInstanceAtSlot(HelmetSlot) == Helmet));
		ASSERT_THAT(IsTrue(Equipment.GetEquippedInstanceAtSlot(RightHandSlot) == Sword));
		ASSERT_THAT(AreEqual(2, Equipment.GetAllEquippedItems().Num()));

		Env->RemoveOwnerTag(ObsidianGameplayTags::Equipment::BlockActions);

		ASSERT_THAT(IsTrue(Equipment.UnequipItem(Helmet).bActionSuccessful));
	}
};

#endif // WITH_DEV_AUTOMATION_TESTS
