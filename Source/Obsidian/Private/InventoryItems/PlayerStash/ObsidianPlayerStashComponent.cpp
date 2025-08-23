// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/PlayerStash/ObsidianPlayerStashComponent.h"

// ~ Core
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

// ~ Project
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"

DEFINE_LOG_CATEGORY(LogPlayerStash)

UObsidianPlayerStashComponent::UObsidianPlayerStashComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, StashItemList(this)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
	
}

void UObsidianPlayerStashComponent::BeginPlay()
{
	Super::BeginPlay();

	StashItemList.InitializeStashTabs(StashTabsConfig);
}

void UObsidianPlayerStashComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, StashItemList);
}

UObsidianStashTabsConfig* UObsidianPlayerStashComponent::GetStashTabConfig() const
{
	return StashTabsConfig;
}

bool UObsidianPlayerStashComponent::CanOwnerModifyPlayerStashState()
{
	if(CachedOwnerPlayerController == nullptr)
	{
		CachedOwnerPlayerController = Cast<AObsidianPlayerController>(GetOwner());
	}
	if(const UObsidianAbilitySystemComponent* OwnerASC = CachedOwnerPlayerController->GetObsidianAbilitySystemComponent())
	{
		return !OwnerASC->HasMatchingGameplayTag(ObsidianGameplayTags::PlayerStash_BlockActions);
	}
	return false;
}

TArray<UObsidianInventoryItemInstance*> UObsidianPlayerStashComponent::GetAllItems() const
{
	return StashItemList.GetAllItems();
}

TArray<UObsidianInventoryItemInstance*> UObsidianPlayerStashComponent::GetAllItemsFromStashTab(const FGameplayTag& StashTabTag)
{
	return StashItemList.GetAllItemsFromStashTab(StashTabTag);
}

int32 UObsidianPlayerStashComponent::FindAllStacksForGivenItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef)
{
	//TODO Implement
	return 0;
}

int32 UObsidianPlayerStashComponent::FindAllStacksForGivenItem(const UObsidianInventoryItemInstance* ItemInstance)
{
	//TODO Implement
	return 0;
}

bool UObsidianPlayerStashComponent::CheckSpecifiedPosition(const FIntPoint& ItemGridSpan, const FObsidianItemPosition& SpecifiedPosition, const FGameplayTag& StashTabTag)
{
	return true;
}

FObsidianAddingStacksResult UObsidianPlayerStashComponent::TryAddingStacksToExistingItems(const TSubclassOf<UObsidianInventoryItemDefinition>& AddingFromItemDef, const int32 StacksToAdd, const FGameplayTag& InTabTag, TArray<UObsidianInventoryItemInstance*>& OutAddedToInstances)
{
	FObsidianAddingStacksResult Result = FObsidianAddingStacksResult();
	Result.AddedStacks = 0;
	Result.StacksLeft = StacksToAdd;
	
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
		return Result; 
	}

	if(CanOwnerModifyPlayerStashState() == false)
	{
		return Result;
	}
	
	if(StacksToAdd <= 0)
	{
		return Result;
	}
	
	TArray<UObsidianInventoryItemInstance*> Items = StashItemList.GetAllItems();
	for(UObsidianInventoryItemInstance* Instance : Items)
	{
		if(!IsValid(Instance))
		{
			UE_LOG(LogPlayerStash, Error, TEXT("Instance is invalid in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
			continue;
		}
		
		if(AddingFromItemDef == Instance->GetItemDef())
		{
			const int32 CurrentStackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
			if(CurrentStackCount == 0)
			{
				continue;
			}
			
			const int32 StacksLeft = Result.StacksLeft;
			const int32 MaxStackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Max);
			int32 AmountThatCanBeAddedToInstance = FMath::Clamp((MaxStackCount - CurrentStackCount), 0, StacksLeft);
			AmountThatCanBeAddedToInstance = FMath::Min(AmountThatCanBeAddedToInstance, StacksLeft);
			if(AmountThatCanBeAddedToInstance <= 0)
			{
				continue;
			}
			
			UE_LOG(LogTemp, Warning, TEXT("Added [%d] stacks to [%s]."), AmountThatCanBeAddedToInstance, *GetNameSafe(Instance));
			
			Instance->AddItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, AmountThatCanBeAddedToInstance);
			StashItemList.ChangedEntryStacks(Instance, CurrentStackCount, InTabTag);
			
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

FObsidianItemOperationResult UObsidianPlayerStashComponent::AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const FGameplayTag& StashTabTag, const int32 StackCount)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();
	Result.StacksLeft = StackCount;
	
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
	
	UObsidianInventoryItemInstance* Instance = StashItemList.AddEntry(ItemDef, Result.StacksLeft, StashTabTag, AvailablePosition);
	Instance->AddItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, Result.StacksLeft);
	
	if(Instance && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(Instance);
	}

	Result.bActionSuccessful = true;
	Result.AffectedInstance = Instance;
	return Result;
}

FObsidianItemOperationResult UObsidianPlayerStashComponent::AddItemDefinitionToSpecifiedSlot(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const FGameplayTag& StashTabTag, const FObsidianItemPosition& ItemPosition, const int32 StackCount, const int32 StackToAddOverride)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();
	Result.StacksLeft = StackCount;
	
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
	
	if(CanFitItemDefinitionToSpecifiedSlot(ItemPosition, StashTabTag, ItemDef) == false)
	{
		//TODO Inventory is full, add voice over?
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("Inventory is full at specified slot!")));
		return Result;
	}
	
	Result.StacksLeft -= StacksAvailableToAdd;
	Result.bActionSuccessful = Result.StacksLeft == 0; // We don't need any item duplication logic (like in AddItemInstanceToSpecificSlot)
														// as we still have a valid item definition in hands if the whole item isn't added here.
	ensure(Result.StacksLeft >= 0);
	
	UObsidianInventoryItemInstance* Instance = StashItemList.AddEntry(ItemDef, StacksAvailableToAdd, StashTabTag, ItemPosition);
	Instance->AddItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, StacksAvailableToAdd);
	
	if(Instance && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(Instance);
	}

	Result.AffectedInstance = Instance;
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
	if(CanFitItemInstance(AvailablePosition, StashTabTag, InstanceToAdd) == false)
	{
		//TODO Inventory is full, add voice over?
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("Inventory is full!")));
		return Result;
	}
	
	if(Result.StacksLeft > 0)
	{
		if(UObsidianInventoryItemInstance* NewInstance = UObsidianInventoryItemInstance::DuplicateItem(InstanceToAdd, GetOwner()))
		{
			InstanceToAdd = NewInstance;
		}
	}
	
	InstanceToAdd->OverrideItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, Result.StacksLeft);
	StashItemList.AddEntry(InstanceToAdd, StashTabTag, AvailablePosition);
	
	if(InstanceToAdd && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(InstanceToAdd);
	}
	
	Result.bActionSuccessful = true;
	Result.AffectedInstance = InstanceToAdd;
	return Result;
}

FObsidianItemOperationResult UObsidianPlayerStashComponent::AddItemInstanceToSpecificSlot(UObsidianInventoryItemInstance* InstanceToAdd, const FGameplayTag& StashTabTag, const FObsidianItemPosition& ItemPosition, const int32 StackToAddOverride)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();
	
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
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
	
	if(CanFitItemInstanceToSpecificSlot(ItemPosition, StashTabTag, InstanceToAdd) == false)
	{
		//TODO Inventory is full, add voice over?
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("Player Stash is full at specified slot!")));
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
	StashItemList.AddEntry(InstanceToAdd, StashTabTag, ItemPosition);
	
	if(InstanceToAdd && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(InstanceToAdd);
	}
	
	Result.AffectedInstance = InstanceToAdd;
	return Result;
}

FObsidianItemOperationResult UObsidianPlayerStashComponent::RemoveItemInstance(UObsidianInventoryItemInstance* InstanceToRemove, const FGameplayTag& StashTabTag)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();
	
	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
		return Result; 
	}
	
	if(CanOwnerModifyPlayerStashState() == false)
	{
		return Result;
	}

	if(InstanceToRemove == nullptr)
	{
		UE_LOG(LogPlayerStash, Error, TEXT("Passed InstanceToRemove is invalid in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
		return Result;
	}
	
	StashItemList.RemoveEntry(InstanceToRemove, StashTabTag);
	
	if(InstanceToRemove && IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(InstanceToRemove);
	}

	Result.bActionSuccessful = true;
	Result.AffectedInstance = InstanceToRemove;
	return Result;
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
	return true;
}

bool UObsidianPlayerStashComponent::CanFitItemDefinitionToSpecifiedSlot(const FObsidianItemPosition& SpecifiedSlot, const FGameplayTag& StashTabTag, const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef)
{
	return true;
}

bool UObsidianPlayerStashComponent::CanFitItemInstance(FObsidianItemPosition& OutAvailablePosition, const FGameplayTag& StashTabTag, UObsidianInventoryItemInstance* Instance)
{
	return true;
}

bool UObsidianPlayerStashComponent::CanFitItemInstanceToSpecificSlot(const FObsidianItemPosition& SpecifiedSlot, const FGameplayTag& StashTabTag, const UObsidianInventoryItemInstance* Instance)
{
	return true;
}



