// Copyright 2024 out of sCope team - Michał Ogiński


#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemFragment.h"
#include "Net/UnrealNetwork.h"

UObsidianInventoryItemInstance::UObsidianInventoryItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UObsidianInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemDef);
	DOREPLIFETIME(ThisClass, ItemStackTags);

	//TODO Test if this actually needs to be replicated.
	DOREPLIFETIME(ThisClass, ItemGridSize);
	DOREPLIFETIME(ThisClass, ItemGridSpan);
}

const UObsidianInventoryItemFragment* UObsidianInventoryItemInstance::FindFragmentByClass(const TSubclassOf<UObsidianInventoryItemFragment> FragmentClass) const
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<UObsidianInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}

void UObsidianInventoryItemInstance::AddItemStackCount(const FGameplayTag ToTag, const int32 StackCount)
{
	ItemStackTags.AddStack(ToTag, StackCount);
}

void UObsidianInventoryItemInstance::RemoveItemStackCount(const FGameplayTag FromTag, const int32 StackCount)
{
	ItemStackTags.RemoveStack(FromTag, StackCount);
}

int32 UObsidianInventoryItemInstance::GetItemStackCount(const FGameplayTag Tag) const
{
	return ItemStackTags.GetStackCount(Tag);
}

bool UObsidianInventoryItemInstance::HasStackCountForTag(const FGameplayTag Tag) const
{
	return ItemStackTags.ContainsTag(Tag);
}

TArray<FVector2D> UObsidianInventoryItemInstance::GetItemGridSize() const
{
	return ItemGridSize;
}

void UObsidianInventoryItemInstance::SetItemGridSize(const TArray<FVector2D>& GridSizeToSet)
{
	ItemGridSize = GridSizeToSet;
}

FVector2D UObsidianInventoryItemInstance::GetItemGridSpan() const
{
	return ItemGridSpan;
}

void UObsidianInventoryItemInstance::SetItemGridSpan(const FVector2D GridSpanToSet)
{
	ItemGridSpan = GridSpanToSet;
}

