// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Equipment/ObsidianEquipmentComponent.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "Engine/ActorChannel.h"
#include "InventoryItems/Equipment/ObsidianSpawnedEquipmentPiece.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogEquipment);

//
// Equipment Slot Definition
//

bool FObsidianEquipmentSlotDefinition::IsValid() const
{
	return SlotTag.IsValid();
}

bool FObsidianEquipmentSlotDefinition::CanEquipToSlot(const FGameplayTag& EquipmentCategory) const
{
	if(AcceptedEquipmentCategories.HasTagExact(EquipmentCategory) && BannedEquipmentCategories.HasTagExact(EquipmentCategory) == false)
	{
		return true;
	}
	return false;
}

void FObsidianEquipmentSlotDefinition::AddBannedEquipmentCategory(const FGameplayTag& InBannedCategory)
{
	BannedEquipmentCategories.AddTag(InBannedCategory);
}

void FObsidianEquipmentSlotDefinition::AddBannedEquipmentCategories(const FGameplayTagContainer& InBannedCategories)
{
	BannedEquipmentCategories.AppendTags(InBannedCategories);
}

void FObsidianEquipmentSlotDefinition::RemoveBannedEquipmentCategory(const FGameplayTag& BannedCategoryToRemove)
{
#if !UE_BUILD_SHIPPING
	if(BannedEquipmentCategories.HasTag(BannedCategoryToRemove) == false)
	{
		UE_LOG(LogEquipment, Error, TEXT("Trying to remove Banned Equipment Tag [%s] but the Tag does not exist in BannedEquipmentCategories."), *BannedCategoryToRemove.ToString());
	}
#endif
	BannedEquipmentCategories.RemoveTag(BannedCategoryToRemove);
}

void FObsidianEquipmentSlotDefinition::RemoveBannedEquipmentCategories(const FGameplayTagContainer& BannedCategoriesToRemove)
{
#if !UE_BUILD_SHIPPING
	for(FGameplayTag Tag : BannedEquipmentCategories)
	{
		if(BannedEquipmentCategories.HasTag(Tag) == false)
		{
			UE_LOG(LogEquipment, Error, TEXT("Trying to remove Banned Equipment Tag [%s] but the Tag does not exist in BannedEquipmentCategories."), *Tag.ToString());
		}
	}
#endif
	BannedEquipmentCategories.RemoveTags(BannedCategoriesToRemove);
}

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
	for(FObsidianEquipmentSlotDefinition Slot : EquipmentSlots)
	{
		if(Slot.SlotTag == SlotTag)
		{
			return Slot;
		}
	}
	
	return FObsidianEquipmentSlotDefinition();
}

TArray<FObsidianEquipmentSlotDefinition> UObsidianEquipmentComponent::FindMatchingEquipmentSlotsByItemCategory(const FGameplayTag& ItemCategory)
{
	TArray<FObsidianEquipmentSlotDefinition> MatchingSlots;
	
	for(FObsidianEquipmentSlotDefinition Slot : EquipmentSlots)
	{
		if(Slot.CanEquipToSlot(ItemCategory))
		{
			MatchingSlots.Add(Slot);
		}
	}
	
	return MatchingSlots;
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
	
	for(FObsidianEquipmentSlotDefinition Slot : FindMatchingEquipmentSlotsByItemCategory(InstanceToEquip->GetItemCategory()))
	{
		if(EquipmentList.SlotToEquipmentMap.Contains(Slot.SlotTag))
		{
			continue; // We already have an item equipped in this slot, we shouldn't try to equip it.
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

	if(DoesItemFitEquipmentSlot(SlotTag, Instance->GetItemCategory()) == false)
	{
		return EObsidianEquipResult::ItemUnfitForCategory;
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

	for(FObsidianEquipmentSlotDefinition Slot : FindMatchingEquipmentSlotsByItemCategory(DefaultObject->GetItemCategoryTag()))
	{
		if(EquipmentList.SlotToEquipmentMap.Contains(Slot.SlotTag))
		{
			continue; // We already have an item equipped in this slot, we shouldn't try to equip it.
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

	if(DoesItemFitEquipmentSlot(SlotTag, DefaultObject->GetItemCategoryTag()) == false)
	{
		return EObsidianEquipResult::ItemUnfitForCategory;
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

bool UObsidianEquipmentComponent::DoesItemFitEquipmentSlot(const FGameplayTag& SlotTag, const FGameplayTag& ItemCategory)
{
	const FObsidianEquipmentSlotDefinition Slot = FindEquipmentSlotByTag(SlotTag);
	if(Slot.IsValid() && Slot.CanEquipToSlot(ItemCategory))
	{
		return true;
	}
	return false;
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
	const TArray<FGameplayTag> RightHandAcceptedEquipment =
		{
		ObsidianGameplayTags::Item_Category_Flail, ObsidianGameplayTags::Item_Category_Dagger, ObsidianGameplayTags::Item_Category_Wand,
		ObsidianGameplayTags::Item_Category_Bow_TwoHand, ObsidianGameplayTags::Item_Category_Staff_TwoHand, ObsidianGameplayTags::Item_Category_Mace_OneHand,
		ObsidianGameplayTags::Item_Category_Mace_TwoHand, ObsidianGameplayTags::Item_Category_Axe_OneHand, ObsidianGameplayTags::Item_Category_Axe_TwoHand,
		ObsidianGameplayTags::Item_Category_Sword_OneHand, ObsidianGameplayTags::Item_Category_Sword_TwoHand
		};
	
	const TArray<FGameplayTag> LeftHandAcceptedEquipment =
		{
		ObsidianGameplayTags::Item_Category_Flail, ObsidianGameplayTags::Item_Category_Dagger, ObsidianGameplayTags::Item_Category_Wand,
		ObsidianGameplayTags::Item_Category_Mace_OneHand, ObsidianGameplayTags::Item_Category_Axe_OneHand, ObsidianGameplayTags::Item_Category_Sword_OneHand,
		ObsidianGameplayTags::Item_Category_Quiver, ObsidianGameplayTags::Item_Category_Shield
		};
	
	EquipmentSlots =
		{
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Equipment_Slot_Weapon_RightHand, FGameplayTagContainer(FGameplayTagContainer::CreateFromArray(RightHandAcceptedEquipment)))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Equipment_Slot_Weapon_LeftHand, FGameplayTagContainer(FGameplayTagContainer::CreateFromArray(LeftHandAcceptedEquipment)))},
		
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Equipment_Slot_Helmet, FGameplayTagContainer(ObsidianGameplayTags::Item_Category_Helmet))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Equipment_Slot_BodyArmor, FGameplayTagContainer(ObsidianGameplayTags::Item_Category_BodyArmor))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Equipment_Slot_Belt, FGameplayTagContainer(ObsidianGameplayTags::Item_Category_Belt))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Equipment_Slot_Gloves, FGameplayTagContainer(ObsidianGameplayTags::Item_Category_Gloves))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Equipment_Slot_Boots, FGameplayTagContainer(ObsidianGameplayTags::Item_Category_Boots))},
		
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Equipment_Slot_Amulet, FGameplayTagContainer(ObsidianGameplayTags::Item_Category_Amulet))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Equipment_Slot_RightRing, FGameplayTagContainer(ObsidianGameplayTags::Item_Category_Ring))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Equipment_Slot_LeftRing, FGameplayTagContainer(ObsidianGameplayTags::Item_Category_Ring))},
		};
}



