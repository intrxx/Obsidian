// Copyright 2024 out of sCope team - Michał Ogiński

#include "ObsidianTypes/ObsidianItemTypes.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Affixes.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"

namespace ObsidianInventoryItemsStatics
{
	using namespace ObsidianGameplayTags;
	
	const TMap<FGameplayTag, FName> SlotToAttachSocketMap =
		{
		{
			{Equipment_Slot_Weapon_RightHand, ObsidianMeshSocketNames::RightHandWeaponSocket},
			{Equipment_Slot_Weapon_LeftHand, ObsidianMeshSocketNames::LeftHandWeaponSocket}
			}
		};

	const TArray<FGameplayTag> RightHandAcceptedEquipment =
		{
		{
			Item_Category_Flail, Item_Category_Dagger, Item_Category_Wand, Item_Category_Bow_TwoHand, Item_Category_Staff_TwoHand,
			Item_Category_Mace_OneHand,Item_Category_Mace_TwoHand, Item_Category_Axe_OneHand, Item_Category_Axe_TwoHand,
			Item_Category_Sword_OneHand, Item_Category_Sword_TwoHand
			}
		};
	
	const TArray<FGameplayTag> LeftHandAcceptedEquipment =
		{
			 {
			Item_Category_Flail, Item_Category_Dagger, Item_Category_Wand, Item_Category_Mace_OneHand, Item_Category_Axe_OneHand,
			Item_Category_Sword_OneHand, Item_Category_Quiver, Item_Category_Shield
			}
		};

	const TMap<FGameplayTag, FGameplayTagContainer> DefaultAcceptedEquipmentCategories =
		{
		{
			{Equipment_Slot_Weapon_RightHand, FGameplayTagContainer::CreateFromArray(RightHandAcceptedEquipment)},
			{Equipment_Slot_Weapon_LeftHand, FGameplayTagContainer::CreateFromArray(LeftHandAcceptedEquipment)},
			{Equipment_Slot_Helmet, FGameplayTagContainer(Item_Category_Helmet)},
			{Equipment_Slot_BodyArmor, FGameplayTagContainer(Item_Category_BodyArmor)},
			{Equipment_Slot_Belt, FGameplayTagContainer(Item_Category_Belt)},
			{Equipment_Slot_Gloves, FGameplayTagContainer(Item_Category_Gloves)},
			{Equipment_Slot_Boots, FGameplayTagContainer(Item_Category_Boots)},
			{Equipment_Slot_Amulet, FGameplayTagContainer(Item_Category_Amulet)},
			{Equipment_Slot_RightRing, FGameplayTagContainer(Item_Category_Ring)},
			{Equipment_Slot_LeftRing, FGameplayTagContainer(Item_Category_Ring)}
			}
		};

	const TArray<FGameplayTag> OneHandAcceptedEquipmentCategories =
		{
			{
				Item_Category_Dagger, Item_Category_Wand, Item_Category_Flail, Item_Category_Mace_OneHand, Item_Category_Sword_OneHand,
				Item_Category_Axe_OneHand, Item_Category_Shield
			}
		};

	const TMap<FGameplayTag, FGameplayTagContainer> AcceptedSisterSlotEquipmentCategoriesPerEquipmentCategory =
		{
			{
				{Item_Category_Dagger, FGameplayTagContainer::CreateFromArray(OneHandAcceptedEquipmentCategories)},
				{Item_Category_Wand, FGameplayTagContainer::CreateFromArray(OneHandAcceptedEquipmentCategories)},
				{Item_Category_Flail, FGameplayTagContainer::CreateFromArray(OneHandAcceptedEquipmentCategories)},
				{Item_Category_Bow_TwoHand, FGameplayTagContainer(Item_Category_Quiver)},
				{Item_Category_Staff_TwoHand, FGameplayTagContainer::EmptyContainer},
				{Item_Category_Mace_OneHand, FGameplayTagContainer::CreateFromArray(OneHandAcceptedEquipmentCategories)},
				{Item_Category_Mace_TwoHand, FGameplayTagContainer::EmptyContainer},
				{Item_Category_Sword_OneHand, FGameplayTagContainer::CreateFromArray(OneHandAcceptedEquipmentCategories)},
				{Item_Category_Sword_TwoHand, FGameplayTagContainer::EmptyContainer},
				{Item_Category_Axe_OneHand, FGameplayTagContainer::CreateFromArray(OneHandAcceptedEquipmentCategories)},
				{Item_Category_Axe_TwoHand, FGameplayTagContainer::EmptyContainer},
				{Item_Category_Shield, FGameplayTagContainer::CreateFromArray(OneHandAcceptedEquipmentCategories)}
			}
		};
}

// ~ FDraggedItem

FDraggedItem::FDraggedItem(UObsidianInventoryItemInstance* InInstance)
	: Instance(InInstance)
	, Stacks(InInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current))
{}

// ~ FObsidianDescriptionAffixRow

void FObsidianAffixDescriptionRow::SetAffixRowDescription(const FText& InAffixDescription, const int32 InTempMagnitude)
{
	AffixRowDescription = FText::FromString(FString::Printf(TEXT("Adds %d %s"), InTempMagnitude, *InAffixDescription.ToString()));
	
}

void FObsidianAffixDescriptionRow::SetAffixAdditionalDescription(const EObsidianAffixType& InAffixType, const int32 InAffixTier)
{
	AffixType = InAffixType;
	FText AffixTypeText = FText();
	
	switch (InAffixType)
	{
	case EObsidianAffixType::Implicit:
		AffixTypeText = FText::FromString(FString::Printf(TEXT("Implicit, ")));
		break;
	case EObsidianAffixType::Prefix:
		AffixTypeText = FText::FromString(FString::Printf(TEXT("Prefix, ")));
		break;
	case EObsidianAffixType::Suffix:
		AffixTypeText = FText::FromString(FString::Printf(TEXT("Suffix, ")));
		break;
		default:
			AffixTypeText = FText::FromString(FString::Printf(TEXT("Affix, ")));
			break;
	}
	
	AffixAdditionalDescription = FText::FromString(FString::Printf(TEXT("%s tier: %d "), *AffixTypeText.ToString(), InAffixTier));
}

// ~ End of FObsidianDescriptionAffixRow
