// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"

#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Characters/Heroes/ObsidianHero.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Stacks.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianItemsFunctionLibrary.h"
#include "InventoryItems/PlayerStash/ObsidianPlayerStashComponent.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "Obsidian/ObsidianLogCategories.h"


UObsidianInventoryComponent::UObsidianInventoryComponent(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
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

void UObsidianInventoryComponent::InitSaveData(const bool bInReceivedInitialItems)
{
	bReceivedInitialInventoryItems = bInReceivedInitialItems;
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

int32 UObsidianInventoryComponent::GetTotalItemCountByDefinition(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef) const
{
	int32 FinalCount = 0;
	for(const FObsidianInventoryEntry& Entry : InventoryGrid.Entries)
	{
		UObsidianInventoryItemInstance* Instance = Entry.Instance;
		if(IsValid(Instance))
		{
			if(Instance->GetItemDef() == InItemDef)
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

UObsidianInventoryItemInstance* UObsidianInventoryComponent::GetItemInstanceAtLocation(const FIntPoint& InLocation) const
{
	return InventoryGrid.GridLocationToItemMap.FindRef(InLocation);
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
	const TSubclassOf<UObsidianInventoryItemDefinition> InItemDef) const
{
	for(const FObsidianInventoryEntry& Entry : InventoryGrid.Entries)
	{
		UObsidianInventoryItemInstance* Instance = Entry.Instance;
		if(IsValid(Instance))
		{
			if(Instance->GetItemDef() == InItemDef)
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
	const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef)
{
	if(const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(InItemDef))
	{
		if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(
			ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
		{
			return CheckAvailablePosition(OutAvailablePositions, AppearanceFrag->GetItemGridSpanFromDesc());
		}
	}
	return false;
}

bool UObsidianInventoryComponent::CanFitItemDefinitionToSpecifiedSlot(const FIntPoint& InSpecifiedSlot, 
	const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef)
{
	if(const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(InItemDef))
	{
		if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(
			ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
		{
			return CheckSpecifiedPosition(AppearanceFrag->GetItemGridSpanFromDesc(), InSpecifiedSlot);
		}
	}
	return false;
}

bool UObsidianInventoryComponent::CheckReplacementPossible(const FIntPoint& InItemToReplaceOriginPosition,
	const FIntPoint& InAtGridSlot, const FIntPoint& InGridSpanAtPosition, const FIntPoint& InReplacingGridSpan) const
{
	TMap<FIntPoint, bool> TempInventoryStateMap = InventoryGrid.InventoryStateMap;
	
	for(int32 SpanX = 0; SpanX < InGridSpanAtPosition.X; ++SpanX)
	{
		for(int32 SpanY = 0; SpanY < InGridSpanAtPosition.Y; ++SpanY)
		{
			const FIntPoint GridSlotToCheck = InItemToReplaceOriginPosition + FIntPoint(SpanX, SpanY);
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
	const bool* InitialPositionFreePtr = TempInventoryStateMap.Find(InAtGridSlot);
	if(InitialPositionFreePtr && *InitialPositionFreePtr == false) // Initial location is free
	{
		bCanReplace = true;
		for(int32 SpanX = 0; SpanX < InReplacingGridSpan.X; ++SpanX)
		{
			for(int32 SpanY = 0; SpanY < InReplacingGridSpan.Y; ++SpanY)
			{
				const FIntPoint GridSlotToCheck = InAtGridSlot + FIntPoint(SpanX, SpanY);
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

FObsidianItemOperationResult UObsidianInventoryComponent::AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> InItemDef,
	const FObsidianItemGeneratedData& InItemGeneratedData)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();
	Result.StacksLeft = InItemGeneratedData.GetStackCount();
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}
	
	if(CanOwnerModifyInventoryState() == false)
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
	
	TArray<UObsidianInventoryItemInstance*> OutAddedToInstances;
	if(DefaultObject->IsStackable())
	{
		const FObsidianAddingStacksResult AddingStacksResult = TryAddingStacksToExistingItems(InItemDef, Result.StacksLeft, /** OUT */ OutAddedToInstances);

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
	const int32 StacksAvailableToAdd = GetNumberOfStacksAvailableToAddToInventory(InItemDef, Result.StacksLeft);
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
	if(CanFitItemDefinition(AvailablePosition, InItemDef) == false)
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
	
	UObsidianInventoryItemInstance* Instance = PlaceItemDefinition(InItemDef, InItemGeneratedData, StacksAvailableToAdd, AvailablePosition);

	Result.bActionSuccessful = true;
	Result.AffectedInstance = Instance;
	return Result;
}

FObsidianItemOperationResult UObsidianInventoryComponent::AddItemDefinitionToSpecifiedSlot(const TSubclassOf<UObsidianInventoryItemDefinition> InItemDef,
	const FIntPoint& InToGridSlot, const FObsidianItemGeneratedData& InItemGeneratedData, const int32 InStackToAddOverride)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();
	Result.StacksLeft = InItemGeneratedData.GetStackCount();
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}

	if(CanOwnerModifyInventoryState() == false)
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

	int32 StacksAvailableToAdd = 1;
	if(DefaultObject->IsStackable())
	{
		StacksAvailableToAdd = GetNumberOfStacksAvailableToAddToInventory(InItemDef, Result.StacksLeft);
		if(StacksAvailableToAdd == 0)
		{
			UE_LOG(ObLogInventory, Verbose, TEXT("Can no longer add this item to inventory!"));
			return Result;
		}

		if(InStackToAddOverride != -1)
		{
			StacksAvailableToAdd = ClampStacksToAdd(StacksAvailableToAdd, InStackToAddOverride);
		}
	}
	
	if(CanFitItemDefinitionToSpecifiedSlot(InToGridSlot, InItemDef) == false)
	{
		//TODO(intrxx) Inventory is full, add voice over?
		UE_LOG(ObLogInventory, Verbose, TEXT("Inventory is full at specified slot!"));
		return Result;
	}
	
	Result.StacksLeft -= StacksAvailableToAdd;
	Result.bActionSuccessful = Result.StacksLeft == 0; // We don't need any item duplication logic (like in AddItemInstanceToSpecificSlot)
														// as we still have a valid item definition in hands if the whole item isn't added here.
	ensure(Result.StacksLeft >= 0);
	
	UObsidianInventoryItemInstance* Instance = PlaceItemDefinition(InItemDef, InItemGeneratedData, StacksAvailableToAdd, InToGridSlot);

	Result.AffectedInstance = Instance;
	return Result;
}

FObsidianItemOperationResult UObsidianInventoryComponent::AddItemInstance(UObsidianInventoryItemInstance* InInstanceToAdd)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}
	
	if(InInstanceToAdd == nullptr)
	{
		return Result;
	}

	Result.StacksLeft = InInstanceToAdd->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	
	if(CanOwnerModifyInventoryState() == false)
	{
		return Result;
	}
	
	if(InInstanceToAdd->IsStackable())
	{
		TArray<UObsidianInventoryItemInstance*> OutAddedToInstances;
		const FObsidianAddingStacksResult AddingStacksResult = TryAddingStacksToExistingItems(InInstanceToAdd->GetItemDef(), Result.StacksLeft, /** OUT */ OutAddedToInstances);

		if(AddingStacksResult.AddingStacksResult == EObsidianAddingStacksResultType::ASR_SomeOfTheStacksAdded)
		{
			Result.StacksLeft = AddingStacksResult.StacksLeft;
			Result.AffectedInstance = OutAddedToInstances.Last();
			InInstanceToAdd->OverrideItemStackCount(ObsidianGameplayTags::Item::StackCount::Current, Result.StacksLeft);
		}
		
		if(AddingStacksResult.AddingStacksResult == EObsidianAddingStacksResultType::ASR_WholeItemAsStacksAdded)
		{
			Result.bActionSuccessful = true;
			Result.StacksLeft = AddingStacksResult.StacksLeft;
			Result.AffectedInstance = OutAddedToInstances.Last();
			return Result;
		}
	}
	
	const int32 StacksAvailableToAdd = GetNumberOfStacksAvailableToAddToInventory(InInstanceToAdd);
	if(StacksAvailableToAdd == 0)
	{
		//TODO(intrxx) We can no longer add this item to the inventory, add voice over?
		UE_LOG(ObLogInventory, Verbose, TEXT("Can no longer add this item to inventory!"));
		return Result;
	}
	
	FIntPoint AvailablePosition;
	if(CheckAvailablePosition(AvailablePosition, InInstanceToAdd->GetItemGridSpan()) == false)
	{
		//TODO(intrxx) Inventory is full, add voice over?
		UE_LOG(ObLogInventory, Verbose, TEXT("Inventory is full!"));
		return Result;
	}

	Result.bActionSuccessful = true;
	Result.StacksLeft -= StacksAvailableToAdd;
	ensure(Result.StacksLeft >= 0);

	bool bWholeItemPlaced = false;
	Result.AffectedInstance = PlaceItemInstance(InInstanceToAdd, StacksAvailableToAdd, AvailablePosition, /** OUT */ bWholeItemPlaced);
	Result.bActionSuccessful = bWholeItemPlaced; // If only some of the stacks were placed, the rest is still held by the provided Instance.
	return Result;
}

FObsidianItemOperationResult UObsidianInventoryComponent::AddItemInstanceToSpecificSlot(
	UObsidianInventoryItemInstance* InInstanceToAdd, const FIntPoint& InToGridSlot, const int32 InStackToAddOverride)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}
	
	if(InInstanceToAdd == nullptr)
	{
		return Result;
	}

	Result.StacksLeft = InInstanceToAdd->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);

	if(CanOwnerModifyInventoryState() == false)
	{
		return Result;
	}

	int32 StacksAvailableToAdd = 1;
	if(InInstanceToAdd->IsStackable())
	{
		StacksAvailableToAdd = GetNumberOfStacksAvailableToAddToInventory(InInstanceToAdd);
		if(StacksAvailableToAdd == 0)
		{
			//TODO(intrxx) We can no longer add this item to the inventory, add voice over?
			UE_LOG(ObLogInventory, Verbose, TEXT("Can no longer add this item to inventory!"));
			return Result;
		}
		
		if(InStackToAddOverride != INDEX_NONE)
		{
			StacksAvailableToAdd = ClampStacksToAdd(StacksAvailableToAdd, InStackToAddOverride);
		}
	}
	
	if(CheckSpecifiedPosition(InInstanceToAdd->GetItemGridSpan(), InToGridSlot) == false)
	{
		//TODO(intrxx) Inventory is full, add voice over?
		UE_LOG(ObLogInventory, Verbose, TEXT("Inventory is full at specified slot!"));
		return Result;
	}

	Result.bActionSuccessful = true;
	Result.StacksLeft -= StacksAvailableToAdd;
	ensure(Result.StacksLeft >= 0);

	bool bWholeItemPlaced = false;
	Result.AffectedInstance = PlaceItemInstance(InInstanceToAdd, StacksAvailableToAdd, InToGridSlot, /** OUT */ bWholeItemPlaced);
	Result.bActionSuccessful = bWholeItemPlaced; // If only some of the stacks were placed, the rest is still held by the provided Instance.
	return Result;
}

FObsidianItemOperationResult UObsidianInventoryComponent::TakeOutFromItemInstance(UObsidianInventoryItemInstance* InTakingFromInstance, const int32 InStacksToTake)
{
	return TakeOutStacksFromItem(InTakingFromInstance, InStacksToTake);
}

FObsidianAddingStacksResult UObsidianInventoryComponent::TryAddingStacksToExistingItems(const TSubclassOf<UObsidianInventoryItemDefinition>& InAddingFromItemDef, const int32 InStacksToAdd, TArray<UObsidianInventoryItemInstance*>& OutAddedToInstances)
{
	FObsidianAddingStacksResult Result = FObsidianAddingStacksResult();
	Result.AddedStacks = 0;
	Result.StacksLeft = InStacksToAdd;
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}

	if(CanOwnerModifyInventoryState() == false)
	{
		return Result;
	}
	
	if(InStacksToAdd <= 0)
	{
		return Result;
	}
	
	int32 StacksHeld = FindAllStacksForGivenItem(InAddingFromItemDef);
	if(UObsidianPlayerStashComponent* PlayerStashComponent = UObsidianPlayerStashComponent::FindPlayerStashComponent(GetOwner()))
	{
		StacksHeld += PlayerStashComponent->FindAllStacksForGivenItem(InAddingFromItemDef);
	}
	
	TArray<UObsidianInventoryItemInstance*> Items = InventoryGrid.GetAllItems();
	for(UObsidianInventoryItemInstance* Instance : Items)
	{
		if(!IsValid(Instance))
		{
			UE_LOG(ObLogInventory, Error, TEXT("Instance is invalid in UObsidianInventoryComponent::TryAddingStacksToExistingItem."));
			continue;
		}
		
		if(InAddingFromItemDef == Instance->GetItemDef())
		{
			const int32 LimitStackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Limit);
			if((LimitStackCount == 1) || (LimitStackCount > 0 && StacksHeld >= LimitStackCount))
			{
				break;
			}
			
			const int32 CurrentStackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
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
			
			const int32 MaxStackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Max);
			int32 AmountThatCanBeAddedToInstance = FMath::Clamp((MaxStackCount - CurrentStackCount), 0, StacksThatCanBeAddedToInventory);
			AmountThatCanBeAddedToInstance = FMath::Min(AmountThatCanBeAddedToInstance, StacksLeft);
			if(AmountThatCanBeAddedToInstance <= 0)
			{
				continue;
			}
			UE_LOG(ObLogInventory, Verbose, TEXT("Added [%d] stacks to [%s]."), AmountThatCanBeAddedToInstance, *GetNameSafe(Instance));
			
			Instance->AddItemStackCount(ObsidianGameplayTags::Item::StackCount::Current, AmountThatCanBeAddedToInstance);
			InventoryGrid.ChangedEntryStacks(Instance, CurrentStackCount);
			
			StacksHeld += AmountThatCanBeAddedToInstance;
			Result.AddedStacks += AmountThatCanBeAddedToInstance;
			Result.StacksLeft -= AmountThatCanBeAddedToInstance;
			OutAddedToInstances.AddUnique(Instance);
			
			if(Result.AddedStacks == InStacksToAdd)
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
	const TSubclassOf<UObsidianInventoryItemDefinition>& InAddingFromItemDef, const int32 InAddingFromItemDefCurrentStacks, 
	const FIntPoint& InAtGridSlot, const int32 InStackToAddOverride)
{
	return AddStacksToItemFromDefinition(InAddingFromItemDef, InAddingFromItemDefCurrentStacks,
		GetItemInstanceAtLocation(InAtGridSlot), InStackToAddOverride);
}

FObsidianAddingStacksResult UObsidianInventoryComponent::TryAddingStacksToSpecificSlotWithInstance(
	UObsidianInventoryItemInstance* InAddingFromInstance, const FIntPoint& InAtGridSlot, const int32 InStackToAddOverride)
{
	return AddStacksToItemFromInstance(InAddingFromInstance, GetItemInstanceAtLocation(InAtGridSlot), 
		InStackToAddOverride);
}

int32 UObsidianInventoryComponent::FindAllStacksForGivenItem(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef)
{
	return CountStacksOfItem(InItemDef);
}

int32 UObsidianInventoryComponent::FindAllStacksForGivenItem(const UObsidianInventoryItemInstance* InItemInstance)
{
	return InItemInstance ? CountStacksOfItem(InItemInstance->GetItemDef()) : 0;
}

int32 UObsidianInventoryComponent::GetNumberOfStacksAvailableToAddToInventory(
	const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef, const int32 InCurrentStacks)
{
	const UObsidianInventoryItemDefinition* DefaultObject = InItemDef.GetDefaultObject();
	if(DefaultObject == nullptr)
	{
		return 0;
	}
	
	const UOInventoryItemFragment_Stacks* StacksFrag = Cast<UOInventoryItemFragment_Stacks>(
		DefaultObject->FindFragmentByClass(UOInventoryItemFragment_Stacks::StaticClass()));
	if(StacksFrag == nullptr)
	{
		return InCurrentStacks;
	}
	
	const int32 LimitStackCount = StacksFrag->GetItemStackNumberByTag(ObsidianGameplayTags::Item::StackCount::Limit);
	if(LimitStackCount == 0) // Item has no limit
	{
		return InCurrentStacks;
	}
	
	int32 AllStacksInStash = 0;
	if(UObsidianPlayerStashComponent* PlayerStashComponent = UObsidianPlayerStashComponent::FindPlayerStashComponent(
		GetOwner()))
	{
		AllStacksInStash = PlayerStashComponent->FindAllStacksForGivenItem(InItemDef);
	}
	
	const int32 AllStacksInInventory = FindAllStacksForGivenItem(InItemDef);
	const int32 CombinedStacks = AllStacksInInventory + AllStacksInStash;
	ensureMsgf(CombinedStacks <= LimitStackCount, TEXT("Combined Stacks of held item is already bigger than Stacks Limit for this item, something went wrong."));
	
	return FMath::Clamp(LimitStackCount - CombinedStacks, 0, InCurrentStacks);
}

int32 UObsidianInventoryComponent::GetNumberOfStacksAvailableToAddToInventory(const UObsidianInventoryItemInstance* InItemInstance)
{
	const int32 CurrentStacks = InItemInstance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	const int32 LimitStackCount = InItemInstance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Limit);
	if(LimitStackCount == 0) // Item has no limit
	{
		return CurrentStacks;
	}

	//TODO(intrxx) Rethink it, I kinda dont want to limit items in stash
	int32 AllStacksInStash = 0;
	if(UObsidianPlayerStashComponent* PlayerStashComponent = UObsidianPlayerStashComponent::FindPlayerStashComponent(
		GetOwner()))
	{
		AllStacksInStash = PlayerStashComponent->FindAllStacksForGivenItem(InItemInstance);
	}
	
	const int32 AllStacksInInventory = FindAllStacksForGivenItem(InItemInstance);
	const int32 CombinedStacks = AllStacksInInventory + AllStacksInStash;
	ensureMsgf(CombinedStacks <= LimitStackCount, TEXT("Combined Stacks of held item is already bigger than Stacks Limit for this item, something went wrong."));
	
	return  FMath::Clamp(LimitStackCount - CombinedStacks, 0, CurrentStacks);
}

FObsidianItemOperationResult UObsidianInventoryComponent::RemoveItemInstance(UObsidianInventoryItemInstance* InInstanceToRemove)
{
	return RemoveItemFromContainer(InInstanceToRemove);
}

void UObsidianInventoryComponent::LoadInventorizedItem(const FObsidianSavedItem& InInventorizedSavedItem)
{
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return;
	}
	
	UObsidianInventoryItemInstance* LoadedInstance = InventoryGrid.LoadEntry(InInventorizedSavedItem);

	RegisterItemInstanceForReplication(LoadedInstance);
}

TArray<UObsidianInventoryItemInstance*> UObsidianInventoryComponent::GetContainedItems() const
{
	return InventoryGrid.GetAllItems();
}

FGameplayTag UObsidianInventoryComponent::GetBlockActionsTag() const
{
	return ObsidianGameplayTags::Inventory::BlockActions;
}

void UObsidianInventoryComponent::AddItemInstanceToList(UObsidianInventoryItemInstance* InInstance,
	const FObsidianItemPosition& InToPosition)
{
	InventoryGrid.AddEntry(InInstance, InToPosition.GetItemGridPosition());
}

UObsidianInventoryItemInstance* UObsidianInventoryComponent::AddItemDefinitionToList(
	const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef, const FObsidianItemGeneratedData& InItemGeneratedData,
	const int32 InStackCount, const FObsidianItemPosition& InToPosition)
{
	return InventoryGrid.AddEntry(InItemDef, InItemGeneratedData, InStackCount, InToPosition.GetItemGridPosition());
}

void UObsidianInventoryComponent::RemoveItemInstanceFromList(UObsidianInventoryItemInstance* InInstance)
{
	InventoryGrid.RemoveEntry(InInstance);
}

void UObsidianInventoryComponent::HandleItemStacksChanged(UObsidianInventoryItemInstance* InInstance, 
	const int32 InOldStackCount)
{
	InventoryGrid.ChangedEntryStacks(InInstance, InOldStackCount);
}

void UObsidianInventoryComponent::HandleItemChanged(UObsidianInventoryItemInstance* InInstance)
{
	InventoryGrid.GeneralEntryChange(InInstance);
}

FIntPoint UObsidianInventoryComponent::GetItemLocationFromGrid(UObsidianInventoryItemInstance* InItemInstance) const
{
	if(InItemInstance == nullptr)
	{
		return FIntPoint::NoneValue;
	}

	const FIntPoint* GridLocation = InventoryGrid.GridLocationToItemMap.FindKey(InItemInstance);
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

bool UObsidianInventoryComponent::CheckAvailablePosition(FIntPoint& OutAvailablePosition, const FIntPoint& InItemGridSpan)
{
	bool bCanFit = false;
	
	for(const TTuple<FIntPoint, bool>& Location : InventoryGrid.InventoryStateMap)
	{
		if(Location.Value == false) // Location is free
		{
			bCanFit = true;
			
			for(int32 SpanX = 0; SpanX < InItemGridSpan.X; ++SpanX)
			{
				for(int32 SpanY = 0; SpanY < InItemGridSpan.Y; ++SpanY)
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

bool UObsidianInventoryComponent::CheckSpecifiedPosition(const FIntPoint& InItemGridSpan, const FIntPoint& InSpecifiedPosition)
{
	bool bCanFit = false;

	const bool* InitialPositionFreePtr = InventoryGrid.InventoryStateMap.Find(InSpecifiedPosition);
	if(InitialPositionFreePtr && *InitialPositionFreePtr == false) // Initial location is free
	{
		bCanFit = true;
		for(int32 SpanX = 0; SpanX < InItemGridSpan.X; ++SpanX)
		{
			for(int32 SpanY = 0; SpanY < InItemGridSpan.Y; ++SpanY)
			{
				const FIntPoint LocationToCheck = InSpecifiedPosition + FIntPoint(SpanX, SpanY);
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

bool UObsidianInventoryComponent::CanFitItemInstance(const UObsidianInventoryItemInstance* InInstance)
{
	FIntPoint AvailablePosition = FIntPoint::NoneValue;
	return CheckAvailablePosition(AvailablePosition, InInstance->GetItemGridSpan());
}

bool UObsidianInventoryComponent::CanReplaceItemAtSpecificSlotWithInstance(const FIntPoint& InClickedInstancePosition,
	const FIntPoint& InClickedGridPosition, UObsidianInventoryItemInstance* InReplacingInstance)
{
	const UObsidianInventoryItemInstance* InstanceAtLocation = GetItemInstanceAtLocation(InClickedInstancePosition);
	if (InstanceAtLocation == nullptr)
	{
		return false;
	}
	
	if(GetNumberOfStacksAvailableToAddToInventory(InReplacingInstance) <= 0)
	{
		//TODO(intrxx) Limit of stacks reached, add voiceover?
		return false;
	}
	
	return CheckReplacementPossible(InClickedInstancePosition, InClickedGridPosition,
		InstanceAtLocation->GetItemGridSpan(), InReplacingInstance->GetItemGridSpan());
}

bool UObsidianInventoryComponent::CanReplaceItemAtSpecificSlotWithDef(const FIntPoint& InClickedInstancePosition,
	const FIntPoint& InClickedGridPosition, const TSubclassOf<UObsidianInventoryItemDefinition> InItemDef,
	const int32 InStackCount)
{
	const UObsidianInventoryItemInstance* InstanceAtLocation = GetItemInstanceAtLocation(InClickedInstancePosition);
	if (InstanceAtLocation == nullptr)
	{
		return false;
	}
	
	if(GetNumberOfStacksAvailableToAddToInventory(InItemDef, InStackCount) <= 0)
	{
		//TODO(intrxx) Limit of stacks reached, add voiceover?
		return false;
	}
	
	if(const UObsidianInventoryItemDefinition* DefaultItem = InItemDef.GetDefaultObject())
	{
		if(const UOInventoryItemFragment_Appearance* Appearance = Cast<UOInventoryItemFragment_Appearance>(
			DefaultItem->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
		{
			return CheckReplacementPossible(InClickedInstancePosition, InClickedGridPosition,
				InstanceAtLocation->GetItemGridSpan(), Appearance->GetItemGridSpanFromDesc());
		}
	}

	return false;
}

bool UObsidianInventoryComponent::CanFitItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef)
{
	bool bCanFit = false;
	
	if(const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(InItemDef))
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



