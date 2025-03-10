// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Equipment/ObsidianEquipmentList.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "Obsidian/ObsidianGameplayTags.h"

UObsidianInventoryItemInstance* FObsidianEquipmentList::AddEntry(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDefClass, const FGameplayTag& EquipmentSlotTag)
{
	check(ItemDefClass != nullptr);
	check(OwnerComponent);

	const AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor);

	FObsidianEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<UObsidianInventoryItemInstance>(OwnerComponent->GetOwner());
	NewEntry.EquipmentSlotTag = EquipmentSlotTag;
	NewEntry.Instance->SetItemDef(ItemDefClass);

	const UObsidianInventoryItemDefinition* DefaultObject = GetDefault<UObsidianInventoryItemDefinition>(ItemDefClass);
	for(const UObsidianInventoryItemFragment* Fragment : DefaultObject->ItemFragments)
	{
		if(Fragment)
		{
			Fragment->OnInstancedCreated(NewEntry.Instance);
		}
	}

	NewEntry.Instance->SetItemDebugName(DefaultObject->GetDebugName());

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

#if !UE_BUILD_SHIPPING
	if(SlotToEquipmentMap.Contains(EquipmentSlotTag))
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Provided EquipmentSlotTag [%s] already contains an item."),
			*EquipmentSlotTag.GetTagName().ToString()), ELogVerbosity::Error);
	}
#endif

	FObsidianEquipmentEntry& NewEntry = Entries.Emplace_GetRef(Instance, EquipmentSlotTag);
	SlotToEquipmentMap.Add(EquipmentSlotTag, Instance);

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
			It.RemoveCurrent();
			MarkArrayDirty();

			BroadcastChangeMessage(FObsidianEquipmentEntry(Instance, CachedSlotTag), EObsidianEquipmentChangeType::ECT_ItemUnequipped);
		}
	}
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
