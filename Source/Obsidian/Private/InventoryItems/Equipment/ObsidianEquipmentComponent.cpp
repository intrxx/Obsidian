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

	inline EObsidianEquipCheckResult GetResultBasedOnAttribute(const FGameplayAttribute& Attribute)
	{
		if (const EObsidianEquipCheckResult* Result = GetAttributeToResultMap().Find(Attribute))
		{
			return *Result;
		}
		return EObsidianEquipCheckResult::None;
	}
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
	DOREPLIFETIME(ThisClass, bReceivedInitialEquipmentItems); //TODO(intrxx) Test replicating only once?
}

void UObsidianEquipmentComponent::InitSaveData(const bool bReceivedInitialItems)
{
	bReceivedInitialEquipmentItems = bReceivedInitialItems;
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

TArray<FObsidianEquipmentSlotDefinition> UObsidianEquipmentComponent::FindMatchingSlotsForItemCategory(
	const FGameplayTag& ItemCategory)
{
	return EquipmentList.FindMatchingEquipmentSlotsForItemCategory(ItemCategory);
}

TArray<FObsidianEquipmentSlotDefinition> UObsidianEquipmentComponent::FindPossibleSlotsForEquipping_WithInstance(
	const UObsidianInventoryItemInstance* ForInstance)
{
	TArray<FObsidianEquipmentSlotDefinition> MatchingSlots;
	if (ForInstance == nullptr)
	{
		return MatchingSlots;
	}
	
	for (const FObsidianEquipmentSlotDefinition& PossibleSlot : 
		EquipmentList.FindMatchingEquipmentSlotsForItemCategory(ForInstance->GetItemCategoryTag()))
	{
		if (CanEquipInstance(ForInstance, PossibleSlot.GetEquipmentSlotTag()) == EObsidianEquipCheckResult::CanEquip)
		{
			MatchingSlots.Add(PossibleSlot);
		}
	}

	return MatchingSlots;
}

TArray<FObsidianEquipmentSlotDefinition> UObsidianEquipmentComponent::FindPossibleSlotsForEquipping_WithItemDef(
	const TSubclassOf<UObsidianInventoryItemDefinition>& ForItemDef,
	const FObsidianItemGeneratedData& ItemGeneratedData)
{
	TArray<FObsidianEquipmentSlotDefinition> MatchingSlots;
	if (ForItemDef == nullptr)
	{
		return MatchingSlots;
	}

	const UObsidianInventoryItemDefinition* DefaultObject = ForItemDef.GetDefaultObject();
	if (DefaultObject == nullptr)
	{
		return MatchingSlots;
	}
	
	for (const FObsidianEquipmentSlotDefinition& PossibleSlot : 
		EquipmentList.FindMatchingEquipmentSlotsForItemCategory(DefaultObject->GetItemCategoryTag()))
	{
		if (CanEquipTemplate(ForItemDef, PossibleSlot.GetEquipmentSlotTag(), ItemGeneratedData) == EObsidianEquipCheckResult::CanEquip)
		{
			MatchingSlots.Add(PossibleSlot);
		}
	}

	return MatchingSlots;
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
	return CanOwnerModifyContainerState();
}

FObsidianEquipmentResult UObsidianEquipmentComponent::AutomaticallyEquipItem(UObsidianInventoryItemInstance* InstanceToEquip)
{
	FObsidianEquipmentResult Result = FObsidianEquipmentResult();
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
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
	const bool bIsTwoHanded = UObsidianItemsFunctionLibrary::IsTwoHanded_WithCategory(ItemCategoryTag);
	for(FObsidianEquipmentSlotDefinition Slot : FindMatchingSlotsForItemCategory(ItemCategoryTag))
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
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}

	if(InstanceToEquip == nullptr)
	{
		return Result;
	}

	const EObsidianEquipCheckResult EquipResult = CanEquipInstance(InstanceToEquip, SlotTag);
	if(EquipResult != EObsidianEquipCheckResult::CanEquip)
	{
		//TODO(intrxx) Send Client RPC with some voice over passing EquipResult?
#if !UE_BUILD_SHIPPING
		UE_LOG(ObLogEquipment, Verbose, TEXT("Item cannot be equipped, reason: [%s]"), 
			*ObsidianEquipmentDebugHelpers::GetEquipResultString(EquipResult));
#endif
		return Result;
	}
	
	PlaceWholeItemInstance(InstanceToEquip, SlotTag);
	
	Result.bActionSuccessful = true;
	Result.AffectedInstance = InstanceToEquip;
	return Result;
}

FObsidianEquipmentResult UObsidianEquipmentComponent::ReplaceItemAtSpecificSlot(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef,
	const FGameplayTag& SlotTag, const FObsidianItemGeneratedData& ItemGeneratedData, const FGameplayTag& EquipSlotTagOverride)
{
	FObsidianEquipmentResult Result = FObsidianEquipmentResult();
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
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

	EObsidianEquipCheckResult EquipResult = CanReplaceTemplate(ItemDef, SlotTag, ItemGeneratedData);
	if(EquipResult != EObsidianEquipCheckResult::CanEquip)
	{
		//TODO(intrxx) Send Client RPC to add voiceover passing EquipResult
#if !UE_BUILD_SHIPPING
		UE_LOG(ObLogEquipment, Verbose, TEXT("Item cannot be equipped, reason: [%s]"), 
			*ObsidianEquipmentDebugHelpers::GetEquipResultString(EquipResult));
#endif
		return Result;
	}

	if(DefaultObject->DoesItemNeedsTwoSlots() && MoveSisterSlotItemToInventory(SlotTag) == false)
	{
		return Result;
	}

	const FGameplayTag EquipTag = EquipSlotTagOverride == FGameplayTag::EmptyTag ? SlotTag : EquipSlotTagOverride;
	checkf(ItemGeneratedData.GetStackCount() == 1, TEXT("Equipment Items should have 1 stack only."));
	UObsidianInventoryItemInstance* Instance = PlaceItemDefinition(ItemDef, ItemGeneratedData, 
		ItemGeneratedData.GetStackCount(), EquipTag);

	Result.bActionSuccessful = Instance != nullptr;
	Result.AffectedInstance = Instance;
	return Result;
}

FObsidianEquipmentResult UObsidianEquipmentComponent::ReplaceItemAtSpecificSlot(UObsidianInventoryItemInstance* InstanceToEquip,
	const FGameplayTag& SlotTag, const FGameplayTag& EquipSlotTagOverride)
{
	FObsidianEquipmentResult Result = FObsidianEquipmentResult();
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}

	if(InstanceToEquip == nullptr)
	{
		return Result;
	}

	const EObsidianEquipCheckResult EquipResult = CanReplaceInstance(InstanceToEquip, SlotTag);
	if(EquipResult != EObsidianEquipCheckResult::CanEquip)
	{
		//TODO(intrxx) Send Client RPC with some voice over passing EquipResult?
#if !UE_BUILD_SHIPPING
		UE_LOG(ObLogEquipment, Verbose, TEXT("Item cannot be equipped, reason: [%s]"), 
			*ObsidianEquipmentDebugHelpers::GetEquipResultString(EquipResult));
#endif
		return Result;
	}

	if(InstanceToEquip->DoesItemNeedTwoSlots() && MoveSisterSlotItemToInventory(SlotTag) == false)
	{
		return Result;
	}

	const FGameplayTag EquipTag = EquipSlotTagOverride == FGameplayTag::EmptyTag ? SlotTag : EquipSlotTagOverride;
	PlaceWholeItemInstance(InstanceToEquip, EquipTag);

	Result.bActionSuccessful = true;
	Result.AffectedInstance = InstanceToEquip;
	return Result;
}

FObsidianEquipmentResult UObsidianEquipmentComponent::AutomaticallyEquipItem(
	const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FObsidianItemGeneratedData& ItemGeneratedData)
{
	FObsidianEquipmentResult Result = FObsidianEquipmentResult();
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
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
	const bool bIsTwoHanded = UObsidianItemsFunctionLibrary::IsTwoHanded_WithCategory(ItemCategoryTag);
	for(const FObsidianEquipmentSlotDefinition& Slot : FindMatchingSlotsForItemCategory(ItemCategoryTag))
	{
		const FGameplayTag SlotTag = Slot.GetEquipmentSlotTag();
		if(EquipmentList.SlotToEquipmentMap.Contains(SlotTag) || (bIsTwoHanded && EquipmentList.SlotToEquipmentMap.Contains(Slot.SisterSlotTag)))
		{
			continue; // We already have an item equipped in this slot, we shouldn't try to equip it. || Initial slot is free but the other one is occupied so we don't want to automatically equip.
		}
		
		if(const FObsidianEquipmentResult& InternalResult = EquipItemToSpecificSlot(ItemDef, SlotTag, ItemGeneratedData))
		{
			return InternalResult;
		}
	}

	return Result;
}

FObsidianEquipmentResult UObsidianEquipmentComponent::EquipItemToSpecificSlot(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef,
	const FGameplayTag& SlotTag, const FObsidianItemGeneratedData& ItemGeneratedData)
{
	FObsidianEquipmentResult Result = FObsidianEquipmentResult();
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
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

	EObsidianEquipCheckResult EquipResult = CanEquipTemplate(ItemDef, SlotTag, ItemGeneratedData);
	if(EquipResult != EObsidianEquipCheckResult::CanEquip)
	{
		//TODO(intrxx) Send Client RPC to add voiceover passing EquipResult
#if !UE_BUILD_SHIPPING
		UE_LOG(ObLogEquipment, Verbose, TEXT("Item cannot be equipped, reason: [%s]"), *ObsidianEquipmentDebugHelpers::GetEquipResultString(EquipResult));
#endif
		return Result;
	}
	
	checkf(ItemGeneratedData.GetStackCount() == 1, TEXT("Equipment Items should have 1 stack only."));
	UObsidianInventoryItemInstance* Instance = PlaceItemDefinition(ItemDef, ItemGeneratedData, ItemGeneratedData.GetStackCount(), SlotTag);

	Result.bActionSuccessful = Instance != nullptr;
	Result.AffectedInstance = Instance;
	return Result;
}

EObsidianEquipCheckResult UObsidianEquipmentComponent::CanEquipInstance(const UObsidianInventoryItemInstance* Instance, const FGameplayTag& SlotTag)
{
	const EObsidianEquipCheckResult PossibilityResult = IsItemEquippingPossible(Instance);
	if (PossibilityResult != EObsidianEquipCheckResult::CanEquip)
	{
		return PossibilityResult;
	}

	const FGameplayTag ItemCategoryTag = Instance->GetItemCategoryTag();
	const EObsidianEquipCheckResult Result = CanPlaceItemAtEquipmentSlot(SlotTag, ItemCategoryTag);
	if(Result != EObsidianEquipCheckResult::CanEquip)
	{
		return Result;
	}
	
	return EObsidianEquipCheckResult::CanEquip;
}

EObsidianEquipCheckResult UObsidianEquipmentComponent::CanEquipTemplate(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef,
	const FGameplayTag& SlotTag, const FObsidianItemGeneratedData& ItemGeneratedData)
{
	if(ItemDef == nullptr)
	{
		return EObsidianEquipCheckResult::None;
	}

	const UObsidianInventoryItemDefinition* DefaultObject = GetDefault<UObsidianInventoryItemDefinition>(ItemDef);
	const EObsidianEquipCheckResult PossibilityResult = IsItemEquippingPossible(DefaultObject, ItemGeneratedData);
	if (PossibilityResult != EObsidianEquipCheckResult::CanEquip)
	{
		return PossibilityResult;
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
	const EObsidianEquipCheckResult PossibilityResult = IsItemEquippingPossible(Instance);
	if (PossibilityResult != EObsidianEquipCheckResult::CanEquip)
	{
		return PossibilityResult;
	}

	return CanReplaceItemAtEquipmentSlot(SlotTag, Instance->GetItemCategoryTag(), Instance->DoesItemNeedTwoSlots());
}

EObsidianEquipCheckResult UObsidianEquipmentComponent::CanReplaceTemplate(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef,
	const FGameplayTag& SlotTag, const FObsidianItemGeneratedData& ItemGeneratedData)
{
	if(ItemDef == nullptr)
	{
		return EObsidianEquipCheckResult::None;
	}

	const UObsidianInventoryItemDefinition* DefaultObject = GetDefault<UObsidianInventoryItemDefinition>(ItemDef);
	const EObsidianEquipCheckResult PossibilityResult = IsItemEquippingPossible(DefaultObject, ItemGeneratedData);
	if (PossibilityResult != EObsidianEquipCheckResult::CanEquip)
	{
		return PossibilityResult;
	}

	return CanReplaceItemAtEquipmentSlot(SlotTag, DefaultObject->GetItemCategoryTag(), DefaultObject->DoesItemNeedsTwoSlots());
}

EObsidianEquipCheckResult UObsidianEquipmentComponent::CanReplaceItemAtEquipmentSlot(const FGameplayTag& SlotTag,
	const FGameplayTag& ItemCategory, const bool bItemNeedsTwoSlots)
{
	const FObsidianEquipmentSlotDefinition Slot = FindEquipmentSlotByTag(SlotTag);

	const EObsidianPlacingAtSlotResult PlacingResult = Slot.CanEquipAtSlot(ItemCategory);
	if(PlacingResult != EObsidianPlacingAtSlotResult::CanPlace)
	{
		return EObsidianEquipCheckResult::CannotEquipToSlot;
	}

	if(bItemNeedsTwoSlots)
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
	else if (CanEquipWithOtherWeaponType(Slot, ItemCategory) == false)
	{
		return EObsidianEquipCheckResult::UnableToEquip_DoesNotFitWithOtherWeaponType;
	}

	return EObsidianEquipCheckResult::CanEquip;
}

bool UObsidianEquipmentComponent::MoveSisterSlotItemToInventory(const FGameplayTag& SlotTag)
{
	const FObsidianEquipmentSlotDefinition PressedSlot = FindEquipmentSlotByTag(SlotTag);

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

EObsidianEquipCheckResult UObsidianEquipmentComponent::IsItemEquippingPossible(const UObsidianInventoryItemInstance* Instance)
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

	const EObsidianEquipCheckResult ItemRequirementsResult = CheckItemRequirements(Instance->GetEquippingRequirements());
	if (ItemRequirementsResult != EObsidianEquipCheckResult::CanEquip)
	{
		return ItemRequirementsResult;
	}

	return EObsidianEquipCheckResult::CanEquip;
}

EObsidianEquipCheckResult UObsidianEquipmentComponent::IsItemEquippingPossible(const UObsidianInventoryItemDefinition* Definition,
	const FObsidianItemGeneratedData& ItemGeneratedData)
{
	if (Definition == nullptr)
	{
		return EObsidianEquipCheckResult::None;
	}

	if(CanOwnerModifyEquipmentState() == false)
	{
		return EObsidianEquipCheckResult::EquipmentActionsBlocked;
	}
	
	if(Definition->IsEquippable() == false)
	{
		return EObsidianEquipCheckResult::ItemUnequippable;
	}
	
	if(UObsidianItemsFunctionLibrary::IsDefinitionIdentified(Definition, ItemGeneratedData) == false)
	{
		return EObsidianEquipCheckResult::ItemUnientified;
	}
	
	const EObsidianEquipCheckResult ItemRequirementsResult = CheckItemRequirements(ItemGeneratedData.ItemEquippingRequirements);
	if (ItemRequirementsResult != EObsidianEquipCheckResult::CanEquip)
	{
		return ItemRequirementsResult;
	}

	return EObsidianEquipCheckResult::CanEquip;
}

EObsidianEquipCheckResult UObsidianEquipmentComponent::CheckItemRequirements(const FObsidianItemRequirements& ItemRequirements) const
{
	const AObsidianPlayerController* ObsidianPC = GetOwnerPlayerController();
	if (ObsidianPC == nullptr)
	{
		return EObsidianEquipCheckResult::None;
	}
	
	UE_LOG(ObLogEquipment, VeryVerbose, TEXT("Checking Req | Owning Hero Class: [%d], Required Hero Class: [%d]"),
		ObsidianPC->GetHeroClass(), ItemRequirements.HeroClassRequirement);
	if (ItemRequirements.HeroClassRequirement != EObsidianHeroClass::None && ItemRequirements.HeroClassRequirement != ObsidianPC->GetHeroClass())
	{
		return EObsidianEquipCheckResult::WrongHeroClass;
	}
	
	const AObsidianPlayerState* ObsidianPS = ObsidianPC->GetObsidianPlayerState();
	if (ObsidianPS == nullptr)
	{
		return EObsidianEquipCheckResult::None;
	}

	UE_LOG(ObLogEquipment, VeryVerbose, TEXT("Checking Req | Owning Hero Level: [%d], Required Hero Level: [%d]"),
		ObsidianPS->GetHeroLevel(), ItemRequirements.RequiredLevel);
	if (ObsidianPS->GetHeroLevel() < ItemRequirements.RequiredLevel)
	{
		return EObsidianEquipCheckResult::HeroLevelTooLow;
	}
	
	const UObsidianAbilitySystemComponent* ObsidianASC = ObsidianPS->GetObsidianAbilitySystemComponent();
	if (ObsidianASC == nullptr)
	{
		return EObsidianEquipCheckResult::None;
	}
	
	for (const FObsidianAttributeRequirement& AttributeReq : ItemRequirements.AttributeRequirements)
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

FObsidianEquipmentResult UObsidianEquipmentComponent::UnequipItem(UObsidianInventoryItemInstance* InstanceToUnequip)
{
	FObsidianEquipmentResult Result = FObsidianEquipmentResult();

	const FObsidianItemOperationResult RemovingResult = RemoveItemFromContainer(InstanceToUnequip);
	Result.bActionSuccessful = RemovingResult.bActionSuccessful;
	Result.AffectedInstance = RemovingResult.AffectedInstance;
	return Result;
}

void UObsidianEquipmentComponent::LoadEquippedItem(const FObsidianSavedItem& EquippedSavedItem)
{
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return;
	}
	
	UObsidianInventoryItemInstance* LoadedInstance = EquipmentList.LoadEntry(EquippedSavedItem);

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

void UObsidianEquipmentComponent::AddItemInstanceToList(UObsidianInventoryItemInstance* Instance,
	const FObsidianItemPosition& ToPosition)
{
	EquipmentList.AddEntry(Instance, ToPosition.GetItemSlotTag());
}

UObsidianInventoryItemInstance* UObsidianEquipmentComponent::AddItemDefinitionToList(
	const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FObsidianItemGeneratedData& ItemGeneratedData,
	const int32 StackCount, const FObsidianItemPosition& ToPosition)
{
	return EquipmentList.AddEntry(ItemDef, ItemGeneratedData, ToPosition.GetItemSlotTag());
}

void UObsidianEquipmentComponent::RemoveItemInstanceFromList(UObsidianInventoryItemInstance* Instance)
{
	EquipmentList.RemoveEntry(Instance);
}

EObsidianEquipCheckResult UObsidianEquipmentComponent::CanPlaceItemAtEquipmentSlot(const FGameplayTag& SlotTag, const FGameplayTag& ItemCategory)
{
	const FObsidianEquipmentSlotDefinition Slot = FindEquipmentSlotByTag(SlotTag);
	if(Slot.IsValid() == false)
	{
		return EObsidianEquipCheckResult::None;
	}
	
	const EObsidianPlacingAtSlotResult PlacingResult = Slot.CanEquipAtSlot(ItemCategory);
	if(PlacingResult != EObsidianPlacingAtSlotResult::CanPlace)
	{
		return EObsidianEquipCheckResult::CannotEquipToSlot;
	}
	
	if (CanEquipWithOtherWeaponType(Slot, ItemCategory) == false)
	{
		return EObsidianEquipCheckResult::UnableToEquip_DoesNotFitWithOtherWeaponType;
	}
	
	return EObsidianEquipCheckResult::CanEquip;
}

bool UObsidianEquipmentComponent::CanEquipWithOtherWeaponType(const FObsidianEquipmentSlotDefinition& PrimarySlot, const FGameplayTag& PrimaryWeaponCategory)
{
	if(PrimarySlot.SisterSlotTag.IsValid() == false)
	{
		return true;
	}
	
	const UObsidianInventoryItemInstance* InstanceAtOtherSlot = GetEquippedInstanceAtSlot(PrimarySlot.SisterSlotTag);
	if (InstanceAtOtherSlot == nullptr)
	{
		return true;
	}
		
	const FGameplayTag OtherInstanceCategory = InstanceAtOtherSlot->GetItemCategoryTag();
	if(const FGameplayTagContainer* AcceptedCategories = ObsidianGameplayTags::GetSisterSlotAcceptedCategoriesMap().Find(OtherInstanceCategory))
	{
		if(AcceptedCategories->HasTagExact(PrimaryWeaponCategory) == false)
		{
			return false;
		}
		return true;
	}
	
	checkf(false, TEXT("Other Weapon type is not defined in SisterSlotAcceptedCategoriesMap."));
	return true;
}

void UObsidianEquipmentComponent::AddBannedEquipmentCategoryToSlot(const FGameplayTag& SlotTag, const FGameplayTag& InItemCategory)
{
	// FindEquipmentSlotByTag returns a copy, the actual Slot from the list needs to be modified.
	for(FObsidianEquipmentSlotDefinition& Slot : EquipmentList.EquipmentSlots)
	{
		if(Slot.GetEquipmentSlotTag() == SlotTag)
		{
			Slot.AddBannedEquipmentCategory(InItemCategory);
			return;
		}
	}
}

void UObsidianEquipmentComponent::AddBannedEquipmentCategoriesToSlot(const FGameplayTag& SlotTag, const FGameplayTagContainer& InItemCategories)
{
	// FindEquipmentSlotByTag returns a copy, the actual Slot from the list needs to be modified.
	for(FObsidianEquipmentSlotDefinition& Slot : EquipmentList.EquipmentSlots)
	{
		if(Slot.GetEquipmentSlotTag() == SlotTag)
		{
			Slot.AddBannedEquipmentCategories(InItemCategories);
			return;
		}
	}
}

void UObsidianEquipmentComponent::RemoveBannedEquipmentCategoryToSlot(const FGameplayTag& SlotTag, const FGameplayTag& ItemCategoryToRemove)
{
	// FindEquipmentSlotByTag returns a copy, the actual Slot from the list needs to be modified.
	for(FObsidianEquipmentSlotDefinition& Slot : EquipmentList.EquipmentSlots)
	{
		if(Slot.GetEquipmentSlotTag() == SlotTag)
		{
			Slot.RemoveBannedEquipmentCategory(ItemCategoryToRemove);
			return;
		}
	}
}

void UObsidianEquipmentComponent::RemoveBannedEquipmentCategoriesToSlot(const FGameplayTag& SlotTag, const FGameplayTagContainer& ItemCategoriesToRemove)
{
	// FindEquipmentSlotByTag returns a copy, the actual Slot from the list needs to be modified.
	for(FObsidianEquipmentSlotDefinition& Slot : EquipmentList.EquipmentSlots)
	{
		if(Slot.GetEquipmentSlotTag() == SlotTag)
		{
			Slot.RemoveBannedEquipmentCategories(ItemCategoriesToRemove);
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



