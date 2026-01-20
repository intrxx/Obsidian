// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/Equipment/ObsidianEquipmentComponent.h"

#include <Engine/ActorChannel.h>
#include <Net/UnrealNetwork.h>

#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "Characters/Player/ObsidianPlayerState.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Characters/Heroes/ObsidianHero.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "Core/ObsidianGameplayStatics.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianItemsFunctionLibrary.h"
#include "InventoryItems/Equipment/ObsidianSpawnedEquipmentPiece.h"

DEFINE_LOG_CATEGORY(LogEquipment);


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

			const FObsidianEquipmentResult Result = EquipItemToSpecificSlot(DefaultEquipmentTemplate.DefaultItemDef, DefaultEquipmentTemplate.EquipmentSlotTag, DefaultEquipmentTemplate.StackCount);
			ensureMsgf(Result, TEXT("Were unable to equip default item in UObsidianEquipmentComponent::EquipDefaultItems()."));
		}
		bReceivedInitialEquipmentItems = true;
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

UObsidianAbilitySystemComponent* UObsidianEquipmentComponent::GetObsidianAbilitySystemComponentFromOwner() const
{
	if(const AObsidianPlayerController* OwningPlayerController = Cast<AObsidianPlayerController>(GetOwner()))
	{
		return OwningPlayerController->GetObsidianAbilitySystemComponent();
	}
	return nullptr;
}

AObsidianPlayerState* UObsidianEquipmentComponent::GetObsidianPlayerStateFromOwner() const
{
	if(const AObsidianPlayerController* OwningPlayerController = Cast<AObsidianPlayerController>(GetOwner()))
	{
		return OwningPlayerController->GetObsidianPlayerState();
	}
	return nullptr;
}

AObsidianPlayerController* UObsidianEquipmentComponent::GetOwnerPlayerController() const
{
	if(AObsidianPlayerController* OwningPlayerController = Cast<AObsidianPlayerController>(GetOwner()))
	{
		return OwningPlayerController;
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

TArray<FObsidianEquipmentSlotDefinition> UObsidianEquipmentComponent::FindMatchingSlotsForItemCategory(const FGameplayTag& ItemCategory)
{
	return EquipmentList.FindMatchingEquipmentSlotsForItemCategory(ItemCategory);
}

TArray<FObsidianEquipmentSlotDefinition> UObsidianEquipmentComponent::FindPossibleSlotsForEquipping_WithInstance(const UObsidianInventoryItemInstance* ForInstance)
{
	TArray<FObsidianEquipmentSlotDefinition> MatchingSlots;
	if (ForInstance == nullptr)
	{
		return MatchingSlots;
	}
	
	for (const FObsidianEquipmentSlotDefinition& PossibleSlot : EquipmentList.FindMatchingEquipmentSlotsForItemCategory(ForInstance->GetItemCategoryTag()))
	{
		if (CanEquipInstance(ForInstance, PossibleSlot.GetEquipmentSlotTag()) == EObsidianEquipCheckResult::CanEquip)
		{
			MatchingSlots.Add(PossibleSlot);
		}
	}

	return MatchingSlots;
}

TArray<FObsidianEquipmentSlotDefinition> UObsidianEquipmentComponent::FindPossibleSlotsForEquipping_WithItemDef(const TSubclassOf<UObsidianInventoryItemDefinition>& ForItemDef,
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
	
	for (const FObsidianEquipmentSlotDefinition& PossibleSlot : EquipmentList.FindMatchingEquipmentSlotsForItemCategory(DefaultObject->GetItemCategoryTag()))
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
		//TODO(intrxx) Send Client RPC with some voice over passing EquipResult?
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

FObsidianEquipmentResult UObsidianEquipmentComponent::ReplaceItemAtSpecificSlot(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef,
	const FGameplayTag& SlotTag, const FObsidianItemGeneratedData& ItemGeneratedData, const FGameplayTag& EquipSlotTagOverride)
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

	EObsidianEquipCheckResult EquipResult = CanReplaceTemplate(ItemDef, SlotTag, ItemGeneratedData);
	if(EquipResult != EObsidianEquipCheckResult::CanEquip)
	{
		//TODO(intrxx) Send Client RPC to add voiceover passing EquipResult
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
	UObsidianInventoryItemInstance* Instance = EquipmentList.AddEntry(ItemDef, ItemGeneratedData, EquipTag);
	checkf(ItemGeneratedData.AvailableStackCount == 1, TEXT("Equipment Items should have 1 stack only."));
	Instance->AddItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, ItemGeneratedData.AvailableStackCount);
	Instance->SetIdentified(UObsidianItemsFunctionLibrary::IsDefinitionIdentified(DefaultObject, ItemGeneratedData));
	
	if(Instance && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(Instance);
	}

	Result.bActionSuccessful = Instance != nullptr;
	Result.AffectedInstance = Instance;
	return Result;
}

FObsidianEquipmentResult UObsidianEquipmentComponent::ReplaceItemAtSpecificSlot(UObsidianInventoryItemInstance* InstanceToEquip,
	const FGameplayTag& SlotTag, const FGameplayTag& EquipSlotTagOverride)
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
		//TODO(intrxx) Send Client RPC with some voice over passing EquipResult?
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

FObsidianEquipmentResult UObsidianEquipmentComponent::AutomaticallyEquipItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef,
	const FObsidianItemGeneratedData& ItemGeneratedData)
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

	EObsidianEquipCheckResult EquipResult = CanEquipTemplate(ItemDef, SlotTag, ItemGeneratedData);
	if(EquipResult != EObsidianEquipCheckResult::CanEquip)
	{
		//TODO(intrxx) Send Client RPC to add voiceover passing EquipResult
#if !UE_BUILD_SHIPPING
		UE_LOG(LogEquipment, Warning, TEXT("Item cannot be equipped, reason: [%s]"), *ObsidianEquipmentDebugHelpers::GetEquipResultString(EquipResult));
#endif
		return Result;
	}
	
	UObsidianInventoryItemInstance* Instance = EquipmentList.AddEntry(ItemDef, ItemGeneratedData, SlotTag);
	checkf(ItemGeneratedData.AvailableStackCount == 1, TEXT("Equipment Items should have 1 stack only."));
	Instance->AddItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, ItemGeneratedData.AvailableStackCount);
	Instance->SetIdentified(UObsidianItemsFunctionLibrary::IsDefinitionIdentified(DefaultObject, ItemGeneratedData));
	
	if(Instance && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(Instance);
	}

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
	
	const FGameplayTag ItemCategory = Instance->GetItemCategoryTag();
	const FObsidianEquipmentSlotDefinition Slot = FindEquipmentSlotByTag(SlotTag);
	
	const EObsidianPlacingAtSlotResult PlacingResult = Slot.CanEquipAtSlot(ItemCategory);
	if(PlacingResult != EObsidianPlacingAtSlotResult::CanPlace)
	{
		return EObsidianEquipCheckResult::CannotEquipToSlot;
	}
	
	if(Instance->DoesItemNeedTwoSlots())
	{
		UE_LOG(LogTemp, Warning, TEXT("Item needs a second slot."));
		
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

	const FGameplayTag ItemCategory = DefaultObject->GetItemCategoryTag();
	const FObsidianEquipmentSlotDefinition Slot = FindEquipmentSlotByTag(SlotTag);
	
	const EObsidianPlacingAtSlotResult PlacingResult = Slot.CanEquipAtSlot(ItemCategory);
	if(PlacingResult != EObsidianPlacingAtSlotResult::CanPlace)
	{
		return EObsidianEquipCheckResult::CannotEquipToSlot;
	}

	if(DefaultObject->DoesItemNeedsTwoSlots())
	{
		UE_LOG(LogTemp, Warning, TEXT("Item needs a second slot."));
		
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
	
	UE_LOG(LogTemp, Display, TEXT("Checking Req | Owning Hero Class: [%d], Required Hero Class: [%d]"),
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

	UE_LOG(LogTemp, Display, TEXT("Checking Req | Owning Hero Level: [%d], Required Hero Level: [%d]"),
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
		UE_LOG(LogTemp, Display, TEXT("Checking Req | Attribute [%s], Owning Magnitude: [%f], Item Req Magnitude [%d]"),
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

void UObsidianEquipmentComponent::LoadEquippedItem(const FObsidianSavedItem& EquippedSavedItem)
{
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogEquipment, Warning, TEXT("No Authority in [%hs]"), __FUNCTION__);
		return; 
	}
	
	UObsidianInventoryItemInstance* LoadedInstance = EquipmentList.LoadEntry(EquippedSavedItem);

	if(LoadedInstance && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(LoadedInstance);
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
		ObsidianGameplayTags::Item_Category_Equipment_Weapon_Melee_OneHand_Flail, ObsidianGameplayTags::Item_Category_Equipment_Weapon_Melee_OneHand_Dagger,
		ObsidianGameplayTags::Item_Category_Equipment_Weapon_Ranged_OneHand_Wand, ObsidianGameplayTags::Item_Category_Equipment_Weapon_Ranged_OneHand_Bow,
		ObsidianGameplayTags::Item_Category_Equipment_Weapon_Ranged_TwoHand_Staff, ObsidianGameplayTags::Item_Category_Equipment_Weapon_Melee_OneHand_Mace,
		ObsidianGameplayTags::Item_Category_Equipment_Weapon_Melee_TwoHand_Mace, ObsidianGameplayTags::Item_Category_Equipment_Weapon_Melee_OneHand_Axe,
		ObsidianGameplayTags::Item_Category_Equipment_Weapon_Melee_TwoHand_Axe,ObsidianGameplayTags::Item_Category_Equipment_Weapon_Melee_OneHand_Sword,
		ObsidianGameplayTags::Item_Category_Equipment_Weapon_Melee_TwoHand_Sword
		};
	
	const TArray<FGameplayTag> LeftHandAcceptedEquipment =
		{
		ObsidianGameplayTags::Item_Category_Equipment_Weapon_Melee_OneHand_Flail, ObsidianGameplayTags::Item_Category_Equipment_Weapon_Melee_OneHand_Dagger,
		ObsidianGameplayTags::Item_Category_Equipment_Weapon_Ranged_OneHand_Wand, ObsidianGameplayTags::Item_Category_Equipment_Weapon_Melee_OneHand_Mace,
		ObsidianGameplayTags::Item_Category_Equipment_Weapon_Melee_OneHand_Axe, ObsidianGameplayTags::Item_Category_Equipment_Weapon_Melee_OneHand_Sword,
		ObsidianGameplayTags::Item_Category_Equipment_Offhand_Quiver, ObsidianGameplayTags::Item_Category_Equipment_Offhand_Shield
		};
	
	EquipmentList.EquipmentSlots =
		{
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item_Slot_Equipment_Weapon_RightHand,
				ObsidianGameplayTags::Item_Slot_Equipment_Weapon_LeftHand,
				FGameplayTagContainer::CreateFromArray(RightHandAcceptedEquipment))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item_Slot_Equipment_Weapon_LeftHand,
				ObsidianGameplayTags::Item_Slot_Equipment_Weapon_RightHand,
				FGameplayTagContainer::CreateFromArray(LeftHandAcceptedEquipment))},
		
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item_Slot_Equipment_Helmet,
				FGameplayTagContainer(ObsidianGameplayTags::Item_Category_Equipment_Armor_Helmet))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item_Slot_Equipment_BodyArmor,
				FGameplayTagContainer(ObsidianGameplayTags::Item_Category_Equipment_Armor_BodyArmor))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item_Slot_Equipment_Belt,
				FGameplayTagContainer(ObsidianGameplayTags::Item_Category_Equipment_Armor_Belt))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item_Slot_Equipment_Gloves,
				FGameplayTagContainer(ObsidianGameplayTags::Item_Category_Equipment_Armor_Gloves))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item_Slot_Equipment_Boots,
				FGameplayTagContainer(ObsidianGameplayTags::Item_Category_Equipment_Armor_Boots))},
		
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item_Slot_Equipment_Amulet,
				FGameplayTagContainer(ObsidianGameplayTags::Item_Category_Equipment_Jewellery_Amulet))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item_Slot_Equipment_Ring_RightHand,
				FGameplayTagContainer(ObsidianGameplayTags::Item_Category_Equipment_Jewellery_Ring))},
			{FObsidianEquipmentSlotDefinition(ObsidianGameplayTags::Item_Slot_Equipment_Ring_LeftHand,
				FGameplayTagContainer(ObsidianGameplayTags::Item_Category_Equipment_Jewellery_Ring))},
		};
}



