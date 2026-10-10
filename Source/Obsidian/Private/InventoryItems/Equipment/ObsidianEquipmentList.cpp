// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/Equipment/ObsidianEquipmentList.h"

#include "AbilitySystemGlobals.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Kismet/GameplayStatics.h"

#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Characters/Heroes/ObsidianHero.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "InventoryItems/Equipment/ObsidianEquipmentComponent.h"
#include "InventoryItems/ItemDrop/ObsidianItemDataLoaderSubsystem.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianItemsFunctionLibrary.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "Obsidian/ObsidianLogCategories.h"


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

EObsidianPlacingAtSlotResult FObsidianEquipmentSlotDefinition::CanEquipAtSlot(const FGameplayTag& InItemCategory) const
{
	return BaseSlotDefinition.CanPlaceAtSlot(InItemCategory);
}

void FObsidianEquipmentSlotDefinition::AddBannedEquipmentCategory(const FGameplayTag& InBannedCategory)
{
	BaseSlotDefinition.AddBannedItemCategory(InBannedCategory);
}

void FObsidianEquipmentSlotDefinition::AddBannedEquipmentCategories(const FGameplayTagContainer& InBannedCategories)
{
	BaseSlotDefinition.AddBannedItemCategories(InBannedCategories);
}

void FObsidianEquipmentSlotDefinition::RemoveBannedEquipmentCategory(const FGameplayTag& InBannedCategoryToRemove)
{
	BaseSlotDefinition.RemoveBannedItemCategory(InBannedCategoryToRemove);
}

void FObsidianEquipmentSlotDefinition::RemoveBannedEquipmentCategories(const FGameplayTagContainer& InBannedCategoriesToRemove)
{
	BaseSlotDefinition.RemoveBannedItemCategories(InBannedCategoriesToRemove);
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

UObsidianInventoryItemInstance* FObsidianEquipmentList::GetEquipmentPieceByTag(const FGameplayTag& InSlotTag) const
{
	if (UObsidianInventoryItemInstance* const* Item = SlotToEquipmentMap.Find(InSlotTag))
	{
		return *Item;
	}
	return nullptr;
}

UObsidianAbilitySystemComponent* FObsidianEquipmentList::GetObsidianAbilitySystemComponent() const
{
	check(OwnerComponent);
	const AObsidianPlayerController* OwningController = Cast<AObsidianPlayerController>(OwnerComponent->GetOwner());
	return OwningController ? OwningController->GetObsidianAbilitySystemComponent() : nullptr;
}

AObsidianHero* FObsidianEquipmentList::GetObsidianHero() const
{
	check(OwnerComponent);
	const AObsidianPlayerController* OwningController = Cast<AObsidianPlayerController>(OwnerComponent->GetOwner());
	return OwningController ? OwningController->GetObsidianHero() : nullptr;
}

FObsidianEquipmentSlotDefinition FObsidianEquipmentList::FindEquipmentSlotByTag(const FGameplayTag& InSlotTag)
{
	for(const FObsidianEquipmentSlotDefinition& Slot : EquipmentSlots)
	{
		if (Slot.GetEquipmentSlotTag() == InSlotTag)
		{
			return Slot;
		}
	}

	return FObsidianEquipmentSlotDefinition::InvalidSlot;
}

TArray<FObsidianEquipmentSlotDefinition> FObsidianEquipmentList::FindMatchingEquipmentSlotsForItemCategory(const FGameplayTag& InItemCategory)
{
	TArray<FObsidianEquipmentSlotDefinition> MatchingSlots;
	
	for(const FObsidianEquipmentSlotDefinition& Slot : EquipmentSlots)
	{
		if(Slot.CanEquipAtSlot(InItemCategory) == EObsidianPlacingAtSlotResult::CanPlace)
		{
			MatchingSlots.Add(Slot);
		}
	}
	
	return MatchingSlots;
}

UObsidianInventoryItemInstance* FObsidianEquipmentList::AddEntry(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDefClass,
	const FObsidianItemGeneratedData& InItemGeneratedData, const FGameplayTag& InEquipmentSlotTag)
{
	check(InItemDefClass != nullptr);
	check(OwnerComponent);

	const AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor);
	
	if(ValidateEquipmentSlot(InEquipmentSlotTag) == false)
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Provided EquipmentSlotTag [%s] does not match any EquipmentSlot."),
			*InEquipmentSlotTag.GetTagName().ToString()), ELogVerbosity::Error);
		return nullptr;
	}

	FObsidianEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = UObsidianItemsFunctionLibrary::CreateItemInstanceFromDefinition(OwnerComponent->GetOwner(), InItemDefClass,
		InItemGeneratedData, InEquipmentSlotTag);
	NewEntry.EquipmentSlotTag = InEquipmentSlotTag;
	
	UObsidianInventoryItemInstance* Item = NewEntry.Instance;
	SlotToEquipmentMap.Add(InEquipmentSlotTag, Item);
	
	AddItemAffixesToOwner(Item, &NewEntry.GrantedHandles);
	
	Item->SpawnEquipmentActors(InEquipmentSlotTag);

	MarkItemDirty(NewEntry);

	BroadcastChangeMessage(NewEntry, InEquipmentSlotTag, FGameplayTag::EmptyTag, EObsidianEquipmentChangeType::ECT_ItemEquipped);
	return Item;
}

void FObsidianEquipmentList::AddEntry(UObsidianInventoryItemInstance* InInstance, const FGameplayTag& InEquipmentSlotTag)
{
	check(InInstance);
	check(OwnerComponent);

	const AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor);
	
	if(ValidateEquipmentSlot(InEquipmentSlotTag) == false)
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Provided EquipmentSlotTag [%s] does not match any EquipmentSlot."),
			*InEquipmentSlotTag.GetTagName().ToString()), ELogVerbosity::Error);
		return;
	}

#if !UE_BUILD_SHIPPING
	if(SlotToEquipmentMap.Contains(InEquipmentSlotTag))
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Provided EquipmentSlotTag [%s] already contains an item."),
			*InEquipmentSlotTag.GetTagName().ToString()), ELogVerbosity::Error);
	}
#endif

	FObsidianEquipmentEntry& NewEntry = Entries.Emplace_GetRef(InInstance, InEquipmentSlotTag);
	SlotToEquipmentMap.Add(InEquipmentSlotTag, InInstance);
	InInstance->SetItemCurrentPosition(InEquipmentSlotTag);

	AddItemAffixesToOwner(InInstance, &NewEntry.GrantedHandles);
	
	InInstance->SpawnEquipmentActors(InEquipmentSlotTag);

	MarkItemDirty(NewEntry);

	BroadcastChangeMessage(NewEntry, InEquipmentSlotTag, FGameplayTag::EmptyTag, EObsidianEquipmentChangeType::ECT_ItemEquipped);
}

UObsidianInventoryItemInstance* FObsidianEquipmentList::LoadEntry(const FObsidianSavedItem& InEquippedSavedItem)
{
	check(OwnerComponent);
	
	UObsidianInventoryItemInstance* LoadedInstance = NewObject<UObsidianInventoryItemInstance>(OwnerComponent->GetOwner());
	LoadedInstance->ConstructFromSavedItem(InEquippedSavedItem);

	const FGameplayTag LoadedSlotTag = LoadedInstance->GetItemCurrentPosition().GetItemSlotTag();
	FObsidianEquipmentEntry& NewEntry = Entries.Emplace_GetRef(LoadedInstance, LoadedSlotTag);
	SlotToEquipmentMap.Add(LoadedSlotTag, LoadedInstance);
	
	AddItemAffixesToOwner(LoadedInstance, &NewEntry.GrantedHandles);
	
	LoadedInstance->SpawnEquipmentActors(LoadedSlotTag);

	MarkItemDirty(NewEntry);

	BroadcastChangeMessage(NewEntry, LoadedSlotTag, FGameplayTag::EmptyTag, EObsidianEquipmentChangeType::ECT_ItemEquipped);

	return LoadedInstance;
}

void FObsidianEquipmentList::RemoveEntry(UObsidianInventoryItemInstance* InInstance)
{
	const AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor);
	
	for(auto It = Entries.CreateIterator(); It; ++It)
	{
		FObsidianEquipmentEntry& Entry = *It;
		if(Entry.Instance == InInstance)
		{
			const FGameplayTag CachedSlotTag = Entry.EquipmentSlotTag;
			
			SlotToEquipmentMap.Remove(CachedSlotTag);
			InInstance->ResetItemCurrentPosition();
			
			if(UObsidianAbilitySystemComponent* ObsidianASC = GetObsidianAbilitySystemComponent())
			{
				Entry.GrantedHandles.TakeFromAbilitySystem(ObsidianASC);
			}
			else
			{
				FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Obsidian Ability Sytem Component is invalid on Owning Actor [%s]."),
					*GetNameSafe(OwningActor)), ELogVerbosity::Error);
			}
			InInstance->DestroyEquipmentActors();
			
			
			It.RemoveCurrent();
			MarkArrayDirty();

			BroadcastChangeMessage(InInstance, FGameplayTag::EmptyTag, CachedSlotTag, EObsidianEquipmentChangeType::ECT_ItemUnequipped);
		}
	}
}

void FObsidianEquipmentList::MoveWeaponToSwap(UObsidianInventoryItemInstance* InInstance)
{
	check(InInstance);
	check(OwnerComponent);

	const AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor);
	
	const FGameplayTag CurrentWeaponSlotTag = InInstance->GetItemCurrentPosition().GetItemSlotTag();

#if !UE_BUILD_SHIPPING
	const FGameplayTag WeaponSlotTag = FGameplayTag::RequestGameplayTag("Item.Slot.Equipment.Weapon");
	if(CurrentWeaponSlotTag.MatchesTag(WeaponSlotTag) == false)
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Provided Instance [%s] with Tag [%s] is not currentely in Weapon Slot, swapping shouldn't happen."),
			*InInstance->GetItemDebugName(), *CurrentWeaponSlotTag.GetTagName().ToString()), ELogVerbosity::Error);
	}
	
	if(SlotToEquipmentMap.Contains(CurrentWeaponSlotTag) == false)
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Provided EquipmentSlotTag [%s] is not equipped, why swapping?"),
			*CurrentWeaponSlotTag.GetTagName().ToString()), ELogVerbosity::Error);
	}
#endif

	FGameplayTag SwapTag = ObsidianGameplayTags::Item::SwapSlot::Equipment::Weapon::RightHand;
	if(CurrentWeaponSlotTag == ObsidianGameplayTags::Item::Slot::Equipment::Weapon::LeftHand)
	{
		SwapTag = ObsidianGameplayTags::Item::SwapSlot::Equipment::Weapon::LeftHand;
	}

	bool bSuccess = false;
	for(FObsidianEquipmentEntry& Entry : Entries)
	{
		if(Entry.Instance == InInstance)
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
			InInstance->DestroyEquipmentActors();
			
			MarkItemDirty(Entry);
			bSuccess = true;
		}
	}

	if(bSuccess)
	{
		InInstance->SetItemCurrentPosition(SwapTag);
		
		//TODO(intrxx) Do anything unequipping related

		const bool bSwappedBothWays = SlotToEquipmentMap.Contains(CurrentWeaponSlotTag) && SlotToEquipmentMap.Contains(SwapTag);
		const FGameplayTag TagToClear = bSwappedBothWays ? FGameplayTag::EmptyTag : CurrentWeaponSlotTag;
		
		SlotToEquipmentMap.Add(SwapTag, InInstance);
		
		BroadcastChangeMessage(InInstance, SwapTag, TagToClear, EObsidianEquipmentChangeType::ECT_ItemSwapped);
		
		if(bSwappedBothWays == false)
		{
			SlotToEquipmentMap.Remove(CurrentWeaponSlotTag);
		}
	}
}

void FObsidianEquipmentList::MoveWeaponFromSwap(UObsidianInventoryItemInstance* InInstance)
{
	check(InInstance);
	check(OwnerComponent);

	const AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor);
	
	const FGameplayTag CurrentSwapTag = InInstance->GetItemCurrentPosition().GetItemSlotTag();

#if !UE_BUILD_SHIPPING
	const FGameplayTag WeaponSlotTag = FGameplayTag::RequestGameplayTag("Item.SwapSlot.Equipment.Weapon");
	if(CurrentSwapTag.MatchesTag(WeaponSlotTag) == false)
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Provided Instance [%s] with Tag [%s] is not currentely in Weapon Slot, swapping shouldn't happen."),
			*InInstance->GetItemDebugName(), *CurrentSwapTag.GetTagName().ToString()), ELogVerbosity::Error);
	}
#endif

	FGameplayTag MainWeaponSlotTag = ObsidianGameplayTags::Item::Slot::Equipment::Weapon::RightHand;
	if(CurrentSwapTag == ObsidianGameplayTags::Item::SwapSlot::Equipment::Weapon::LeftHand)
	{
		MainWeaponSlotTag = ObsidianGameplayTags::Item::Slot::Equipment::Weapon::LeftHand;
	}

	bool bSuccess = false;
	for(FObsidianEquipmentEntry& Entry : Entries)
	{
		if(Entry.Instance == InInstance)
		{
			Entry.EquipmentSlotTag = MainWeaponSlotTag;
			Entry.bSwappedOut = false;
			
			AddItemAffixesToOwner(InInstance, &Entry.GrantedHandles);
			
			InInstance->SpawnEquipmentActors(MainWeaponSlotTag);
			
			MarkItemDirty(Entry);
			bSuccess = true;
		}
	}

	if(bSuccess)
	{
		InInstance->SetItemCurrentPosition(MainWeaponSlotTag);

		//TODO(intrxx) Do anything equipping related

		const bool bSwappedBothWays = SlotToEquipmentMap.Contains(CurrentSwapTag) && SlotToEquipmentMap.Contains(MainWeaponSlotTag);
		const FGameplayTag TagToClear = bSwappedBothWays ? FGameplayTag::EmptyTag : CurrentSwapTag;
		
		SlotToEquipmentMap.Add(MainWeaponSlotTag, InInstance);
		
		BroadcastChangeMessage(InInstance, MainWeaponSlotTag, TagToClear, EObsidianEquipmentChangeType::ECT_ItemSwapped);

		if(bSwappedBothWays == false)
		{
			SlotToEquipmentMap.Remove(CurrentSwapTag);	
		}
	}
}

bool FObsidianEquipmentList::ValidateEquipmentSlot(const FGameplayTag& InSlotTag)
{
	for(const FGameplayTag& Tag : ObsidianGameplayTags::EquipmentSlots)
	{
		if(Tag == InSlotTag)
		{
			return true;
		}
	}
	return false;
}

void FObsidianEquipmentList::PreReplicatedRemove(const TArrayView<int32> InRemovedIndices, int32 InFinalSize)
{
	for(const int32 Index : InRemovedIndices)
	{
		FObsidianEquipmentEntry& Entry = Entries[Index];
		if(Entry.Instance == nullptr || Entry.LastObservedEquipmentSlotTag == FGameplayTag::EmptyTag) // Item was never added on this Client.
		{
			continue;
		}

		Entry.LastObservedEquipmentSlotTag = FGameplayTag::EmptyTag;
		SlotToEquipmentMap.Remove(Entry.EquipmentSlotTag);
		
		BroadcastChangeMessage(Entry, FGameplayTag::EmptyTag, Entry.EquipmentSlotTag, EObsidianEquipmentChangeType::ECT_ItemUnequipped);

		UE_LOG(ObLogEquipment, Verbose, TEXT("Replicated un-equipping [%s] item."), *Entry.Instance->GetItemDebugName());
	}
}

void FObsidianEquipmentList::PostReplicatedAdd(const TArrayView<int32> InAddedIndices, int32 InFinalSize)
{
	for(const int32 Index : InAddedIndices)
	{
		FObsidianEquipmentEntry& Entry = Entries[Index];
		if(Entry.Instance == nullptr)
		{
			// The Item Instance subobject did not arrive yet, PostReplicatedChange will add the Item once it gets resolved.
			UE_LOG(ObLogEquipment, Verbose, TEXT("Replicated Item at index [%d] has no Instance yet, deferring."), Index);
			continue;
		}

		Entry.LastObservedEquipmentSlotTag = Entry.EquipmentSlotTag;
		SlotToEquipmentMap.Add(Entry.EquipmentSlotTag, Entry.Instance);
		
		BroadcastChangeMessage(Entry, Entry.EquipmentSlotTag, FGameplayTag::EmptyTag, EObsidianEquipmentChangeType::ECT_ItemEquipped);

		UE_LOG(ObLogEquipment, Verbose, TEXT("Replicated equipping [%s] item."), *Entry.Instance->GetItemDebugName());
	}
}

void FObsidianEquipmentList::PostReplicatedChange(const TArrayView<int32> InChangedIndices, int32 InFinalSize)
{
	for(const int32 Index : InChangedIndices)
	{
		FObsidianEquipmentEntry& Entry = Entries[Index];
		if(Entry.Instance == nullptr)
		{
			continue;
		}

		if(Entry.LastObservedEquipmentSlotTag == FGameplayTag::EmptyTag) // Adding was deferred until the Item Instance got resolved.
		{
			int32 AddedIndex = Index;
			PostReplicatedAdd(MakeArrayView(&AddedIndex, 1), InFinalSize);
			continue;
		}

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

		UE_LOG(ObLogEquipment, Verbose, TEXT("Replicated changing [%s] item."), *Entry.Instance->GetItemDebugName());
	}
}

void FObsidianEquipmentList::AddItemAffixesToOwner(UObsidianInventoryItemInstance* InFromItemInstance, FObsidianAffixAbilitySet_GrantedHandles* OutItemGrantedHandles)
{
	UObsidianAbilitySystemComponent* ObsidianASC = GetObsidianAbilitySystemComponent();
	if(ObsidianASC == nullptr)
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Obsidian Ability System Component is invalid on Owner [%s]."),
			*GetNameSafe(OwnerComponent)), ELogVerbosity::Error);
		return;
	}

	AObsidianHero* ObsidianHero = GetObsidianHero();
	if(ObsidianHero == nullptr)
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Obsidian Hero Owner is invalid on Owner [%s]."),
			*GetNameSafe(OwnerComponent)), ELogVerbosity::Error);
		return;
	}

	TArray<FObsidianActiveItemAffix> BatchedAffixesToAdd;
	for(const FObsidianActiveItemAffix& ItemAffix : InFromItemInstance->GetAllItemAffixes())
	{
		if (const UObsidianAffixAbilitySet* AffixAbilitySet = ItemAffix.SoftAbilitySetToApply.LoadSynchronous()) // Item has a unique Ability Set, add from it.
		{
			AffixAbilitySet->GiveToAbilitySystem(ObsidianASC, ItemAffix.AffixTag, ItemAffix.CurrentAffixValue, OutItemGrantedHandles, ObsidianHero);
			continue;
		}
		BatchedAffixesToAdd.Add(ItemAffix);
	}

	if (BatchedAffixesToAdd.IsEmpty() == false)
	{
		CachedDefaultAbilitySet = CachedDefaultAbilitySet == nullptr ? GetDefaultAffixSet() : CachedDefaultAbilitySet;
		if (CachedDefaultAbilitySet)
		{
			CachedDefaultAbilitySet->GiveItemAffixesToAbilitySystem(ObsidianASC, BatchedAffixesToAdd, OutItemGrantedHandles, ObsidianHero);
		}
	}
}

UObsidianAffixAbilitySet* FObsidianEquipmentList::GetDefaultAffixSet()
{
	if (OwnerComponent == nullptr)
	{
		UE_LOG(ObLogAffixes, Error, TEXT("OwnerComponent is nullptr in [%hs]"), __FUNCTION__);
		return nullptr;
	}
	
	const UWorld* World = OwnerComponent->GetWorld();
	if (World == nullptr)
	{
		UE_LOG(ObLogAffixes, Error, TEXT("World is nullptr in [%hs]"), __FUNCTION__);
		return nullptr;
	}
	
	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(World);
	if (GameInstance == nullptr)
	{
		UE_LOG(ObLogAffixes, Error, TEXT("GameInstance is nullptr in [%hs]"), __FUNCTION__);
		return nullptr;
	}
	
	if (const UObsidianItemDataLoaderSubsystem* DataLoaderSubsystem = GameInstance->GetSubsystem<UObsidianItemDataLoaderSubsystem>())
	{
		return DataLoaderSubsystem->DefaultAffixAbilitySet;
	}
	return nullptr;
}

void FObsidianEquipmentList::BroadcastChangeMessage(const FObsidianEquipmentEntry& InEntry, const FGameplayTag& InEquipmentSlotTag, const FGameplayTag& InSlotTagToClear, const EObsidianEquipmentChangeType InChangeType) const
{
	FObsidianEquipmentChangeMessage Message;
	Message.EquipmentOwner = OwnerComponent;
	Message.ItemInstance = InEntry.Instance;
	Message.SlotTag = InEquipmentSlotTag;
	Message.ChangeType = InChangeType;
	Message.SlotTagToClear = InSlotTagToClear;
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSubsystem.BroadcastMessage(ObsidianGameplayTags::Message::Equipment::Changed, Message);
}



