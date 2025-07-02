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

			const FObsidianEquipmentResult Result = EquipItemToSpecificSlot(DefaultEquipmentTemplate.DefaultItemDef, DefaultEquipmentTemplate.EquipmentSlotTag);
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

UObsidianInventoryItemInstance* UObsidianEquipmentComponent::GetEquippedInstanceAtSlot(const FGameplayTag& SlotTag)
{
	if(EquipmentList.SlotToEquipmentMap.Contains(SlotTag))
	{
		return EquipmentList.SlotToEquipmentMap[SlotTag];
	}
	return nullptr;
}

UObsidianInventoryItemInstance* UObsidianEquipmentComponent::GetEquippedInstanceAtSlot(const FObsidianEquipmentSlotDefinition& Slot)
{
	if(EquipmentList.SlotToEquipmentMap.Contains(Slot.SlotTag))
	{
		return EquipmentList.SlotToEquipmentMap[Slot.SlotTag];
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
	for(FObsidianEquipmentSlotDefinition Slot : FindMatchingEquipmentSlotsByItemCategory(ItemCategoryTag))
	{
		if(EquipmentList.SlotToEquipmentMap.Contains(Slot.SlotTag) || (bIsTwoHanded && EquipmentList.SlotToEquipmentMap.Contains(Slot.SisterSlotTag)))
		{
			continue; // We already have an item equipped in this slot, we shouldn't try to equip it. || Initial slot is free but the other one is occupied so we don't want to automatically equip.
		}
		
		if(EquipItemToSpecificSlot(InstanceToEquip, Slot.SlotTag))
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
			
			UnequipItem(InstanceAtSecondSlot);
			
			InventoryComponent->AddItemInstance(InstanceAtSecondSlot);
			checkf(Result, TEXT("Adding item to Inventory failed in UObsidianEquipmentComponent::ReplaceItemAtSpecificSlot."));
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
			
			UnequipItem(InstanceAtSecondSlot);
			
			InventoryComponent->AddItemInstance(InstanceAtSecondSlot);
			checkf(Result, TEXT("Adding item to Inventory failed in UObsidianEquipmentComponent::ReplaceItemAtSpecificSlot."));
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

FObsidianEquipmentResult UObsidianEquipmentComponent::AutomaticallyEquipItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef)
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
	for(FObsidianEquipmentSlotDefinition Slot : FindMatchingEquipmentSlotsByItemCategory(ItemCategoryTag))
	{
		if(EquipmentList.SlotToEquipmentMap.Contains(Slot.SlotTag) || (bIsTwoHanded && EquipmentList.SlotToEquipmentMap.Contains(Slot.SisterSlotTag)))
		{
			continue; // We already have an item equipped in this slot, we shouldn't try to equip it. || Initial slot is free but the other one is occupied so we don't want to automatically equip.
		}
		
		if(const FObsidianEquipmentResult& InternalResult = EquipItemToSpecificSlot(ItemDef, Slot.SlotTag))
		{
			return InternalResult;
		}
	}

	return Result;
}

FObsidianEquipmentResult UObsidianEquipmentComponent::EquipItemToSpecificSlot(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FGameplayTag& SlotTag)
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

	const FGameplayTag ItemCategoryTag = Instance->GetItemCategoryTag();
	const FObsidianEquipmentSlotDefinition PressedSlot = FindEquipmentSlotByTag(SlotTag);
	
	const EObsidianEquipCheckResult Result = PressedSlot.CanEquipToSlot(ItemCategoryTag);
	//const EObsidianEquipCheckResult Result = CanPlaceItemAtEquipmentSlot(SlotTag, ItemCategoryTag);
	if(Result != EObsidianEquipCheckResult::CanEquip)
	{
		return Result;
	}
	
	if(Instance->DoesItemNeedTwoSlots())
	{
		UE_LOG(LogTemp, Warning, TEXT("Item needs a second slot."));
		
		UObsidianInventoryItemInstance* InstanceAtSecondSlot = GetEquippedInstanceAtSlot(PressedSlot.SisterSlotTag);
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

	const FGameplayTag ItemCategoryTag = DefaultObject->GetItemCategoryTag();
	const FObsidianEquipmentSlotDefinition PressedSlot = FindEquipmentSlotByTag(SlotTag);
	
	const EObsidianEquipCheckResult Result = PressedSlot.CanEquipToSlot(ItemCategoryTag);
	//const EObsidianEquipCheckResult Result = CanPlaceItemAtEquipmentSlot(SlotTag, ItemCategoryTag);
	if(Result != EObsidianEquipCheckResult::CanEquip)
	{
		return Result;
	}

	if(DefaultObject->DoesItemNeedsTwoSlots())
	{
		UE_LOG(LogTemp, Warning, TEXT("Item needs a second slot."));
		
		UObsidianInventoryItemInstance* InstanceAtSecondSlot = GetEquippedInstanceAtSlot(PressedSlot.SisterSlotTag);
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
	
	EquipResult = Slot.CanEquipToSlot(ItemCategory); // Can equip to slot in the first place.
	if(EquipResult != EObsidianEquipCheckResult::CanEquip)
	{
		return EquipResult;
	}
	
	if(Slot.SisterSlotTag.IsValid())
	{
		UObsidianInventoryItemInstance* InstanceAtOtherSlot = GetEquippedInstanceAtSlot(Slot.SisterSlotTag);
		if (InstanceAtOtherSlot == nullptr)
		{
			return EquipResult;
		}
		
		const FGameplayTag OtherInstanceCategory = InstanceAtOtherSlot->GetItemCategoryTag();
		if(const FGameplayTagContainer* AcceptedCategories = ObsidianGameplayTags::GetSisterSlotAcceptedCategoriesMap().Find(OtherInstanceCategory))
		{
			if(AcceptedCategories->HasTagExact(ItemCategory) == false)
			{
				return EObsidianEquipCheckResult::UnableToEquip_DoesNotFitWithOtherWeaponType;
			}
		}
	}
	return EquipResult;
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
		ObsidianGameplayTags::Item_Category_Bow, ObsidianGameplayTags::Item_Category_Staff_TwoHand, ObsidianGameplayTags::Item_Category_Mace_OneHand,
		ObsidianGameplayTags::Item_Category_Mace_TwoHand, ObsidianGameplayTags::Item_Category_Axe_OneHand, ObsidianGameplayTags::Item_Category_Axe_TwoHand,
		ObsidianGameplayTags::Item_Category_Sword_OneHand, ObsidianGameplayTags::Item_Category_Sword_TwoHand
		};
	
	const TArray<FGameplayTag> LeftHandAcceptedEquipment =
		{
		ObsidianGameplayTags::Item_Category_Flail, ObsidianGameplayTags::Item_Category_Dagger, ObsidianGameplayTags::Item_Category_Wand,
		ObsidianGameplayTags::Item_Category_Mace_OneHand, ObsidianGameplayTags::Item_Category_Axe_OneHand, ObsidianGameplayTags::Item_Category_Sword_OneHand,
		ObsidianGameplayTags::Item_Category_Quiver, ObsidianGameplayTags::Item_Category_Shield
		};
	
	EquipmentList.EquipmentSlots =
		{
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Equipment_Slot_Weapon_RightHand, ObsidianGameplayTags::Equipment_Slot_Weapon_LeftHand, FGameplayTagContainer::CreateFromArray(RightHandAcceptedEquipment))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Equipment_Slot_Weapon_LeftHand, ObsidianGameplayTags::Equipment_Slot_Weapon_RightHand, FGameplayTagContainer::CreateFromArray(LeftHandAcceptedEquipment))},
		
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



