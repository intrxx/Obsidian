// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Equipment/ObsidianEquipmentComponent.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogEquipment);

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

void UObsidianEquipmentComponent::AutomaticallyEquipItem(UObsidianInventoryItemInstance* InstanceToEquip)
{
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::AddItemInstance."));
		return; 
	}

	if(InstanceToEquip == nullptr)
	{
		return;
	}

	//TODO Need some Item Tag to determine if item can equipped 
	// const EObsidianEquipResult EquipResult = CanEquipInstance(InstanceToEquip);
	// if(EquipResult != EObsidianEquipResult::CanEquip)
	// {
	// 	//TODO Shouldn't add voice over here
	// 	return;
	// }
	//
	// EquipmentList.AddEntry(InstanceToEquip);
	
	if(InstanceToEquip && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(InstanceToEquip);
	}
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

EObsidianEquipResult UObsidianEquipmentComponent::CanEquipInstance(UObsidianInventoryItemInstance* Instance, const FGameplayTag& SlotTag)
{
	if(Instance == nullptr)
	{
		return EObsidianEquipResult::None;
	}

	if(DoesItemFitEquipmentSlot(SlotTag, Instance->GetItemCategory()) == false)
	{
		return EObsidianEquipResult::ItemUnfitForCategory;
	}

	if(Instance->IsItemEquippable() == false)
	{
		return EObsidianEquipResult::ItemUnequippable;
	}

	if(Instance->IsItemIdentified() == false)
	{
		return EObsidianEquipResult::ItemUnientified;
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

	//TODO Need some Item Tag to determine if item can equipped 
	// const EObsidianEquipResult EquipResult = CanEquipTemplate(ItemDef);
	// if(EquipResult != EObsidianEquipResult::CanEquip)
	// {
	//  //TODO Shouldn't add voice over here
	// 	return nullptr;
	// }


	// if(EquippedInstance && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	// {
	// 	AddReplicatedSubObject(EquippedInstance);
	// }

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

	UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject();
	if(DefaultObject == nullptr)
	{
		return EObsidianEquipResult::None;
	}

	if(DoesItemFitEquipmentSlot(SlotTag, DefaultObject->GetItemCategoryTag()) == false)
	{
		return EObsidianEquipResult::ItemUnfitForCategory;
	}

	if(DefaultObject->IsEquippable() == false)
	{
		return EObsidianEquipResult::ItemUnequippable;
	}

	if(DefaultObject->IsIdentified() == false)
	{
		return EObsidianEquipResult::ItemUnientified;
	}
	
	return EObsidianEquipResult::CanEquip;
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
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Equipment_Slot_RightHand, FGameplayTagContainer(FGameplayTagContainer::CreateFromArray(RightHandAcceptedEquipment)))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Equipment_Slot_LeftHand, FGameplayTagContainer(FGameplayTagContainer::CreateFromArray(LeftHandAcceptedEquipment)))},
		
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


