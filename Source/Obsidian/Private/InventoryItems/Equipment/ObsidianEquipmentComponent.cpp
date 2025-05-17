// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Equipment/ObsidianEquipmentComponent.h"

#include "Core/ObsidianGameplayStatics.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "Engine/ActorChannel.h"
#include "InventoryItems/Equipment/ObsidianSpawnedEquipmentPiece.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogEquipment);

/** Defined to use in matches. */
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Obsidian_TwoHand, "TwoHand");

//
// Equipment Component 
//

UObsidianEquipmentComponent::UObsidianEquipmentComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentList(this)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);

	CreateDefaultEquipmentSlots();
}

void UObsidianEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquipmentList);
}

UObsidianInventoryItemInstance* UObsidianEquipmentComponent::GetEquippedInstanceAtSlot(const FGameplayTag& SlotTag)
{
	if(EquipmentList.SlotToEquipmentMap.Contains(SlotTag))
	{
		return EquipmentList.SlotToEquipmentMap[SlotTag];
	}
	return nullptr;
}

TArray<UObsidianInventoryItemInstance*> UObsidianEquipmentComponent::GetAllEquippedItems() const
{
	return EquipmentList.GetAllEquippedItems();
}

UObsidianInventoryItemInstance* UObsidianEquipmentComponent::GetEquipmentPieceByTag(const FGameplayTag& SlotTag) const
{
	return EquipmentList.GetEquipmentPieceByTag(SlotTag);
}

USkeletalMeshComponent* UObsidianEquipmentComponent::GetMainEquippedMeshFromSlot(const FGameplayTag& SlotTag) const
{
	if(const UObsidianInventoryItemInstance* Instance = GetEquipmentPieceByTag(SlotTag))
	{
		TArray<AObsidianSpawnedEquipmentPiece*> SpawnedPieces = Instance->GetSpawnedActors();
		const AObsidianSpawnedEquipmentPiece* Piece = SpawnedPieces[0];
		if(IsValid(Piece) && Piece->bMainEquipmentPiece) // This is most likely to be the first one anyway, no need to iterate with for
		{
			return Piece->GetEquipmentPieceMesh();
		}
			
		for(const AObsidianSpawnedEquipmentPiece* SpawnedPiece : SpawnedPieces)
		{
			if(IsValid(SpawnedPiece) && SpawnedPiece->bMainEquipmentPiece)
			{
				return SpawnedPiece->GetEquipmentPieceMesh();
			}
		}
	}
	return nullptr;
}

FObsidianEquipmentSlotDefinition UObsidianEquipmentComponent::FindEquipmentSlotByTag(const FGameplayTag& SlotTag)
{
	return EquipmentList.FindEquipmentSlotByTag(SlotTag);
}

FObsidianEquipmentSlotDefinition& UObsidianEquipmentComponent::GetEquipmentSlotReferenceByTag(const FGameplayTag& SlotTag)
{
	return EquipmentList.GetEquipmentSlotReferenceByTag(SlotTag);
}

TArray<FObsidianEquipmentSlotDefinition> UObsidianEquipmentComponent::FindMatchingEquipmentSlotsByItemCategory(const FGameplayTag& ItemCategory)
{
	return EquipmentList.FindMatchingEquipmentSlotsByItemCategory(ItemCategory);
}

FObsidianItemStats UObsidianEquipmentComponent::GetItemStatsBySlotTag(const FGameplayTag& SlotTag)
{
	const UObsidianInventoryItemInstance* ItemInstance = GetEquippedInstanceAtSlot(SlotTag);
	check(ItemInstance);

	FObsidianItemStats Stats;
	
	if(ItemInstance->IsStackable())
	{
		Stats.SetStacks(ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current),
				ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Max));
	}

	Stats.SetItemImage(ItemInstance->GetItemImage(), ItemInstance->GetItemGridSpan());
	Stats.SetDisplayName(ItemInstance->GetItemDisplayName());
	Stats.SetDescription(ItemInstance->GetItemDescription());
	Stats.SetAdditionalDescription(ItemInstance->GetItemAdditionalDescription());

	Stats.ItemRarity = ItemInstance->GetItemRarity();
	
	const bool bIdentified = ItemInstance->IsItemIdentified();
	Stats.SetIdentified(bIdentified);
	if(bIdentified)
	{
		Stats.SetAffixDescriptionRows(ItemInstance->GetAffixesAsUIDescription());
	}

	return Stats;
}

bool UObsidianEquipmentComponent::AutomaticallyEquipItem(UObsidianInventoryItemInstance* InstanceToEquip)
{
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::AddItemInstance."));
		return false; 
	}

	if(InstanceToEquip == nullptr)
	{
		return false;
	}

	const FGameplayTag& ItemCategoryTag = InstanceToEquip->GetItemCategoryTag();
	const bool bIsTwoHanded = UObsidianGameplayStatics::DoesTagMatchesAnySubTag(ItemCategoryTag, TAG_Obsidian_TwoHand);
	for(FObsidianEquipmentSlotDefinition Slot : FindMatchingEquipmentSlotsByItemCategory(ItemCategoryTag))
	{
		if(EquipmentList.SlotToEquipmentMap.Contains(Slot.SlotTag) || (bIsTwoHanded && EquipmentList.SlotToEquipmentMap.Contains(Slot.SisterSlotTag)))
		{
			continue; // We already have an item equipped in this slot, we shouldn't try to equip it. || Initial slot is free but the other one is occupied so we don't want to automatically equip.
		}
		
		if(EquipItemToSpecificSlot(InstanceToEquip, Slot.SlotTag))
		{
			return true;
		}
	}

	return false;
}

bool UObsidianEquipmentComponent::EquipItemToSpecificSlot(UObsidianInventoryItemInstance* InstanceToEquip, const FGameplayTag& SlotTag)
{
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::AddItemInstance."));
		return false; 
	}

	if(InstanceToEquip == nullptr)
	{
		return false;
	}

	const EObsidianEquipResult EquipResult = CanEquipInstance(InstanceToEquip, SlotTag);
	if(EquipResult != EObsidianEquipResult::CanEquip)
	{
		//TODO Send Client RPC with some voice over passing EquipResult?
#if !UE_BUILD_SHIPPING
		UE_LOG(LogEquipment, Warning, TEXT("Item cannot be equipped, reason: [%s]"), *ObsidianEquipmentDebugHelpers::GetEquipResultString(EquipResult));
#endif
		return false;
	}
	
	EquipmentList.AddEntry(InstanceToEquip, SlotTag);
	
	if(InstanceToEquip && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(InstanceToEquip);
	}

	return true;
}

EObsidianEquipResult UObsidianEquipmentComponent::CanEquipInstance(const UObsidianInventoryItemInstance* Instance, const FGameplayTag& SlotTag)
{
	if(Instance == nullptr)
	{
		return EObsidianEquipResult::None;
	}
	
	if(Instance->IsItemEquippable() == false)
	{
		return EObsidianEquipResult::ItemUnequippable;
	}
	
	if(Instance->IsItemIdentified() == false)
	{
		return EObsidianEquipResult::ItemUnientified;
	}

	const FGameplayTag ItemCategoryTag = Instance->GetItemCategoryTag();
	const EObsidianEquipResult Result = DoesItemFitEquipmentSlot(SlotTag, ItemCategoryTag);
	if(Result != EObsidianEquipResult::CanEquip)
	{
		return Result;
	}

	const bool bIsTwoHanded = UObsidianGameplayStatics::DoesTagMatchesAnySubTag(ItemCategoryTag, TAG_Obsidian_TwoHand);
	if(bIsTwoHanded)
	{
		UE_LOG(LogTemp, Warning, TEXT("YO YO YO TWO HANDED"));
		// 1. Check if slot in the other hand is free
		//		a. if yes - early positive return
		//		b. if no - check if the item can be unequipped and placed in the inventory (//TODO simulate any attributes removal)
		return Result; // Temp
	}
	
	return EObsidianEquipResult::CanEquip;
}

UObsidianInventoryItemInstance* UObsidianEquipmentComponent::AutomaticallyEquipItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef)
{
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::AddItemInstance."));
		return nullptr; 
	}

	if(ItemDef == nullptr)
	{
		return nullptr;
	}

	UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject();
	if(DefaultObject == nullptr)
	{
		return nullptr;
	}
	
	const FGameplayTag& ItemCategoryTag = DefaultObject->GetItemCategoryTag();
	const bool bIsTwoHanded = UObsidianGameplayStatics::DoesTagMatchesAnySubTag(ItemCategoryTag, TAG_Obsidian_TwoHand);
	for(FObsidianEquipmentSlotDefinition Slot : FindMatchingEquipmentSlotsByItemCategory(ItemCategoryTag))
	{
		if(EquipmentList.SlotToEquipmentMap.Contains(Slot.SlotTag) || (bIsTwoHanded && EquipmentList.SlotToEquipmentMap.Contains(Slot.SisterSlotTag)))
		{
			continue; // We already have an item equipped in this slot, we shouldn't try to equip it. || Initial slot is free but the other one is occupied so we don't want to automatically equip.
		}
		
		if(UObsidianInventoryItemInstance* Instance = EquipItemToSpecificSlot(ItemDef, Slot.SlotTag))
		{
			return Instance;
		}
	}

	return nullptr;
}

UObsidianInventoryItemInstance* UObsidianEquipmentComponent::EquipItemToSpecificSlot(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FGameplayTag& SlotTag)
{
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::AddItemInstance."));
		return nullptr; 
	}

	if(ItemDef == nullptr)
	{
		return nullptr;
	}

	const UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject();
	if(DefaultObject == nullptr)
	{
		return nullptr;
	}

	EObsidianEquipResult EquipResult = CanEquipTemplate(ItemDef, SlotTag);
	if(EquipResult != EObsidianEquipResult::CanEquip)
	{
		//TODO Send Client RPC to add voiceover passing EquipResult
#if !UE_BUILD_SHIPPING
		UE_LOG(LogEquipment, Warning, TEXT("Item cannot be equipped, reason: [%s]"), *ObsidianEquipmentDebugHelpers::GetEquipResultString(EquipResult));
#endif
		return nullptr;
	}
	
	UObsidianInventoryItemInstance* Instance = EquipmentList.AddEntry(ItemDef, SlotTag);

	if(Instance && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(Instance);
	}

	return Instance;
}

EObsidianEquipResult UObsidianEquipmentComponent::CanEquipTemplate(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FGameplayTag& SlotTag)
{
	if(ItemDef == nullptr)
	{
		return EObsidianEquipResult::None;
	}

	const UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject();
	if(DefaultObject == nullptr)
	{
		return EObsidianEquipResult::None;
	}
	
	if(DefaultObject->IsEquippable() == false)
	{
		return EObsidianEquipResult::ItemUnequippable;
	}
	
	if(DefaultObject->IsIdentified() == false)
	{
		return EObsidianEquipResult::ItemUnientified;
	}

	const FGameplayTag ItemCategoryTag = DefaultObject->GetItemCategoryTag();
	const EObsidianEquipResult Result = DoesItemFitEquipmentSlot(SlotTag, ItemCategoryTag);
	if(Result != EObsidianEquipResult::CanEquip)
	{
		return Result;
	}

	if(UObsidianGameplayStatics::DoesTagMatchesAnySubTag(ItemCategoryTag, TAG_Obsidian_TwoHand))
	{
		UE_LOG(LogTemp, Warning, TEXT("YO YO YO TWO HANDED"));
		return Result; // Temp
	}
	
	return EObsidianEquipResult::CanEquip;
}

void UObsidianEquipmentComponent::WeaponSwap()
{
	TArray<UObsidianInventoryItemInstance*> EquipmentToMoveToSwap = EquipmentList.GetEquippedWeapons();
	TArray<UObsidianInventoryItemInstance*> EquipmentToMoveFromSwap = EquipmentList.GetSwappedWeapons();
	
	for(UObsidianInventoryItemInstance* Equipped : EquipmentToMoveToSwap)
	{
		if(IsValid(Equipped))
		{
			EquipmentList.MoveWeaponToSwap(Equipped);
		}
	}
	
	for(UObsidianInventoryItemInstance* Swap : EquipmentToMoveFromSwap)
	{
		if(IsValid(Swap))
		{
			EquipmentList.MoveWeaponFromSwap(Swap);
		}
	}
}

void UObsidianEquipmentComponent::UnequipItem(UObsidianInventoryItemInstance* InstanceToUnequip)
{
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogEquipment, Warning, TEXT("No Authority in UObsidianInventoryComponent::AddItemInstance."));
		return; 
	}

	if(InstanceToUnequip == nullptr)
	{
		UE_LOG(LogEquipment, Error, TEXT("Passed InstanceToUnequip is invalid in UObsidianEquipmentComponent::UnequipItem."));
		return;
	}

	EquipmentList.RemoveEntry(InstanceToUnequip);

	if(InstanceToUnequip && IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(InstanceToUnequip);
	}
}

bool UObsidianEquipmentComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for(const FObsidianEquipmentEntry& Entry : EquipmentList.Entries)
	{
		UObsidianInventoryItemInstance* Instance = Entry.Instance;

		if(Instance && IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}
	return WroteSomething;
}

void UObsidianEquipmentComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	if(IsUsingRegisteredSubObjectList())
	{
		for(const FObsidianEquipmentEntry& Entry : EquipmentList.Entries)
		{
			UObsidianInventoryItemInstance* Instance = Entry.Instance;

			if(Instance && IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

EObsidianEquipResult UObsidianEquipmentComponent::DoesItemFitEquipmentSlot(const FGameplayTag& SlotTag, const FGameplayTag& ItemCategory)
{
	const FObsidianEquipmentSlotDefinition Slot = FindEquipmentSlotByTag(SlotTag);
	if(Slot.IsValid() == false)
	{
		return EObsidianEquipResult::ItemUnfitForCategory;
	}
	return Slot.CanEquipToSlot(ItemCategory);
}

void UObsidianEquipmentComponent::AddBannedEquipmentCategoryToSlot(const FGameplayTag& SlotTag, const FGameplayTag& InItemCategory)
{
	FObsidianEquipmentSlotDefinition Slot = FindEquipmentSlotByTag(SlotTag);
	if(Slot.IsValid())
	{
		Slot.AddBannedEquipmentCategory(InItemCategory);
	}
}

void UObsidianEquipmentComponent::AddBannedEquipmentCategoriesToSlot(const FGameplayTag& SlotTag, const FGameplayTagContainer& InItemCategories)
{
	FObsidianEquipmentSlotDefinition Slot = FindEquipmentSlotByTag(SlotTag);
	if(Slot.IsValid())
	{
		Slot.AddBannedEquipmentCategories(InItemCategories);
	}
}

void UObsidianEquipmentComponent::RemoveBannedEquipmentCategoryToSlot(const FGameplayTag& SlotTag, const FGameplayTag& ItemCategoryToRemove)
{
	FObsidianEquipmentSlotDefinition Slot = FindEquipmentSlotByTag(SlotTag);
	if(Slot.IsValid())
	{
		Slot.RemoveBannedEquipmentCategory(ItemCategoryToRemove);
	}
}

void UObsidianEquipmentComponent::RemoveBannedEquipmentCategoriesToSlot(const FGameplayTag& SlotTag, const FGameplayTagContainer& ItemCategoriesToRemove)
{
	FObsidianEquipmentSlotDefinition Slot = FindEquipmentSlotByTag(SlotTag);
	if(Slot.IsValid())
	{
		Slot.RemoveBannedEquipmentCategories(ItemCategoriesToRemove);
	}
}

void UObsidianEquipmentComponent::CreateDefaultEquipmentSlots()
{
	using namespace ObsidianGameplayTags;
	using namespace ObsidianInventoryItemsStatics;
	
	EquipmentList.EquipmentSlots =
		{
			{FObsidianEquipmentSlotDefinition(Equipment_Slot_Weapon_RightHand, Equipment_Slot_Weapon_LeftHand, DefaultAcceptedEquipmentCategories[Equipment_Slot_Weapon_RightHand])},
			{FObsidianEquipmentSlotDefinition(Equipment_Slot_Weapon_LeftHand, Equipment_Slot_Weapon_RightHand, DefaultAcceptedEquipmentCategories[Equipment_Slot_Weapon_LeftHand])},
		
			{FObsidianEquipmentSlotDefinition(Equipment_Slot_Helmet, DefaultAcceptedEquipmentCategories[Equipment_Slot_Helmet])},
			{FObsidianEquipmentSlotDefinition(Equipment_Slot_BodyArmor, DefaultAcceptedEquipmentCategories[Equipment_Slot_BodyArmor])},
			{FObsidianEquipmentSlotDefinition(Equipment_Slot_Belt, DefaultAcceptedEquipmentCategories[Equipment_Slot_Belt])},
			{FObsidianEquipmentSlotDefinition(Equipment_Slot_Gloves, DefaultAcceptedEquipmentCategories[Equipment_Slot_Gloves])},
			{FObsidianEquipmentSlotDefinition(Equipment_Slot_Boots, DefaultAcceptedEquipmentCategories[Equipment_Slot_Boots])},
		
			{FObsidianEquipmentSlotDefinition(Equipment_Slot_Amulet, DefaultAcceptedEquipmentCategories[Equipment_Slot_Amulet])},
			{FObsidianEquipmentSlotDefinition(Equipment_Slot_RightRing, DefaultAcceptedEquipmentCategories[Equipment_Slot_RightRing])},
			{FObsidianEquipmentSlotDefinition(Equipment_Slot_LeftRing, DefaultAcceptedEquipmentCategories[Equipment_Slot_LeftRing])},
		};
}



