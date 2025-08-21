// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Equipment/ObsidianEquipmentList.h"

// ~ Core
#include "AbilitySystemGlobals.h"

// ~ Project
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "AbilitySystem/Data/ObsidianAbilitySet.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/Equipment/ObsidianEquipmentComponent.h"
#include "Obsidian/ObsidianGameplayTags.h"

// ~ FObsidianEquipmentSlotDefinition

FObsidianEquipmentSlotDefinition const FObsidianEquipmentSlotDefinition::InvalidSlot;

bool FObsidianEquipmentSlotDefinition::IsValid() const
{
	return BaseSlotDefinition.IsValid();
}

FGameplayTag FObsidianEquipmentSlotDefinition::GetEquipmentSlotTag() const
{
	return BaseSlotDefinition.GetSlotTag();
}

EObsidianEquipCheckResult FObsidianEquipmentSlotDefinition::CanEquipAtSlot(const FGameplayTag& ItemCategory) const
{
	return BaseSlotDefinition.CanPlaceAtSlot(ItemCategory);
}

void FObsidianEquipmentSlotDefinition::AddBannedEquipmentCategory(const FGameplayTag& InBannedCategory)
{
	BaseSlotDefinition.AddBannedItemCategory(InBannedCategory);
}

void FObsidianEquipmentSlotDefinition::AddBannedEquipmentCategories(const FGameplayTagContainer& InBannedCategories)
{
	BaseSlotDefinition.AddBannedItemCategories(InBannedCategories);
}

void FObsidianEquipmentSlotDefinition::RemoveBannedEquipmentCategory(const FGameplayTag& BannedCategoryToRemove)
{
	BaseSlotDefinition.RemoveBannedItemCategory(BannedCategoryToRemove);
}

void FObsidianEquipmentSlotDefinition::RemoveBannedEquipmentCategories(const FGameplayTagContainer& BannedCategoriesToRemove)
{
	BaseSlotDefinition.RemoveBannedItemCategories(BannedCategoriesToRemove);
}

// ~ End of FObsidianEquipmentSlotDefinition

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

TArray<UObsidianInventoryItemInstance*> FObsidianEquipmentList::GetSwappedWeapons()
{
	const FGameplayTag WeaponSwapSlotTag = FGameplayTag::RequestGameplayTag(TEXT("Item.SwapSlot.Equipment.Weapon"), true);
	
	TArray<UObsidianInventoryItemInstance*> SwappedWeapons;
	SwappedWeapons.Reserve(2);

	for(const FObsidianEquipmentEntry& Entry : Entries)
	{
		if(Entry.Instance && Entry.EquipmentSlotTag.MatchesTag(WeaponSwapSlotTag))
		{
			SwappedWeapons.Add(Entry.Instance);
		}
	}
	return SwappedWeapons;
}

TArray<UObsidianInventoryItemInstance*> FObsidianEquipmentList::GetEquippedWeapons()
{
	const FGameplayTag WeaponSlotTag = FGameplayTag::RequestGameplayTag(TEXT("Item.Slot.Equipment.Weapon"), true);
	
	TArray<UObsidianInventoryItemInstance*> EquippedWeapons;
	EquippedWeapons.Reserve(2);

	for(const FObsidianEquipmentEntry& Entry : Entries)
	{
		if(Entry.Instance && Entry.EquipmentSlotTag.MatchesTag(WeaponSlotTag))
		{
			EquippedWeapons.Add(Entry.Instance);
		}
	}
	return EquippedWeapons;
}

UObsidianInventoryItemInstance* FObsidianEquipmentList::GetEquipmentPieceByTag(const FGameplayTag& SlotTag) const
{
	return SlotToEquipmentMap.FindRef(SlotTag);
}

UObsidianAbilitySystemComponent* FObsidianEquipmentList::GetObsidianAbilitySystemComponent() const
{
	check(OwnerComponent);
	const AObsidianPlayerController* OwningController = Cast<AObsidianPlayerController>(OwnerComponent->GetOwner());
	return OwningController ? OwningController->GetObsidianAbilitySystemComponent() : nullptr;
}

FObsidianEquipmentSlotDefinition FObsidianEquipmentList::FindEquipmentSlotByTag(const FGameplayTag& SlotTag)
{
	for(const FObsidianEquipmentSlotDefinition& Slot : EquipmentSlots)
	{
		if (Slot.GetEquipmentSlotTag() == SlotTag)
		{
			return Slot;
		}
	}

	return FObsidianEquipmentSlotDefinition::InvalidSlot;
}

TArray<FObsidianEquipmentSlotDefinition> FObsidianEquipmentList::FindMatchingEquipmentSlotsByItemCategory(const FGameplayTag& ItemCategory)
{
	TArray<FObsidianEquipmentSlotDefinition> MatchingSlots;
	
	for(FObsidianEquipmentSlotDefinition Slot : EquipmentSlots)
	{
		if(Slot.CanEquipAtSlot(ItemCategory) == EObsidianEquipCheckResult::CanEquip)
		{
			MatchingSlots.Add(Slot);
		}
	}
	
	return MatchingSlots;
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
	NewEntry.Instance->SetItemCurrentPosition(EquipmentSlotTag);

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
	NewEntry.Instance->OnInstanceCreatedAndInitialized();
	
	UObsidianInventoryItemInstance* Item = NewEntry.Instance;
	SlotToEquipmentMap.Add(EquipmentSlotTag, Item);
	
	if(UObsidianAbilitySystemComponent* ObsidianASC = GetObsidianAbilitySystemComponent())
	{
		for(UObsidianAbilitySet*& AbilitySet : Item->GetOwningAbilitySets())
		{
			AbilitySet->GiveToAbilitySystem(ObsidianASC, &NewEntry.GrantedHandles, Item);
		}
	}
	else
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Obsidian Ability Sytem Component is invalid on Owning Actor [%s]."),
			*GetNameSafe(OwningActor)), ELogVerbosity::Error);
	}

	Item->SpawnEquipmentActors(EquipmentSlotTag);

	MarkItemDirty(NewEntry);

	BroadcastChangeMessage(NewEntry, EquipmentSlotTag, FGameplayTag::EmptyTag, EObsidianEquipmentChangeType::ECT_ItemEquipped);
	return Item;
}

void FObsidianEquipmentList::AddEntry(UObsidianInventoryItemInstance* Instance, const FGameplayTag& EquipmentSlotTag)
{
	check(Instance);
	check(OwnerComponent);

	const AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor);
	
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
	Instance->SetItemCurrentPosition(EquipmentSlotTag);

	if(UObsidianAbilitySystemComponent* ObsidianASC = GetObsidianAbilitySystemComponent())
	{
		for(UObsidianAbilitySet*& AbilitySet : Instance->GetOwningAbilitySets())
		{
			AbilitySet->GiveToAbilitySystem(ObsidianASC, &NewEntry.GrantedHandles, Instance);
		}
	}
	else
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Obsidian Ability Sytem Component is invalid on Owning Actor [%s]."),
			*GetNameSafe(OwningActor)), ELogVerbosity::Error);
	}
	
	Instance->SpawnEquipmentActors(EquipmentSlotTag);

	MarkItemDirty(NewEntry);

	BroadcastChangeMessage(NewEntry, EquipmentSlotTag, FGameplayTag::EmptyTag, EObsidianEquipmentChangeType::ECT_ItemEquipped);
}

void FObsidianEquipmentList::MoveWeaponToSwap(UObsidianInventoryItemInstance* Instance)
{
	check(Instance);
	check(OwnerComponent);

	const AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor);
	
	const FGameplayTag CurrentWeaponSlotTag = Instance->GetItemCurrentPosition().GetItemSlotTag();

#if !UE_BUILD_SHIPPING
	const FGameplayTag WeaponSlotTag = FGameplayTag::RequestGameplayTag("Item.Slot.Equipment.Weapon");
	if(CurrentWeaponSlotTag.MatchesTag(WeaponSlotTag) == false)
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Provided Instance [%s] with Tag [%s] is not currentely in Weapon Slot, swapping shouldn't happen."),
			*Instance->GetItemDebugName(), *CurrentWeaponSlotTag.GetTagName().ToString()), ELogVerbosity::Error);
	}
	
	if(SlotToEquipmentMap.Contains(CurrentWeaponSlotTag) == false)
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Provided EquipmentSlotTag [%s] is not equipped, why swapping?"),
			*CurrentWeaponSlotTag.GetTagName().ToString()), ELogVerbosity::Error);
	}
#endif

	FGameplayTag SwapTag = ObsidianGameplayTags::Item_SwapSlot_Equipment_Weapon_RightHand;
	if(CurrentWeaponSlotTag == ObsidianGameplayTags::Item_Slot_Equipment_Weapon_LeftHand)
	{
		SwapTag = ObsidianGameplayTags::Item_SwapSlot_Equipment_Weapon_LeftHand;
	}

	bool bSuccess = false;
	for(FObsidianEquipmentEntry& Entry : Entries)
	{
		if(Entry.Instance == Instance)
		{
			
			Entry.EquipmentSlotTag = SwapTag;
			Entry.bSwappedOut = true;

			if(UObsidianAbilitySystemComponent* ObsidianASC = GetObsidianAbilitySystemComponent())
			{
				Entry.GrantedHandles.TakeFromAbilitySystem(ObsidianASC);
			}
			else
			{
				FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Obsidian Ability Sytem Component is invalid on Owning Actor [%s]."),
					*GetNameSafe(OwningActor)), ELogVerbosity::Error);
			}
			Instance->DestroyEquipmentActors();
			
			MarkItemDirty(Entry);
			bSuccess = true;
		}
	}

	if(bSuccess)
	{
		Instance->SetItemCurrentPosition(SwapTag);
		
		//TODO Do anything unequipping related

		const bool bSwappedBothWays = SlotToEquipmentMap.Contains(CurrentWeaponSlotTag) && SlotToEquipmentMap.Contains(SwapTag);
		const FGameplayTag TagToClear = bSwappedBothWays ? FGameplayTag::EmptyTag : CurrentWeaponSlotTag;
		
		SlotToEquipmentMap.Add(SwapTag, Instance);
		
		BroadcastChangeMessage(Instance, SwapTag, TagToClear, EObsidianEquipmentChangeType::ECT_ItemSwapped);
		
		if(bSwappedBothWays == false)
		{
			SlotToEquipmentMap.Remove(CurrentWeaponSlotTag);
		}
	}
}

void FObsidianEquipmentList::MoveWeaponFromSwap(UObsidianInventoryItemInstance* Instance)
{
	check(Instance);
	check(OwnerComponent);

	const AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor);
	
	const FGameplayTag CurrentSwapTag = Instance->GetItemCurrentPosition().GetItemSlotTag();

#if !UE_BUILD_SHIPPING
	const FGameplayTag WeaponSlotTag = FGameplayTag::RequestGameplayTag("Item.SwapSlot.Equipment.Weapon");
	if(CurrentSwapTag.MatchesTag(WeaponSlotTag) == false)
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Provided Instance [%s] with Tag [%s] is not currentely in Weapon Slot, swapping shouldn't happen."),
			*Instance->GetItemDebugName(), *CurrentSwapTag.GetTagName().ToString()), ELogVerbosity::Error);
	}
#endif

	FGameplayTag MainWeaponSlotTag = ObsidianGameplayTags::Item_Slot_Equipment_Weapon_RightHand;
	if(CurrentSwapTag == ObsidianGameplayTags::Item_SwapSlot_Equipment_Weapon_LeftHand)
	{
		MainWeaponSlotTag = ObsidianGameplayTags::Item_Slot_Equipment_Weapon_LeftHand;
	}

	bool bSuccess = false;
	for(FObsidianEquipmentEntry& Entry : Entries)
	{
		if(Entry.Instance == Instance)
		{
			Entry.EquipmentSlotTag = MainWeaponSlotTag;
			Entry.bSwappedOut = false;
			
			if(UObsidianAbilitySystemComponent* ObsidianASC = GetObsidianAbilitySystemComponent())
			{
				for(UObsidianAbilitySet*& AbilitySet : Instance->GetOwningAbilitySets())
				{
					AbilitySet->GiveToAbilitySystem(ObsidianASC, &Entry.GrantedHandles, Instance);
				}
			}
			else
			{
				FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Obsidian Ability Sytem Component is invalid on Owning Actor [%s]."),
					*GetNameSafe(OwningActor)), ELogVerbosity::Error);
			}
			Instance->SpawnEquipmentActors(MainWeaponSlotTag);
			
			MarkItemDirty(Entry);
			bSuccess = true;
		}
	}

	if(bSuccess)
	{
		Instance->SetItemCurrentPosition(MainWeaponSlotTag);

		//TODO Do anything equipping related

		const bool bSwappedBothWays = SlotToEquipmentMap.Contains(CurrentSwapTag) && SlotToEquipmentMap.Contains(MainWeaponSlotTag);
		const FGameplayTag TagToClear = bSwappedBothWays ? FGameplayTag::EmptyTag : CurrentSwapTag;
		
		SlotToEquipmentMap.Add(MainWeaponSlotTag, Instance);
		
		BroadcastChangeMessage(Instance, MainWeaponSlotTag, TagToClear, EObsidianEquipmentChangeType::ECT_ItemSwapped);

		if(bSwappedBothWays == false)
		{
			SlotToEquipmentMap.Remove(CurrentSwapTag);	
		}
	}
}

void FObsidianEquipmentList::RemoveEntry(UObsidianInventoryItemInstance* Instance)
{
	const AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor);
	
	for(auto It = Entries.CreateIterator(); It; ++It)
	{
		FObsidianEquipmentEntry& Entry = *It;
		if(Entry.Instance == Instance)
		{
			const FGameplayTag CachedSlotTag = Entry.EquipmentSlotTag;
			
			SlotToEquipmentMap.Remove(CachedSlotTag);
			Instance->ResetItemCurrentPosition();
			
			if(UObsidianAbilitySystemComponent* ObsidianASC = GetObsidianAbilitySystemComponent())
			{
				Entry.GrantedHandles.TakeFromAbilitySystem(ObsidianASC);
			}
			else
			{
				FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Obsidian Ability Sytem Component is invalid on Owning Actor [%s]."),
					*GetNameSafe(OwningActor)), ELogVerbosity::Error);
			}
			Instance->DestroyEquipmentActors();
			
			
			It.RemoveCurrent();
			MarkArrayDirty();

			BroadcastChangeMessage(Instance, FGameplayTag::EmptyTag, CachedSlotTag, EObsidianEquipmentChangeType::ECT_ItemUnequipped);
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
		Entry.LastObservedEquipmentSlotTag = FGameplayTag::EmptyTag;
		SlotToEquipmentMap.Remove(Entry.EquipmentSlotTag);
		
		BroadcastChangeMessage(Entry, FGameplayTag::EmptyTag, Entry.EquipmentSlotTag, EObsidianEquipmentChangeType::ECT_ItemUnequipped);

		UE_LOG(LogEquipment, Display, TEXT("Replicated un-equipping [%s] item."), *Entry.Instance->GetItemDebugName());
	}
}

void FObsidianEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for(const int32 Index : AddedIndices)
	{
		FObsidianEquipmentEntry& Entry = Entries[Index];
		Entry.LastObservedEquipmentSlotTag = Entry.EquipmentSlotTag;
		SlotToEquipmentMap.Add(Entry.EquipmentSlotTag, Entry.Instance);
		
		BroadcastChangeMessage(Entry, Entry.EquipmentSlotTag, FGameplayTag::EmptyTag, EObsidianEquipmentChangeType::ECT_ItemEquipped);

		UE_LOG(LogEquipment, Display, TEXT("Replicated equipping [%s] item."), *Entry.Instance->GetItemDebugName());
	}
}

void FObsidianEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for(const int32 Index : ChangedIndices)
	{
		FObsidianEquipmentEntry& Entry = Entries[Index];
		check(Entry.LastObservedEquipmentSlotTag != FGameplayTag::EmptyTag);
		if(Entry.LastObservedEquipmentSlotTag != Entry.EquipmentSlotTag)
		{
			const bool bSwappedBothWays = SlotToEquipmentMap.Contains(Entry.EquipmentSlotTag) && SlotToEquipmentMap.Contains(Entry.LastObservedEquipmentSlotTag);
			const FGameplayTag TagToClear = bSwappedBothWays ? FGameplayTag::EmptyTag : Entry.LastObservedEquipmentSlotTag;
			
			SlotToEquipmentMap.Add(Entry.EquipmentSlotTag, Entry.Instance);
			
			BroadcastChangeMessage(Entry, Entry.EquipmentSlotTag, TagToClear, EObsidianEquipmentChangeType::ECT_ItemSwapped);
			
			if(bSwappedBothWays == false)
			{
				SlotToEquipmentMap.Remove(Entry.LastObservedEquipmentSlotTag);
			}		
		}
		Entry.LastObservedEquipmentSlotTag = Entry.EquipmentSlotTag;

		UE_LOG(LogEquipment, Display, TEXT("Replicated changing [%s] item."), *Entry.Instance->GetItemDebugName());
	}
}

void FObsidianEquipmentList::BroadcastChangeMessage(const FObsidianEquipmentEntry& Entry, const FGameplayTag& EquipmentSlotTag, const FGameplayTag& SlotTagToClear, const EObsidianEquipmentChangeType ChangeType) const
{
	FObsidianEquipmentChangeMessage Message;
	Message.EquipmentOwner = OwnerComponent;
	Message.ItemInstance = Entry.Instance;
	Message.SlotTag = EquipmentSlotTag;
	Message.ChangeType = ChangeType;
	Message.SlotTagToClear = SlotTagToClear;
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSubsystem.BroadcastMessage(ObsidianGameplayTags::Message_Equipment_Changed, Message);
}

