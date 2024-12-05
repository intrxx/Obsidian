// Copyright 2024 out of sCope team - Michał Ogiński


#include "InventoryItems/ObsidianInventoryComponent.h"
#include "Engine/ActorChannel.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "Net/UnrealNetwork.h"

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

UObsidianInventoryItemInstance* UObsidianInventoryComponent::Internal_GetItemInstanceForLocation(const FVector2D& Position) const
{
	const TMap<FVector2D, UObsidianInventoryItemInstance*> LocToItemMap = InventoryGrid.GridLocationToItemMap;
	if(LocToItemMap.Contains(Position))
	{
		return LocToItemMap[Position];
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

bool UObsidianInventoryComponent::CanAddItemDefinitionAtSpecifiedSlot(const FVector2D& SpecifiedSlot, const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, int32 StackCount)
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

UObsidianInventoryItemInstance* UObsidianInventoryComponent::AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const int32 StackCount)
{
	if(ItemDef == nullptr)
	{
		return nullptr;
	}

	FVector2D AvailablePosition;
	if(CanAddItemDefinition(AvailablePosition, ItemDef, StackCount) == false)
	{
		//TODO Inventory is full, add voice over?
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta,
			FString::Printf(TEXT("Inventory is full!")));
		return nullptr;
	}
	
	UObsidianInventoryItemInstance* Instance = InventoryGrid.AddEntry(ItemDef, StackCount, AvailablePosition);
	Item_MarkSpace(AvailablePosition, Instance);
	
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
	
	if(CanAddItemDefinitionAtSpecifiedSlot(ToSlot, ItemDef, StackCount) == false)
	{
		//TODO Inventory is full, add voice over?
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta,
			FString::Printf(TEXT("Inventory is full at specified slot!")));
		return nullptr;
	}

	UObsidianInventoryItemInstance* Instance = InventoryGrid.AddEntry(ItemDef, StackCount, ToSlot);
	Item_MarkSpace(ToSlot, Instance);
	
	if(Instance && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(Instance);
	}

	OnItemAddedToInventoryDelegate.Broadcast(Instance, ToSlot);
	
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

void UObsidianInventoryComponent::AddItemInstance(UObsidianInventoryItemInstance* InstanceToAdd)
{
	if(InstanceToAdd == nullptr)
	{
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
	
	InventoryGrid.AddEntry(InstanceToAdd, AvailablePosition);
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

	if(CanAddItemInstanceToSpecificSlot(ToSlot, InstanceToAdd) == false)
	{
		//TODO Inventory is full, add voice over?
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta,
			FString::Printf(TEXT("Inventory is full at specified slot!")));
		return false;
	}

	InventoryGrid.AddEntry(InstanceToAdd, ToSlot);
	Item_MarkSpace(ToSlot, InstanceToAdd);
	
	if(InstanceToAdd && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(InstanceToAdd);
	}

	OnItemAddedToInventoryDelegate.Broadcast(InstanceToAdd, ToSlot);
	return true;
}

void UObsidianInventoryComponent::RemoveItemInstance(UObsidianInventoryItemInstance* InstanceToRemove)
{
	const FVector2D FromLocation = GetItemLocationFromGrid(InstanceToRemove);
	Item_UnMarkSpace(FromLocation, InstanceToRemove);
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
		Item_MarkSpace(ItemLoc.Key, ItemLoc.Value);
	}
}

void UObsidianInventoryComponent::Item_MarkSpace(const FVector2D AtPosition, const UObsidianInventoryItemInstance* ItemInstance)
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

void UObsidianInventoryComponent::Item_UnMarkSpace(const FVector2D AtPosition, const UObsidianInventoryItemInstance* ItemInstance)
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
	bool bCanFit = true; //TODO shouldn't it be false?
	
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


