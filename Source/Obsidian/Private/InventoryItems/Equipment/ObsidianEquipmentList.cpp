// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Equipment/ObsidianEquipmentList.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/Equipment/ObsidianEquipmentComponent.h"
#include "Obsidian/ObsidianGameplayTags.h"

TArray<UObsidianInventoryItemInstance*> FObsidianEquipmentList::GetAllEquippedItems() const
{
	TArray<UObsidianInventoryItemInstance*> Items;
	Items.Reserve(Entries.Num());

	for(const FObsidianEquipmentEntry& Entry : Entries)
	{
		if(Entry.Instance)
		{
			Items.Add(Entry.Instance);
		}
	}
	return Items;
}

UObsidianInventoryItemInstance* FObsidianEquipmentList::AddEntry(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDefClass, const FGameplayTag& EquipmentSlotTag)
{
	check(ItemDefClass != nullptr);
	check(OwnerComponent);

	const AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor);

	if(ValidateEquipmentSlot(EquipmentSlotTag) == false)
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Provided EquipmentSlotTag [%s] does not match any EquipmentSlot."),
			*EquipmentSlotTag.GetTagName().ToString()), ELogVerbosity::Error);
		return nullptr;
	}

	FObsidianEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<UObsidianInventoryItemInstance>(OwnerComponent->GetOwner());
	NewEntry.EquipmentSlotTag = EquipmentSlotTag;
	NewEntry.Instance->SetItemDef(ItemDefClass);
	NewEntry.Instance->SetItemCurrentEquipmentSlot(EquipmentSlotTag);

	const FGameplayTag WeaponSlotTag = FGameplayTag::RequestGameplayTag("Equipment.Slot.Weapon");
	if(EquipmentSlotTag.MatchesTag(WeaponSlotTag))
	{
		NewEntry.AssociatedSwap = CurrentWeaponSwap;
	}

	const UObsidianInventoryItemDefinition* DefaultObject = GetDefault<UObsidianInventoryItemDefinition>(ItemDefClass);
	for(const UObsidianInventoryItemFragment* Fragment : DefaultObject->ItemFragments)
	{
		if(Fragment)
		{
			Fragment->OnInstancedCreated(NewEntry.Instance);
		}
	}

	NewEntry.Instance->SetItemDebugName(DefaultObject->GetDebugName());
	NewEntry.Instance->SetItemCategory(DefaultObject->GetItemCategoryTag());
	
	UObsidianInventoryItemInstance* Item = NewEntry.Instance;
	SlotToEquipmentMap.Add(EquipmentSlotTag, Item);

	MarkItemDirty(NewEntry);

	BroadcastChangeMessage(NewEntry, EObsidianEquipmentChangeType::ECT_ItemEquipped);
	return Item;
}

void FObsidianEquipmentList::AddEntry(UObsidianInventoryItemInstance* Instance, const FGameplayTag& EquipmentSlotTag)
{
	check(Instance);
	check(OwnerComponent);

	if(ValidateEquipmentSlot(EquipmentSlotTag) == false)
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Provided EquipmentSlotTag [%s] does not match any EquipmentSlot."),
			*EquipmentSlotTag.GetTagName().ToString()), ELogVerbosity::Error);
		return;
	}

#if !UE_BUILD_SHIPPING
	if(SlotToEquipmentMap.Contains(EquipmentSlotTag))
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Provided EquipmentSlotTag [%s] already contains an item."),
			*EquipmentSlotTag.GetTagName().ToString()), ELogVerbosity::Error);
	}
#endif

	FObsidianEquipmentEntry& NewEntry = Entries.Emplace_GetRef(Instance, EquipmentSlotTag);
	SlotToEquipmentMap.Add(EquipmentSlotTag, Instance);
	Instance->SetItemCurrentEquipmentSlot(EquipmentSlotTag);

	const FGameplayTag WeaponSlotTag = FGameplayTag::RequestGameplayTag("Equipment.Slot.Weapon");
	if(EquipmentSlotTag.MatchesTag(WeaponSlotTag))
	{
		NewEntry.AssociatedSwap = CurrentWeaponSwap;
	}

	MarkItemDirty(NewEntry);

	BroadcastChangeMessage(NewEntry, EObsidianEquipmentChangeType::ECT_ItemEquipped);
}

void FObsidianEquipmentList::RemoveEntry(UObsidianInventoryItemInstance* Instance)
{
	for(auto It = Entries.CreateIterator(); It; ++It)
	{
		FObsidianEquipmentEntry& Entry = *It;
		if(Entry.Instance == Instance)
		{
			const FGameplayTag CachedSlotTag = Entry.EquipmentSlotTag;
			SlotToEquipmentMap.Remove(CachedSlotTag);
			Entry.Instance->ResetItemCurrentEquipmentSlot();
			It.RemoveCurrent();
			MarkArrayDirty();

			BroadcastChangeMessage(FObsidianEquipmentEntry(Instance, CachedSlotTag), EObsidianEquipmentChangeType::ECT_ItemUnequipped);
		}
	}
}

bool FObsidianEquipmentList::ValidateEquipmentSlot(const FGameplayTag& SlotTag)
{
	for(const FGameplayTag& Tag : ObsidianGameplayTags::EquipmentSlots)
	{
		if(Tag == SlotTag)
		{
			return true;
		}
	}
	return false;
}

void FObsidianEquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for(const int32 Index : RemovedIndices)
	{
		FObsidianEquipmentEntry& Entry = Entries[Index];
		SlotToEquipmentMap.Remove(Entry.EquipmentSlotTag);

		BroadcastChangeMessage(Entry, EObsidianEquipmentChangeType::ECT_ItemUnequipped);
	}
}

void FObsidianEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for(const int32 Index : AddedIndices)
	{
		FObsidianEquipmentEntry& Entry = Entries[Index];
		SlotToEquipmentMap.Add(Entry.EquipmentSlotTag, Entry.Instance);

		BroadcastChangeMessage(Entry, EObsidianEquipmentChangeType::ECT_ItemEquipped);
	}
}

void FObsidianEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	// Don't have a use case for that rn, maybe upgrading items while equipped later?
}

void FObsidianEquipmentList::BroadcastChangeMessage(const FObsidianEquipmentEntry& Entry, const EObsidianEquipmentChangeType ChangeType) const
{
	FObsidianEquipmentChangeMessage Message;
	Message.EquipmentOwner = OwnerComponent;
	Message.ItemInstance = Entry.Instance;
	Message.SlotTag = Entry.EquipmentSlotTag;
	Message.ChangeType = ChangeType;

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSubsystem.BroadcastMessage(ObsidianGameplayTags::Message_Equipment_Changed, Message);
}
