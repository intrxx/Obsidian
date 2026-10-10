// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"

// ~ Core
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

// ~ Project
#include "Obsidian/ObsidianLogCategories.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Characters/Heroes/ObsidianHero.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianItemsFunctionLibrary.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Stacks.h"
#include "InventoryItems/PlayerStash/ObsidianPlayerStashComponent.h"
#include "Obsidian/ObsidianGameplayTags.h"

UObsidianInventoryComponent::UObsidianInventoryComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryGrid(this)
	, InventoryGridSize(InventoryGridWidth * InventoryGridHeight)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
	
	InitInventoryState();
}

void UObsidianInventoryComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryGrid);
	DOREPLIFETIME(ThisClass, bReceivedInitialInventoryItems); //TODO(intrxx) Test replicating only once?
}

void UObsidianInventoryComponent::InitSaveData(const bool bReceivedInitialItems)
{
	bReceivedInitialInventoryItems = bReceivedInitialItems;
	if (bReceivedInitialInventoryItems == false)
	{
		AddDefaultItems();
	}
}

bool UObsidianInventoryComponent::DidReceiveInitialInventoryItems() const
{
	return bReceivedInitialInventoryItems;
}

void UObsidianInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UObsidianInventoryComponent::AddDefaultItems()
{
	const AActor* OwningActor = GetOwner();
	if(OwningActor && OwningActor->HasAuthority())
	{
		for(const FObsidianDefaultItemTemplate& DefaultItemTemplate : DefaultInventoryItems)
		{
			if(DefaultItemTemplate.bOverrideInventoryPosition)
			{
				if(!AddItemDefinitionToSpecifiedSlot(DefaultItemTemplate.DefaultItemDef, 
					DefaultItemTemplate.InventoryPositionOverride, DefaultItemTemplate.StackCount))
				{
					AddItemDefinition(DefaultItemTemplate.DefaultItemDef, DefaultItemTemplate.StackCount);
				}
				continue;
			}
			AddItemDefinition(DefaultItemTemplate.DefaultItemDef, DefaultItemTemplate.StackCount);
		}
		bReceivedInitialInventoryItems = true;
	}
}

int32 UObsidianInventoryComponent::GetInventoryGridWidth() const
{
	return InventoryGridWidth;
}

int32 UObsidianInventoryComponent::GetInventoryGridHeight() const
{
	return InventoryGridHeight;
}

int32 UObsidianInventoryComponent::GetTotalItemCountByDefinition(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef) const
{
	int32 FinalCount = 0;
	for(const FObsidianInventoryEntry& Entry : InventoryGrid.Entries)
	{
		UObsidianInventoryItemInstance* Instance = Entry.Instance;
		if(IsValid(Instance))
		{
			if(Instance->GetItemDef() == ItemDef)
			{
				++FinalCount;
			}
		}
	}
	return FinalCount;
}

TMap<FIntPoint, UObsidianInventoryItemInstance*> UObsidianInventoryComponent::Internal_GetLocationToInstanceMap()
{
	return InventoryGrid.GridLocationToItemMap;
}

UObsidianInventoryItemInstance* UObsidianInventoryComponent::GetItemInstanceAtLocation(const FIntPoint& Location) const
{
	return InventoryGrid.GridLocationToItemMap.FindRef(Location);
}

TArray<UObsidianInventoryItemInstance*> UObsidianInventoryComponent::GetAllItems() const
{
	return InventoryGrid.GetAllItems();
}

TMap<FIntPoint, bool> UObsidianInventoryComponent::GetGridStateMap() const
{
	return InventoryGrid.InventoryStateMap;
}

UObsidianInventoryItemInstance* UObsidianInventoryComponent::FindFirstItemInstanceForDefinition(
	const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef) const
{
	for(const FObsidianInventoryEntry& Entry : InventoryGrid.Entries)
	{
		UObsidianInventoryItemInstance* Instance = Entry.Instance;
		if(IsValid(Instance))
		{
			if(Instance->GetItemDef() == ItemDef)
			{
				return Instance;
			}
		}
	}
	return nullptr;
}

bool UObsidianInventoryComponent::CanOwnerModifyInventoryState()
{
	return CanOwnerModifyContainerState();
}

bool UObsidianInventoryComponent::CanFitItemDefinition(FIntPoint& OutAvailablePositions, 
	const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef)
{
	if(const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef))
	{
		if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(
			ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
		{
			return CheckAvailablePosition(OutAvailablePositions, AppearanceFrag->GetItemGridSpanFromDesc());
		}
	}
	return false;
}

bool UObsidianInventoryComponent::CanFitItemDefinitionToSpecifiedSlot(const FIntPoint& SpecifiedSlot, 
	const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef)
{
	if(const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef))
	{
		if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(
			ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
		{
			return CheckSpecifiedPosition(AppearanceFrag->GetItemGridSpanFromDesc(), SpecifiedSlot);
		}
	}
	return false;
}

bool UObsidianInventoryComponent::CheckReplacementPossible(const FIntPoint& ItemToReplaceOriginPosition,
	const FIntPoint& AtGridSlot, const FIntPoint& GridSpanAtPosition, const FIntPoint& ReplacingGridSpan) const
{
	TMap<FIntPoint, bool> TempInventoryStateMap = InventoryGrid.InventoryStateMap;
	
	for(int32 SpanX = 0; SpanX < GridSpanAtPosition.X; ++SpanX)
	{
		for(int32 SpanY = 0; SpanY < GridSpanAtPosition.Y; ++SpanY)
		{
			const FIntPoint GridSlotToCheck = ItemToReplaceOriginPosition + FIntPoint(SpanX, SpanY);
			if(bool* TempLocation = TempInventoryStateMap.Find(GridSlotToCheck))
			{
				*TempLocation = false;
			}
#if !UE_BUILD_SHIPPING
			else
			{
				FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Trying to UnMark a Location [x: %d, y: %d] that doesn't"
				"exist in the TempInventoryStateMap in UObsidianInventoryComponent::CanReplaceItemAtSpecificSlot."), GridSlotToCheck.X, GridSlotToCheck.Y), ELogVerbosity::Warning);
			}
#endif
		}
	}
	
	bool bCanReplace = false;
	const bool* InitialPositionFreePtr = TempInventoryStateMap.Find(AtGridSlot);
	if(InitialPositionFreePtr && *InitialPositionFreePtr == false) // Initial location is free
	{
		bCanReplace = true;
		for(int32 SpanX = 0; SpanX < ReplacingGridSpan.X; ++SpanX)
		{
			for(int32 SpanY = 0; SpanY < ReplacingGridSpan.Y; ++SpanY)
			{
				const FIntPoint GridSlotToCheck = AtGridSlot + FIntPoint(SpanX, SpanY);
				const bool* bExistingOccupied = TempInventoryStateMap.Find(GridSlotToCheck);
				if(bExistingOccupied == nullptr || *bExistingOccupied)
				{
					bCanReplace = false;
					break;
				}
			}
		}
	}
	return bCanReplace;
}

FObsidianItemOperationResult UObsidianInventoryComponent::AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef,
	const FObsidianItemGeneratedData& ItemGeneratedData)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();
	Result.StacksLeft = ItemGeneratedData.GetStackCount();
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}
	
	if(CanOwnerModifyInventoryState() == false)
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
		const FObsidianAddingStacksResult AddingStacksResult = TryAddingStacksToExistingItems(ItemDef, Result.StacksLeft, /** OUT */ OutAddedToInstances);

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

	//TODO(intrxx) Shouldn't this be in the aboves if?
	const int32 StacksAvailableToAdd = GetNumberOfStacksAvailableToAddToInventory(ItemDef, Result.StacksLeft);
	if(StacksAvailableToAdd == 0)
	{
		//TODO(intrxx) We can no longer add this item to the inventory, add voice over?
		UE_LOG(ObLogInventory, Verbose, TEXT("Can no longer add this item to inventory!"));
		if(!OutAddedToInstances.IsEmpty())
		{
			Result.bActionSuccessful = true;
			Result.AffectedInstance = OutAddedToInstances.Last();
			return Result;
		}
		return Result;
	}
	
	FIntPoint AvailablePosition;
	if(CanFitItemDefinition(AvailablePosition, ItemDef) == false)
	{
		//TODO(intrxx) Inventory is full, add voice over?
		UE_LOG(ObLogInventory, Verbose, TEXT("Inventory is full!"));
		if(!OutAddedToInstances.IsEmpty())
		{
			Result.bActionSuccessful = true;
			Result.AffectedInstance = OutAddedToInstances.Last();
			return Result;
		}
		return Result;
	}
	
	Result.StacksLeft -= StacksAvailableToAdd;
	
	UObsidianInventoryItemInstance* Instance = PlaceItemDefinition(ItemDef, ItemGeneratedData, StacksAvailableToAdd, AvailablePosition);

	Result.bActionSuccessful = true;
	Result.AffectedInstance = Instance;
	return Result;
}

FObsidianItemOperationResult UObsidianInventoryComponent::AddItemDefinitionToSpecifiedSlot(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef,
	const FIntPoint& ToGridSlot, const FObsidianItemGeneratedData& ItemGeneratedData, const int32 StackToAddOverride)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();
	Result.StacksLeft = ItemGeneratedData.GetStackCount();
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}

	if(CanOwnerModifyInventoryState() == false)
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

	int32 StacksAvailableToAdd = 1;
	if(DefaultObject->IsStackable())
	{
		StacksAvailableToAdd = GetNumberOfStacksAvailableToAddToInventory(ItemDef, Result.StacksLeft);
		if(StacksAvailableToAdd == 0)
		{
			UE_LOG(ObLogInventory, Verbose, TEXT("Can no longer add this item to inventory!"));
			return Result;
		}

		if(StackToAddOverride != -1)
		{
			StacksAvailableToAdd = ClampStacksToAdd(StacksAvailableToAdd, StackToAddOverride);
		}
	}
	
	if(CanFitItemDefinitionToSpecifiedSlot(ToGridSlot, ItemDef) == false)
	{
		//TODO(intrxx) Inventory is full, add voice over?
		UE_LOG(ObLogInventory, Verbose, TEXT("Inventory is full at specified slot!"));
		return Result;
	}
	
	Result.StacksLeft -= StacksAvailableToAdd;
	Result.bActionSuccessful = Result.StacksLeft == 0; // We don't need any item duplication logic (like in AddItemInstanceToSpecificSlot)
														// as we still have a valid item definition in hands if the whole item isn't added here.
	ensure(Result.StacksLeft >= 0);
	
	UObsidianInventoryItemInstance* Instance = PlaceItemDefinition(ItemDef, ItemGeneratedData, StacksAvailableToAdd, ToGridSlot);

	Result.AffectedInstance = Instance;
	return Result;
}

FObsidianItemOperationResult UObsidianInventoryComponent::AddItemInstance(UObsidianInventoryItemInstance* InstanceToAdd)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}
	
	if(InstanceToAdd == nullptr)
	{
		return Result;
	}

	Result.StacksLeft = InstanceToAdd->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	
	if(CanOwnerModifyInventoryState() == false)
	{
		return Result;
	}
	
	if(InstanceToAdd->IsStackable())
	{
		TArray<UObsidianInventoryItemInstance*> OutAddedToInstances;
		const FObsidianAddingStacksResult AddingStacksResult = TryAddingStacksToExistingItems(InstanceToAdd->GetItemDef(), Result.StacksLeft, /** OUT */ OutAddedToInstances);

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
	
	const int32 StacksAvailableToAdd = GetNumberOfStacksAvailableToAddToInventory(InstanceToAdd);
	if(StacksAvailableToAdd == 0)
	{
		//TODO(intrxx) We can no longer add this item to the inventory, add voice over?
		UE_LOG(ObLogInventory, Verbose, TEXT("Can no longer add this item to inventory!"));
		return Result;
	}
	
	FIntPoint AvailablePosition;
	if(CheckAvailablePosition(AvailablePosition, InstanceToAdd->GetItemGridSpan()) == false)
	{
		//TODO(intrxx) Inventory is full, add voice over?
		UE_LOG(ObLogInventory, Verbose, TEXT("Inventory is full!"));
		return Result;
	}

	Result.bActionSuccessful = true;
	Result.StacksLeft -= StacksAvailableToAdd;
	ensure(Result.StacksLeft >= 0);

	bool bWholeItemPlaced = false;
	Result.AffectedInstance = PlaceItemInstance(InstanceToAdd, StacksAvailableToAdd, AvailablePosition, /** OUT */ bWholeItemPlaced);
	Result.bActionSuccessful = bWholeItemPlaced; // If only some of the stacks were placed, the rest is still held by the provided Instance.
	return Result;
}

FObsidianItemOperationResult UObsidianInventoryComponent::AddItemInstanceToSpecificSlot(
	UObsidianInventoryItemInstance* InstanceToAdd, const FIntPoint& ToGridSlot, const int32 StackToAddOverride)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}
	
	if(InstanceToAdd == nullptr)
	{
		return Result;
	}

	Result.StacksLeft = InstanceToAdd->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);

	if(CanOwnerModifyInventoryState() == false)
	{
		return Result;
	}

	int32 StacksAvailableToAdd = 1;
	if(InstanceToAdd->IsStackable())
	{
		StacksAvailableToAdd = GetNumberOfStacksAvailableToAddToInventory(InstanceToAdd);
		if(StacksAvailableToAdd == 0)
		{
			//TODO(intrxx) We can no longer add this item to the inventory, add voice over?
			UE_LOG(ObLogInventory, Verbose, TEXT("Can no longer add this item to inventory!"));
			return Result;
		}
		
		if(StackToAddOverride != INDEX_NONE)
		{
			StacksAvailableToAdd = ClampStacksToAdd(StacksAvailableToAdd, StackToAddOverride);
		}
	}
	
	if(CheckSpecifiedPosition(InstanceToAdd->GetItemGridSpan(), ToGridSlot) == false)
	{
		//TODO(intrxx) Inventory is full, add voice over?
		UE_LOG(ObLogInventory, Verbose, TEXT("Inventory is full at specified slot!"));
		return Result;
	}

	Result.bActionSuccessful = true;
	Result.StacksLeft -= StacksAvailableToAdd;
	ensure(Result.StacksLeft >= 0);

	bool bWholeItemPlaced = false;
	Result.AffectedInstance = PlaceItemInstance(InstanceToAdd, StacksAvailableToAdd, ToGridSlot, /** OUT */ bWholeItemPlaced);
	Result.bActionSuccessful = bWholeItemPlaced; // If only some of the stacks were placed, the rest is still held by the provided Instance.
	return Result;
}

FObsidianItemOperationResult UObsidianInventoryComponent::TakeOutFromItemInstance(UObsidianInventoryItemInstance* TakingFromInstance, const int32 StacksToTake)
{
	return TakeOutStacksFromItem(TakingFromInstance, StacksToTake);
}

FObsidianAddingStacksResult UObsidianInventoryComponent::TryAddingStacksToExistingItems(const TSubclassOf<UObsidianInventoryItemDefinition>& AddingFromItemDef, const int32 StacksToAdd, TArray<UObsidianInventoryItemInstance*>& OutAddedToInstances)
{
	FObsidianAddingStacksResult Result = FObsidianAddingStacksResult();
	Result.AddedStacks = 0;
	Result.StacksLeft = StacksToAdd;
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}

	if(CanOwnerModifyInventoryState() == false)
	{
		return Result;
	}
	
	if(StacksToAdd <= 0)
	{
		return Result;
	}
	
	int32 StacksHeld = FindAllStacksForGivenItem(AddingFromItemDef);
	if(UObsidianPlayerStashComponent* PlayerStashComponent = UObsidianPlayerStashComponent::FindPlayerStashComponent(GetOwner()))
	{
		StacksHeld += PlayerStashComponent->FindAllStacksForGivenItem(AddingFromItemDef);
	}
	
	TArray<UObsidianInventoryItemInstance*> Items = InventoryGrid.GetAllItems();
	for(UObsidianInventoryItemInstance* Instance : Items)
	{
		if(!IsValid(Instance))
		{
			UE_LOG(ObLogInventory, Error, TEXT("Instance is invalid in UObsidianInventoryComponent::TryAddingStacksToExistingItem."));
			continue;
		}
		
		if(AddingFromItemDef == Instance->GetItemDef())
		{
			const int32 LimitStackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Limit);
			if((LimitStackCount == 1) || (LimitStackCount > 0 && StacksHeld >= LimitStackCount))
			{
				break;
			}
			
			const int32 CurrentStackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
			if(CurrentStackCount == 0)
			{
				continue;
			}

			const int32 StacksLeft = Result.StacksLeft;
			const int32 StacksThatCanBeAddedToInventory = LimitStackCount == 0 ? StacksLeft : LimitStackCount - StacksHeld;
			if(StacksThatCanBeAddedToInventory <= 0)
			{
				continue;
			}
			
			const int32 MaxStackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Max);
			int32 AmountThatCanBeAddedToInstance = FMath::Clamp((MaxStackCount - CurrentStackCount), 0, StacksThatCanBeAddedToInventory);
			AmountThatCanBeAddedToInstance = FMath::Min(AmountThatCanBeAddedToInstance, StacksLeft);
			if(AmountThatCanBeAddedToInstance <= 0)
			{
				continue;
			}
			UE_LOG(ObLogInventory, Verbose, TEXT("Added [%d] stacks to [%s]."), AmountThatCanBeAddedToInstance, *GetNameSafe(Instance));
			
			Instance->AddItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, AmountThatCanBeAddedToInstance);
			InventoryGrid.ChangedEntryStacks(Instance, CurrentStackCount);
			
			StacksHeld += AmountThatCanBeAddedToInstance;
			Result.AddedStacks += AmountThatCanBeAddedToInstance;
			Result.StacksLeft -= AmountThatCanBeAddedToInstance;
			OutAddedToInstances.AddUnique(Instance);
			
			if(Result.AddedStacks == StacksToAdd)
			{
				Result.AddingStacksResult = EObsidianAddingStacksResultType::ASR_WholeItemAsStacksAdded;
				return Result;
			}
		}
	}
	if(OutAddedToInstances.Num() > 0)
	{
		Result.AddingStacksResult = EObsidianAddingStacksResultType::ASR_SomeOfTheStacksAdded;
	}
	return Result;
}

FObsidianAddingStacksResult UObsidianInventoryComponent::TryAddingStacksToSpecificSlotWithItemDef(
	const TSubclassOf<UObsidianInventoryItemDefinition>& AddingFromItemDef, const int32 AddingFromItemDefCurrentStacks, 
	const FIntPoint& AtGridSlot, const int32 StackToAddOverride)
{
	return AddStacksToItemFromDefinition(AddingFromItemDef, AddingFromItemDefCurrentStacks,
		GetItemInstanceAtLocation(AtGridSlot), StackToAddOverride);
}

FObsidianAddingStacksResult UObsidianInventoryComponent::TryAddingStacksToSpecificSlotWithInstance(
	UObsidianInventoryItemInstance* AddingFromInstance, const FIntPoint& AtGridSlot, const int32 StackToAddOverride)
{
	return AddStacksToItemFromInstance(AddingFromInstance, GetItemInstanceAtLocation(AtGridSlot), 
		StackToAddOverride);
}

int32 UObsidianInventoryComponent::FindAllStacksForGivenItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef)
{
	return CountStacksOfItem(ItemDef);
}

int32 UObsidianInventoryComponent::FindAllStacksForGivenItem(const UObsidianInventoryItemInstance* ItemInstance)
{
	return ItemInstance ? CountStacksOfItem(ItemInstance->GetItemDef()) : 0;
}

int32 UObsidianInventoryComponent::GetNumberOfStacksAvailableToAddToInventory(
	const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const int32 CurrentStacks)
{
	const UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject();
	if(DefaultObject == nullptr)
	{
		return 0;
	}
	
	const UOInventoryItemFragment_Stacks* StacksFrag = Cast<UOInventoryItemFragment_Stacks>(
		DefaultObject->FindFragmentByClass(UOInventoryItemFragment_Stacks::StaticClass()));
	if(StacksFrag == nullptr)
	{
		return CurrentStacks;
	}
	
	const int32 LimitStackCount = StacksFrag->GetItemStackNumberByTag(ObsidianGameplayTags::Item_StackCount_Limit);
	if(LimitStackCount == 0) // Item has no limit
	{
		return CurrentStacks;
	}
	
	int32 AllStacksInStash = 0;
	if(UObsidianPlayerStashComponent* PlayerStashComponent = UObsidianPlayerStashComponent::FindPlayerStashComponent(
		GetOwner()))
	{
		AllStacksInStash = PlayerStashComponent->FindAllStacksForGivenItem(ItemDef);
	}
	
	const int32 AllStacksInInventory = FindAllStacksForGivenItem(ItemDef);
	const int32 CombinedStacks = AllStacksInInventory + AllStacksInStash;
	ensureMsgf(CombinedStacks <= LimitStackCount, TEXT("Combined Stacks of held item is already bigger than Stacks Limit for this item, something went wrong."));
	
	return FMath::Clamp(LimitStackCount - CombinedStacks, 0, CurrentStacks);
}

int32 UObsidianInventoryComponent::GetNumberOfStacksAvailableToAddToInventory(const UObsidianInventoryItemInstance* ItemInstance)
{
	const int32 CurrentStacks = ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	const int32 LimitStackCount = ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Limit);
	if(LimitStackCount == 0) // Item has no limit
	{
		return CurrentStacks;
	}

	//TODO(intrxx) Rethink it, I kinda dont want to limit items in stash
	int32 AllStacksInStash = 0;
	if(UObsidianPlayerStashComponent* PlayerStashComponent = UObsidianPlayerStashComponent::FindPlayerStashComponent(
		GetOwner()))
	{
		AllStacksInStash = PlayerStashComponent->FindAllStacksForGivenItem(ItemInstance);
	}
	
	const int32 AllStacksInInventory = FindAllStacksForGivenItem(ItemInstance);
	const int32 CombinedStacks = AllStacksInInventory + AllStacksInStash;
	ensureMsgf(CombinedStacks <= LimitStackCount, TEXT("Combined Stacks of held item is already bigger than Stacks Limit for this item, something went wrong."));
	
	return  FMath::Clamp(LimitStackCount - CombinedStacks, 0, CurrentStacks);
}

FObsidianItemOperationResult UObsidianInventoryComponent::RemoveItemInstance(UObsidianInventoryItemInstance* InstanceToRemove)
{
	return RemoveItemFromContainer(InstanceToRemove);
}

void UObsidianInventoryComponent::LoadInventorizedItem(const FObsidianSavedItem& InventorizedSavedItem)
{
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return;
	}
	
	UObsidianInventoryItemInstance* LoadedInstance = InventoryGrid.LoadEntry(InventorizedSavedItem);

	RegisterItemInstanceForReplication(LoadedInstance);
}

TArray<UObsidianInventoryItemInstance*> UObsidianInventoryComponent::GetContainedItems() const
{
	return InventoryGrid.GetAllItems();
}

FGameplayTag UObsidianInventoryComponent::GetBlockActionsTag() const
{
	return ObsidianGameplayTags::Inventory_BlockActions;
}

void UObsidianInventoryComponent::AddItemInstanceToList(UObsidianInventoryItemInstance* Instance,
	const FObsidianItemPosition& ToPosition)
{
	InventoryGrid.AddEntry(Instance, ToPosition.GetItemGridPosition());
}

UObsidianInventoryItemInstance* UObsidianInventoryComponent::AddItemDefinitionToList(
	const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FObsidianItemGeneratedData& ItemGeneratedData,
	const int32 StackCount, const FObsidianItemPosition& ToPosition)
{
	return InventoryGrid.AddEntry(ItemDef, ItemGeneratedData, StackCount, ToPosition.GetItemGridPosition());
}

void UObsidianInventoryComponent::RemoveItemInstanceFromList(UObsidianInventoryItemInstance* Instance)
{
	InventoryGrid.RemoveEntry(Instance);
}

void UObsidianInventoryComponent::HandleItemStacksChanged(UObsidianInventoryItemInstance* Instance, 
	const int32 OldStackCount)
{
	InventoryGrid.ChangedEntryStacks(Instance, OldStackCount);
}

void UObsidianInventoryComponent::HandleItemChanged(UObsidianInventoryItemInstance* Instance)
{
	InventoryGrid.GeneralEntryChange(Instance);
}

FIntPoint UObsidianInventoryComponent::GetItemLocationFromGrid(UObsidianInventoryItemInstance* ItemInstance) const
{
	if(ItemInstance == nullptr)
	{
		return FIntPoint::NoneValue;
	}

	const FIntPoint* GridLocation = InventoryGrid.GridLocationToItemMap.FindKey(ItemInstance);
	return GridLocation ? *GridLocation : FIntPoint::NoneValue;
}

void UObsidianInventoryComponent::InitInventoryState()
{
	int16 GridX = 0;
	int16 GridY = 0;
	
	for(int32 i = 0; i < InventoryGridSize; i++)
	{
		InventoryGrid.InventoryStateMap.Add(FIntPoint(GridX, GridY), false);
		
		if(GridX == InventoryGridWidth - 1)
		{
			GridX = 0;
			GridY++;
		}
		else
		{
			GridX++;
		}
	}
	
	for(const TTuple<FIntPoint, UObsidianInventoryItemInstance*>& ItemLoc : InventoryGrid.GridLocationToItemMap)
	{
		InventoryGrid.Item_MarkSpace(ItemLoc.Value, ItemLoc.Key);
	}
}

bool UObsidianInventoryComponent::CheckAvailablePosition(FIntPoint& OutAvailablePosition, const FIntPoint& ItemGridSpan)
{
	bool bCanFit = false;
	
	for(const TTuple<FIntPoint, bool>& Location : InventoryGrid.InventoryStateMap)
	{
		if(Location.Value == false) // Location is free
		{
			bCanFit = true;
			
			for(int32 SpanX = 0; SpanX < ItemGridSpan.X; ++SpanX)
			{
				for(int32 SpanY = 0; SpanY < ItemGridSpan.Y; ++SpanY)
				{
					const FIntPoint LocationToCheck = Location.Key + FIntPoint(SpanX, SpanY);
					const bool* bExistingOccupied = InventoryGrid.InventoryStateMap.Find(LocationToCheck);
					if(bExistingOccupied == nullptr || *bExistingOccupied)
					{
						bCanFit = false;
						break;
					}
				}
			}
			
			if(bCanFit) // Return if we get Available Position
			{
				OutAvailablePosition = Location.Key;
				return bCanFit;
			}
		}
	}

	return bCanFit;
}

bool UObsidianInventoryComponent::CheckSpecifiedPosition(const FIntPoint& ItemGridSpan, const FIntPoint& SpecifiedPosition)
{
	bool bCanFit = false;

	const bool* InitialPositionFreePtr = InventoryGrid.InventoryStateMap.Find(SpecifiedPosition);
	if(InitialPositionFreePtr && *InitialPositionFreePtr == false) // Initial location is free
	{
		bCanFit = true;
		for(int32 SpanX = 0; SpanX < ItemGridSpan.X; ++SpanX)
		{
			for(int32 SpanY = 0; SpanY < ItemGridSpan.Y; ++SpanY)
			{
				const FIntPoint LocationToCheck = SpecifiedPosition + FIntPoint(SpanX, SpanY);
				const bool* ExistingOccupiedPtr = InventoryGrid.InventoryStateMap.Find(LocationToCheck);
				if(ExistingOccupiedPtr == nullptr || *ExistingOccupiedPtr)
				{
					bCanFit = false;
					break;
				}
			}
		}
	}
	return bCanFit;
}

bool UObsidianInventoryComponent::CanFitItemInstance(const UObsidianInventoryItemInstance* Instance)
{
	FIntPoint AvailablePosition = FIntPoint::NoneValue;
	return CheckAvailablePosition(AvailablePosition, Instance->GetItemGridSpan());
}

bool UObsidianInventoryComponent::CanReplaceItemAtSpecificSlotWithInstance(const FIntPoint& ClickedInstancePosition,
	const FIntPoint& ClickedGridPosition, UObsidianInventoryItemInstance* ReplacingInstance)
{
	const UObsidianInventoryItemInstance* InstanceAtLocation = GetItemInstanceAtLocation(ClickedInstancePosition);
	if (InstanceAtLocation == nullptr)
	{
		return false;
	}
	
	if(GetNumberOfStacksAvailableToAddToInventory(ReplacingInstance) <= 0)
	{
		//TODO(intrxx) Limit of stacks reached, add voiceover?
		return false;
	}
	
	return CheckReplacementPossible(ClickedInstancePosition, ClickedGridPosition,
		InstanceAtLocation->GetItemGridSpan(), ReplacingInstance->GetItemGridSpan());
}

bool UObsidianInventoryComponent::CanReplaceItemAtSpecificSlotWithDef(const FIntPoint& ClickedInstancePosition,
	const FIntPoint& ClickedGridPosition, const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef,
	const int32 StackCount)
{
	const UObsidianInventoryItemInstance* InstanceAtLocation = GetItemInstanceAtLocation(ClickedInstancePosition);
	if (InstanceAtLocation == nullptr)
	{
		return false;
	}
	
	if(GetNumberOfStacksAvailableToAddToInventory(ItemDef, StackCount) <= 0)
	{
		//TODO(intrxx) Limit of stacks reached, add voiceover?
		return false;
	}
	
	if(const UObsidianInventoryItemDefinition* DefaultItem = ItemDef.GetDefaultObject())
	{
		if(const UOInventoryItemFragment_Appearance* Appearance = Cast<UOInventoryItemFragment_Appearance>(
			DefaultItem->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
		{
			return CheckReplacementPossible(ClickedInstancePosition, ClickedGridPosition,
				InstanceAtLocation->GetItemGridSpan(), Appearance->GetItemGridSpanFromDesc());
		}
	}

	return false;
}

bool UObsidianInventoryComponent::CanFitItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef)
{
	bool bCanFit = false;
	
	if(const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef))
	{
		if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(
			ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
		{
			const FIntPoint ItemGridSpan = AppearanceFrag->GetItemGridSpanFromDesc();

			FIntPoint AvailablePosition = FIntPoint::NoneValue;
			bCanFit = CheckAvailablePosition(AvailablePosition, ItemGridSpan);
			return bCanFit;
		}
	}
	return bCanFit;
}



