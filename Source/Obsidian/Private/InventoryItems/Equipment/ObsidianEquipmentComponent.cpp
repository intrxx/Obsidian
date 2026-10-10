// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/Equipment/ObsidianEquipmentComponent.h"

#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Characters/Heroes/ObsidianHero.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Characters/Player/ObsidianPlayerState.h"
#include "Core/ObsidianGameplayStatics.h"
#include "InventoryItems/Equipment/ObsidianSpawnedEquipmentPiece.h"
#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianItemsFunctionLibrary.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "Obsidian/ObsidianLogCategories.h"


namespace EquipmentHelpers
{
	// Construction of the map needs to happen in a function so that the map isn't contructed at load time but at function call time.
	static const TMap<FGameplayAttribute, EObsidianEquipCheckResult>& GetAttributeToResultMap()
	{
		static const TMap<FGameplayAttribute, EObsidianEquipCheckResult> Map = {
			{UObsidianHeroAttributeSet::GetDexterityAttribute(), EObsidianEquipCheckResult::NotEnoughDexterity},
			{UObsidianHeroAttributeSet::GetIntelligenceAttribute(), EObsidianEquipCheckResult::NotEnoughIntelligence},
			{UObsidianHeroAttributeSet::GetStrengthAttribute(), EObsidianEquipCheckResult::NotEnoughStrength},
			{UObsidianHeroAttributeSet::GetFaithAttribute(), EObsidianEquipCheckResult::NotEnoughFaith},
		};
		return Map;
	}

	inline EObsidianEquipCheckResult GetResultBasedOnAttribute(const FGameplayAttribute& InAttribute)
	{
		if (const EObsidianEquipCheckResult* Result = GetAttributeToResultMap().Find(InAttribute))
		{
			return *Result;
		}
		return EObsidianEquipCheckResult::None;
	}
}

UObsidianEquipmentComponent::UObsidianEquipmentComponent(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
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
	DOREPLIFETIME(ThisClass, bReceivedInitialEquipmentItems); //TODO(intrxx) Test replicating only once?
}

void UObsidianEquipmentComponent::InitSaveData(const bool bInReceivedInitialItems)
{
	bReceivedInitialEquipmentItems = bInReceivedInitialItems;
	if (bReceivedInitialEquipmentItems == false)
	{
		EquipDefaultItems();
	}
}

bool UObsidianEquipmentComponent::DidReceiveInitialEquipmentItems() const
{
	return bReceivedInitialEquipmentItems;
}

void UObsidianEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
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

			const FObsidianEquipmentResult Result = EquipItemToSpecificSlot(DefaultEquipmentTemplate.DefaultItemDef, 
				DefaultEquipmentTemplate.EquipmentSlotTag, DefaultEquipmentTemplate.StackCount);
			ensureMsgf(Result, TEXT("Were unable to equip default item in UObsidianEquipmentComponent::EquipDefaultItems()."));
		}
		bReceivedInitialEquipmentItems = true;
	}
}

TArray<FObsidianEquipmentSlotDefinition> UObsidianEquipmentComponent::Internal_GetEquipmentSlots() const
{
	return EquipmentList.EquipmentSlots;
}

UObsidianInventoryItemInstance* UObsidianEquipmentComponent::GetEquippedInstanceAtSlot(const FGameplayTag& InSlotTag) const
{
	return EquipmentList.GetEquipmentPieceByTag(InSlotTag);
}

UObsidianInventoryItemInstance* UObsidianEquipmentComponent::GetEquippedInstanceAtSlot(const FObsidianEquipmentSlotDefinition& InSlot) const
{
	return EquipmentList.GetEquipmentPieceByTag(InSlot.GetEquipmentSlotTag());
}

TArray<UObsidianInventoryItemInstance*> UObsidianEquipmentComponent::GetAllEquippedItems() const
{
	return EquipmentList.GetAllEquippedItems();
}

USkeletalMeshComponent* UObsidianEquipmentComponent::GetMainEquippedMeshFromSlot(const FGameplayTag& InSlotTag) const
{
	if(const UObsidianInventoryItemInstance* Instance = GetEquippedInstanceAtSlot(InSlotTag))
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

FObsidianEquipmentSlotDefinition UObsidianEquipmentComponent::FindEquipmentSlotByTag(const FGameplayTag& InSlotTag)
{
	return EquipmentList.FindEquipmentSlotByTag(InSlotTag);
}

TArray<FObsidianEquipmentSlotDefinition> UObsidianEquipmentComponent::FindMatchingSlotsForItemCategory(
	const FGameplayTag& InItemCategory)
{
	return EquipmentList.FindMatchingEquipmentSlotsForItemCategory(InItemCategory);
}

TArray<FObsidianEquipmentSlotDefinition> UObsidianEquipmentComponent::FindPossibleSlotsForEquipping_WithInstance(
	const UObsidianInventoryItemInstance* InForInstance)
{
	TArray<FObsidianEquipmentSlotDefinition> MatchingSlots;
	if (InForInstance == nullptr)
	{
		return MatchingSlots;
	}
	
	for (const FObsidianEquipmentSlotDefinition& PossibleSlot : 
		EquipmentList.FindMatchingEquipmentSlotsForItemCategory(InForInstance->GetItemCategoryTag()))
	{
		if (CanEquipInstance(InForInstance, PossibleSlot.GetEquipmentSlotTag()) == EObsidianEquipCheckResult::CanEquip)
		{
			MatchingSlots.Add(PossibleSlot);
		}
	}

	return MatchingSlots;
}

TArray<FObsidianEquipmentSlotDefinition> UObsidianEquipmentComponent::FindPossibleSlotsForEquipping_WithItemDef(
	const TSubclassOf<UObsidianInventoryItemDefinition>& InForItemDef,
	const FObsidianItemGeneratedData& InItemGeneratedData)
{
	TArray<FObsidianEquipmentSlotDefinition> MatchingSlots;
	if (InForItemDef == nullptr)
	{
		return MatchingSlots;
	}

	const UObsidianInventoryItemDefinition* DefaultObject = InForItemDef.GetDefaultObject();
	if (DefaultObject == nullptr)
	{
		return MatchingSlots;
	}
	
	for (const FObsidianEquipmentSlotDefinition& PossibleSlot : 
		EquipmentList.FindMatchingEquipmentSlotsForItemCategory(DefaultObject->GetItemCategoryTag()))
	{
		if (CanEquipTemplate(InForItemDef, PossibleSlot.GetEquipmentSlotTag(), InItemGeneratedData) == EObsidianEquipCheckResult::CanEquip)
		{
			MatchingSlots.Add(PossibleSlot);
		}
	}

	return MatchingSlots;
}


bool UObsidianEquipmentComponent::IsItemEquippedAtSlot(const FGameplayTag& InSlotTag)
{
	if(GetEquippedInstanceAtSlot(InSlotTag))
	{
		return true;
	}
	return false;
}

bool UObsidianEquipmentComponent::CanOwnerModifyEquipmentState()
{
	return CanOwnerModifyContainerState();
}

FObsidianEquipmentResult UObsidianEquipmentComponent::AutomaticallyEquipItem(UObsidianInventoryItemInstance* InInstanceToEquip)
{
	FObsidianEquipmentResult Result = FObsidianEquipmentResult();
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}

	if(InInstanceToEquip == nullptr)
	{
		return Result;
	}

	if(CanOwnerModifyEquipmentState() == false)
	{
		return Result;
	}

	const FGameplayTag& ItemCategoryTag = InInstanceToEquip->GetItemCategoryTag();
	const bool bIsTwoHanded = UObsidianItemsFunctionLibrary::IsTwoHanded_WithCategory(ItemCategoryTag);
	for(FObsidianEquipmentSlotDefinition Slot : FindMatchingSlotsForItemCategory(ItemCategoryTag))
	{
		const FGameplayTag SlotTag = Slot.GetEquipmentSlotTag();
		if(EquipmentList.SlotToEquipmentMap.Contains(SlotTag) || (bIsTwoHanded && EquipmentList.SlotToEquipmentMap.Contains(Slot.SisterSlotTag)))
		{
			continue; // We already have an item equipped in this slot, we shouldn't try to equip it. || Initial slot is free but the other one is occupied so we don't want to automatically equip.
		}
		
		if(EquipItemToSpecificSlot(InInstanceToEquip, SlotTag))
		{
			Result.bActionSuccessful = true;
			Result.AffectedInstance = InInstanceToEquip;
			return Result;
		}
	}

	return Result;
}

FObsidianEquipmentResult UObsidianEquipmentComponent::EquipItemToSpecificSlot(UObsidianInventoryItemInstance* InInstanceToEquip, const FGameplayTag& InSlotTag)
{
	FObsidianEquipmentResult Result = FObsidianEquipmentResult();
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}

	if(InInstanceToEquip == nullptr)
	{
		return Result;
	}

	const EObsidianEquipCheckResult EquipResult = CanEquipInstance(InInstanceToEquip, InSlotTag);
	if(EquipResult != EObsidianEquipCheckResult::CanEquip)
	{
		//TODO(intrxx) Send Client RPC with some voice over passing EquipResult?
#if !UE_BUILD_SHIPPING
		UE_LOG(ObLogEquipment, Verbose, TEXT("Item cannot be equipped, reason: [%s]"), 
			*ObsidianEquipmentDebugHelpers::GetEquipResultString(EquipResult));
#endif
		return Result;
	}
	
	PlaceWholeItemInstance(InInstanceToEquip, InSlotTag);
	
	Result.bActionSuccessful = true;
	Result.AffectedInstance = InInstanceToEquip;
	return Result;
}

FObsidianEquipmentResult UObsidianEquipmentComponent::ReplaceItemAtSpecificSlot(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef,
	const FGameplayTag& InSlotTag, const FObsidianItemGeneratedData& InItemGeneratedData, const FGameplayTag& InEquipSlotTagOverride)
{
	FObsidianEquipmentResult Result = FObsidianEquipmentResult();
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}

	if(InItemDef == nullptr)
	{
		return Result;
	}

	const UObsidianInventoryItemDefinition* DefaultObject = InItemDef.GetDefaultObject();
	if(DefaultObject == nullptr)
	{
		return Result;
	}

	EObsidianEquipCheckResult EquipResult = CanReplaceTemplate(InItemDef, InSlotTag, InItemGeneratedData);
	if(EquipResult != EObsidianEquipCheckResult::CanEquip)
	{
		//TODO(intrxx) Send Client RPC to add voiceover passing EquipResult
#if !UE_BUILD_SHIPPING
		UE_LOG(ObLogEquipment, Verbose, TEXT("Item cannot be equipped, reason: [%s]"), 
			*ObsidianEquipmentDebugHelpers::GetEquipResultString(EquipResult));
#endif
		return Result;
	}

	if(DefaultObject->DoesItemNeedsTwoSlots() && MoveSisterSlotItemToInventory(InSlotTag) == false)
	{
		return Result;
	}

	const FGameplayTag EquipTag = InEquipSlotTagOverride == FGameplayTag::EmptyTag ? InSlotTag : InEquipSlotTagOverride;
	checkf(InItemGeneratedData.GetStackCount() == 1, TEXT("Equipment Items should have 1 stack only."));
	UObsidianInventoryItemInstance* Instance = PlaceItemDefinition(InItemDef, InItemGeneratedData, 
		InItemGeneratedData.GetStackCount(), EquipTag);

	Result.bActionSuccessful = Instance != nullptr;
	Result.AffectedInstance = Instance;
	return Result;
}

FObsidianEquipmentResult UObsidianEquipmentComponent::ReplaceItemAtSpecificSlot(UObsidianInventoryItemInstance* InInstanceToEquip,
	const FGameplayTag& InSlotTag, const FGameplayTag& InEquipSlotTagOverride)
{
	FObsidianEquipmentResult Result = FObsidianEquipmentResult();
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}

	if(InInstanceToEquip == nullptr)
	{
		return Result;
	}

	const EObsidianEquipCheckResult EquipResult = CanReplaceInstance(InInstanceToEquip, InSlotTag);
	if(EquipResult != EObsidianEquipCheckResult::CanEquip)
	{
		//TODO(intrxx) Send Client RPC with some voice over passing EquipResult?
#if !UE_BUILD_SHIPPING
		UE_LOG(ObLogEquipment, Verbose, TEXT("Item cannot be equipped, reason: [%s]"), 
			*ObsidianEquipmentDebugHelpers::GetEquipResultString(EquipResult));
#endif
		return Result;
	}

	if(InInstanceToEquip->DoesItemNeedTwoSlots() && MoveSisterSlotItemToInventory(InSlotTag) == false)
	{
		return Result;
	}

	const FGameplayTag EquipTag = InEquipSlotTagOverride == FGameplayTag::EmptyTag ? InSlotTag : InEquipSlotTagOverride;
	PlaceWholeItemInstance(InInstanceToEquip, EquipTag);

	Result.bActionSuccessful = true;
	Result.AffectedInstance = InInstanceToEquip;
	return Result;
}

FObsidianEquipmentResult UObsidianEquipmentComponent::AutomaticallyEquipItem(
	const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef, const FObsidianItemGeneratedData& InItemGeneratedData)
{
	FObsidianEquipmentResult Result = FObsidianEquipmentResult();
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}

	if(InItemDef == nullptr)
	{
		return Result;
	}

	UObsidianInventoryItemDefinition* DefaultObject = InItemDef.GetDefaultObject();
	if(DefaultObject == nullptr)
	{
		return Result;
	}

	if(CanOwnerModifyEquipmentState() == false)
	{
		return Result;
	}
	
	const FGameplayTag& ItemCategoryTag = DefaultObject->GetItemCategoryTag();
	const bool bIsTwoHanded = UObsidianItemsFunctionLibrary::IsTwoHanded_WithCategory(ItemCategoryTag);
	for(const FObsidianEquipmentSlotDefinition& Slot : FindMatchingSlotsForItemCategory(ItemCategoryTag))
	{
		const FGameplayTag SlotTag = Slot.GetEquipmentSlotTag();
		if(EquipmentList.SlotToEquipmentMap.Contains(SlotTag) || (bIsTwoHanded && EquipmentList.SlotToEquipmentMap.Contains(Slot.SisterSlotTag)))
		{
			continue; // We already have an item equipped in this slot, we shouldn't try to equip it. || Initial slot is free but the other one is occupied so we don't want to automatically equip.
		}
		
		if(const FObsidianEquipmentResult& InternalResult = EquipItemToSpecificSlot(InItemDef, SlotTag, InItemGeneratedData))
		{
			return InternalResult;
		}
	}

	return Result;
}

FObsidianEquipmentResult UObsidianEquipmentComponent::EquipItemToSpecificSlot(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef,
	const FGameplayTag& InSlotTag, const FObsidianItemGeneratedData& InItemGeneratedData)
{
	FObsidianEquipmentResult Result = FObsidianEquipmentResult();
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}

	if(InItemDef == nullptr)
	{
		return Result;
	}

	const UObsidianInventoryItemDefinition* DefaultObject = InItemDef.GetDefaultObject();
	if(DefaultObject == nullptr)
	{
		return Result;
	}

	EObsidianEquipCheckResult EquipResult = CanEquipTemplate(InItemDef, InSlotTag, InItemGeneratedData);
	if(EquipResult != EObsidianEquipCheckResult::CanEquip)
	{
		//TODO(intrxx) Send Client RPC to add voiceover passing EquipResult
#if !UE_BUILD_SHIPPING
		UE_LOG(ObLogEquipment, Verbose, TEXT("Item cannot be equipped, reason: [%s]"), *ObsidianEquipmentDebugHelpers::GetEquipResultString(EquipResult));
#endif
		return Result;
	}
	
	checkf(InItemGeneratedData.GetStackCount() == 1, TEXT("Equipment Items should have 1 stack only."));
	UObsidianInventoryItemInstance* Instance = PlaceItemDefinition(InItemDef, InItemGeneratedData, InItemGeneratedData.GetStackCount(), InSlotTag);

	Result.bActionSuccessful = Instance != nullptr;
	Result.AffectedInstance = Instance;
	return Result;
}

EObsidianEquipCheckResult UObsidianEquipmentComponent::CanEquipInstance(const UObsidianInventoryItemInstance* InInstance, const FGameplayTag& InSlotTag)
{
	const EObsidianEquipCheckResult PossibilityResult = IsItemEquippingPossible(InInstance);
	if (PossibilityResult != EObsidianEquipCheckResult::CanEquip)
	{
		return PossibilityResult;
	}

	const FGameplayTag ItemCategoryTag = InInstance->GetItemCategoryTag();
	const EObsidianEquipCheckResult Result = CanPlaceItemAtEquipmentSlot(InSlotTag, ItemCategoryTag);
	if(Result != EObsidianEquipCheckResult::CanEquip)
	{
		return Result;
	}
	
	return EObsidianEquipCheckResult::CanEquip;
}

EObsidianEquipCheckResult UObsidianEquipmentComponent::CanEquipTemplate(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef,
	const FGameplayTag& InSlotTag, const FObsidianItemGeneratedData& InItemGeneratedData)
{
	if(InItemDef == nullptr)
	{
		return EObsidianEquipCheckResult::None;
	}

	const UObsidianInventoryItemDefinition* DefaultObject = GetDefault<UObsidianInventoryItemDefinition>(InItemDef);
	const EObsidianEquipCheckResult PossibilityResult = IsItemEquippingPossible(DefaultObject, InItemGeneratedData);
	if (PossibilityResult != EObsidianEquipCheckResult::CanEquip)
	{
		return PossibilityResult;
	}

	const FGameplayTag ItemCategoryTag = DefaultObject->GetItemCategoryTag();
	const EObsidianEquipCheckResult Result = CanPlaceItemAtEquipmentSlot(InSlotTag, ItemCategoryTag);
	if(Result != EObsidianEquipCheckResult::CanEquip)
	{
		return Result;
	}
	
	return EObsidianEquipCheckResult::CanEquip;
}

EObsidianEquipCheckResult UObsidianEquipmentComponent::CanReplaceInstance(const UObsidianInventoryItemInstance* InInstance, const FGameplayTag& InSlotTag)
{
	const EObsidianEquipCheckResult PossibilityResult = IsItemEquippingPossible(InInstance);
	if (PossibilityResult != EObsidianEquipCheckResult::CanEquip)
	{
		return PossibilityResult;
	}

	return CanReplaceItemAtEquipmentSlot(InSlotTag, InInstance->GetItemCategoryTag(), InInstance->DoesItemNeedTwoSlots());
}

EObsidianEquipCheckResult UObsidianEquipmentComponent::CanReplaceTemplate(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef,
	const FGameplayTag& InSlotTag, const FObsidianItemGeneratedData& InItemGeneratedData)
{
	if(InItemDef == nullptr)
	{
		return EObsidianEquipCheckResult::None;
	}

	const UObsidianInventoryItemDefinition* DefaultObject = GetDefault<UObsidianInventoryItemDefinition>(InItemDef);
	const EObsidianEquipCheckResult PossibilityResult = IsItemEquippingPossible(DefaultObject, InItemGeneratedData);
	if (PossibilityResult != EObsidianEquipCheckResult::CanEquip)
	{
		return PossibilityResult;
	}

	return CanReplaceItemAtEquipmentSlot(InSlotTag, DefaultObject->GetItemCategoryTag(), DefaultObject->DoesItemNeedsTwoSlots());
}

EObsidianEquipCheckResult UObsidianEquipmentComponent::CanReplaceItemAtEquipmentSlot(const FGameplayTag& InSlotTag,
	const FGameplayTag& InItemCategory, const bool bInItemNeedsTwoSlots)
{
	const FObsidianEquipmentSlotDefinition Slot = FindEquipmentSlotByTag(InSlotTag);

	const EObsidianPlacingAtSlotResult PlacingResult = Slot.CanEquipAtSlot(InItemCategory);
	if(PlacingResult != EObsidianPlacingAtSlotResult::CanPlace)
	{
		return EObsidianEquipCheckResult::CannotEquipToSlot;
	}

	if(bInItemNeedsTwoSlots)
	{
		UObsidianInventoryItemInstance* InstanceAtSecondSlot = GetEquippedInstanceAtSlot(Slot.SisterSlotTag);
		if(InstanceAtSecondSlot == nullptr) // If there is no item at second slot we can just return as the item at pressed slot will be added to the cursor.
		{
			return EObsidianEquipCheckResult::CanEquip;
		}

		UObsidianInventoryComponent* InventoryComponent = GetInventoryComponentFromOwner();
		if(InventoryComponent == nullptr)
		{
			return EObsidianEquipCheckResult::None;
		}

		if(InventoryComponent->CanFitItemInstance(InstanceAtSecondSlot) == false)
		{
			return EObsidianEquipCheckResult::UnableToEquip_NoSufficientInventorySpace;
		}
	}
	else if (CanEquipWithOtherWeaponType(Slot, InItemCategory) == false)
	{
		return EObsidianEquipCheckResult::UnableToEquip_DoesNotFitWithOtherWeaponType;
	}

	return EObsidianEquipCheckResult::CanEquip;
}

bool UObsidianEquipmentComponent::MoveSisterSlotItemToInventory(const FGameplayTag& InSlotTag)
{
	const FObsidianEquipmentSlotDefinition PressedSlot = FindEquipmentSlotByTag(InSlotTag);

	UObsidianInventoryItemInstance* InstanceAtSecondSlot = GetEquippedInstanceAtSlot(PressedSlot.SisterSlotTag);
	if(InstanceAtSecondSlot == nullptr)
	{
		return true; // Nothing to move.
	}

	UObsidianInventoryComponent* InventoryComponent = GetInventoryComponentFromOwner();
	if(InventoryComponent == nullptr)
	{
		return false;
	}

	if(InventoryComponent->CanOwnerModifyInventoryState() == false)
	{
		return false;
	}

	const FObsidianEquipmentResult LocalUnequippingResult = UnequipItem(InstanceAtSecondSlot);
	checkf(LocalUnequippingResult, TEXT("Unequipping item failed in [%hs]"), __FUNCTION__);

	const FObsidianItemOperationResult LocalAddingResult = InventoryComponent->AddItemInstance(InstanceAtSecondSlot);
	checkf(LocalAddingResult, TEXT("Adding item to Inventory failed in [%hs]"), __FUNCTION__);
	return true;
}

EObsidianEquipCheckResult UObsidianEquipmentComponent::IsItemEquippingPossible(const UObsidianInventoryItemInstance* InInstance)
{
	if(InInstance == nullptr)
	{
		return EObsidianEquipCheckResult::None;
	}
	
	if(CanOwnerModifyEquipmentState() == false)
	{
		return EObsidianEquipCheckResult::EquipmentActionsBlocked;
	}
	
	if(InInstance->IsItemEquippable() == false)
	{
		return EObsidianEquipCheckResult::ItemUnequippable;
	}
	
	if(InInstance->IsItemIdentified() == false)
	{
		return EObsidianEquipCheckResult::ItemUnientified;
	}

	const EObsidianEquipCheckResult ItemRequirementsResult = CheckItemRequirements(InInstance->GetEquippingRequirements());
	if (ItemRequirementsResult != EObsidianEquipCheckResult::CanEquip)
	{
		return ItemRequirementsResult;
	}

	return EObsidianEquipCheckResult::CanEquip;
}

EObsidianEquipCheckResult UObsidianEquipmentComponent::IsItemEquippingPossible(const UObsidianInventoryItemDefinition* InDefinition,
	const FObsidianItemGeneratedData& InItemGeneratedData)
{
	if (InDefinition == nullptr)
	{
		return EObsidianEquipCheckResult::None;
	}

	if(CanOwnerModifyEquipmentState() == false)
	{
		return EObsidianEquipCheckResult::EquipmentActionsBlocked;
	}
	
	if(InDefinition->IsEquippable() == false)
	{
		return EObsidianEquipCheckResult::ItemUnequippable;
	}
	
	if(UObsidianItemsFunctionLibrary::IsDefinitionIdentified(InDefinition, InItemGeneratedData) == false)
	{
		return EObsidianEquipCheckResult::ItemUnientified;
	}
	
	const EObsidianEquipCheckResult ItemRequirementsResult = CheckItemRequirements(InItemGeneratedData.ItemEquippingRequirements);
	if (ItemRequirementsResult != EObsidianEquipCheckResult::CanEquip)
	{
		return ItemRequirementsResult;
	}

	return EObsidianEquipCheckResult::CanEquip;
}

EObsidianEquipCheckResult UObsidianEquipmentComponent::CheckItemRequirements(const FObsidianItemRequirements& InItemRequirements) const
{
	const AObsidianPlayerController* ObsidianPC = GetOwnerPlayerController();
	if (ObsidianPC == nullptr)
	{
		return EObsidianEquipCheckResult::None;
	}
	
	UE_LOG(ObLogEquipment, VeryVerbose, TEXT("Checking Req | Owning Hero Class: [%d], Required Hero Class: [%d]"),
		ObsidianPC->GetHeroClass(), InItemRequirements.HeroClassRequirement);
	if (InItemRequirements.HeroClassRequirement != EObsidianHeroClass::None && InItemRequirements.HeroClassRequirement != ObsidianPC->GetHeroClass())
	{
		return EObsidianEquipCheckResult::WrongHeroClass;
	}
	
	const AObsidianPlayerState* ObsidianPS = ObsidianPC->GetObsidianPlayerState();
	if (ObsidianPS == nullptr)
	{
		return EObsidianEquipCheckResult::None;
	}

	UE_LOG(ObLogEquipment, VeryVerbose, TEXT("Checking Req | Owning Hero Level: [%d], Required Hero Level: [%d]"),
		ObsidianPS->GetHeroLevel(), InItemRequirements.RequiredLevel);
	if (ObsidianPS->GetHeroLevel() < InItemRequirements.RequiredLevel)
	{
		return EObsidianEquipCheckResult::HeroLevelTooLow;
	}
	
	const UObsidianAbilitySystemComponent* ObsidianASC = ObsidianPS->GetObsidianAbilitySystemComponent();
	if (ObsidianASC == nullptr)
	{
		return EObsidianEquipCheckResult::None;
	}
	
	for (const FObsidianAttributeRequirement& AttributeReq : InItemRequirements.AttributeRequirements)
	{
		UE_LOG(ObLogEquipment, VeryVerbose, TEXT("Checking Req | Attribute [%s], Owning Magnitude: [%f], Item Req Magnitude [%d]"),
			*AttributeReq.RequiredAttribute.GetName(), ObsidianASC->GetNumericAttribute(AttributeReq.RequiredAttribute),
			AttributeReq.RequiredAttributeMagnitude);
		if (ObsidianASC->GetNumericAttribute(AttributeReq.RequiredAttribute) < AttributeReq.RequiredAttributeMagnitude)
		{
			return EquipmentHelpers::GetResultBasedOnAttribute(AttributeReq.RequiredAttribute);
		}
	}

	return EObsidianEquipCheckResult::CanEquip;
}

void UObsidianEquipmentComponent::WeaponSwap()
{
	if(CanOwnerModifyEquipmentState() == false)
	{
		return;
	}

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

FObsidianEquipmentResult UObsidianEquipmentComponent::UnequipItem(UObsidianInventoryItemInstance* InInstanceToUnequip)
{
	FObsidianEquipmentResult Result = FObsidianEquipmentResult();

	const FObsidianItemOperationResult RemovingResult = RemoveItemFromContainer(InInstanceToUnequip);
	Result.bActionSuccessful = RemovingResult.bActionSuccessful;
	Result.AffectedInstance = RemovingResult.AffectedInstance;
	return Result;
}

void UObsidianEquipmentComponent::LoadEquippedItem(const FObsidianSavedItem& InEquippedSavedItem)
{
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return;
	}
	
	UObsidianInventoryItemInstance* LoadedInstance = EquipmentList.LoadEntry(InEquippedSavedItem);

	RegisterItemInstanceForReplication(LoadedInstance);
}

TArray<UObsidianInventoryItemInstance*> UObsidianEquipmentComponent::GetContainedItems() const
{
	return EquipmentList.GetAllEquippedItems();
}

FGameplayTag UObsidianEquipmentComponent::GetBlockActionsTag() const
{
	return ObsidianGameplayTags::Equipment::BlockActions;
}

void UObsidianEquipmentComponent::AddItemInstanceToList(UObsidianInventoryItemInstance* InInstance,
	const FObsidianItemPosition& InToPosition)
{
	EquipmentList.AddEntry(InInstance, InToPosition.GetItemSlotTag());
}

UObsidianInventoryItemInstance* UObsidianEquipmentComponent::AddItemDefinitionToList(
	const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef, const FObsidianItemGeneratedData& InItemGeneratedData,
	const int32 InStackCount, const FObsidianItemPosition& InToPosition)
{
	return EquipmentList.AddEntry(InItemDef, InItemGeneratedData, InToPosition.GetItemSlotTag());
}

void UObsidianEquipmentComponent::RemoveItemInstanceFromList(UObsidianInventoryItemInstance* InInstance)
{
	EquipmentList.RemoveEntry(InInstance);
}

EObsidianEquipCheckResult UObsidianEquipmentComponent::CanPlaceItemAtEquipmentSlot(const FGameplayTag& InSlotTag, const FGameplayTag& InItemCategory)
{
	const FObsidianEquipmentSlotDefinition Slot = FindEquipmentSlotByTag(InSlotTag);
	if(Slot.IsValid() == false)
	{
		return EObsidianEquipCheckResult::None;
	}
	
	const EObsidianPlacingAtSlotResult PlacingResult = Slot.CanEquipAtSlot(InItemCategory);
	if(PlacingResult != EObsidianPlacingAtSlotResult::CanPlace)
	{
		return EObsidianEquipCheckResult::CannotEquipToSlot;
	}
	
	if (CanEquipWithOtherWeaponType(Slot, InItemCategory) == false)
	{
		return EObsidianEquipCheckResult::UnableToEquip_DoesNotFitWithOtherWeaponType;
	}
	
	return EObsidianEquipCheckResult::CanEquip;
}

bool UObsidianEquipmentComponent::CanEquipWithOtherWeaponType(const FObsidianEquipmentSlotDefinition& InPrimarySlot, const FGameplayTag& InPrimaryWeaponCategory)
{
	if(InPrimarySlot.SisterSlotTag.IsValid() == false)
	{
		return true;
	}
	
	const UObsidianInventoryItemInstance* InstanceAtOtherSlot = GetEquippedInstanceAtSlot(InPrimarySlot.SisterSlotTag);
	if (InstanceAtOtherSlot == nullptr)
	{
		return true;
	}
		
	const FGameplayTag OtherInstanceCategory = InstanceAtOtherSlot->GetItemCategoryTag();
	if(const FGameplayTagContainer* AcceptedCategories = ObsidianGameplayTags::GetSisterSlotAcceptedCategoriesMap().Find(OtherInstanceCategory))
	{
		if(AcceptedCategories->HasTagExact(InPrimaryWeaponCategory) == false)
		{
			return false;
		}
		return true;
	}
	
	checkf(false, TEXT("Other Weapon type is not defined in SisterSlotAcceptedCategoriesMap."));
	return true;
}

void UObsidianEquipmentComponent::AddBannedEquipmentCategoryToSlot(const FGameplayTag& InSlotTag, const FGameplayTag& InItemCategory)
{
	// FindEquipmentSlotByTag returns a copy, the actual Slot from the list needs to be modified.
	for(FObsidianEquipmentSlotDefinition& Slot : EquipmentList.EquipmentSlots)
	{
		if(Slot.GetEquipmentSlotTag() == InSlotTag)
		{
			Slot.AddBannedEquipmentCategory(InItemCategory);
			return;
		}
	}
}

void UObsidianEquipmentComponent::AddBannedEquipmentCategoriesToSlot(const FGameplayTag& InSlotTag, const FGameplayTagContainer& InItemCategories)
{
	// FindEquipmentSlotByTag returns a copy, the actual Slot from the list needs to be modified.
	for(FObsidianEquipmentSlotDefinition& Slot : EquipmentList.EquipmentSlots)
	{
		if(Slot.GetEquipmentSlotTag() == InSlotTag)
		{
			Slot.AddBannedEquipmentCategories(InItemCategories);
			return;
		}
	}
}

void UObsidianEquipmentComponent::RemoveBannedEquipmentCategoryToSlot(const FGameplayTag& InSlotTag, const FGameplayTag& InItemCategoryToRemove)
{
	// FindEquipmentSlotByTag returns a copy, the actual Slot from the list needs to be modified.
	for(FObsidianEquipmentSlotDefinition& Slot : EquipmentList.EquipmentSlots)
	{
		if(Slot.GetEquipmentSlotTag() == InSlotTag)
		{
			Slot.RemoveBannedEquipmentCategory(InItemCategoryToRemove);
			return;
		}
	}
}

void UObsidianEquipmentComponent::RemoveBannedEquipmentCategoriesToSlot(const FGameplayTag& InSlotTag, const FGameplayTagContainer& InItemCategoriesToRemove)
{
	// FindEquipmentSlotByTag returns a copy, the actual Slot from the list needs to be modified.
	for(FObsidianEquipmentSlotDefinition& Slot : EquipmentList.EquipmentSlots)
	{
		if(Slot.GetEquipmentSlotTag() == InSlotTag)
		{
			Slot.RemoveBannedEquipmentCategories(InItemCategoriesToRemove);
			return;
		}
	}
}

void UObsidianEquipmentComponent::CreateDefaultEquipmentSlots()
{
	const TArray<FGameplayTag> RightHandAcceptedEquipment =
		{
		ObsidianGameplayTags::Item::Category::Equipment::Weapon::Melee::OneHand::Flail, ObsidianGameplayTags::Item::Category::Equipment::Weapon::Melee::OneHand::Dagger,
		ObsidianGameplayTags::Item::Category::Equipment::Weapon::Ranged::OneHand::Wand, ObsidianGameplayTags::Item::Category::Equipment::Weapon::Ranged::OneHand::Bow,
		ObsidianGameplayTags::Item::Category::Equipment::Weapon::Ranged::TwoHand::Staff, ObsidianGameplayTags::Item::Category::Equipment::Weapon::Melee::OneHand::Mace,
		ObsidianGameplayTags::Item::Category::Equipment::Weapon::Melee::TwoHand::Mace, ObsidianGameplayTags::Item::Category::Equipment::Weapon::Melee::OneHand::Axe,
		ObsidianGameplayTags::Item::Category::Equipment::Weapon::Melee::TwoHand::Axe,ObsidianGameplayTags::Item::Category::Equipment::Weapon::Melee::OneHand::Sword,
		ObsidianGameplayTags::Item::Category::Equipment::Weapon::Melee::TwoHand::Sword
		};
	
	const TArray<FGameplayTag> LeftHandAcceptedEquipment =
		{
		ObsidianGameplayTags::Item::Category::Equipment::Weapon::Melee::OneHand::Flail, ObsidianGameplayTags::Item::Category::Equipment::Weapon::Melee::OneHand::Dagger,
		ObsidianGameplayTags::Item::Category::Equipment::Weapon::Ranged::OneHand::Wand, ObsidianGameplayTags::Item::Category::Equipment::Weapon::Melee::OneHand::Mace,
		ObsidianGameplayTags::Item::Category::Equipment::Weapon::Melee::OneHand::Axe, ObsidianGameplayTags::Item::Category::Equipment::Weapon::Melee::OneHand::Sword,
		ObsidianGameplayTags::Item::Category::Equipment::Offhand::Quiver, ObsidianGameplayTags::Item::Category::Equipment::Offhand::Shield
		};
	
	EquipmentList.EquipmentSlots =
		{
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item::Slot::Equipment::Weapon::RightHand,
				ObsidianGameplayTags::Item::Slot::Equipment::Weapon::LeftHand,
				FGameplayTagContainer::CreateFromArray(RightHandAcceptedEquipment))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item::Slot::Equipment::Weapon::LeftHand,
				ObsidianGameplayTags::Item::Slot::Equipment::Weapon::RightHand,
				FGameplayTagContainer::CreateFromArray(LeftHandAcceptedEquipment))},
		
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item::Slot::Equipment::Helmet,
				FGameplayTagContainer(ObsidianGameplayTags::Item::Category::Equipment::Armor::Helmet))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item::Slot::Equipment::BodyArmor,
				FGameplayTagContainer(ObsidianGameplayTags::Item::Category::Equipment::Armor::BodyArmor))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item::Slot::Equipment::Belt,
				FGameplayTagContainer(ObsidianGameplayTags::Item::Category::Equipment::Armor::Belt))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item::Slot::Equipment::Gloves,
				FGameplayTagContainer(ObsidianGameplayTags::Item::Category::Equipment::Armor::Gloves))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item::Slot::Equipment::Boots,
				FGameplayTagContainer(ObsidianGameplayTags::Item::Category::Equipment::Armor::Boots))},
		
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item::Slot::Equipment::Amulet,
				FGameplayTagContainer(ObsidianGameplayTags::Item::Category::Equipment::Jewellery::Amulet))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item::Slot::Equipment::Ring::RightHand,
				FGameplayTagContainer(ObsidianGameplayTags::Item::Category::Equipment::Jewellery::Ring))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item::Slot::Equipment::Ring::LeftHand,
				FGameplayTagContainer(ObsidianGameplayTags::Item::Category::Equipment::Jewellery::Ring))},
		};
}



