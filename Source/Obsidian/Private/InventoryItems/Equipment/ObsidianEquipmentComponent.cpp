// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Equipment/ObsidianEquipmentComponent.h"

// ~ Core
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

// ~ Project
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "Core/ObsidianGameplayStatics.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/Equipment/ObsidianSpawnedEquipmentPiece.h"

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

void UObsidianEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	EquipDefaultItems();
}

void UObsidianEquipmentComponent::EquipDefaultItems()
{
	const AActor* OwningActor = GetOwner();
	if(OwningActor && OwningActor->HasAuthority())
	{
		for(const FObsidianDefaultEquipmentTemplate& DefaultEquipmentTemplate : DefaultEquipmentItems)
		{
			if(DefaultEquipmentTemplate.EquipmentSlotTag == FGameplayTag::EmptyTag)
			{
				continue;
			}

			const FObsidianEquipmentResult Result = EquipItemToSpecificSlot(DefaultEquipmentTemplate.DefaultItemDef, DefaultEquipmentTemplate.EquipmentSlotTag, DefaultEquipmentTemplate.StackCount);
			ensureMsgf(Result, TEXT("Were unable to equip default item in UObsidianEquipmentComponent::EquipDefaultItems()."));
		}
	}
}

TArray<FObsidianEquipmentSlotDefinition> UObsidianEquipmentComponent::Internal_GetEquipmentSlots() const
{
	return EquipmentList.EquipmentSlots;
}

UObsidianInventoryComponent* UObsidianEquipmentComponent::GetInventoryComponentFromOwner() const
{
	if(const AObsidianPlayerController* OwningPlayerController = Cast<AObsidianPlayerController>(GetOwner()))
	{
		return OwningPlayerController->GetInventoryComponent();
	}
	return nullptr;
}

UObsidianInventoryItemInstance* UObsidianEquipmentComponent::GetEquippedInstanceAtSlot(const FGameplayTag& SlotTag) const
{
	return EquipmentList.GetEquipmentPieceByTag(SlotTag);
}

UObsidianInventoryItemInstance* UObsidianEquipmentComponent::GetEquippedInstanceAtSlot(const FObsidianEquipmentSlotDefinition& Slot) const
{
	return EquipmentList.GetEquipmentPieceByTag(Slot.GetEquipmentSlotTag());
}

TArray<UObsidianInventoryItemInstance*> UObsidianEquipmentComponent::GetAllEquippedItems() const
{
	return EquipmentList.GetAllEquippedItems();
}

USkeletalMeshComponent* UObsidianEquipmentComponent::GetMainEquippedMeshFromSlot(const FGameplayTag& SlotTag) const
{
	if(const UObsidianInventoryItemInstance* Instance = GetEquippedInstanceAtSlot(SlotTag))
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

TArray<FObsidianEquipmentSlotDefinition> UObsidianEquipmentComponent::FindMatchingEquipmentSlotsForItemCategory(const FGameplayTag& ItemCategory)
{
	return EquipmentList.FindMatchingEquipmentSlotsForItemCategory(ItemCategory);
}

bool UObsidianEquipmentComponent::IsItemEquippedAtSlot(const FGameplayTag& SlotTag)
{
	if(GetEquippedInstanceAtSlot(SlotTag))
	{
		return true;
	}
	return false;
}

bool UObsidianEquipmentComponent::CanOwnerModifyEquipmentState()
{
	if(CachedOwnerPlayerController == nullptr)
	{
		CachedOwnerPlayerController = Cast<AObsidianPlayerController>(GetOwner());
	}
	if(const UObsidianAbilitySystemComponent* OwnerASC = CachedOwnerPlayerController->GetObsidianAbilitySystemComponent())
	{
		return !OwnerASC->HasMatchingGameplayTag(ObsidianGameplayTags::Equipment_BlockActions);
	}
	return false;
}

FObsidianEquipmentResult UObsidianEquipmentComponent::AutomaticallyEquipItem(UObsidianInventoryItemInstance* InstanceToEquip)
{
	FObsidianEquipmentResult Result = FObsidianEquipmentResult();
	
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::AddItemInstance."));
		return Result; 
	}

	if(InstanceToEquip == nullptr)
	{
		return Result;
	}

	if(CanOwnerModifyEquipmentState() == false)
	{
		return Result;
	}

	const FGameplayTag& ItemCategoryTag = InstanceToEquip->GetItemCategoryTag();
	const bool bIsTwoHanded = UObsidianGameplayStatics::DoesTagMatchesAnySubTag(ItemCategoryTag, TAG_Obsidian_TwoHand);
	for(FObsidianEquipmentSlotDefinition Slot : FindMatchingEquipmentSlotsForItemCategory(ItemCategoryTag))
	{
		const FGameplayTag SlotTag = Slot.GetEquipmentSlotTag();
		if(EquipmentList.SlotToEquipmentMap.Contains(SlotTag) || (bIsTwoHanded && EquipmentList.SlotToEquipmentMap.Contains(Slot.SisterSlotTag)))
		{
			continue; // We already have an item equipped in this slot, we shouldn't try to equip it. || Initial slot is free but the other one is occupied so we don't want to automatically equip.
		}
		
		if(EquipItemToSpecificSlot(InstanceToEquip, SlotTag))
		{
			Result.bActionSuccessful = true;
			Result.AffectedInstance = InstanceToEquip;
			return Result;
		}
	}

	return Result;
}

FObsidianEquipmentResult UObsidianEquipmentComponent::EquipItemToSpecificSlot(UObsidianInventoryItemInstance* InstanceToEquip, const FGameplayTag& SlotTag)
{
	FObsidianEquipmentResult Result = FObsidianEquipmentResult();
	
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::AddItemInstance."));
		return Result; 
	}

	if(InstanceToEquip == nullptr)
	{
		return Result;
	}

	const EObsidianEquipCheckResult EquipResult = CanEquipInstance(InstanceToEquip, SlotTag);
	if(EquipResult != EObsidianEquipCheckResult::CanEquip)
	{
		//TODO Send Client RPC with some voice over passing EquipResult?
#if !UE_BUILD_SHIPPING
		UE_LOG(LogEquipment, Warning, TEXT("Item cannot be equipped, reason: [%s]"), *ObsidianEquipmentDebugHelpers::GetEquipResultString(EquipResult));
#endif
		return Result;
	}
	
	EquipmentList.AddEntry(InstanceToEquip, SlotTag);
	
	if(InstanceToEquip && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(InstanceToEquip);
	}
	
	Result.bActionSuccessful = true;
	Result.AffectedInstance = InstanceToEquip;
	return Result;
}

FObsidianEquipmentResult UObsidianEquipmentComponent::ReplaceItemAtSpecificSlot(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FGameplayTag& SlotTag, const FGameplayTag& EquipSlotTagOverride)
{
	FObsidianEquipmentResult Result = FObsidianEquipmentResult();
	
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::AddItemInstance."));
		return Result; 
	}

	if(ItemDef == nullptr)
	{
		return Result;
	}

	const UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject();
	if(DefaultObject == nullptr)
	{
		return Result;
	}

	EObsidianEquipCheckResult EquipResult = CanReplaceTemplate(ItemDef, SlotTag);
	if(EquipResult != EObsidianEquipCheckResult::CanEquip)
	{
		//TODO Send Client RPC to add voiceover passing EquipResult
#if !UE_BUILD_SHIPPING
		UE_LOG(LogEquipment, Warning, TEXT("Item cannot be equipped, reason: [%s]"), *ObsidianEquipmentDebugHelpers::GetEquipResultString(EquipResult));
#endif
		return Result;
	}

	if(DefaultObject->DoesItemNeedsTwoSlots())
	{
		const FObsidianEquipmentSlotDefinition PressedSlot = FindEquipmentSlotByTag(SlotTag);
		
		if(UObsidianInventoryItemInstance* InstanceAtSecondSlot = GetEquippedInstanceAtSlot(PressedSlot.SisterSlotTag))
		{
			UObsidianInventoryComponent* InventoryComponent = GetInventoryComponentFromOwner();
			if(InventoryComponent == nullptr)
			{
				return Result;
			}

			if(InventoryComponent->CanOwnerModifyInventoryState() == false)
			{
				return Result;
			}
			
			const FObsidianEquipmentResult LocalUnequippingResult = UnequipItem(InstanceAtSecondSlot);
			checkf(LocalUnequippingResult, TEXT("Unequipping item failed in UObsidianEquipmentComponent::ReplaceItemAtSpecificSlot."));
			
			const FObsidianItemOperationResult LocalAddingResult = InventoryComponent->AddItemInstance(InstanceAtSecondSlot);
			checkf(LocalAddingResult, TEXT("Adding item to Inventory failed in UObsidianEquipmentComponent::ReplaceItemAtSpecificSlot."));
		}
	}

	const FGameplayTag EquipTag = EquipSlotTagOverride == FGameplayTag::EmptyTag ? SlotTag : EquipSlotTagOverride;
	UObsidianInventoryItemInstance* Instance = EquipmentList.AddEntry(ItemDef, EquipTag);

	if(Instance && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(Instance);
	}

	Result.bActionSuccessful = Instance != nullptr;
	Result.AffectedInstance = Instance;
	return Result;
}

FObsidianEquipmentResult UObsidianEquipmentComponent::ReplaceItemAtSpecificSlot(UObsidianInventoryItemInstance* InstanceToEquip, const FGameplayTag& SlotTag, const FGameplayTag& EquipSlotTagOverride)
{
	FObsidianEquipmentResult Result = FObsidianEquipmentResult();
	
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::AddItemInstance."));
		return Result; 
	}

	if(InstanceToEquip == nullptr)
	{
		return Result;
	}

	const EObsidianEquipCheckResult EquipResult = CanReplaceInstance(InstanceToEquip, SlotTag);
	if(EquipResult != EObsidianEquipCheckResult::CanEquip)
	{
		//TODO Send Client RPC with some voice over passing EquipResult?
#if !UE_BUILD_SHIPPING
		UE_LOG(LogEquipment, Warning, TEXT("Item cannot be equipped, reason: [%s]"), *ObsidianEquipmentDebugHelpers::GetEquipResultString(EquipResult));
#endif
		return Result;
	}

	if(InstanceToEquip->DoesItemNeedTwoSlots())
	{
		const FObsidianEquipmentSlotDefinition PressedSlot = FindEquipmentSlotByTag(SlotTag);
		
		if(UObsidianInventoryItemInstance* InstanceAtSecondSlot = GetEquippedInstanceAtSlot(PressedSlot.SisterSlotTag))
		{
			UObsidianInventoryComponent* InventoryComponent = GetInventoryComponentFromOwner();
			if(InventoryComponent == nullptr)
			{
				return Result;
			}

			if(InventoryComponent->CanOwnerModifyInventoryState() == false)
			{
				return Result;
			}
			
			const FObsidianEquipmentResult LocalUnequippingResult = UnequipItem(InstanceAtSecondSlot);
			checkf(LocalUnequippingResult, TEXT("Unequipping item failed in UObsidianEquipmentComponent::ReplaceItemAtSpecificSlot."));
			
			const FObsidianItemOperationResult LocalAddingResult = InventoryComponent->AddItemInstance(InstanceAtSecondSlot);
			checkf(LocalAddingResult, TEXT("Adding item to Inventory failed in UObsidianEquipmentComponent::ReplaceItemAtSpecificSlot."));
		}
	}

	const FGameplayTag EquipTag = EquipSlotTagOverride == FGameplayTag::EmptyTag ? SlotTag : EquipSlotTagOverride;
	EquipmentList.AddEntry(InstanceToEquip, EquipTag);
	
	if(InstanceToEquip && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(InstanceToEquip);
	}

	Result.bActionSuccessful = true;
	Result.AffectedInstance = InstanceToEquip;
	return Result;
}

EObsidianEquipCheckResult UObsidianEquipmentComponent::CanEquipInstance(const UObsidianInventoryItemInstance* Instance, const FGameplayTag& SlotTag)
{
	if(Instance == nullptr)
	{
		return EObsidianEquipCheckResult::None;
	}

	if(CanOwnerModifyEquipmentState() == false)
	{
		return EObsidianEquipCheckResult::EquipmentActionsBlocked;
	}
	
	if(Instance->IsItemEquippable() == false)
	{
		return EObsidianEquipCheckResult::ItemUnequippable;
	}
	
	if(Instance->IsItemIdentified() == false)
	{
		return EObsidianEquipCheckResult::ItemUnientified;
	}

	const FGameplayTag ItemCategoryTag = Instance->GetItemCategoryTag();
	const EObsidianEquipCheckResult Result = CanPlaceItemAtEquipmentSlot(SlotTag, ItemCategoryTag);
	if(Result != EObsidianEquipCheckResult::CanEquip)
	{
		return Result;
	}
	
	return EObsidianEquipCheckResult::CanEquip;
}

FObsidianEquipmentResult UObsidianEquipmentComponent::AutomaticallyEquipItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const int32 StackCount)
{
	FObsidianEquipmentResult Result = FObsidianEquipmentResult();
	
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::AddItemInstance."));
		return Result; 
	}

	if(ItemDef == nullptr)
	{
		return Result;
	}

	UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject();
	if(DefaultObject == nullptr)
	{
		return Result;
	}

	if(CanOwnerModifyEquipmentState() == false)
	{
		return Result;
	}
	
	const FGameplayTag& ItemCategoryTag = DefaultObject->GetItemCategoryTag();
	const bool bIsTwoHanded = UObsidianGameplayStatics::DoesTagMatchesAnySubTag(ItemCategoryTag, TAG_Obsidian_TwoHand);
	for(const FObsidianEquipmentSlotDefinition& Slot : FindMatchingEquipmentSlotsForItemCategory(ItemCategoryTag))
	{
		const FGameplayTag SlotTag = Slot.GetEquipmentSlotTag();
		if(EquipmentList.SlotToEquipmentMap.Contains(SlotTag) || (bIsTwoHanded && EquipmentList.SlotToEquipmentMap.Contains(Slot.SisterSlotTag)))
		{
			continue; // We already have an item equipped in this slot, we shouldn't try to equip it. || Initial slot is free but the other one is occupied so we don't want to automatically equip.
		}
		
		if(const FObsidianEquipmentResult& InternalResult = EquipItemToSpecificSlot(ItemDef, SlotTag, StackCount))
		{
			return InternalResult;
		}
	}

	return Result;
}

FObsidianEquipmentResult UObsidianEquipmentComponent::EquipItemToSpecificSlot(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FGameplayTag& SlotTag, const int32 StackCount)
{
	FObsidianEquipmentResult Result = FObsidianEquipmentResult();
	
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::AddItemInstance."));
		return Result; 
	}

	if(ItemDef == nullptr)
	{
		return Result;
	}

	const UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject();
	if(DefaultObject == nullptr)
	{
		return Result;
	}

	EObsidianEquipCheckResult EquipResult = CanEquipTemplate(ItemDef, SlotTag);
	if(EquipResult != EObsidianEquipCheckResult::CanEquip)
	{
		//TODO Send Client RPC to add voiceover passing EquipResult
#if !UE_BUILD_SHIPPING
		UE_LOG(LogEquipment, Warning, TEXT("Item cannot be equipped, reason: [%s]"), *ObsidianEquipmentDebugHelpers::GetEquipResultString(EquipResult));
#endif
		return Result;
	}
	
	UObsidianInventoryItemInstance* Instance = EquipmentList.AddEntry(ItemDef, SlotTag);
	Instance->AddItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, StackCount);
	
	if(Instance && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(Instance);
	}

	Result.bActionSuccessful = Instance != nullptr;
	Result.AffectedInstance = Instance;
	return Result;
}

EObsidianEquipCheckResult UObsidianEquipmentComponent::CanEquipTemplate(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FGameplayTag& SlotTag)
{
	if(ItemDef == nullptr)
	{
		return EObsidianEquipCheckResult::None;
	}

	const UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject();
	if(DefaultObject == nullptr)
	{
		return EObsidianEquipCheckResult::None;
	}

	if(CanOwnerModifyEquipmentState() == false)
	{
		return EObsidianEquipCheckResult::EquipmentActionsBlocked;
	}
	
	if(DefaultObject->IsEquippable() == false)
	{
		return EObsidianEquipCheckResult::ItemUnequippable;
	}
	
	if(DefaultObject->IsIdentified() == false)
	{
		return EObsidianEquipCheckResult::ItemUnientified;
	}

	const FGameplayTag ItemCategoryTag = DefaultObject->GetItemCategoryTag();
	const EObsidianEquipCheckResult Result = CanPlaceItemAtEquipmentSlot(SlotTag, ItemCategoryTag);
	if(Result != EObsidianEquipCheckResult::CanEquip)
	{
		return Result;
	}
	
	return EObsidianEquipCheckResult::CanEquip;
}

EObsidianEquipCheckResult UObsidianEquipmentComponent::CanReplaceInstance(const UObsidianInventoryItemInstance* Instance, const FGameplayTag& SlotTag)
{
	if(Instance == nullptr)
	{
		return EObsidianEquipCheckResult::None;
	}

	if(CanOwnerModifyEquipmentState() == false)
	{
		return EObsidianEquipCheckResult::EquipmentActionsBlocked;
	}
	
	if(Instance->IsItemEquippable() == false)
	{
		return EObsidianEquipCheckResult::ItemUnequippable;
	}
	
	if(Instance->IsItemIdentified() == false)
	{
		return EObsidianEquipCheckResult::ItemUnientified;
	}

	const FGameplayTag ItemCategory = Instance->GetItemCategoryTag();
	const FObsidianEquipmentSlotDefinition Slot = FindEquipmentSlotByTag(SlotTag);
	
	EObsidianEquipCheckResult Result = Slot.CanEquipAtSlot(ItemCategory);
	if(Result != EObsidianEquipCheckResult::CanEquip)
	{
		return Result;
	}
	
	if(Instance->DoesItemNeedTwoSlots())
	{
		UE_LOG(LogTemp, Warning, TEXT("Item needs a second slot."));
		
		UObsidianInventoryItemInstance* InstanceAtSecondSlot = GetEquippedInstanceAtSlot(Slot.SisterSlotTag);
		if(InstanceAtSecondSlot == nullptr) // If there is no item at second slot we can just return as the item at pressed slot will be added to the cursor.
		{
			return Result;
		}

		UObsidianInventoryItemInstance* InstanceAtPressedSlot = GetEquippedInstanceAtSlot(SlotTag);
		if(InstanceAtPressedSlot == nullptr)
		{
			return Result;
		}

		UObsidianInventoryComponent* InventoryComponent = GetInventoryComponentFromOwner();
		if(InventoryComponent == nullptr)
		{
			return Result;
		}
		
		if(InventoryComponent->CanFitItemInstance(InstanceAtSecondSlot) == false)
		{
			return EObsidianEquipCheckResult::UnableToEquip_NoSufficientInventorySpace;
		}
	}
	else
	{
		CheckEquipmentPairAcceptance(Slot, ItemCategory, Result);
	}
	
	return Result;
}

EObsidianEquipCheckResult UObsidianEquipmentComponent::CanReplaceTemplate(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FGameplayTag& SlotTag)
{
	if(ItemDef == nullptr)
	{
		return EObsidianEquipCheckResult::None;
	}

	const UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject();
	if(DefaultObject == nullptr)
	{
		return EObsidianEquipCheckResult::None;
	}

	if(CanOwnerModifyEquipmentState() == false)
	{
		return EObsidianEquipCheckResult::EquipmentActionsBlocked;
	}
	
	if(DefaultObject->IsEquippable() == false)
	{
		return EObsidianEquipCheckResult::ItemUnequippable;
	}
	
	if(DefaultObject->IsIdentified() == false)
	{
		return EObsidianEquipCheckResult::ItemUnientified;
	}

	const FGameplayTag ItemCategory = DefaultObject->GetItemCategoryTag();
	const FObsidianEquipmentSlotDefinition Slot = FindEquipmentSlotByTag(SlotTag);
	
	EObsidianEquipCheckResult Result = Slot.CanEquipAtSlot(ItemCategory);
	if(Result != EObsidianEquipCheckResult::CanEquip)
	{
		return Result;
	}

	if(DefaultObject->DoesItemNeedsTwoSlots())
	{
		UE_LOG(LogTemp, Warning, TEXT("Item needs a second slot."));
		
		UObsidianInventoryItemInstance* InstanceAtSecondSlot = GetEquippedInstanceAtSlot(Slot.SisterSlotTag);
		if(InstanceAtSecondSlot == nullptr) // If there is no item at second slot we can just return as the item at pressed slot will be added to the cursor.
		{
			return Result;
		}

		UObsidianInventoryItemInstance* InstanceAtPressedSlot = GetEquippedInstanceAtSlot(SlotTag);
		if(InstanceAtPressedSlot == nullptr)
		{
			return Result;
		}

		UObsidianInventoryComponent* InventoryComponent = GetInventoryComponentFromOwner();
		if(InventoryComponent == nullptr)
		{
			return Result;
		}
		
		if(InventoryComponent->CanFitItemInstance(InstanceAtSecondSlot) == false)
		{
			return EObsidianEquipCheckResult::UnableToEquip_NoSufficientInventorySpace;
		}
	}
	else
	{
		CheckEquipmentPairAcceptance(Slot, ItemCategory, Result);
	}
	
	return Result;
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

FObsidianEquipmentResult UObsidianEquipmentComponent::UnequipItem(UObsidianInventoryItemInstance* InstanceToUnequip)
{
	FObsidianEquipmentResult Result = FObsidianEquipmentResult();
	
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogEquipment, Warning, TEXT("No Authority in UObsidianInventoryComponent::AddItemInstance."));
		return Result; 
	}

	if(InstanceToUnequip == nullptr)
	{
		UE_LOG(LogEquipment, Error, TEXT("Passed InstanceToUnequip is invalid in UObsidianEquipmentComponent::UnequipItem."));
		return Result;
	}

	if(CanOwnerModifyEquipmentState() == false)
	{
		return Result;
	}

	EquipmentList.RemoveEntry(InstanceToUnequip);
	
	if(InstanceToUnequip && IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(InstanceToUnequip);
	}

	Result.bActionSuccessful = true;
	Result.AffectedInstance = InstanceToUnequip;
	return Result;
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

EObsidianEquipCheckResult UObsidianEquipmentComponent::CanPlaceItemAtEquipmentSlot(const FGameplayTag& SlotTag, const FGameplayTag& ItemCategory)
{
	EObsidianEquipCheckResult EquipResult = EObsidianEquipCheckResult::ItemUnfitForCategory;
	
	const FObsidianEquipmentSlotDefinition Slot = FindEquipmentSlotByTag(SlotTag);
	if(Slot.IsValid() == false)
	{
		return EquipResult;
	}
	
	EquipResult = Slot.CanEquipAtSlot(ItemCategory); // Can equip to slot in the first place.
	if(EquipResult != EObsidianEquipCheckResult::CanEquip)
	{
		return EquipResult;
	}

	CheckEquipmentPairAcceptance(Slot, ItemCategory, EquipResult);
	
	return EquipResult;
}

void UObsidianEquipmentComponent::CheckEquipmentPairAcceptance(const FObsidianEquipmentSlotDefinition& PrimarySlot, const FGameplayTag& PrimaryWeaponCategory, EObsidianEquipCheckResult& OutResult)
{
	if(PrimarySlot.SisterSlotTag.IsValid() == false)
	{
		return;
	}
	
	const UObsidianInventoryItemInstance* InstanceAtOtherSlot = GetEquippedInstanceAtSlot(PrimarySlot.SisterSlotTag);
	if (InstanceAtOtherSlot == nullptr)
	{
		return;
	}
		
	const FGameplayTag OtherInstanceCategory = InstanceAtOtherSlot->GetItemCategoryTag();
	if(const FGameplayTagContainer* AcceptedCategories = ObsidianGameplayTags::GetSisterSlotAcceptedCategoriesMap().Find(OtherInstanceCategory))
	{
		if(AcceptedCategories->HasTagExact(PrimaryWeaponCategory) == false)
		{
			OutResult = EObsidianEquipCheckResult::UnableToEquip_DoesNotFitWithOtherWeaponType;
		}
	}
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
		ObsidianGameplayTags::Item_Category_Equipment_Flail, ObsidianGameplayTags::Item_Category_Equipment_Dagger,
		ObsidianGameplayTags::Item_Category_Equipment_Wand, ObsidianGameplayTags::Item_Category_Equipment_Bow,
		ObsidianGameplayTags::Item_Category_Equipment_Staff_TwoHand, ObsidianGameplayTags::Item_Category_Equipment_Mace_OneHand,
		ObsidianGameplayTags::Item_Category_Equipment_Mace_TwoHand, ObsidianGameplayTags::Item_Category_Equipment_Axe_OneHand,
		ObsidianGameplayTags::Item_Category_Equipment_Axe_TwoHand,ObsidianGameplayTags::Item_Category_Equipment_Sword_OneHand,
		ObsidianGameplayTags::Item_Category_Equipment_Sword_TwoHand
		};
	
	const TArray<FGameplayTag> LeftHandAcceptedEquipment =
		{
		ObsidianGameplayTags::Item_Category_Equipment_Flail, ObsidianGameplayTags::Item_Category_Equipment_Dagger,
		ObsidianGameplayTags::Item_Category_Equipment_Wand, ObsidianGameplayTags::Item_Category_Equipment_Mace_OneHand,
		ObsidianGameplayTags::Item_Category_Equipment_Axe_OneHand, ObsidianGameplayTags::Item_Category_Equipment_Sword_OneHand,
		ObsidianGameplayTags::Item_Category_Equipment_Quiver, ObsidianGameplayTags::Item_Category_Equipment_Shield
		};
	
	EquipmentList.EquipmentSlots =
		{
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item_Slot_Equipment_Weapon_RightHand, ObsidianGameplayTags::Item_Slot_Equipment_Weapon_LeftHand, FGameplayTagContainer::CreateFromArray(RightHandAcceptedEquipment))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item_Slot_Equipment_Weapon_LeftHand, ObsidianGameplayTags::Item_Slot_Equipment_Weapon_RightHand, FGameplayTagContainer::CreateFromArray(LeftHandAcceptedEquipment))},
		
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item_Slot_Equipment_Helmet, FGameplayTagContainer(ObsidianGameplayTags::Item_Category_Equipment_Helmet))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item_Slot_Equipment_BodyArmor, FGameplayTagContainer(ObsidianGameplayTags::Item_Category_Equipment_BodyArmor))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item_Slot_Equipment_Belt, FGameplayTagContainer(ObsidianGameplayTags::Item_Category_Equipment_Belt))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item_Slot_Equipment_Gloves, FGameplayTagContainer(ObsidianGameplayTags::Item_Category_Equipment_Gloves))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item_Slot_Equipment_Boots, FGameplayTagContainer(ObsidianGameplayTags::Item_Category_Equipment_Boots))},
		
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item_Slot_Equipment_Amulet, FGameplayTagContainer(ObsidianGameplayTags::Item_Category_Equipment_Amulet))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item_Slot_Equipment_Ring_RightHand, FGameplayTagContainer(ObsidianGameplayTags::Item_Category_Equipment_Ring))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item_Slot_Equipment_Ring_LeftHand, FGameplayTagContainer(ObsidianGameplayTags::Item_Category_Equipment_Ring))},
		};
}



