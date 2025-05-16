// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Equipment/ObsidianEquipmentList.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "AbilitySystem/Data/ObsidianAbilitySet.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
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

TArray<UObsidianInventoryItemInstance*> FObsidianEquipmentList::GetSwappedWeapons()
{
	const FGameplayTag WeaponSwapSlotTag = FGameplayTag::RequestGameplayTag(TEXT("Equipment.SwapSlot.Weapon"));
	
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
	const FGameplayTag WeaponSlotTag = FGameplayTag::RequestGameplayTag(TEXT("Equipment.Slot.Weapon"));
	
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
	for(FObsidianEquipmentSlotDefinition Slot : EquipmentSlots)
	{
		if(Slot.SlotTag == SlotTag)
		{
			return Slot;
		}
	}
	
	return FObsidianEquipmentSlotDefinition();
}

TArray<FObsidianEquipmentSlotDefinition> FObsidianEquipmentList::FindMatchingEquipmentSlotsByItemCategory(const FGameplayTag& ItemCategory)
{
	TArray<FObsidianEquipmentSlotDefinition> MatchingSlots;
	
	for(FObsidianEquipmentSlotDefinition Slot : EquipmentSlots)
	{
		if(Slot.CanEquipToSlot(ItemCategory) == EObsidianEquipResult::CanEquip)
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
	NewEntry.Instance->SetItemCurrentEquipmentSlot(EquipmentSlotTag);

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

	FObsidianEquipmentSlotDefinition Slot = FindEquipmentSlotByTag(EquipmentSlotTag);
	if(Slot.SisterSlotTag.IsValid()) // Slot has a sister slot that needs to be updated with allowed Equipment Types
	{
		const FGameplayTag EquipmentCategory = NewEntry.Instance->GetItemCategoryTag();
		FObsidianEquipmentSlotDefinition SisterSlot = FindEquipmentSlotByTag(Slot.SisterSlotTag);
		SisterSlot.OverrideAcceptedEquipmentCategories(ObsidianInventoryItemsStatics::AcceptedSisterSlotEquipmentCategoriesPerEquipmentCategory[EquipmentCategory]);
	}
	
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
	Instance->SetItemCurrentEquipmentSlot(EquipmentSlotTag);

	FObsidianEquipmentSlotDefinition Slot = FindEquipmentSlotByTag(EquipmentSlotTag);
	if(Slot.SisterSlotTag.IsValid()) // Slot has a sister slot that needs to be updated with allowed Equipment Types
	{
		const FGameplayTag EquipmentCategory = NewEntry.Instance->GetItemCategoryTag();
		FObsidianEquipmentSlotDefinition SisterSlot = FindEquipmentSlotByTag(Slot.SisterSlotTag);
		SisterSlot.OverrideAcceptedEquipmentCategories(ObsidianInventoryItemsStatics::AcceptedSisterSlotEquipmentCategoriesPerEquipmentCategory[EquipmentCategory]);
	}

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
	
	const FGameplayTag CurrentWeaponSlotTag = Instance->GetItemCurrentEquipmentSlot();

#if !UE_BUILD_SHIPPING
	const FGameplayTag WeaponSlotTag = FGameplayTag::RequestGameplayTag("Equipment.Slot.Weapon");
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

	FGameplayTag SwapTag = ObsidianGameplayTags::Equipment_SwapSlot_Weapon_RightHand;
	if(CurrentWeaponSlotTag == ObsidianGameplayTags::Equipment_Slot_Weapon_LeftHand)
	{
		SwapTag = ObsidianGameplayTags::Equipment_SwapSlot_Weapon_LeftHand;
	}

	bool bSuccess = false;
	for(FObsidianEquipmentEntry& Entry : Entries)
	{
		if(Entry.Instance == Instance)
		{
			FObsidianEquipmentSlotDefinition FirstSlot = FindEquipmentSlotByTag(Entry.EquipmentSlotTag);
			FObsidianEquipmentSlotDefinition SisterSlot = FindEquipmentSlotByTag(FirstSlot.SisterSlotTag);
			SisterSlot.ResetAcceptedEquipmentCategoriesToDefault();
			
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
		Instance->SetItemCurrentEquipmentSlot(SwapTag);
		
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
	
	const FGameplayTag CurrentSwapTag = Instance->GetItemCurrentEquipmentSlot();

#if !UE_BUILD_SHIPPING
	const FGameplayTag WeaponSlotTag = FGameplayTag::RequestGameplayTag("Equipment.SwapSlot.Weapon");
	if(CurrentSwapTag.MatchesTag(WeaponSlotTag) == false)
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Provided Instance [%s] with Tag [%s] is not currentely in Weapon Slot, swapping shouldn't happen."),
			*Instance->GetItemDebugName(), *CurrentSwapTag.GetTagName().ToString()), ELogVerbosity::Error);
	}
#endif

	FGameplayTag MainWeaponSlotTag = ObsidianGameplayTags::Equipment_Slot_Weapon_RightHand;
	if(CurrentSwapTag == ObsidianGameplayTags::Equipment_SwapSlot_Weapon_LeftHand)
	{
		MainWeaponSlotTag = ObsidianGameplayTags::Equipment_Slot_Weapon_LeftHand;
	}

	bool bSuccess = false;
	for(FObsidianEquipmentEntry& Entry : Entries)
	{
		if(Entry.Instance == Instance)
		{
			Entry.EquipmentSlotTag = MainWeaponSlotTag;
			Entry.bSwappedOut = false;

			FObsidianEquipmentSlotDefinition Slot = FindEquipmentSlotByTag(MainWeaponSlotTag);
			FObsidianEquipmentSlotDefinition SisterSlot = FindEquipmentSlotByTag(Slot.SisterSlotTag);
			SisterSlot.OverrideAcceptedEquipmentCategories(ObsidianInventoryItemsStatics::AcceptedSisterSlotEquipmentCategoriesPerEquipmentCategory[Entry.Instance->GetItemCategoryTag()]);

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
		Instance->SetItemCurrentEquipmentSlot(MainWeaponSlotTag);

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
			FObsidianEquipmentSlotDefinition Slot = FindEquipmentSlotByTag(CachedSlotTag);
			if(Slot.SisterSlotTag.IsValid()) // Slot has a sister slot that needs to be updated with allowed Equipment Types
			{
				FObsidianEquipmentSlotDefinition SisterSlot = FindEquipmentSlotByTag(Slot.SisterSlotTag);
				SisterSlot.ResetAcceptedEquipmentCategoriesToDefault();
			}
			
			SlotToEquipmentMap.Remove(CachedSlotTag);
			Instance->ResetItemCurrentEquipmentSlot();
			
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

		FObsidianEquipmentSlotDefinition Slot = FindEquipmentSlotByTag(Entry.EquipmentSlotTag);
		if(Slot.SisterSlotTag.IsValid()) // Slot has a sister slot that needs to be updated with allowed Equipment Types
		{
			FObsidianEquipmentSlotDefinition SisterSlot = FindEquipmentSlotByTag(Slot.SisterSlotTag);
			SisterSlot.ResetAcceptedEquipmentCategoriesToDefault();
		}
		
		BroadcastChangeMessage(Entry, FGameplayTag::EmptyTag, Entry.EquipmentSlotTag, EObsidianEquipmentChangeType::ECT_ItemUnequipped);
	}
}

void FObsidianEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for(const int32 Index : AddedIndices)
	{
		FObsidianEquipmentEntry& Entry = Entries[Index];
		Entry.LastObservedEquipmentSlotTag = Entry.EquipmentSlotTag;
		SlotToEquipmentMap.Add(Entry.EquipmentSlotTag, Entry.Instance);

		FObsidianEquipmentSlotDefinition Slot = FindEquipmentSlotByTag(Entry.EquipmentSlotTag);
		if(Slot.SisterSlotTag.IsValid()) // Slot has a sister slot that needs to be updated with allowed Equipment Types
		{
			const FGameplayTag EquipmentCategory = Entry.Instance->GetItemCategoryTag();
			FObsidianEquipmentSlotDefinition SisterSlot = FindEquipmentSlotByTag(Slot.SisterSlotTag);
			SisterSlot.OverrideAcceptedEquipmentCategories(ObsidianInventoryItemsStatics::AcceptedSisterSlotEquipmentCategoriesPerEquipmentCategory[EquipmentCategory]);
		}

		BroadcastChangeMessage(Entry, Entry.EquipmentSlotTag, FGameplayTag::EmptyTag, EObsidianEquipmentChangeType::ECT_ItemEquipped);
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

			FObsidianEquipmentSlotDefinition Slot = FindEquipmentSlotByTag(Entry.EquipmentSlotTag);
			if(Slot.SisterSlotTag.IsValid()) // Slot has a sister slot that needs to be updated with allowed Equipment Types
			{
				const FGameplayTag EquipmentCategory = Entry.Instance->GetItemCategoryTag();
				FObsidianEquipmentSlotDefinition SisterSlot = FindEquipmentSlotByTag(Slot.SisterSlotTag);
				SisterSlot.OverrideAcceptedEquipmentCategories(ObsidianInventoryItemsStatics::AcceptedSisterSlotEquipmentCategoriesPerEquipmentCategory[EquipmentCategory]);
			}
			
			BroadcastChangeMessage(Entry, Entry.EquipmentSlotTag, TagToClear, EObsidianEquipmentChangeType::ECT_ItemSwapped);
			
			if(bSwappedBothWays == false)
			{
				SlotToEquipmentMap.Remove(Entry.LastObservedEquipmentSlotTag);
				
				if(Slot.SisterSlotTag.IsValid()) // Slot has a sister slot that needs to be updated with allowed Equipment Types
				{
					FObsidianEquipmentSlotDefinition SisterSlot = FindEquipmentSlotByTag(Slot.SisterSlotTag);
					SisterSlot.ResetAcceptedEquipmentCategoriesToDefault();
				}
			}		
		}
		Entry.LastObservedEquipmentSlotTag = Entry.EquipmentSlotTag;
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
