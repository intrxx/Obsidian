// Copyright 2024 out of sCope team - Michał Ogiński


#include "InventoryItems/ObsidianInventoryComponent.h"
#include "Engine/ActorChannel.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_GridSize.h"
#include "Net/UnrealNetwork.h"

UObsidianInventoryComponent::UObsidianInventoryComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryGrid(this)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	
	SetIsReplicatedByDefault(true);
	
	InventoryGridSize = InventoryGridWidth * InventoryGridHeight;
	
	InitInventoryState();
}

void UObsidianInventoryComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryGrid);
}

int32 UObsidianInventoryComponent::GetTotalItemCountByDefinition(TSubclassOf<UObsidianInventoryItemDefinition> ItemDef) const
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

TArray<UObsidianInventoryItemInstance*> UObsidianInventoryComponent::GetAllItems() const
{
	return InventoryGrid.GetAllItems();
}

UObsidianInventoryItemInstance* UObsidianInventoryComponent::FindFirstItemStackForDefinition(TSubclassOf<UObsidianInventoryItemDefinition> ItemDef) const
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

bool UObsidianInventoryComponent::CanAddItemDefinition(FVector2D& OutAvailablePosition, TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, int32 StackCount)
{
	bool bCanAdd = false;
	
	TArray<FVector2D> ItemGridSize;
	
	if(const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef))
	{
		if(const UOInventoryItemFragment_GridSize* GridSizeFragment = Cast<UOInventoryItemFragment_GridSize>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_GridSize::StaticClass())))
		{
			ItemGridSize = GridSizeFragment->GetItemGridSizeFromDesc();
		}
	}
	
	for(FVector2D SizeComp : ItemGridSize)
	{
		UE_LOG(LogTemp, Warning, TEXT("X: %f, Y: %f"), SizeComp.X, SizeComp.Y);
	}
	
	for(const TTuple<FVector2D, bool>& Location : InventoryStateMap)
	{
		if(Location.Value == false) // Location is free
		{
			bool bCanFit = true;
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
				return true;
			}
		}
	}
	
	return bCanAdd;
}

UObsidianInventoryItemInstance* UObsidianInventoryComponent::AddItemDefinition(TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, int32 StackCount)
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

	OnItemAddedToInventoryDelegate.Broadcast(Instance);
	
	return Instance;
}

void UObsidianInventoryComponent::AddItemInstance(UObsidianInventoryItemInstance* InstanceToAdd)
{
	InventoryGrid.AddEntry(InstanceToAdd);
	if(InstanceToAdd && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(InstanceToAdd);
	}
}

void UObsidianInventoryComponent::RemoveItemInstance(UObsidianInventoryItemInstance* InstanceToRemove)
{
	InventoryGrid.RemoveEntry(InstanceToRemove);
	if(InstanceToRemove && IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(InstanceToRemove);
	}
}

bool UObsidianInventoryComponent::ConsumeItemsByDefinition(TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const int32 NumberOfItemsToConsume)
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
		else
		{
			FFrame::KismetExecutionMessage(TEXT("Provided NumberOfItemsToConsume is greater than the instances of the item, or the Instance is invalid"),
				ELogVerbosity::Warning);
			return false;
		}
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
		InventoryStateMap[Location] = true;
	}
	
}


