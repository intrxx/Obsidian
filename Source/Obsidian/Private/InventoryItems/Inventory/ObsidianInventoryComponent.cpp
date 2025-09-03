// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"

// ~ Core
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

// ~ Project
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianItemsFunctionLibrary.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Stacks.h"
#include "InventoryItems/PlayerStash/ObsidianPlayerStashComponent.h"
#include "Obsidian/ObsidianGameplayTags.h"

DEFINE_LOG_CATEGORY(LogInventory);

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
}

void UObsidianInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	AddDefaultItems();
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
				if(!AddItemDefinitionToSpecifiedSlot(DefaultItemTemplate.DefaultItemDef, DefaultItemTemplate.InventoryPositionOverride, DefaultItemTemplate.StackCount))
				{
					AddItemDefinition(DefaultItemTemplate.DefaultItemDef, DefaultItemTemplate.StackCount);
				}
				continue;
			}
			AddItemDefinition(DefaultItemTemplate.DefaultItemDef, DefaultItemTemplate.StackCount);
		}
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

UObsidianInventoryItemInstance* UObsidianInventoryComponent::FindFirstItemInstanceForDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef) const
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
	if(CachedOwnerPlayerController == nullptr)
	{
		CachedOwnerPlayerController = Cast<AObsidianPlayerController>(GetOwner());
	}
	if(const UObsidianAbilitySystemComponent* OwnerASC = CachedOwnerPlayerController->GetObsidianAbilitySystemComponent())
	{
		return !OwnerASC->HasMatchingGameplayTag(ObsidianGameplayTags::Inventory_BlockActions);
	}
	return false;
}

bool UObsidianInventoryComponent::CanFitItemDefinition(FIntPoint& OutAvailablePositions, const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef)
{
	if(const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef))
	{
		if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
		{
			return CheckAvailablePosition(OutAvailablePositions, AppearanceFrag->GetItemGridSpanFromDesc());
		}
	}
	return false;
}

bool UObsidianInventoryComponent::CanFitItemDefinitionToSpecifiedSlot(const FIntPoint& SpecifiedSlot, const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef)
{
	if(const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef))
	{
		if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
		{
			return CheckSpecifiedPosition(AppearanceFrag->GetItemGridSpanFromDesc(), SpecifiedSlot);
		}
	}
	return false;
}

bool UObsidianInventoryComponent::CheckReplacementPossible(const FIntPoint& AtGridSlot, const FIntPoint& GridSpanAtPosition, const FIntPoint& ReplacingGridSpan) const
{
	TMap<FIntPoint, bool> TempInventoryStateMap = InventoryGrid.InventoryStateMap;
	
	for(int32 SpanX = 0; SpanX < GridSpanAtPosition.X; ++SpanX)
	{
		for(int32 SpanY = 0; SpanY < GridSpanAtPosition.Y; ++SpanY)
		{
			const FIntPoint GridSlotToCheck = AtGridSlot + FIntPoint(SpanX, SpanY);
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
	if(TempInventoryStateMap[AtGridSlot] == false) // Initial location is free
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

FObsidianItemOperationResult UObsidianInventoryComponent::AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const int32 StackCount)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();
	Result.StacksLeft = StackCount;
	
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::AddItemDefinition."));
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

	//TODO Shouldn't this be in the aboves if?
	const int32 StacksAvailableToAdd = GetNumberOfStacksAvailableToAddToInventory(ItemDef, Result.StacksLeft);
	if(StacksAvailableToAdd == 0)
	{
		//TODO We can no longer add this item to the inventory, add voice over?
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta,FString::Printf(TEXT("Can no longer add this item to inventory!")));
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
		//TODO Inventory is full, add voice over?
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta,FString::Printf(TEXT("Inventory is full!")));
		if(!OutAddedToInstances.IsEmpty())
		{
			Result.bActionSuccessful = true;
			Result.AffectedInstance = OutAddedToInstances.Last();
			return Result;
		}
		return Result;
	}
	
	Result.StacksLeft -= StacksAvailableToAdd;
	
	UObsidianInventoryItemInstance* Instance = InventoryGrid.AddEntry(ItemDef, StacksAvailableToAdd, AvailablePosition);
	Instance->AddItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, StacksAvailableToAdd);
	
	if(Instance && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(Instance);
	}

	Result.bActionSuccessful = true;
	Result.AffectedInstance = Instance;
	return Result;
}

FObsidianItemOperationResult UObsidianInventoryComponent::AddItemDefinitionToSpecifiedSlot(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const FIntPoint& ToGridSlot, const int32 StackCount, const int32 StackToAddOverride)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();
	Result.StacksLeft = StackCount;
	
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::AddItemDefinitionToSpecifiedSlot."));
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
		StacksAvailableToAdd = GetNumberOfStacksAvailableToAddToInventory(ItemDef, StackCount);
		if(StacksAvailableToAdd == 0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta,FString::Printf(TEXT("Can no longer add this item to inventory!")));
			return Result;
		}

		if(StackToAddOverride != -1)
		{
			StacksAvailableToAdd = FMath::Clamp<int32>((FMath::Min<int32>(StacksAvailableToAdd, StackToAddOverride)),
				1, StacksAvailableToAdd);
		}
	}
	
	if(CanFitItemDefinitionToSpecifiedSlot(ToGridSlot, ItemDef) == false)
	{
		//TODO Inventory is full, add voice over?
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("Inventory is full at specified slot!")));
		return Result;
	}
	
	Result.StacksLeft -= StacksAvailableToAdd;
	Result.bActionSuccessful = Result.StacksLeft == 0; // We don't need any item duplication logic (like in AddItemInstanceToSpecificSlot)
														// as we still have a valid item definition in hands if the whole item isn't added here.
	ensure(Result.StacksLeft >= 0);
	
	UObsidianInventoryItemInstance* Instance = InventoryGrid.AddEntry(ItemDef, StacksAvailableToAdd, ToGridSlot);
	Instance->AddItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, StacksAvailableToAdd);
	
	if(Instance && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(Instance);
	}

	Result.AffectedInstance = Instance;
	return Result;
}

FObsidianItemOperationResult UObsidianInventoryComponent::AddItemInstance(UObsidianInventoryItemInstance* InstanceToAdd)
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
	
	//TODO Shouldn't this be in the aboves if?
	const int32 StacksAvailableToAdd = GetNumberOfStacksAvailableToAddToInventory(InstanceToAdd);
	if(StacksAvailableToAdd == 0)
	{
		//TODO We can no longer add this item to the inventory, add voice over?
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta,FString::Printf(TEXT("Can no longer add this item to inventory!")));
		return Result;
	}
	
	FIntPoint AvailablePosition;
	if(CheckAvailablePosition(AvailablePosition, InstanceToAdd->GetItemGridSpan()) == false)
	{
		//TODO Inventory is full, add voice over?
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("Inventory is full!")));
		return Result;
	}

	Result.StacksLeft -= StacksAvailableToAdd;
	if(Result.StacksLeft > 0)
	{
		if(UObsidianInventoryItemInstance* NewInstance = UObsidianInventoryItemInstance::DuplicateItem(InstanceToAdd, GetOwner()))
		{
			InstanceToAdd = NewInstance;
		}
	}
	
	InstanceToAdd->OverrideItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, StacksAvailableToAdd);
	InventoryGrid.AddEntry(InstanceToAdd, AvailablePosition);
	
	if(InstanceToAdd && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(InstanceToAdd);
	}
	
	Result.bActionSuccessful = true;
	Result.AffectedInstance = InstanceToAdd;
	return Result;
}

FObsidianItemOperationResult UObsidianInventoryComponent::AddItemInstanceToSpecificSlot(UObsidianInventoryItemInstance* InstanceToAdd, const FIntPoint& ToGridSlot, const int32 StackToAddOverride)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();
	
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::AddItemInstanceToSpecificSlot."));
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
			//TODO We can no longer add this item to the inventory, add voice over?
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta,FString::Printf(TEXT("Can no longer add this item to inventory!")));
			return Result;
		}
		
		if(StackToAddOverride != INDEX_NONE)
		{
			StacksAvailableToAdd = FMath::Clamp<int32>((FMath::Min<int32>(StacksAvailableToAdd, StackToAddOverride)),
				1, StacksAvailableToAdd);
		}
	}
	
	if(CheckSpecifiedPosition(InstanceToAdd->GetItemGridSpan(), ToGridSlot) == false)
	{
		//TODO Inventory is full, add voice over?
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("Inventory is full at specified slot!")));
		return Result;
	}

	Result.bActionSuccessful = true;
	Result.StacksLeft -= StacksAvailableToAdd;
	ensure(Result.StacksLeft >= 0);
	
	if(Result.StacksLeft > 0)
	{
		Result.bActionSuccessful = false;
		InstanceToAdd->OverrideItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, Result.StacksLeft);
		
		if(UObsidianInventoryItemInstance* NewInstance  = UObsidianInventoryItemInstance::DuplicateItem(InstanceToAdd, GetOwner()))
		{
			InstanceToAdd = NewInstance;
		}
	}
	
	InstanceToAdd->OverrideItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, StacksAvailableToAdd);
	InventoryGrid.AddEntry(InstanceToAdd, ToGridSlot);
	
	if(InstanceToAdd && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(InstanceToAdd);
	}
	
	Result.AffectedInstance = InstanceToAdd;
	return Result;
}

FObsidianItemOperationResult UObsidianInventoryComponent::TakeOutFromItemInstance(UObsidianInventoryItemInstance* TakingFromInstance, const int32 StacksToTake)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();
	
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::TakeOutFromItemInstance."));
		return Result; 
	}

	if(CanOwnerModifyInventoryState() == false)
	{
		return Result;
	}
	
	const int32 CurrentTakingFromInstanceStacks = TakingFromInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	if(!ensureMsgf(((StacksToTake == 0) || (CurrentTakingFromInstanceStacks != StacksToTake)), TEXT("This function shouldn't be called if you want to take the whole item out. Simply Pickup the item instead.")))
	{
		return Result;
	}
	
	UObsidianInventoryItemInstance* NewInstance = UObsidianInventoryItemInstance::DuplicateItem(TakingFromInstance, GetOwner());
	if(NewInstance == nullptr)
	{
		return Result;
	}
	
	const int32 NewCurrentTakingFromInstanceStacks = CurrentTakingFromInstanceStacks - StacksToTake;
	TakingFromInstance->OverrideItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, NewCurrentTakingFromInstanceStacks);
	InventoryGrid.ChangedEntryStacks(TakingFromInstance, CurrentTakingFromInstanceStacks);

	// Since the only valid number of stacks to take is in range [1, x - 1] we can clamp it for extra safety.
	const int32 StackToTakeSafe = FMath::Clamp<int32>(StacksToTake, 1, CurrentTakingFromInstanceStacks - 1);
	NewInstance->OverrideItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, StackToTakeSafe);

	Result.bActionSuccessful = true;
	Result.AffectedInstance = NewInstance;
	return Result;
}

FObsidianAddingStacksResult UObsidianInventoryComponent::TryAddingStacksToExistingItems(const TSubclassOf<UObsidianInventoryItemDefinition>& AddingFromItemDef, const int32 StacksToAdd, TArray<UObsidianInventoryItemInstance*>& OutAddedToInstances)
{
	FObsidianAddingStacksResult Result = FObsidianAddingStacksResult();
	Result.AddedStacks = 0;
	Result.StacksLeft = StacksToAdd;
	
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::TryAddingStacksToExistingItems."));
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
	
	const int32 StacksInInventory = FindAllStacksForGivenItem(AddingFromItemDef);
	
	TArray<UObsidianInventoryItemInstance*> Items = InventoryGrid.GetAllItems();
	for(UObsidianInventoryItemInstance* Instance : Items)
	{
		if(!IsValid(Instance))
		{
			UE_LOG(LogInventory, Error, TEXT("Instance is invalid in UObsidianInventoryComponent::TryAddingStacksToExistingItem."));
			continue;
		}
		
		if(AddingFromItemDef == Instance->GetItemDef())
		{
			const int32 LimitStackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Limit);
			if((LimitStackCount == 1) || (LimitStackCount == StacksInInventory))
			{
				break;
			}
			
			const int32 CurrentStackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
			if(CurrentStackCount == 0)
			{
				continue;
			}

			const int32 StacksLeft = Result.StacksLeft;
			const int32 StacksThatCanBeAddedToInventory = LimitStackCount == 0 ? StacksLeft : LimitStackCount - StacksInInventory;
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
			UE_LOG(LogTemp, Warning, TEXT("Added [%d] stacks to [%s]."), AmountThatCanBeAddedToInstance, *GetNameSafe(Instance));
			
			Instance->AddItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, AmountThatCanBeAddedToInstance);
			InventoryGrid.ChangedEntryStacks(Instance, CurrentStackCount);
			
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

FObsidianAddingStacksResult UObsidianInventoryComponent::TryAddingStacksToSpecificSlotWithItemDef(const TSubclassOf<UObsidianInventoryItemDefinition>& AddingFromItemDef, const int32 AddingFromItemDefCurrentStacks, const FIntPoint& AtGridSlot, const int32 StackToAddOverride)
{
	FObsidianAddingStacksResult Result = FObsidianAddingStacksResult();
	Result.StacksLeft = AddingFromItemDefCurrentStacks;
	
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::TryAddingStacksToSpecificSlotWithItemDef."));
		return Result; 
	}

	if(CanOwnerModifyInventoryState() == false)
	{
		return Result; 
	}
	
	UObsidianInventoryItemInstance* InstanceToAddTo = GetItemInstanceAtLocation(AtGridSlot);
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
	InventoryGrid.ChangedEntryStacks(InstanceToAddTo, OldStackCount);

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

FObsidianAddingStacksResult UObsidianInventoryComponent::TryAddingStacksToSpecificSlotWithInstance(UObsidianInventoryItemInstance* AddingFromInstance, const FIntPoint& AtGridSlot, const int32 StackToAddOverride)
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
	
	if(CanOwnerModifyInventoryState() == false)
	{
		return Result;
	}
	
	UObsidianInventoryItemInstance* InstanceToAddTo = GetItemInstanceAtLocation(AtGridSlot);
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
	InventoryGrid.ChangedEntryStacks(InstanceToAddTo, OldStackCount);
	
	AddingFromInstance->RemoveItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, AmountThatCanBeAddedToInstance);
	// We should not call the InventoryGrid.ChangedEntryStacks function as the AddingFromInstance is not part of the inventory anyway
	
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

int32 UObsidianInventoryComponent::FindAllStacksForGivenItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef)
{
	int32 AllStacks = 0;
	for(const FObsidianInventoryEntry& Entry : InventoryGrid.Entries)
	{
		UObsidianInventoryItemInstance* Instance = Entry.Instance;
		if(IsValid(Instance))
		{
			if(Instance->GetItemDef() == ItemDef)
			{
				AllStacks += Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
			}
		}
	}
	return AllStacks;
}

int32 UObsidianInventoryComponent::FindAllStacksForGivenItem(const UObsidianInventoryItemInstance* ItemInstance)
{
	int32 AllStacks = 0;
	const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = ItemInstance->GetItemDef();
	for(const FObsidianInventoryEntry& Entry : InventoryGrid.Entries)
	{
		UObsidianInventoryItemInstance* Instance = Entry.Instance;
		if(IsValid(Instance))
		{
			if(Instance->GetItemDef() == ItemDef)
			{
				AllStacks += Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
			}
		}
	}
	return AllStacks;
}

int32 UObsidianInventoryComponent::GetNumberOfStacksAvailableToAddToInventory(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const int32 CurrentStacks)
{
	const UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject();
	if(DefaultObject == nullptr)
	{
		return 0;
	}
	
	const UOInventoryItemFragment_Stacks* StacksFrag = Cast<UOInventoryItemFragment_Stacks>(DefaultObject->FindFragmentByClass(UOInventoryItemFragment_Stacks::StaticClass()));
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
	if(UObsidianPlayerStashComponent* PlayerStashComponent = UObsidianPlayerStashComponent::FindPlayerStashComponent(GetOwner()))
	{
		AllStacksInStash = PlayerStashComponent->FindAllStacksForGivenItem(ItemDef);
	}
	
	const int32 AllStacksInInventory = FindAllStacksForGivenItem(ItemDef);
	const int32 CombinedStacks = AllStacksInInventory + AllStacksInStash;
	checkf(CombinedStacks <= LimitStackCount, TEXT("Combined Stacks of held item is already bigger than Stacks Limit for this item, something went wrong."));
	
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

	//TODO Rethink it, I kinda dont want to limit items in stash
	int32 AllStacksInStash = 0;
	if(UObsidianPlayerStashComponent* PlayerStashComponent = UObsidianPlayerStashComponent::FindPlayerStashComponent(GetOwner()))
	{
		AllStacksInStash = PlayerStashComponent->FindAllStacksForGivenItem(ItemInstance);
	}
	
	const int32 AllStacksInInventory = FindAllStacksForGivenItem(ItemInstance);
	const int32 CombinedStacks = AllStacksInInventory + AllStacksInStash;
	checkf(CombinedStacks <= LimitStackCount, TEXT("Combined Stacks of held item is already bigger than Stacks Limit for this item, something went wrong."));
	
	return  FMath::Clamp(LimitStackCount - CombinedStacks, 0, CurrentStacks);
}

FObsidianItemOperationResult UObsidianInventoryComponent::RemoveItemInstance(UObsidianInventoryItemInstance* InstanceToRemove)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();
	
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::RemoveItemInstance."));
		return Result; 
	}
	
	if(CanOwnerModifyInventoryState() == false)
	{
		return Result;
	}

	if(InstanceToRemove == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Passed InstanceToRemove is invalid in UObsidianInventoryComponent::RemoveItemInstance."));
		return Result;
	}
	
	InventoryGrid.RemoveEntry(InstanceToRemove);
	
	if(InstanceToRemove && IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(InstanceToRemove);
	}

	Result.bActionSuccessful = true;
	Result.AffectedInstance = InstanceToRemove;
	return Result;
}

void UObsidianInventoryComponent::UseItem(UObsidianInventoryItemInstance* UsingInstance, UObsidianInventoryItemInstance* UsingOntoInstance)
{
	if(UsingInstance == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("UsingInstance is invalid in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
		return;
	}

	if(UsingInstance->IsItemUsable() == false)
	{
		UE_LOG(LogInventory, Error, TEXT("Trying to use unusable Item [%s] in [%hs]"), *UsingInstance->GetItemDebugName(), ANSI_TO_TCHAR(__FUNCTION__));
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
		UE_LOG(LogInventory, Error, TEXT("Trying to use Item [%s] that has no more stacks in [%hs]"), *UsingInstance->GetItemDebugName(), ANSI_TO_TCHAR(__FUNCTION__));
		return;
	}

	bool bUsageSuccessful = false;
	const EObsidianUsableItemType ItemType = UsingInstance->GetUsableItemType();
	if(ItemType == EObsidianUsableItemType::UIT_Crafting)
	{
		if(UsingOntoInstance == nullptr)
		{
			UE_LOG(LogInventory, Error, TEXT("UsingOntoInstance is invalid in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
			return;
		}
		
		if(UsingInstance->UseItem(OwningPlayerController, UsingOntoInstance))
		{
			InventoryGrid.GeneralEntryChange(UsingOntoInstance);
			bUsageSuccessful = true;
		}
	}
	else if(ItemType == EObsidianUsableItemType::UIT_Activation)
	{
		bUsageSuccessful = UsingInstance->UseItem(OwningPlayerController, nullptr);
	}

	if(bUsageSuccessful)
	{
		if(CurrentUsingInstanceStacks > 1)
		{
			UsingInstance->RemoveItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, 1);
			InventoryGrid.ChangedEntryStacks(UsingInstance, CurrentUsingInstanceStacks);
			return;
		}
	
		InventoryGrid.RemoveEntry(UsingInstance);

		if(UsingInstance && IsUsingRegisteredSubObjectList())
		{
			RemoveReplicatedSubObject(UsingInstance);
		}
	}
	else
	{
		//TODO Usage failed, Play some VO?
	}
}

bool UObsidianInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for(const FObsidianInventoryEntry& Entry : InventoryGrid.Entries)
	{
		UObsidianInventoryItemInstance* Instance = Entry.Instance;

		if(Instance && IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void UObsidianInventoryComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing UObsidianInventoryItemInstance
	if(IsUsingRegisteredSubObjectList())
	{
		for(const FObsidianInventoryEntry& Entry : InventoryGrid.Entries)
		{
			UObsidianInventoryItemInstance* Instance = Entry.Instance;

			if(IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

FIntPoint UObsidianInventoryComponent::GetItemLocationFromGrid(UObsidianInventoryItemInstance* ItemInstance) const
{
	return ItemInstance == nullptr ? FIntPoint::NoneValue : *InventoryGrid.GridLocationToItemMap.FindKey(ItemInstance);
}

bool UObsidianInventoryComponent::IsLocallyControlled()
{
	if(bIsLocallyControlled)
	{
		return true;
	}
	
	if(AActor* Owner = GetOwner())
	{
		if(AController* Controller = Cast<AController>(Owner))
		{
			bIsLocallyControlled = Controller->IsLocalController();
			return bIsLocallyControlled;
		}
	}
	return false;
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
	
	if(InventoryGrid.InventoryStateMap[SpecifiedPosition] == false) // Initial location is free
	{
		bCanFit = true;
		for(int32 SpanX = 0; SpanX < ItemGridSpan.X; ++SpanX)
		{
			for(int32 SpanY = 0; SpanY < ItemGridSpan.Y; ++SpanY)
			{
				const FIntPoint LocationToCheck = SpecifiedPosition + FIntPoint(SpanX, SpanY);
				const bool* bExistingOccupied = InventoryGrid.InventoryStateMap.Find(LocationToCheck);
				if(bExistingOccupied == nullptr || *bExistingOccupied)
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

bool UObsidianInventoryComponent::CanReplaceItemAtSpecificSlotWithInstance(const FIntPoint& AtGridSlot, UObsidianInventoryItemInstance* ReplacingInstance)
{
	const UObsidianInventoryItemInstance* InstanceAtLocation = GetItemInstanceAtLocation(AtGridSlot);
	if (InstanceAtLocation == nullptr)
	{
		return false;
	}
	
	if(GetNumberOfStacksAvailableToAddToInventory(ReplacingInstance) <= 0)
	{
		//TODO Limit of stacks reached, add voiceover?
		return false;
	}
	
	return CheckReplacementPossible(AtGridSlot, InstanceAtLocation->GetItemGridSpan(), ReplacingInstance->GetItemGridSpan());
}

bool UObsidianInventoryComponent::CanReplaceItemAtSpecificSlotWithDef(const FIntPoint& AtGridSlot, const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const int32 StackCount)
{
	const UObsidianInventoryItemInstance* InstanceAtLocation = GetItemInstanceAtLocation(AtGridSlot);
	if (InstanceAtLocation == nullptr)
	{
		return false;
	}
	
	if(GetNumberOfStacksAvailableToAddToInventory(ItemDef, StackCount) <= 0)
	{
		//TODO Limit of stacks reached, add voiceover?
		return false;
	}
	
	if(const UObsidianInventoryItemDefinition* DefaultItem = ItemDef.GetDefaultObject())
	{
		if(const UOInventoryItemFragment_Appearance* Appearance = Cast<UOInventoryItemFragment_Appearance>(DefaultItem->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
		{
			return CheckReplacementPossible(AtGridSlot, InstanceAtLocation->GetItemGridSpan(), Appearance->GetItemGridSpanFromDesc());
		}
	}

	return false;
}

bool UObsidianInventoryComponent::CanFitItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef)
{
	bool bCanFit = false;
	
	if(const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef))
	{
		if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
		{
			const FIntPoint ItemGridSpan = AppearanceFrag->GetItemGridSpanFromDesc();

			FIntPoint AvailablePosition = FIntPoint::NoneValue;
			bCanFit = CheckAvailablePosition(AvailablePosition, ItemGridSpan);
			return bCanFit;
		}
	}
	return bCanFit;
}



