// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/PlayerStash/ObsidianPlayerStashComponent.h"

#include <Net/UnrealNetwork.h>
#include <Engine/ActorChannel.h>

#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "InventoryItems/ObsidianItemsFunctionLibrary.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "InventoryItems/PlayerStash/ObsidianStashTab.h"
#include "InventoryItems/PlayerStash/ObsidianStashTabsConfig.h"
#include "InventoryItems/PlayerStash/Tabs/ObsidianStashTab_Slots.h"

DEFINE_LOG_CATEGORY(LogPlayerStash)

UObsidianPlayerStashComponent::UObsidianPlayerStashComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, StashItemList(this)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
	
}

void UObsidianPlayerStashComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, StashItemList);
	DOREPLIFETIME(ThisClass, CurrentStashTab);
}

TConstArrayView<TObjectPtr<UObsidianStashTab>> UObsidianPlayerStashComponent::GetAllStashTabs() const
{
	return StashTabs;
}

UObsidianInventoryComponent* UObsidianPlayerStashComponent::GetInventoryComponentFromOwner() const
{
	if(const AObsidianPlayerController* ObsidianPC = CastChecked<AObsidianPlayerController>(GetOwner(),
		ECastCheckedType::NullAllowed))
	{
		return ObsidianPC->GetInventoryComponent();
	}
	return nullptr;
}

bool UObsidianPlayerStashComponent::CanOwnerModifyPlayerStashState()
{
	if (CachedOwnerPlayerController == nullptr)
	{
		CachedOwnerPlayerController = Cast<AObsidianPlayerController>(GetOwner());
	}
	if (const UObsidianAbilitySystemComponent* OwnerASC = CachedOwnerPlayerController->GetObsidianAbilitySystemComponent())
	{
		return !OwnerASC->HasMatchingGameplayTag(ObsidianGameplayTags::PlayerStash_BlockActions);
	}
	return false;
}

TArray<UObsidianInventoryItemInstance*> UObsidianPlayerStashComponent::GetAllItems() const
{
	return StashItemList.GetAllItems();
}

TArray<UObsidianInventoryItemInstance*> UObsidianPlayerStashComponent::GetAllPersonalItems() const
{
	return StashItemList.GetAllPersonalItems();	
}

TArray<UObsidianInventoryItemInstance*> UObsidianPlayerStashComponent::GetAllSharedItems() const
{
	return StashItemList.GetAllSharedItems();
}

TArray<UObsidianInventoryItemInstance*> UObsidianPlayerStashComponent::GetAllItemsFromStashTab(const FGameplayTag& StashTabTag)
{
	return StashItemList.GetAllItemsFromStashTab(StashTabTag);
}

UObsidianInventoryItemInstance* UObsidianPlayerStashComponent::GetItemInstanceFromTabAtPosition(const FObsidianItemPosition& ItemPosition)
{
	if (UObsidianStashTab* StashTab = GetStashTabForTag(ItemPosition.GetOwningStashTabTag()))
	{
		return StashTab->GetInstanceAtPosition(ItemPosition);
	}
	return nullptr;
}

void UObsidianPlayerStashComponent::InitializeStashTabs()
{
	if (ensure(StashTabsConfig))
	{
		StashTabs.Empty(StashTabsConfig->StashTabCount());
		StashTabs = StashItemList.InitializeStashTabs(StashTabsConfig);
	}
}

TArray<FObsidianStashSlotDefinition> UObsidianPlayerStashComponent::FindMatchingSlotsForItemCategory(const FGameplayTag& ItemCategory, const FGameplayTag& ItemBaseType)
{
	if (const UObsidianStashTab_Slots* SlotsStashTab = Cast<UObsidianStashTab_Slots>(GetStashTabForTag(GetActiveStashTag())))
	{
		return StashItemList.FindMatchingSlotsForItemCategory(ItemCategory, ItemBaseType, SlotsStashTab);
	}

	return {};
}

TArray<FObsidianStashSlotDefinition> UObsidianPlayerStashComponent::FindPossibleSlotsForPlacingItem_WithInstance(const UObsidianInventoryItemInstance* ForInstance)
{
	TArray<FObsidianStashSlotDefinition> MatchingSlots;
	if (ForInstance == nullptr)
	{
		return MatchingSlots;
	}
	
	if (UObsidianStashTab_Slots* SlotsStashTab = Cast<UObsidianStashTab_Slots>(GetStashTabForTag(GetActiveStashTag())))
	{
		const FGameplayTag ItemCategoryTag = ForInstance->GetItemCategoryTag();
		const FGameplayTag ItemBaseTypeTag = ForInstance->GetItemBaseTypeTag();
		for (const FObsidianStashSlotDefinition& PossibleSlot : StashItemList.FindMatchingSlotsForItemCategory(ItemCategoryTag, ItemBaseTypeTag, SlotsStashTab))
		{
			if (SlotsStashTab->CanPlaceItemAtSpecificPosition(PossibleSlot.GetStashSlotTag(), ItemCategoryTag, ItemBaseTypeTag, FIntPoint::NoneValue))
			{
				MatchingSlots.Add(PossibleSlot);
			}
		}
	}

	return MatchingSlots;
}

TArray<FObsidianStashSlotDefinition> UObsidianPlayerStashComponent::FindPossibleSlotsForPlacingItem_WithItemDef(const TSubclassOf<UObsidianInventoryItemDefinition>& ForItemDef)
{
	TArray<FObsidianStashSlotDefinition> MatchingSlots;
	if (ForItemDef == nullptr)
	{
		return MatchingSlots;
	}

	const UObsidianInventoryItemDefinition* DefaultObject = ForItemDef.GetDefaultObject();
	if (DefaultObject == nullptr)
	{
		return MatchingSlots;
	}
	
	if (UObsidianStashTab_Slots* SlotsStashTab = Cast<UObsidianStashTab_Slots>(GetStashTabForTag(GetActiveStashTag())))
	{
		const FGameplayTag ItemCategoryTag = DefaultObject->GetItemCategoryTag();
		const FGameplayTag ItemBaseTypeTag = DefaultObject->GetItemBaseTypeTag();
		for (const FObsidianStashSlotDefinition& PossibleSlot : StashItemList.FindMatchingSlotsForItemCategory(ItemCategoryTag, ItemBaseTypeTag, SlotsStashTab))
		{
			if (SlotsStashTab->CanPlaceItemAtSpecificPosition(PossibleSlot.GetStashSlotTag(), ItemCategoryTag, ItemBaseTypeTag, FIntPoint::NoneValue))
			{
				MatchingSlots.Add(PossibleSlot);
			}
		}
	}

	return MatchingSlots;
}

UObsidianStashTab* UObsidianPlayerStashComponent::GetStashTabForTag(const FGameplayTag& StashTabTag)
{
	return StashItemList.GetStashTabForTag(StashTabTag);
}

int32 UObsidianPlayerStashComponent::FindAllStacksForGivenItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef)
{
	//TODO(intrxx) Implement
	return 0;
}

int32 UObsidianPlayerStashComponent::FindAllStacksForGivenItem(const UObsidianInventoryItemInstance* ItemInstance)
{
	//TODO(intrxx) Implement
	return 0;
}

FObsidianAddingStacksResult UObsidianPlayerStashComponent::TryAddingStacksToExistingItems(const TSubclassOf<UObsidianInventoryItemDefinition>& AddingFromItemDef, const int32 StacksToAdd, const FGameplayTag& InTabTag, TArray<UObsidianInventoryItemInstance*>& OutAddedToInstances)
{
	FObsidianAddingStacksResult Result = FObsidianAddingStacksResult();
	Result.AddedStacks = 0;
	Result.StacksLeft = StacksToAdd;
	
	if (!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in [%hs]"), __FUNCTION__);
		return Result; 
	}

	if (CanOwnerModifyPlayerStashState() == false)
	{
		return Result;
	}
	
	if (StacksToAdd <= 0)
	{
		return Result;
	}
	
	TArray<UObsidianInventoryItemInstance*> Items = StashItemList.GetAllItems();
	for (UObsidianInventoryItemInstance* Instance : Items)
	{
		if (!IsValid(Instance))
		{
			UE_LOG(LogPlayerStash, Error, TEXT("Instance is invalid in [%hs]"), __FUNCTION__);
			continue;
		}
		
		if (AddingFromItemDef == Instance->GetItemDef())
		{
			const int32 CurrentStackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
			if (CurrentStackCount == 0)
			{
				continue;
			}
			
			const int32 StacksLeft = Result.StacksLeft;
			const int32 MaxStackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Max);
			int32 AmountThatCanBeAddedToInstance = FMath::Clamp((MaxStackCount - CurrentStackCount), 0, StacksLeft);
			AmountThatCanBeAddedToInstance = FMath::Min(AmountThatCanBeAddedToInstance, StacksLeft);
			if (AmountThatCanBeAddedToInstance <= 0)
			{
				continue;
			}
			
			UE_LOG(LogTemp, Warning, TEXT("Added [%d] stacks to [%s]."), AmountThatCanBeAddedToInstance, *GetNameSafe(Instance));
			
			Instance->AddItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, AmountThatCanBeAddedToInstance);
			StashItemList.ChangedEntryStacks(Instance, CurrentStackCount, InTabTag);
			
			Result.AddedStacks += AmountThatCanBeAddedToInstance;
			Result.StacksLeft -= AmountThatCanBeAddedToInstance;
			OutAddedToInstances.AddUnique(Instance);
			
			if (Result.AddedStacks == StacksToAdd)
			{
				Result.AddingStacksResult = EObsidianAddingStacksResultType::ASR_WholeItemAsStacksAdded;
				return Result;
			}
		}
	}
	if (OutAddedToInstances.Num() > 0)
	{
		Result.AddingStacksResult = EObsidianAddingStacksResultType::ASR_SomeOfTheStacksAdded;
	}
	return Result;
}

FObsidianItemOperationResult UObsidianPlayerStashComponent::AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const FGameplayTag& StashTabTag, const FObsidianItemGeneratedData& ItemGeneratedData)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();
	Result.StacksLeft = ItemGeneratedData.AvailableStackCount;
	
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::AddItemDefinition."));
		return Result; 
	}
	
	if(CanOwnerModifyPlayerStashState() == false)
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
	
	TArray<UObsidianInventoryItemInstance*> OutAddedToInstances;
	if(DefaultObject->IsStackable())
	{
		const FObsidianAddingStacksResult AddingStacksResult = TryAddingStacksToExistingItems(ItemDef, Result.StacksLeft, StashTabTag, /** OUT */ OutAddedToInstances);

		if(AddingStacksResult.AddingStacksResult == EObsidianAddingStacksResultType::ASR_SomeOfTheStacksAdded)
		{
			Result.StacksLeft = AddingStacksResult.StacksLeft;
			Result.AffectedInstance = OutAddedToInstances.Last();
		}
		
		if(AddingStacksResult.AddingStacksResult == EObsidianAddingStacksResultType::ASR_WholeItemAsStacksAdded)
		{
			Result.bActionSuccessful = true;
			Result.StacksLeft = AddingStacksResult.StacksLeft;
			Result.AffectedInstance = OutAddedToInstances.Last();
			return Result;
		}
	}
	
	FObsidianItemPosition AvailablePosition;
	if(CanFitItemDefinition(AvailablePosition, StashTabTag, ItemDef) == false)
	{
		//TODO(intrxx) Inventory is full, add voice over?
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta,FString::Printf(TEXT("Inventory is full!")));
		if(!OutAddedToInstances.IsEmpty())
		{
			Result.bActionSuccessful = true;
			Result.AffectedInstance = OutAddedToInstances.Last();
			return Result;
		}
		return Result;
	}
	
	UObsidianInventoryItemInstance* Instance = StashItemList.AddEntry(ItemDef, ItemGeneratedData, Result.StacksLeft, AvailablePosition);
	Instance->AddItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, Result.StacksLeft);
	Instance->SetIdentified(UObsidianItemsFunctionLibrary::IsDefinitionIdentified(DefaultObject, ItemGeneratedData));
	
	if(Instance && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(Instance);
	}

	Result.bActionSuccessful = true;
	Result.AffectedInstance = Instance;
	return Result;
}

FObsidianItemOperationResult UObsidianPlayerStashComponent::AddItemDefinitionToSpecifiedSlot(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const FObsidianItemPosition& ItemPosition, const FObsidianItemGeneratedData& ItemGeneratedData, const int32 StackToAddOverride)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();
	Result.StacksLeft = ItemGeneratedData.AvailableStackCount;

	ensure((ItemPosition.GetItemGridPosition(false) != FIntPoint::NoneValue || ItemPosition.GetItemSlotTag(false) != FGameplayTag::EmptyTag) && ItemPosition.GetOwningStashTabTag() != FGameplayTag::EmptyTag);
	
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::AddItemDefinitionToSpecifiedSlot."));
		return Result; 
	}

	if(CanOwnerModifyPlayerStashState() == false)
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

	int32 StacksAvailableToAdd = Result.StacksLeft;
	if(DefaultObject->IsStackable() && StackToAddOverride != INDEX_NONE)
	{
		StacksAvailableToAdd = FMath::Clamp<int32>((FMath::Min<int32>(StacksAvailableToAdd, StackToAddOverride)), 1, StacksAvailableToAdd);
	}
	
	if(CanFitItemDefinitionToSpecifiedSlot(ItemPosition, ItemDef) == false)
	{
		//TODO(intrxx) Inventory is full, add voice over?
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("Inventory is full at specified slot!")));
		return Result;
	}
	
	Result.StacksLeft -= StacksAvailableToAdd;
	Result.bActionSuccessful = Result.StacksLeft == 0; // We don't need any item duplication logic (like in AddItemInstanceToSpecificSlot)
														// as we still have a valid item definition in hands if the whole item isn't added here.
	ensure(Result.StacksLeft >= 0);
	
	UObsidianInventoryItemInstance* Instance = StashItemList.AddEntry(ItemDef, ItemGeneratedData, StacksAvailableToAdd, ItemPosition);
	Instance->AddItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, StacksAvailableToAdd);
	Instance->SetIdentified(UObsidianItemsFunctionLibrary::IsDefinitionIdentified(DefaultObject, ItemGeneratedData));
	
	if(Instance && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(Instance);
	}

	Result.AffectedInstance = Instance;
	return Result;
}

FObsidianAddingStacksResult UObsidianPlayerStashComponent::TryAddingStacksToSpecificSlotWithItemDef(const TSubclassOf<UObsidianInventoryItemDefinition>& AddingFromItemDef, const int32 AddingFromItemDefCurrentStacks, const FObsidianItemPosition& AtPosition, const int32 StackToAddOverride)
{
	FObsidianAddingStacksResult Result = FObsidianAddingStacksResult();
	Result.StacksLeft = AddingFromItemDefCurrentStacks;
	
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::TryAddingStacksToSpecificSlotWithItemDef."));
		return Result; 
	}

	if(CanOwnerModifyPlayerStashState() == false)
	{
		return Result; 
	}
	
	UObsidianInventoryItemInstance* InstanceToAddTo = GetItemInstanceFromTabAtPosition(AtPosition);
	if(UObsidianItemsFunctionLibrary::IsTheSameItem_WithDef(InstanceToAddTo, AddingFromItemDef) == false)
	{
		return Result;
	}
	
	int32 AmountThatCanBeAddedToInstance = UObsidianItemsFunctionLibrary::GetAmountOfStacksAllowedToAddToItem_WithDef(GetOwner(), AddingFromItemDef, AddingFromItemDefCurrentStacks, InstanceToAddTo);
	if(AmountThatCanBeAddedToInstance <= 0)
	{
		return Result;
	}
	
	if(StackToAddOverride != -1)
	{
		AmountThatCanBeAddedToInstance = FMath::Clamp<int32>((FMath::Min<int32>(AmountThatCanBeAddedToInstance, StackToAddOverride)),
			1, AmountThatCanBeAddedToInstance);
	}

	const int32 OldStackCount = InstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	InstanceToAddTo->AddItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, AmountThatCanBeAddedToInstance);
	StashItemList.ChangedEntryStacks(InstanceToAddTo, OldStackCount, AtPosition.GetOwningStashTabTag());

	Result.StacksLeft -= AmountThatCanBeAddedToInstance;
	Result.AddedStacks = AmountThatCanBeAddedToInstance;
	Result.LastAddedToInstance = InstanceToAddTo;

	if(AmountThatCanBeAddedToInstance == AddingFromItemDefCurrentStacks)
	{
		Result.AddingStacksResult = EObsidianAddingStacksResultType::ASR_WholeItemAsStacksAdded;
	}
	else
	{
		Result.AddingStacksResult = EObsidianAddingStacksResultType::ASR_SomeOfTheStacksAdded;
	}
	
	return Result;
}

FObsidianItemOperationResult UObsidianPlayerStashComponent::AddItemInstance(UObsidianInventoryItemInstance* InstanceToAdd, const FGameplayTag& StashTabTag)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();
	
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::AddItemInstance."));
		return Result; 
	}
	
	if(InstanceToAdd == nullptr)
	{
		return Result;
	}

	Result.StacksLeft = InstanceToAdd->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	
	if(CanOwnerModifyPlayerStashState() == false)
	{
		return Result;
	}
	
	if(InstanceToAdd->IsStackable())
	{
		TArray<UObsidianInventoryItemInstance*> OutAddedToInstances;
		const FObsidianAddingStacksResult AddingStacksResult = TryAddingStacksToExistingItems(InstanceToAdd->GetItemDef(), Result.StacksLeft, StashTabTag, /** OUT */ OutAddedToInstances);

		if(AddingStacksResult.AddingStacksResult == EObsidianAddingStacksResultType::ASR_SomeOfTheStacksAdded)
		{
			Result.StacksLeft = AddingStacksResult.StacksLeft;
			Result.AffectedInstance = OutAddedToInstances.Last();
			InstanceToAdd->OverrideItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, Result.StacksLeft);
		}
		
		if(AddingStacksResult.AddingStacksResult == EObsidianAddingStacksResultType::ASR_WholeItemAsStacksAdded)
		{
			Result.bActionSuccessful = true;
			Result.StacksLeft = AddingStacksResult.StacksLeft;
			Result.AffectedInstance = OutAddedToInstances.Last();
			return Result;
		}
	}
	
	FObsidianItemPosition AvailablePosition;
	if(CheckAvailablePosition(AvailablePosition, InstanceToAdd->GetItemGridSpan(), InstanceToAdd->GetItemCategoryTag(), InstanceToAdd->GetItemBaseTypeTag(), StashTabTag) == false)
	{
		//TODO(intrxx) Inventory is full, add voice over?
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("Inventory is full!")));
		return Result;
	}
	
	StashItemList.AddEntry(InstanceToAdd, AvailablePosition);
	
	if(InstanceToAdd && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(InstanceToAdd);
	}
	
	Result.bActionSuccessful = true;
	Result.AffectedInstance = InstanceToAdd;
	return Result;
}

FObsidianItemOperationResult UObsidianPlayerStashComponent::AddItemInstanceToSpecificSlot(UObsidianInventoryItemInstance* InstanceToAdd, const FObsidianItemPosition& ItemPosition, const int32 StackToAddOverride)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();

	ensure((ItemPosition.GetItemGridPosition(false) != FIntPoint::NoneValue || ItemPosition.GetItemSlotTag(false) != FGameplayTag::EmptyTag) && ItemPosition.GetOwningStashTabTag() != FGameplayTag::EmptyTag);
	
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in [%hs]"), __FUNCTION__);
		return Result; 
	}
	
	if(InstanceToAdd == nullptr)
	{
		return Result;
	}

	Result.StacksLeft = InstanceToAdd->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);

	if(CanOwnerModifyPlayerStashState() == false)
	{
		return Result;
	}

	int32 StacksAvailableToAdd = Result.StacksLeft;
	if(InstanceToAdd->IsStackable() && StackToAddOverride != INDEX_NONE)
	{
		StacksAvailableToAdd = FMath::Clamp<int32>((FMath::Min<int32>(StacksAvailableToAdd, StackToAddOverride)), 1, StacksAvailableToAdd);
	}
	
	if(CheckSpecifiedPosition(ItemPosition, InstanceToAdd->GetItemCategoryTag(), InstanceToAdd->GetItemBaseTypeTag(), InstanceToAdd->GetItemGridSpan()) == false)
	{
		//TODO(intrxx) Inventory is full, add voice over?
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("Player Stash is full at specified slot!")));
		return Result;
	}

	Result.bActionSuccessful = true;
	Result.StacksLeft -= StacksAvailableToAdd;
	ensure(Result.StacksLeft >= 0);
	
	const int32 CurrentHeldItemStacks = InstanceToAdd->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	if(StacksAvailableToAdd == CurrentHeldItemStacks)
	{
		StashItemList.AddEntry(InstanceToAdd, ItemPosition);
	}
	else
	{
		InstanceToAdd->OverrideItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, CurrentHeldItemStacks - StacksAvailableToAdd);
		const bool bCachedIdentified = InstanceToAdd->IsItemIdentified();
		
		Result.bActionSuccessful = false;
		
		FObsidianItemGeneratedData CachedGeneratedData;
		UObsidianItemsFunctionLibrary::FillItemGeneratedData(CachedGeneratedData, InstanceToAdd);
		InstanceToAdd = StashItemList.AddEntry(InstanceToAdd->GetItemDef(), CachedGeneratedData,  StacksAvailableToAdd, ItemPosition);
		InstanceToAdd->AddItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, StacksAvailableToAdd);
		InstanceToAdd->SetIdentified(bCachedIdentified);
	}
	
	if(InstanceToAdd && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(InstanceToAdd);
	}
	
	Result.AffectedInstance = InstanceToAdd;
	return Result;
}

FObsidianAddingStacksResult UObsidianPlayerStashComponent::TryAddingStacksToSpecificSlotWithInstance(UObsidianInventoryItemInstance* AddingFromInstance, const FObsidianItemPosition& AtPosition, const int32 StackToAddOverride)
{
	FObsidianAddingStacksResult Result = FObsidianAddingStacksResult();
	
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::TryAddingStacksToSpecificSlotWithInstance."));
		return Result; 
	}

	if(AddingFromInstance == nullptr)
	{
		return Result;
	}
	
	const int32 AddingFromInstanceCurrentStacks = AddingFromInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	Result.StacksLeft = AddingFromInstanceCurrentStacks;
	
	if(CanOwnerModifyPlayerStashState() == false)
	{
		return Result;
	}
	
	UObsidianInventoryItemInstance* InstanceToAddTo = GetItemInstanceFromTabAtPosition(AtPosition);
	if(UObsidianItemsFunctionLibrary::IsTheSameItem(AddingFromInstance, InstanceToAddTo) == false)
	{
		return Result;
	}
	
	int32 AmountThatCanBeAddedToInstance = UObsidianItemsFunctionLibrary::GetAmountOfStacksAllowedToAddToItem(GetOwner(), AddingFromInstance, InstanceToAddTo);
	if(AmountThatCanBeAddedToInstance <= 0)
	{
		return Result;
	}

	if(StackToAddOverride != -1)
	{
		AmountThatCanBeAddedToInstance = FMath::Clamp<int32>((FMath::Min<int32>(AmountThatCanBeAddedToInstance, StackToAddOverride)),
			1, AmountThatCanBeAddedToInstance);
	}

	const int32 OldStackCount = InstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	InstanceToAddTo->AddItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, AmountThatCanBeAddedToInstance);
	StashItemList.ChangedEntryStacks(InstanceToAddTo, OldStackCount, AtPosition.GetOwningStashTabTag());

	// We should not call the InventoryGrid.ChangedEntryStacks function as the AddingFromInstance is not part of the inventory anyway, let other systems take care of it.
	AddingFromInstance->RemoveItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, AmountThatCanBeAddedToInstance);
	
	Result.AddedStacks = AmountThatCanBeAddedToInstance;
	Result.StacksLeft -= AmountThatCanBeAddedToInstance;
	Result.LastAddedToInstance = InstanceToAddTo;
	
	if(AmountThatCanBeAddedToInstance == AddingFromInstanceCurrentStacks)
	{
		Result.AddingStacksResult = EObsidianAddingStacksResultType::ASR_WholeItemAsStacksAdded;
	}
	else
	{
		Result.AddingStacksResult = EObsidianAddingStacksResultType::ASR_SomeOfTheStacksAdded;
	}
	
	return Result;
}

FObsidianItemOperationResult UObsidianPlayerStashComponent::TakeOutFromItemInstance(UObsidianInventoryItemInstance* TakingFromInstance, const int32 StacksToTake)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();
	
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in [%hs]"), __FUNCTION__);
		return Result; 
	}

	if(CanOwnerModifyPlayerStashState() == false)
	{
		return Result;
	}
	
	const int32 CurrentTakingFromInstanceStacks = TakingFromInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	if(!ensureMsgf(((StacksToTake == 0) || (CurrentTakingFromInstanceStacks != StacksToTake)), TEXT("This function shouldn't be called if you want to take the whole item out. Simply Pickup the item instead.")))
	{
		return Result;
	}
	
	// Since the only valid number of stacks to take is in range [1, x - 1] we can clamp it for extra safety.
	const int32 StackToTakeSafe = FMath::Clamp<int32>(StacksToTake, 1, CurrentTakingFromInstanceStacks - 1);
	TakingFromInstance->RemoveItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, StacksToTake);
	const FObsidianItemPosition ItemPosition = TakingFromInstance->GetItemCurrentPosition();
	StashItemList.ChangedEntryStacks(TakingFromInstance, CurrentTakingFromInstanceStacks, ItemPosition.GetOwningStashTabTag());

	Result.bActionSuccessful = true;
	Result.AffectedInstance = TakingFromInstance;
	Result.StacksLeft = StackToTakeSafe;
	return Result;
}

FObsidianItemOperationResult UObsidianPlayerStashComponent::RemoveItemInstance(UObsidianInventoryItemInstance* InstanceToRemove)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();
	
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in [%hs]"), __FUNCTION__);
		return Result; 
	}
	
	if(CanOwnerModifyPlayerStashState() == false)
	{
		return Result;
	}

	if(InstanceToRemove == nullptr)
	{
		UE_LOG(LogPlayerStash, Error, TEXT("Passed InstanceToRemove is invalid in [%hs]"), __FUNCTION__);
		return Result;
	}

	const FObsidianItemPosition ItemPosition = InstanceToRemove->GetItemCurrentPosition();
	check(ItemPosition.GetOwningStashTabTag() != FGameplayTag::EmptyTag);
	
	StashItemList.RemoveEntry(InstanceToRemove, ItemPosition.GetOwningStashTabTag());
	
	if(InstanceToRemove && IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(InstanceToRemove);
	}

	Result.bActionSuccessful = true;
	Result.AffectedInstance = InstanceToRemove;
	return Result;
}

void UObsidianPlayerStashComponent::UseItem(UObsidianInventoryItemInstance* UsingInstance, UObsidianInventoryItemInstance* UsingOntoInstance)
{
	if(UsingInstance == nullptr)
	{
		UE_LOG(LogPlayerStash, Error, TEXT("UsingInstance is invalid in [%hs]"), __FUNCTION__);
		return;
	}

	if(UsingInstance->IsItemUsable() == false)
	{
		UE_LOG(LogPlayerStash, Error, TEXT("Trying to use unusable Item [%s] in [%hs]"), *UsingInstance->GetItemDebugName(), __FUNCTION__);
		return;
	}
	
	AObsidianPlayerController* OwningPlayerController = Cast<AObsidianPlayerController>(GetOwner());
	if(OwningPlayerController == nullptr || OwningPlayerController->HasAuthority() == false)
	{
		return;
	}
	
	const int32 CurrentUsingInstanceStacks = UsingInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	if(CurrentUsingInstanceStacks <= 0)
	{
		UE_LOG(LogPlayerStash, Error, TEXT("Trying to use Item [%s] that has no more stacks in [%hs]"), *UsingInstance->GetItemDebugName(), __FUNCTION__);
		return;
	}

	bool bUsageSuccessful = false;
	const EObsidianUsableItemType ItemType = UsingInstance->GetUsableItemType();
	if(ItemType == EObsidianUsableItemType::UIT_Crafting)
	{
		if(UsingOntoInstance == nullptr)
		{
			UE_LOG(LogPlayerStash, Error, TEXT("UsingOntoInstance is invalid in [%hs]"), __FUNCTION__);
			return;
		}
		
		if(UsingInstance->UseItem(OwningPlayerController, UsingOntoInstance))
		{
			StashItemList.GeneralEntryChange(UsingOntoInstance, UsingOntoInstance->GetItemCurrentPosition().GetOwningStashTabTag());
			bUsageSuccessful = true;
		}
	}
	else if(ItemType == EObsidianUsableItemType::UIT_Activation)
	{
		bUsageSuccessful = UsingInstance->UseItem(OwningPlayerController, nullptr);
	}

	if(bUsageSuccessful)
	{
		UpdateUsingItemAfterUsage(UsingInstance, CurrentUsingInstanceStacks);
	}
	else
	{
		//TODO(intrxx) Usage failed, Play some VO?
	}
}

void UObsidianPlayerStashComponent::UpdateUsingItemAfterUsage(UObsidianInventoryItemInstance* UsingInstance,
	const int32 CurrentStacks)
{
	const FObsidianItemPosition CurrentUsingItemPosition = UsingInstance->GetItemCurrentPosition();
	if (CurrentUsingItemPosition.IsOnInventoryGrid())
	{
		if (UObsidianInventoryComponent* InventoryComponent = GetInventoryComponentFromOwner())
		{
			InventoryComponent->UpdateUsingItemAfterUsage(UsingInstance, CurrentStacks);
		}
	}
	else if (CurrentUsingItemPosition.IsOnStash())
	{
		const FGameplayTag ItemOwningStashTab = CurrentUsingItemPosition.GetOwningStashTabTag();
		if(CurrentStacks > 1)
		{
			UsingInstance->RemoveItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, 1);
			StashItemList.ChangedEntryStacks(UsingInstance, CurrentStacks, ItemOwningStashTab);
			return;
		}
	
		StashItemList.RemoveEntry(UsingInstance, ItemOwningStashTab);

		if(UsingInstance && IsUsingRegisteredSubObjectList())
		{
			RemoveReplicatedSubObject(UsingInstance);
		}
	}
}

void UObsidianPlayerStashComponent::ServerRegisterAndValidateCurrentStashTab_Implementation(const FGameplayTag& StashTab)
{
	if (StashTab == FGameplayTag::EmptyTag)
	{
		CurrentStashTab = FGameplayTag::EmptyTag;
	}
	else if (GetStashTabForTag(StashTab)) // Stash exists and is valid
	{
		CurrentStashTab = StashTab;
	}
	else //TODO(intrxx) Should I do more here?
	{
		ensureAlwaysMsgf(false, TEXT("There is no such StashTab found for provided Gameplay Tag for Current Player."));
	}
}

FGameplayTag UObsidianPlayerStashComponent::GetActiveStashTag() const
{
	return CurrentStashTab;
}

void UObsidianPlayerStashComponent::LoadStashedItem(const FObsidianSavedItem& StashedSavedItem)
{
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogPlayerStash, Warning, TEXT("No Authority in [%hs]"), __FUNCTION__);
		return; 
	}

	UObsidianInventoryItemInstance* LoadedInstance = StashItemList.LoadEntry(StashedSavedItem);
	
	if(LoadedInstance && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(LoadedInstance);
	}
}

bool UObsidianPlayerStashComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for(const FObsidianStashEntry& Entry : StashItemList.Entries)
	{
		UObsidianInventoryItemInstance* Instance = Entry.Instance;

		if(Instance && IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void UObsidianPlayerStashComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing UObsidianInventoryItemInstance
	if(IsUsingRegisteredSubObjectList())
	{
		for(const FObsidianStashEntry& Entry : StashItemList.Entries)
		{
			UObsidianInventoryItemInstance* Instance = Entry.Instance;

			if(IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

bool UObsidianPlayerStashComponent::CanFitItemDefinition(FObsidianItemPosition& OutAvailablePosition, const FGameplayTag& StashTabTag, const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef)
{
	if(const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef))
	{
		if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
		{
			return CheckAvailablePosition(OutAvailablePosition, AppearanceFrag->GetItemGridSpanFromDesc(), ItemDefault->GetItemCategoryTag(), ItemDefault->GetItemBaseTypeTag(), StashTabTag);
		}
	}
	return false;
}

bool UObsidianPlayerStashComponent::CanFitItemDefinitionToSpecifiedSlot(const FObsidianItemPosition& SpecifiedSlot, const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef)
{
	if(const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef))
	{
		if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
		{
			return CheckSpecifiedPosition(SpecifiedSlot, ItemDefault->GetItemCategoryTag(), ItemDefault->GetItemBaseTypeTag(), AppearanceFrag->GetItemGridSpanFromDesc());
		}
	}
	return false;
}

bool UObsidianPlayerStashComponent::CheckAvailablePosition(FObsidianItemPosition& OutAvailablePosition, const FIntPoint& ItemGridSpan, const FGameplayTag& ItemCategory, const FGameplayTag& ItemBaseTypeTag, const FGameplayTag& StashTabTag)
{
	if (UObsidianStashTab* StashTab = GetStashTabForTag(StashTabTag))
	{
		FObsidianItemPosition ItemPosition;
		if (StashTab->FindFirstAvailablePositionForItem(ItemPosition, ItemCategory, ItemBaseTypeTag, ItemGridSpan))
		{
			OutAvailablePosition = ItemPosition;
			return true;
		}
	}
	return false;
}

bool UObsidianPlayerStashComponent::CanReplaceItemAtPosition(const FObsidianItemPosition& AtItemPosition, const UObsidianInventoryItemInstance* ReplacingInstance)
{
	if (ReplacingInstance == nullptr)
	{
		return false;
	}

	if (CanOwnerModifyPlayerStashState() == false)
	{
		return false;
	}
	
	if (UObsidianStashTab* StashTab = GetStashTabForTag(AtItemPosition.GetOwningStashTabTag()))
	{
		return StashTab->CanReplaceItemAtSpecificPosition(AtItemPosition, ReplacingInstance);
	}
	return false;
}


bool UObsidianPlayerStashComponent::CanReplaceItemAtPosition(const FObsidianItemPosition& AtItemPosition, const TSubclassOf<UObsidianInventoryItemDefinition>& ReplacingDef)
{
	if (ReplacingDef == nullptr)
	{
		return false;
	}

	if (CanOwnerModifyPlayerStashState() == false)
	{
		return false;
	}
	
	if (UObsidianStashTab* StashTab = GetStashTabForTag(AtItemPosition.GetOwningStashTabTag()))
	{
		return StashTab->CanReplaceItemAtSpecificPosition(AtItemPosition, ReplacingDef);
	}
	return false;
}

bool UObsidianPlayerStashComponent::CheckSpecifiedPosition(const FObsidianItemPosition& SpecifiedPosition, const FGameplayTag& ItemCategory, const FGameplayTag& ItemBaseTypeTag, const FIntPoint& ItemGridSpan)
{
	if (UObsidianStashTab* StashTab = GetStashTabForTag(SpecifiedPosition.GetOwningStashTabTag()))
	{
		return StashTab->CanPlaceItemAtSpecificPosition(SpecifiedPosition, ItemCategory, ItemBaseTypeTag, ItemGridSpan);
	}
	return false;
}

bool UObsidianPlayerStashComponent::CanFitInstanceInStashTab(const FIntPoint& ItemGridSpan, const FGameplayTag& ItemCategory, const FGameplayTag& ItemBaseTypeTag, const FGameplayTag& StashTabTag)
{
	FObsidianItemPosition OutPosition;
	return CheckAvailablePosition(OutPosition, ItemGridSpan, ItemCategory, ItemBaseTypeTag, StashTabTag);
}




