// Copyright 2024 out of sCope team - Michał Ogiński


#include "InventoryItems/ObsidianInventoryComponent.h"
#include "Engine/ActorChannel.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Stacks.h"
#include "Net/UnrealNetwork.h"
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

TMap<FVector2D, UObsidianInventoryItemInstance*> UObsidianInventoryComponent::Internal_GetLocationToInstanceMap()
{
	return InventoryGrid.GridLocationToItemMap;
}

TMap<FVector2D, bool> UObsidianInventoryComponent::Internal_GetInventoryStateMap()
{
	return InventoryStateMap;
}

UObsidianInventoryItemInstance* UObsidianInventoryComponent::Internal_GetItemInstanceAtLocation(const FVector2D& Location) const
{
	const TMap<FVector2D, UObsidianInventoryItemInstance*> LocToItemMap = InventoryGrid.GridLocationToItemMap;
	if(LocToItemMap.Contains(Location))
	{
		return LocToItemMap[Location];
	}
	return nullptr;
}

TArray<UObsidianInventoryItemInstance*> UObsidianInventoryComponent::GetAllItems() const
{
	return InventoryGrid.GetAllItems();
}

UObsidianInventoryItemInstance* UObsidianInventoryComponent::FindFirstItemStackForDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef) const
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

bool UObsidianInventoryComponent::CanAddItemDefinition(FVector2D& OutAvailablePosition, const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, int32 StackCount)
{
	bool bCanAdd = false;
	
	if(const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef))
	{
		if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
		{
			const TArray<FVector2D> ItemGridSize = AppearanceFrag->GetItemGridSizeFromDesc();

			bCanAdd = CheckAvailablePosition(ItemGridSize, OutAvailablePosition);
			return bCanAdd;
		}
	}
	return bCanAdd;
}

bool UObsidianInventoryComponent::CanAddItemDefinitionToSpecifiedSlot(const FVector2D& SpecifiedSlot, const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, int32 StackCount)
{
	bool bCanAdd = false;
	
	if(const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef))
	{
		if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
		{
			const TArray<FVector2D> ItemGridSize = AppearanceFrag->GetItemGridSizeFromDesc();

			bCanAdd = CheckSpecifiedPosition(ItemGridSize, SpecifiedSlot);
			return bCanAdd;
		}
	}
	return bCanAdd;
}

UObsidianInventoryItemInstance* UObsidianInventoryComponent::AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, int32& OutStacksLeft, const int32 StackCount)
{
	OutStacksLeft = StackCount;
	
	if(ItemDef == nullptr)
	{
		return nullptr;
	}

	const UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject();
	if(DefaultObject == nullptr)
	{
		return nullptr;
	}

	TArray<UObsidianInventoryItemInstance*> AddedToInstances;
	if(DefaultObject->IsStackable())
	{
		AddedToInstances = TryAddingStacksToExistingItem(ItemDef, OutStacksLeft, OutStacksLeft);
		if(AddedToInstances.IsEmpty() == false)
		{
			TMap<FVector2D, int32> LocationToStacksMap;
			for(UObsidianInventoryItemInstance* AddedToInstance : AddedToInstances)
			{
				const FVector2D InstanceLocation = GetItemLocationFromGrid(AddedToInstance);
				const int32 NewStacks = AddedToInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
				LocationToStacksMap.Add(InstanceLocation, NewStacks);
			}
			OnItemsStacksChangedDelegate.Broadcast(LocationToStacksMap);
		}
		if(OutStacksLeft == 0)
		{
			return AddedToInstances.Last();
		}
	}
	
	const int32 StacksAvailableToAdd = GetNumberOfStacksAvailableToAdd(ItemDef, OutStacksLeft);
	if(StacksAvailableToAdd == 0)
	{
		//TODO We can no longer add this item to the inventory, add voice over?
		return AddedToInstances.Last();
	}
	
	FVector2D AvailablePosition;
	if(CanAddItemDefinition(AvailablePosition, ItemDef, StacksAvailableToAdd) == false)
	{
		//TODO Inventory is full, add voice over?
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta,
			FString::Printf(TEXT("Inventory is full!")));
		return nullptr;
	}
	
	OutStacksLeft -= StacksAvailableToAdd;
	
	UObsidianInventoryItemInstance* Instance = InventoryGrid.AddEntry(ItemDef, StacksAvailableToAdd, AvailablePosition);
	Instance->AddItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, StacksAvailableToAdd);
	Item_MarkSpace(Instance, AvailablePosition);
	
	if(Instance && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(Instance);
	}

	OnItemAddedToInventoryDelegate.Broadcast(Instance, AvailablePosition);
	
	return Instance;
}

UObsidianInventoryItemInstance* UObsidianInventoryComponent::AddItemDefinitionToSpecifiedSlot(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const FVector2D& ToSlot, const int32 StackCount)
{
	if(ItemDef == nullptr)
	{
		return nullptr;
	}

	const UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject();
	if(DefaultObject == nullptr)
	{
		return nullptr;
	}
	
	if(CanAddItemDefinitionToSpecifiedSlot(ToSlot, ItemDef, StackCount) == false)
	{
		//TODO Inventory is full, add voice over?
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta,
			FString::Printf(TEXT("Inventory is full at specified slot!")));
		return nullptr;
	}
	
	bool bAddedWholeItem = true;
	int32 StacksThatCanBeAdded = StackCount;
	if(DefaultObject->IsStackable())
	{
		StacksThatCanBeAdded = GetNumberOfStacksAvailableToAdd(ItemDef, StackCount);
		if(StacksThatCanBeAdded == 0)
		{
			return nullptr;
		}
		
		const int32 StacksToRemove = StackCount - StacksThatCanBeAdded;
		if(StacksToRemove > 0)
		{
			//TODO Update number of stacks on the carried item
			bAddedWholeItem = false;
		}
	}

	UObsidianInventoryItemInstance* Instance = InventoryGrid.AddEntry(ItemDef, StacksThatCanBeAdded, ToSlot);
	Instance->AddItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, StacksThatCanBeAdded);
	Item_MarkSpace(Instance, ToSlot);
	
	if(Instance && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(Instance);
	}

	OnItemAddedToInventoryDelegate.Broadcast(Instance, ToSlot);

	if(bAddedWholeItem == false)
	{
		//TODO Temp
		return nullptr;
	}
	return Instance;
}

bool UObsidianInventoryComponent::CanAddItemInstance(FVector2D& OutAvailablePosition, UObsidianInventoryItemInstance* Instance)
{
	const TArray<FVector2D> ItemGridSize = Instance->GetItemGridSize();
	
	const bool bCanAdd = CheckAvailablePosition(ItemGridSize, OutAvailablePosition);
	return bCanAdd;
}

bool UObsidianInventoryComponent::CanAddItemInstanceToSpecificSlot(const FVector2D& SpecifiedSlot, UObsidianInventoryItemInstance* Instance)
{
	const TArray<FVector2D> ItemGridSize = Instance->GetItemGridSize();
	
	const bool bCanAdd = CheckSpecifiedPosition(ItemGridSize, SpecifiedSlot);
	return bCanAdd;
}

void UObsidianInventoryComponent::AddItemInstance(UObsidianInventoryItemInstance* InstanceToAdd, int32& OutStacksLeft)
{
	if(InstanceToAdd == nullptr)
	{
		return;
	}

	OutStacksLeft = InstanceToAdd->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	if(InstanceToAdd->IsStackable())
	{
		TArray<UObsidianInventoryItemInstance*> AddedToInstances = TryAddingStacksToExistingItem(InstanceToAdd->GetItemDef(), OutStacksLeft, OutStacksLeft);
		if(AddedToInstances.IsEmpty() == false)
		{
			TMap<FVector2D, int32> LocationToStacksMap;
			for(UObsidianInventoryItemInstance* AddedToInstance : AddedToInstances)
			{
				const FVector2D InstanceLocation = GetItemLocationFromGrid(AddedToInstance);
				const int32 NewStacks = AddedToInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
				LocationToStacksMap.Add(InstanceLocation, NewStacks);
			}
			OnItemsStacksChangedDelegate.Broadcast(LocationToStacksMap);
		}
		if(OutStacksLeft == 0)
		{
			return;
		}
	}

	const int32 StacksAvailableToAdd = GetNumberOfStacksAvailableToAdd(InstanceToAdd);
	if(StacksAvailableToAdd == 0)
	{
		//TODO We can no longer add this item to the inventory, add voice over?
		return;
	}
	
	FVector2D AvailablePosition;
	if(CanAddItemInstance(AvailablePosition, InstanceToAdd) == false)
	{
		//TODO Inventory is full, add voice over?
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta,
			FString::Printf(TEXT("Inventory is full!")));
		return;
	}

	OutStacksLeft -= StacksAvailableToAdd;
	UObsidianInventoryItemInstance* NewInstance = nullptr;
	if(OutStacksLeft > 0)
	{
		NewInstance = UObsidianInventoryItemInstance::DuplicateItem(InstanceToAdd, GetOwner());
	}
	if(NewInstance != nullptr)
	{
		InstanceToAdd = NewInstance;
	}
	
	InstanceToAdd->OverrideItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, StacksAvailableToAdd);
	InventoryGrid.AddEntry(InstanceToAdd, AvailablePosition);
	Item_MarkSpace(InstanceToAdd, AvailablePosition);
	if(InstanceToAdd && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(InstanceToAdd);
	}

	OnItemAddedToInventoryDelegate.Broadcast(InstanceToAdd, AvailablePosition);
}

bool UObsidianInventoryComponent::AddItemInstanceToSpecificSlot(UObsidianInventoryItemInstance* InstanceToAdd, const FVector2D& ToSlot)
{
	if(InstanceToAdd == nullptr)
	{
		return false;
	}
	
	const int32 StacksAvailableToAdd = GetNumberOfStacksAvailableToAdd(InstanceToAdd);
	if(StacksAvailableToAdd == 0)
	{
		//TODO We can no longer add this item to the inventory, add voice over?
		return false;
	}
	
	if(CanAddItemInstanceToSpecificSlot(ToSlot, InstanceToAdd) == false)
	{
		//TODO Inventory is full, add voice over?
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta,
			FString::Printf(TEXT("Inventory is full at specified slot!")));
		return false;
	}

	bool bAddedWholeItem = true;
	int32 StacksLeft = InstanceToAdd->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	StacksLeft -= StacksAvailableToAdd;
	UObsidianInventoryItemInstance* NewInstance = nullptr;
	if(StacksLeft > 0)
	{
		bAddedWholeItem = false;
		InstanceToAdd->OverrideItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, StacksLeft);
		NewInstance = UObsidianInventoryItemInstance::DuplicateItem(InstanceToAdd, GetOwner());
	}
	if(NewInstance != nullptr)
	{
		InstanceToAdd = NewInstance;
	}

	InstanceToAdd->OverrideItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, StacksAvailableToAdd);
	InventoryGrid.AddEntry(InstanceToAdd, ToSlot);
	Item_MarkSpace(InstanceToAdd, ToSlot);
	
	if(InstanceToAdd && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(InstanceToAdd);
	}

	OnItemAddedToInventoryDelegate.Broadcast(InstanceToAdd, ToSlot);
	return bAddedWholeItem;
}

TArray<UObsidianInventoryItemInstance*> UObsidianInventoryComponent::TryAddingStacksToExistingItem(const TSubclassOf<UObsidianInventoryItemDefinition>& NewItemDef, const int32 NewItemStacks, int32& OutStacksLeft)
{
	int32 AddedStacks = 0;
	OutStacksLeft = NewItemStacks;
	const int32 StacksInInventory = FindAllStacksForGivenItem(NewItemDef);
	TArray<UObsidianInventoryItemInstance*> AddedToInstances;
	TArray<UObsidianInventoryItemInstance*> Items = InventoryGrid.GetAllItems();
	
	for(UObsidianInventoryItemInstance* Instance : Items)
	{
		if(!IsValid(Instance))
		{
			UE_LOG(LogInventory, Error, TEXT("Instance is invalid in UObsidianInventoryComponent::TryAddingStacksToExistingItem."));
			continue;
		}
		
		if(NewItemDef == Instance->GetItemDef())
		{
			UE_LOG(LogTemp, Warning, TEXT("Checking if can be added to already existing instance."));
			
			const int32 CurrentStackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
			if(CurrentStackCount == 0)
			{
				continue;
			}
			const int32 LimitStackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Limit);
			if((LimitStackCount == 1) || (LimitStackCount == StacksInInventory))
			{
				break;
			}
			
			const int32 StacksThatCanBeAddedToInventory = LimitStackCount == 0 ? OutStacksLeft : LimitStackCount - StacksInInventory;
			if(StacksThatCanBeAddedToInventory <= 0)
			{
				continue;
			}
			
			const int32 MaxStackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Max);
			int32 AmountThatCanBeAddedToInstance = FMath::Clamp((MaxStackCount - CurrentStackCount), 0, StacksThatCanBeAddedToInventory);
			AmountThatCanBeAddedToInstance = FMath::Min(AmountThatCanBeAddedToInstance, OutStacksLeft);
			if(AmountThatCanBeAddedToInstance <= 0)
			{
				continue;
			}
			UE_LOG(LogTemp, Warning, TEXT("Added [%d] stacks to [%s]."), AmountThatCanBeAddedToInstance, *GetNameSafe(Instance));
			
			Instance->AddItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, AmountThatCanBeAddedToInstance);
			AddedStacks += AmountThatCanBeAddedToInstance;
			OutStacksLeft -= AmountThatCanBeAddedToInstance;
			AddedToInstances.AddUnique(Instance);
			
			if((NewItemStacks != 0) && (AddedStacks == NewItemStacks))
			{
				return AddedToInstances;
			}
		}
	}
	return AddedToInstances;
}

UObsidianInventoryItemInstance* UObsidianInventoryComponent::TryAddingStacksToSpecificSlotWithItemDef(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const FVector2D& AtPosition, const int32 NewItemStacks, int32& OutStacksLeft, int32& OutStacksAdded)
{
	OutStacksLeft = NewItemStacks;
	
	UObsidianInventoryItemInstance* InstanceAtLocation = Internal_GetItemInstanceAtLocation(AtPosition);
	if(InstanceAtLocation == nullptr || InstanceAtLocation->GetItemDef().Get() != ItemDef.Get())
	{
		return nullptr;
	}
	
	const int32 CurrentStackCount = InstanceAtLocation->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	if(CurrentStackCount == 0)
	{
		return nullptr;
	}
	
	const int32 StacksInInventory = FindAllStacksForGivenItem(ItemDef);
	const int32 LimitStackCount = InstanceAtLocation->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Limit);
	if((LimitStackCount == 1) || (LimitStackCount == StacksInInventory))
	{
		return nullptr;
	}
			
	const int32 StacksThatCanBeAddedToInventory = LimitStackCount == 0 ? OutStacksLeft : LimitStackCount - StacksInInventory;
	if(StacksThatCanBeAddedToInventory <= 0)
	{
		return nullptr;
	}
			
	const int32 MaxStackCount = InstanceAtLocation->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Max);
	int32 AmountThatCanBeAddedToInstance = FMath::Clamp((MaxStackCount - CurrentStackCount), 0, StacksThatCanBeAddedToInventory);
	AmountThatCanBeAddedToInstance = FMath::Min(AmountThatCanBeAddedToInstance, OutStacksLeft);
	if(AmountThatCanBeAddedToInstance <= 0)
	{
		return nullptr;
	}
	UE_LOG(LogTemp, Warning, TEXT("Added [%d] stacks to [%s]."), AmountThatCanBeAddedToInstance, *GetNameSafe(InstanceAtLocation));
			
	InstanceAtLocation->AddItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, AmountThatCanBeAddedToInstance);
	OutStacksLeft -= AmountThatCanBeAddedToInstance;
	OutStacksAdded = AmountThatCanBeAddedToInstance;
	
	return InstanceAtLocation;
}

bool UObsidianInventoryComponent::TryAddingStacksToSpecificSlotWithInstance(UObsidianInventoryItemInstance* NewItemInstance, const FVector2D& AtPosition, int32& OutStacksLeft, int32& OutStacksAdded)
{
	const int32 NewItemCurrentStacks = NewItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	OutStacksLeft = NewItemCurrentStacks;
	
	UObsidianInventoryItemInstance* InstanceAtLocation = Internal_GetItemInstanceAtLocation(AtPosition);
	if(InstanceAtLocation == nullptr || InstanceAtLocation->GetItemDef().Get() != NewItemInstance->GetItemDef().Get())
	{
		return false;
	}
	
	const int32 CurrentStackCount = InstanceAtLocation->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	if(CurrentStackCount == 0)
	{
		return false;
	}

	const int32 StacksInInventory = FindAllStacksForGivenItem(NewItemInstance);
	const int32 LimitStackCount = InstanceAtLocation->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Limit);
	if((LimitStackCount == 1) || (LimitStackCount == StacksInInventory))
	{
		return false;
	}
			
	const int32 StacksThatCanBeAddedToInventory = LimitStackCount == 0 ? NewItemCurrentStacks : LimitStackCount - StacksInInventory;
	if(StacksThatCanBeAddedToInventory <= 0)
	{
		return false;
	}
			
	const int32 MaxStackCount = InstanceAtLocation->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Max);
	int32 AmountThatCanBeAddedToInstance = FMath::Clamp((MaxStackCount - CurrentStackCount), 0, StacksThatCanBeAddedToInventory);
	AmountThatCanBeAddedToInstance = FMath::Min(AmountThatCanBeAddedToInstance, NewItemCurrentStacks);
	if(AmountThatCanBeAddedToInstance <= 0)
	{
		return false;
	}
	UE_LOG(LogTemp, Warning, TEXT("Added [%d] stacks to [%s]."), AmountThatCanBeAddedToInstance, *GetNameSafe(InstanceAtLocation));
			
	InstanceAtLocation->AddItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, AmountThatCanBeAddedToInstance);
	NewItemInstance->RemoveItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, AmountThatCanBeAddedToInstance);
	OutStacksAdded = AmountThatCanBeAddedToInstance;
	OutStacksLeft -= AmountThatCanBeAddedToInstance;
	
	if(AmountThatCanBeAddedToInstance == NewItemCurrentStacks)
	{
		return true;
	}
	return false;
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

int32 UObsidianInventoryComponent::GetNumberOfStacksAvailableToAdd(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const int32 CurrentStacks)
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
	
	const int32 StacksLimit = StacksFrag->GetItemStackNumberByTag(ObsidianGameplayTags::Item_StackCount_Limit);
	if(StacksLimit == 0) // Item has no limit
	{
		return CurrentStacks;
	}
	const int32 AllStacks = FindAllStacksForGivenItem(ItemDef);
	return  FMath::Clamp(StacksLimit - AllStacks, 0, CurrentStacks);
}

int32 UObsidianInventoryComponent::GetNumberOfStacksAvailableToAdd(const UObsidianInventoryItemInstance* ItemInstance)
{
	const int32 CurrentStacks = ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	const int32 StacksLimit = ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Limit);
	if(StacksLimit == 0) // Item has no limit
	{
		return CurrentStacks;
	}
	
	const int32 AllStacks = FindAllStacksForGivenItem(ItemInstance);
	return  FMath::Clamp(StacksLimit - AllStacks, 0, CurrentStacks);
}

void UObsidianInventoryComponent::RemoveItemInstance(UObsidianInventoryItemInstance* InstanceToRemove)
{
	const FVector2D FromLocation = GetItemLocationFromGrid(InstanceToRemove);
	Item_UnMarkSpace(InstanceToRemove, FromLocation);
	InventoryGrid.RemoveEntry(InstanceToRemove);
	
	if(InstanceToRemove && IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(InstanceToRemove);
	}
}

bool UObsidianInventoryComponent::ConsumeItemsByDefinition(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const int32 NumberOfItemsToConsume)
{
	AActor* OwningActor = GetOwner();
	if(OwningActor == nullptr || OwningActor->HasAuthority() == false)
	{
		return false;
	}

	//TODO: N squared right now as there's no acceleration structure
	int32 TotalConsumed = 0;
	while(TotalConsumed < NumberOfItemsToConsume)
	{
		if(UObsidianInventoryItemInstance* Instance = FindFirstItemStackForDefinition(ItemDef))
		{
			InventoryGrid.RemoveEntry(Instance);
			++TotalConsumed;
		}
#if !UE_BUILD_SHIPPING
		else
		{
			FFrame::KismetExecutionMessage(TEXT("Provided NumberOfItemsToConsume is greater than the instances of the item, or the Instance is invalid"),
				ELogVerbosity::Warning);
			return false;
		}
#endif
	}
	return TotalConsumed == NumberOfItemsToConsume;
}

bool UObsidianInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething =  Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

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

FVector2D UObsidianInventoryComponent::GetItemLocationFromGrid(UObsidianInventoryItemInstance* ItemInstance) const
{
	return ItemInstance == nullptr ? FVector2D::Zero() : *InventoryGrid.GridLocationToItemMap.FindKey(ItemInstance);
}

void UObsidianInventoryComponent::InitInventoryState()
{
	int16 GridX = 0;
	int16 GridY = 0;
	
	for(int32 i = 0; i < InventoryGridSize; i++)
	{
		InventoryStateMap.Add(FVector2D(GridX, GridY), false);
		
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
	
	for(const TTuple<FVector2D, UObsidianInventoryItemInstance*>& ItemLoc : InventoryGrid.GridLocationToItemMap)
	{
		Item_MarkSpace(ItemLoc.Value, ItemLoc.Key);
	}
}

void UObsidianInventoryComponent::Item_MarkSpace(const UObsidianInventoryItemInstance* ItemInstance, const FVector2D AtPosition)
{
	const TArray<FVector2D> ItemGridSize = ItemInstance->GetItemGridSize();
	for(const FVector2D LocationComp : ItemGridSize)
	{
		const FVector2D Location = AtPosition + LocationComp;
		if(InventoryStateMap.Contains(Location))
		{
			InventoryStateMap[Location] = true;
		}
#if !UE_BUILD_SHIPPING
		else
		{
			FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Trying to Mark a Location [x: %f, y: %f] that doesn't"
			 "exist in the InventoryStateMap in UObsidianInventoryComponent::Item_MarkSpace."), Location.X, Location.Y), ELogVerbosity::Error);
		}
#endif
	}
}

void UObsidianInventoryComponent::Item_UnMarkSpace(const UObsidianInventoryItemInstance* ItemInstance, const FVector2D AtPosition)
{
	const TArray<FVector2D> ItemGridSize = ItemInstance->GetItemGridSize();
	for(const FVector2D LocationComp : ItemGridSize)
	{
		const FVector2D Location = AtPosition + LocationComp;
		if(InventoryStateMap.Contains(Location))
		{
			InventoryStateMap[Location] = false;
		}
#if !UE_BUILD_SHIPPING
		else
		{
			FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Trying to UnMark a Location [x: %f, y: %f] that doesn't"
			"exist in the InventoryStateMap in UObsidianInventoryComponent::Item_UnMarkSpace."), Location.X, Location.Y), ELogVerbosity::Error);
		}
#endif
	}
}

bool UObsidianInventoryComponent::CheckAvailablePosition(const TArray<FVector2D>& ItemGridSize, FVector2D& OutAvailablePosition)
{
	bool bCanFit = false;
	
	for(const TTuple<FVector2D, bool>& Location : InventoryStateMap)
	{
		if(Location.Value == false) // Location is free
		{
			bCanFit = true;
			for(FVector2D LocationComp : ItemGridSize)
			{
				const FVector2D Loc = Location.Key + LocationComp;
				if(!InventoryStateMap.Contains(Loc) || InventoryStateMap[Loc] == true)
				{
					bCanFit = false;
					break;
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

bool UObsidianInventoryComponent::CheckSpecifiedPosition(const TArray<FVector2D>& ItemGridSize, const FVector2D& SpecifiedPosition)
{
	bool bCanFit = false;
	
	if(InventoryStateMap[SpecifiedPosition] == false) // Initial location is free
	{
		bCanFit = true;
		for(FVector2D LocationComp : ItemGridSize)
		{
			const FVector2D Loc = SpecifiedPosition + LocationComp;
			if(!InventoryStateMap.Contains(Loc) || InventoryStateMap[Loc] == true)
			{
				bCanFit = false;
				break;
			}
		}
	}
	return bCanFit;
}

bool UObsidianInventoryComponent::CanReplaceItemAtSpecificSlotWithInstance(const FVector2D& Slot, UObsidianInventoryItemInstance* ReplacingInstance)
{
	if(GetNumberOfStacksAvailableToAdd(ReplacingInstance) <= 0)
	{
		//TODO Limit of stacks reached, add voiceover?
		return false;
	}
	
	UObsidianInventoryItemInstance* InstanceAtLocation = Internal_GetItemInstanceAtLocation(Slot);
	const FVector2D ItemOrigin = GetItemLocationFromGrid(InstanceAtLocation);
	const TArray<FVector2D> ItemGridSize = InstanceAtLocation->GetItemGridSize();
	
	TMap<FVector2D, bool> TempInventoryStateMap = InventoryStateMap;
	for(const FVector2D LocationComp : ItemGridSize)
	{
		const FVector2D Location = ItemOrigin + LocationComp;
		if(TempInventoryStateMap.Contains(Location))
		{
			TempInventoryStateMap[Location] = false;
		}
#if !UE_BUILD_SHIPPING
		else
		{
			FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Trying to UnMark a Location [x: %f, y: %f] that doesn't"
			"exist in the TempInventoryStateMap in UObsidianInventoryComponent::CanReplaceItemAtSpecificSlot."), Location.X, Location.Y), ELogVerbosity::Warning);
		}
#endif
	}
	
	const TArray<FVector2D> ReplacingItemGridSize = ReplacingInstance->GetItemGridSize();
	bool bCanReplace = false;
	if(TempInventoryStateMap[Slot] == false) // Initial location is free
	{
		bCanReplace = true;
		for(FVector2D LocationComp : ReplacingItemGridSize)
		{
			const FVector2D Loc = Slot + LocationComp;
			if(!TempInventoryStateMap.Contains(Loc) || TempInventoryStateMap[Loc] == true)
			{
				bCanReplace = false;
				break;
			}
		}
	}
	return bCanReplace;
}

bool UObsidianInventoryComponent::CanReplaceItemAtSpecificSlotWithDef(const FVector2D& Slot, const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const int32 StackCount)
{
	if(GetNumberOfStacksAvailableToAdd(ItemDef, StackCount) <= 0)
	{
		return false;
	}
	
	UObsidianInventoryItemInstance* InstanceAtLocation = Internal_GetItemInstanceAtLocation(Slot);
	const FVector2D ItemOrigin = GetItemLocationFromGrid(InstanceAtLocation);
	const TArray<FVector2D> ItemGridSize = InstanceAtLocation->GetItemGridSize();
	
	TMap<FVector2D, bool> TempInventoryStateMap = InventoryStateMap;
	for(const FVector2D LocationComp : ItemGridSize)
	{
		const FVector2D Location = ItemOrigin + LocationComp;
		if(TempInventoryStateMap.Contains(Location))
		{
			TempInventoryStateMap[Location] = false;
		}
#if !UE_BUILD_SHIPPING
		else
		{
			FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Trying to UnMark a Location [x: %f, y: %f] that doesn't"
			"exist in the TempInventoryStateMap in UObsidianInventoryComponent::CanReplaceItemAtSpecificSlot."), Location.X, Location.Y), ELogVerbosity::Warning);
		}
#endif
	}
	bool bCanReplace = false;
	
	const UObsidianInventoryItemDefinition* DefaultItem = ItemDef.GetDefaultObject();
	if(ensureMsgf(DefaultItem, TEXT("Item Default could not be extracted from provided Item Def in UObsidianInventoryComponent::CanReplaceItemAtSpecificSlotWithDef")))
	{
		const UOInventoryItemFragment_Appearance* Appearance = Cast<UOInventoryItemFragment_Appearance>(DefaultItem->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass()));
		if(ensureMsgf(Appearance, TEXT("Appearance fragment does not exist on provided Item Def in UObsidianInventoryComponent::CanReplaceItemAtSpecificSlotWithDef")))
		{
			TArray<FVector2D> ReplacingItemGridSize = Appearance->GetItemGridSizeFromDesc();

			if(TempInventoryStateMap[Slot] == false) // Initial location is free
			{
				bCanReplace = true;
				for(FVector2D LocationComp : ReplacingItemGridSize)
				{
					const FVector2D Loc = Slot + LocationComp;
					if(!TempInventoryStateMap.Contains(Loc) || TempInventoryStateMap[Loc] == true)
					{
						bCanReplace = false;
						break;
					}
				}
			}
		}
	}
	return bCanReplace;
}


