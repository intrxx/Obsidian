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

FObsidianItemStats UObsidianInventoryComponent::GetItemStatsByInventoryPosition(const FVector2D& InPosition) const
{
	const UObsidianInventoryItemInstance* ItemInstance = Internal_GetItemInstanceAtLocation(InPosition);
	check(ItemInstance);

	FObsidianItemStats Stats;
	
	if(ItemInstance->IsStackable())
	{
		Stats.SetStacks(ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current),
				ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Max));
	}

	Stats.SetItemImage(ItemInstance->GetItemImage());
	Stats.SetDisplayName(ItemInstance->GetItemDisplayName());
	Stats.SetDescription(ItemInstance->GetItemDescription());
	Stats.SetAdditionalDescription(ItemInstance->GetItemAdditionalDescription());

	return Stats;
}

FObsidianItemStats UObsidianInventoryComponent::GetItemStatForInstance(const UObsidianInventoryItemInstance* ItemInstance) const
{
	if(ItemInstance == nullptr)
	{
		return FObsidianItemStats();
	}
	
	FObsidianItemStats Stats;
	
	if(ItemInstance->IsStackable())
	{
		Stats.SetStacks(ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current),
			 ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Max));
	}

	Stats.SetItemImage(ItemInstance->GetItemImage());
	Stats.SetDisplayName(ItemInstance->GetItemDisplayName());
	Stats.SetDescription(ItemInstance->GetItemDescription());
	Stats.SetAdditionalDescription(ItemInstance->GetItemAdditionalDescription());

	return Stats;
}

FObsidianItemStats UObsidianInventoryComponent::GetItemStatsForItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const int32 CurrentItemStacks) const
{
	if(!IsValid(ItemDef))
	{
		return FObsidianItemStats();
	}

	const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef);
	if(ItemDefault == nullptr)
	{
		return FObsidianItemStats();
	}

	FObsidianItemStats Stats;

	if(ItemDefault->IsStackable())
	{
		if(const UOInventoryItemFragment_Stacks* StacksFrag = Cast<UOInventoryItemFragment_Stacks>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Stacks::StaticClass())))
		{
			Stats.SetStacks(CurrentItemStacks, // Current Item Stacks are not present on ItemDef, they are directly on Pickable Item
				 StacksFrag->GetItemStackNumberByTag(ObsidianGameplayTags::Item_StackCount_Max));
		}
	}

	if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
	{
		Stats.SetItemImage(AppearanceFrag->GetItemImage());
		Stats.SetDisplayName(AppearanceFrag->GetItemDisplayName());
		Stats.SetDescription(AppearanceFrag->GetItemDescription());
		Stats.SetAdditionalDescription(AppearanceFrag->GetItemAdditionalDescription());
	}

	return Stats;
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

bool UObsidianInventoryComponent::CanFitItemDefinition(FVector2D& OutAvailablePosition, const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef)
{
	bool bCanFit = false;
	
	if(const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef))
	{
		if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
		{
			const TArray<FVector2D> ItemGridSize = AppearanceFrag->GetItemGridSizeFromDesc();

			bCanFit = CheckAvailablePosition(ItemGridSize, OutAvailablePosition);
			return bCanFit;
		}
	}
	return bCanFit;
}

bool UObsidianInventoryComponent::CanFitItemDefinitionToSpecifiedSlot(const FVector2D& SpecifiedSlot, const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef)
{
	bool bCanFit = false;
	
	if(const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef))
	{
		if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
		{
			const TArray<FVector2D> ItemGridSize = AppearanceFrag->GetItemGridSizeFromDesc();

			bCanFit = CheckSpecifiedPosition(ItemGridSize, SpecifiedSlot);
			return bCanFit;
		}
	}
	return bCanFit;
}

UObsidianInventoryItemInstance* UObsidianInventoryComponent::AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, int32& OutStacksLeft, const int32 StackCount)
{
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::AddItemDefinition."));
		return nullptr; 
	}
	
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
		FObsidianAddingStacksResult AddingStacksResult;
		AddedToInstances = TryAddingStacksToExistingItems(ItemDef, OutStacksLeft, /** OUT */ AddingStacksResult);
		OutStacksLeft = AddingStacksResult.StacksLeft;
		
		if(AddingStacksResult.bAddedSomeOfTheStacks)
		{
			BroadcastVisualStacksUpdate(AddedToInstances);
		}
		if(AddingStacksResult.bAddedWholeItemAsStacks)
		{
			return AddedToInstances.Last();
		}
	}
	
	const int32 StacksAvailableToAdd = GetNumberOfStacksAvailableToAddToInventory(ItemDef, OutStacksLeft);
	if(StacksAvailableToAdd == 0)
	{
		//TODO We can no longer add this item to the inventory, add voice over?
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta,FString::Printf(TEXT("Can no longer add this item to inventory!")));
		if(!AddedToInstances.IsEmpty())
		{
			return AddedToInstances.Last();
		}
		return nullptr;
	}
	
	FVector2D AvailablePosition;
	if(CanFitItemDefinition(AvailablePosition, ItemDef) == false)
	{
		//TODO Inventory is full, add voice over?
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta,FString::Printf(TEXT("Inventory is full!")));
		if(!AddedToInstances.IsEmpty())
		{
			return AddedToInstances.Last();
		}
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

UObsidianInventoryItemInstance* UObsidianInventoryComponent::AddItemDefinitionToSpecifiedSlot(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const FVector2D& ToSlot, int32& StacksLeft, const int32 StackCount, const int32 StackToAddOverride)
{
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::AddItemDefinitionToSpecifiedSlot."));
		return nullptr; 
	}
	
	StacksLeft = StackCount;
	
	if(ItemDef == nullptr)
	{
		return nullptr;
	}
	
	const UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject();
	if(DefaultObject == nullptr)
	{
		return nullptr;
	}

	int32 StacksAvailableToAdd = 1;
	const bool bStackable = DefaultObject->IsStackable();
	if(bStackable)
	{
		StacksAvailableToAdd = GetNumberOfStacksAvailableToAddToInventory(ItemDef, StackCount);
		if(StacksAvailableToAdd == 0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta,FString::Printf(TEXT("Can no longer add this item to inventory!")));
			return nullptr;
		}

		if(StackToAddOverride != -1)
		{
			StacksAvailableToAdd = FMath::Clamp<int32>((FMath::Min<int32>(StacksAvailableToAdd, StackToAddOverride)),
				1, StacksAvailableToAdd);
		}
		StacksLeft = StackCount - StacksAvailableToAdd;
	}
	
	if(CanFitItemDefinitionToSpecifiedSlot(ToSlot, ItemDef) == false)
	{
		//TODO Inventory is full, add voice over?
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("Inventory is full at specified slot!")));
		return nullptr;
	}
	
	UObsidianInventoryItemInstance* Instance = InventoryGrid.AddEntry(ItemDef, StacksAvailableToAdd, ToSlot);
	Instance->AddItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, StacksAvailableToAdd);
	Item_MarkSpace(Instance, ToSlot);
	if(!bStackable)
	{
		StacksLeft = 0; //@Hack that's little bit of a hack unfortunately, I need this to remove the item from hand
	}
	
	if(Instance && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(Instance);
	}

	OnItemAddedToInventoryDelegate.Broadcast(Instance, ToSlot);
	
	return Instance;
}

bool UObsidianInventoryComponent::CanFitItemInstance(FVector2D& OutAvailablePosition, UObsidianInventoryItemInstance* Instance)
{
	const TArray<FVector2D> ItemGridSize = Instance->GetItemGridSize();
	
	const bool bCanAdd = CheckAvailablePosition(ItemGridSize, OutAvailablePosition);
	return bCanAdd;
}

bool UObsidianInventoryComponent::CanFitItemInstanceToSpecificSlot(const FVector2D& SpecifiedSlot, UObsidianInventoryItemInstance* Instance)
{
	const TArray<FVector2D> ItemGridSize = Instance->GetItemGridSize();
	
	const bool bCanAdd = CheckSpecifiedPosition(ItemGridSize, SpecifiedSlot);
	return bCanAdd;
}

void UObsidianInventoryComponent::AddItemInstance(UObsidianInventoryItemInstance* InstanceToAdd, int32& OutStacksLeft)
{
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::AddItemInstance."));
		return; 
	}
	
	if(InstanceToAdd == nullptr)
	{
		return;
	}

	OutStacksLeft = InstanceToAdd->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	if(InstanceToAdd->IsStackable())
	{
		FObsidianAddingStacksResult AddingStacksResult;
		const TArray<UObsidianInventoryItemInstance*> AddedToInstances = TryAddingStacksToExistingItems(InstanceToAdd->GetItemDef(), OutStacksLeft, /** OUT */ AddingStacksResult);

		OutStacksLeft = AddingStacksResult.StacksLeft;
		InstanceToAdd->OverrideItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, OutStacksLeft);
		
		if(AddingStacksResult.bAddedSomeOfTheStacks)
		{
			BroadcastVisualStacksUpdate(AddedToInstances);
		}
		if(AddingStacksResult.bAddedWholeItemAsStacks)
		{
			return;
		}
	}

	const int32 StacksAvailableToAdd = GetNumberOfStacksAvailableToAddToInventory(InstanceToAdd);
	if(StacksAvailableToAdd == 0)
	{
		//TODO We can no longer add this item to the inventory, add voice over?
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta,FString::Printf(TEXT("Can no longer add this item to inventory!")));
		return;
	}
	
	FVector2D AvailablePosition;
	if(CanFitItemInstance(AvailablePosition, InstanceToAdd) == false)
	{
		//TODO Inventory is full, add voice over?
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("Inventory is full!")));
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

bool UObsidianInventoryComponent::AddItemInstanceToSpecificSlot(UObsidianInventoryItemInstance* InstanceToAdd, const FVector2D& ToSlot, const int32 StackToAddOverride)
{
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::AddItemInstanceToSpecificSlot."));
		return false; 
	}
	
	if(InstanceToAdd == nullptr)
	{
		return false;
	}

	int32 StacksAvailableToAdd = 1;
	if(InstanceToAdd->IsStackable())
	{
		StacksAvailableToAdd = GetNumberOfStacksAvailableToAddToInventory(InstanceToAdd);
		if(StacksAvailableToAdd == 0)
		{
			//TODO We can no longer add this item to the inventory, add voice over?
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta,FString::Printf(TEXT("Can no longer add this item to inventory!")));
			return false;
		}
		
		if(StackToAddOverride != -1)
		{
			StacksAvailableToAdd = FMath::Clamp<int32>((FMath::Min<int32>(StacksAvailableToAdd, StackToAddOverride)),
				1, StacksAvailableToAdd);
		}
	}
	
	if(CanFitItemInstanceToSpecificSlot(ToSlot, InstanceToAdd) == false)
	{
		//TODO Inventory is full, add voice over?
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("Inventory is full at specified slot!")));
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

UObsidianInventoryItemInstance* UObsidianInventoryComponent::TakeOutFromItemInstance(UObsidianInventoryItemInstance* TakingFromInstance, const int32 StacksToTake)
{
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::TakeOutFromItemInstance."));
		return nullptr; 
	}
	
	const int32 CurrentTakingFromInstanceStacks = TakingFromInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	if(!ensureMsgf(((StacksToTake == 0) || (CurrentTakingFromInstanceStacks != StacksToTake)), TEXT("This function shouldn't be called if you want to take the whole item out. Simply Pickup the item instead.")))
	{
		return nullptr;
	}
	
	UObsidianInventoryItemInstance* NewInstance = UObsidianInventoryItemInstance::DuplicateItem(TakingFromInstance, GetOwner());
	if(NewInstance == nullptr)
	{
		return nullptr;
	}
	
	const int32 NewCurrentTakingFromInstanceStacks = CurrentTakingFromInstanceStacks - StacksToTake;
	TakingFromInstance->OverrideItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, NewCurrentTakingFromInstanceStacks);

	// Since the only valid number of stacks to take is in range [1, x - 1] we can clamp it for extra safety.
	const int32 StackToTakeSafe = FMath::Clamp<int32>(StacksToTake, 1, CurrentTakingFromInstanceStacks - 1);
	NewInstance->OverrideItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, StackToTakeSafe);
	
	return NewInstance;
}

TArray<UObsidianInventoryItemInstance*> UObsidianInventoryComponent::TryAddingStacksToExistingItems(const TSubclassOf<UObsidianInventoryItemDefinition>& AddingFromItemDef, const int32 StacksToAdd, FObsidianAddingStacksResult& OutAddingStacksResult)
{
	TArray<UObsidianInventoryItemInstance*> AddedToInstances;
	
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::TryAddingStacksToExistingItems."));
		return AddedToInstances; 
	}
	
	
	if(StacksToAdd <= 0)
	{
		return AddedToInstances;
	}
	
	OutAddingStacksResult.AddedStacks = 0;
	OutAddingStacksResult.StacksLeft = StacksToAdd;
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

			const int32 StacksLeft = OutAddingStacksResult.StacksLeft;
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
			OutAddingStacksResult.AddedStacks += AmountThatCanBeAddedToInstance;
			OutAddingStacksResult.StacksLeft -= AmountThatCanBeAddedToInstance;
			AddedToInstances.AddUnique(Instance);
			
			if(OutAddingStacksResult.AddedStacks == StacksToAdd)
			{
				OutAddingStacksResult.bAddedWholeItemAsStacks = true;
				OutAddingStacksResult.bAddedSomeOfTheStacks = true;
				return AddedToInstances;
			}
		}
	}
	if(AddedToInstances.Num() > 0)
	{
		OutAddingStacksResult.bAddedSomeOfTheStacks = true;
	}
	return AddedToInstances;
}

bool UObsidianInventoryComponent::TryAddingStacksToSpecificSlotWithItemDef(const TSubclassOf<UObsidianInventoryItemDefinition>& AddingFromItemDef, const int32 AddingFromItemDefCurrentStacks, const FVector2D& AtPosition, FObsidianAddingStacksResult& OutAddingStacksResult, const int32 StackToAddOverride)
{
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::TryAddingStacksToSpecificSlotWithItemDef."));
		return false; 
	}
	
	OutAddingStacksResult.StacksLeft = AddingFromItemDefCurrentStacks;
	
	UObsidianInventoryItemInstance* InstanceToAddTo = Internal_GetItemInstanceAtLocation(AtPosition);
	if(IsTheSameItem(InstanceToAddTo, AddingFromItemDef) == false)
	{
		return false;
	}
	
	int32 AmountThatCanBeAddedToInstance = GetAmountOfStacksAvailableToAddToItem(AddingFromItemDef, AddingFromItemDefCurrentStacks, InstanceToAddTo);
	if(AmountThatCanBeAddedToInstance <= 0)
	{
		return false;
	}
	
	if(StackToAddOverride != -1)
	{
		AmountThatCanBeAddedToInstance = FMath::Clamp<int32>((FMath::Min<int32>(AmountThatCanBeAddedToInstance, StackToAddOverride)),
			1, AmountThatCanBeAddedToInstance);
	}
	
	InstanceToAddTo->AddItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, AmountThatCanBeAddedToInstance);

	OutAddingStacksResult.StacksLeft -= AmountThatCanBeAddedToInstance;
	OutAddingStacksResult.AddedStacks = AmountThatCanBeAddedToInstance;
	OutAddingStacksResult.LastAddedToInstance = InstanceToAddTo;
	OutAddingStacksResult.bAddedSomeOfTheStacks = true;
	
	UE_LOG(LogTemp, Warning, TEXT("Added [%d] stacks to [%s]."), AmountThatCanBeAddedToInstance, *GetNameSafe(InstanceToAddTo));

	if(AmountThatCanBeAddedToInstance == AddingFromItemDefCurrentStacks)
	{
		OutAddingStacksResult.bAddedWholeItemAsStacks = true;
	}
	return true;
}

bool UObsidianInventoryComponent::TryAddingStacksToSpecificSlotWithInstance(UObsidianInventoryItemInstance* AddingFromInstance, const FVector2D& AtPosition, FObsidianAddingStacksResult& OutAddingStacksResult, const int32 StackToAddOverride)
{
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::TryAddingStacksToSpecificSlotWithInstance."));
		return false; 
	}
	
	const int32 AddingFromInstanceCurrentStacks = AddingFromInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	OutAddingStacksResult.StacksLeft = AddingFromInstanceCurrentStacks;
	
	UObsidianInventoryItemInstance* InstanceToAddTo = Internal_GetItemInstanceAtLocation(AtPosition);
	if(IsTheSameItem(AddingFromInstance, InstanceToAddTo) == false)
	{
		return false;
	}
	
	int32 AmountThatCanBeAddedToInstance = GetAmountOfStacksAvailableToAddToItem(AddingFromInstance, InstanceToAddTo);
	if(AmountThatCanBeAddedToInstance <= 0)
	{
		return false;
	}

	if(StackToAddOverride != -1)
	{
		AmountThatCanBeAddedToInstance = FMath::Clamp<int32>((FMath::Min<int32>(AmountThatCanBeAddedToInstance, StackToAddOverride)),
			1, AmountThatCanBeAddedToInstance);
	}
	
	InstanceToAddTo->AddItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, AmountThatCanBeAddedToInstance);
	AddingFromInstance->RemoveItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, AmountThatCanBeAddedToInstance);

	OutAddingStacksResult.AddedStacks = AmountThatCanBeAddedToInstance;
	OutAddingStacksResult.StacksLeft -= AmountThatCanBeAddedToInstance;
	OutAddingStacksResult.LastAddedToInstance = InstanceToAddTo;
	OutAddingStacksResult.bAddedSomeOfTheStacks = true;
	
	UE_LOG(LogTemp, Warning, TEXT("Added [%d] stacks to [%s]."), AmountThatCanBeAddedToInstance, *GetNameSafe(InstanceToAddTo));
	
	if(AmountThatCanBeAddedToInstance == AddingFromInstanceCurrentStacks)
	{
		OutAddingStacksResult.bAddedWholeItemAsStacks = true;
	}
	return true;
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

int32 UObsidianInventoryComponent::GetAmountOfStacksAvailableToAddToItem(const UObsidianInventoryItemInstance* AddingFromInstance, const UObsidianInventoryItemInstance* InstanceToAddTo)
{
	const int32 CurrentStackCount = InstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	if(CurrentStackCount == 0)
	{
		return 0;
	}

	const int32 StacksInInventory = FindAllStacksForGivenItem(AddingFromInstance);
	const int32 LimitStackCount = InstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Limit);
	if((LimitStackCount == 1) || (LimitStackCount == StacksInInventory))
	{
		return 0;
	}

	const int32 AddingFromInstanceCurrentStacks = AddingFromInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	const int32 StacksThatCanBeAddedToInventory = LimitStackCount == 0 ? AddingFromInstanceCurrentStacks : LimitStackCount - StacksInInventory;
	if(StacksThatCanBeAddedToInventory <= 0)
	{
		return 0;
	}
			
	const int32 MaxStackCount = InstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Max);
	const int32 AmountThatCanBeAddedToInstance = FMath::Clamp<int32>((MaxStackCount - CurrentStackCount), 0, StacksThatCanBeAddedToInventory);
	return FMath::Min<int32>(AmountThatCanBeAddedToInstance, AddingFromInstanceCurrentStacks);
}

int32 UObsidianInventoryComponent::GetAmountOfStacksAvailableToAddToItem(const TSubclassOf<UObsidianInventoryItemDefinition>& AddingFromItemDef, const int32 AddingFromItemDefCurrentStacks, const UObsidianInventoryItemInstance* InstanceToAddTo)
{
	const int32 CurrentStackCount = InstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	if(CurrentStackCount == 0)
	{
		return 0;
	}
	
	const int32 StacksInInventory = FindAllStacksForGivenItem(AddingFromItemDef);
	const int32 LimitStackCount = InstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Limit);
	if((LimitStackCount == 1) || (LimitStackCount == StacksInInventory))
	{
		return 0;
	}
			
	const int32 StacksThatCanBeAddedToInventory = LimitStackCount == 0 ? AddingFromItemDefCurrentStacks : LimitStackCount - StacksInInventory;
	if(StacksThatCanBeAddedToInventory <= 0)
	{
		return 0;
	}
			
	const int32 MaxStackCount = InstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Max);
	const int32 AmountThatCanBeAddedToInstance = FMath::Clamp((MaxStackCount - CurrentStackCount), 0, StacksThatCanBeAddedToInventory);
	return FMath::Min(AmountThatCanBeAddedToInstance, AddingFromItemDefCurrentStacks);
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
	
	const int32 StacksLimit = StacksFrag->GetItemStackNumberByTag(ObsidianGameplayTags::Item_StackCount_Limit);
	if(StacksLimit == 0) // Item has no limit
	{
		return CurrentStacks;
	}
	const int32 AllStacks = FindAllStacksForGivenItem(ItemDef);
	return  FMath::Clamp(StacksLimit - AllStacks, 0, CurrentStacks);
}

int32 UObsidianInventoryComponent::GetNumberOfStacksAvailableToAddToInventory(const UObsidianInventoryItemInstance* ItemInstance)
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
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::RemoveItemInstance."));
		return; 
	}
	
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
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::ConsumeItemsByDefinition."));
		return false; 
	}
	
	AActor* OwningActor = GetOwner();
	if(OwningActor == nullptr || OwningActor->HasAuthority() == false)
	{
		return false;
	}

	//TODO: N squared right now as there's no acceleration structure
	int32 TotalConsumed = 0;
	while(TotalConsumed < NumberOfItemsToConsume)
	{
		if(UObsidianInventoryItemInstance* Instance = FindFirstItemInstanceForDefinition(ItemDef))
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

	// Sync the map with a client, feels like a @HACK. Need second opinion
	// Alternatives I thought about:
	// Replace the map with a struct that can be replicated
	// Different way of handling slot hoverings
	if(GetOwnerRole() != ENetRole::ROLE_Authority)
	{
		ClientItem_MarkSpace(ItemInstance, AtPosition);
	}
}

void UObsidianInventoryComponent::ClientItem_MarkSpace_Implementation(const UObsidianInventoryItemInstance* ItemInstance, const FVector2D AtPosition)
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

	// Sync the map with a client, feels like a @HACK. Need second opinion
	// Alternatives I thought about:
	// Replace the map with a struct that can be replicated
	// Different way of handling slot hoverings
	if(GetOwnerRole() != ENetRole::ROLE_Authority)
	{
		ClientItem_UnMarkSpace(ItemInstance, AtPosition);
	}
}

void UObsidianInventoryComponent::ClientItem_UnMarkSpace_Implementation(const UObsidianInventoryItemInstance* ItemInstance, const FVector2D AtPosition)
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
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::CanReplaceItemAtSpecificSlotWithInstance."));
		return false; 
	}
	
	if(GetNumberOfStacksAvailableToAddToInventory(ReplacingInstance) <= 0)
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
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianInventoryComponent::CanReplaceItemAtSpecificSlotWithDef."));
		return false; 
	}
	
	if(GetNumberOfStacksAvailableToAddToInventory(ItemDef, StackCount) <= 0)
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

bool UObsidianInventoryComponent::IsTheSameItem(const UObsidianInventoryItemInstance* InstanceA, const UObsidianInventoryItemInstance* InstanceB)
{
	if(InstanceA == nullptr || InstanceB == nullptr)
	{
		return false;
	}

	if(InstanceA->GetItemDef().Get() == InstanceB->GetItemDef().Get())
	{
		return true;
	}
	return false;
}

bool UObsidianInventoryComponent::IsTheSameItem(const UObsidianInventoryItemInstance* Instance, const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef)
{
	if(Instance == nullptr || ItemDef == nullptr)
	{
		return false;
	}

	if(Instance->GetItemDef().Get() == ItemDef.Get())
	{
		return true;
	}
	return false;
}

void UObsidianInventoryComponent::BroadcastVisualStacksUpdate(TArray<UObsidianInventoryItemInstance*> AddedToInstances) const
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


