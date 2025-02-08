// Copyright 2024 out of sCope team - Michał Ogiński


#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemFragment.h"
#include "Net/UnrealNetwork.h"
#include "Obsidian/ObsidianGameplayTags.h"

UObsidianInventoryItemInstance::UObsidianInventoryItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UObsidianInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemDef);
	DOREPLIFETIME(ThisClass, ItemStackTags);
	DOREPLIFETIME(ThisClass, bStackable);

	//TODO Test which of these needs replicating, most of them will need to get only replicated once as they will never change, so probably never replicated lol?
	DOREPLIFETIME(ThisClass, ItemGridSize);
	DOREPLIFETIME(ThisClass, ItemGridSpan);
	DOREPLIFETIME(ThisClass, ItemImage);
	DOREPLIFETIME(ThisClass, ItemDisplayName);
	DOREPLIFETIME(ThisClass, ItemSkeletalMesh);
	DOREPLIFETIME(ThisClass, ItemDroppedMesh);
	DOREPLIFETIME(ThisClass, ItemDescription);
	DOREPLIFETIME(ThisClass, ItemAdditionalDescription);
}

const UObsidianInventoryItemFragment* UObsidianInventoryItemInstance::FindFragmentByClass(const TSubclassOf<UObsidianInventoryItemFragment> FragmentClass) const
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<UObsidianInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}

UObsidianInventoryItemInstance* UObsidianInventoryItemInstance::DuplicateItem(const UObsidianInventoryItemInstance* OriginalItem, UObject* Outer)
{
	if(OriginalItem)
	{
		UObsidianInventoryItemInstance* NewInstance = DuplicateObject<UObsidianInventoryItemInstance>(OriginalItem, Outer);
		NewInstance->ItemStackTags.TagToCountMap = OriginalItem->ItemStackTags.TagToCountMap; //@HACK This map does not get copied by the DuplicateObject function, need to copy it manually, there might be more.
		return NewInstance;
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

void UObsidianInventoryItemInstance::OverrideItemStackCount(const FGameplayTag Tag, const int32 NewStackCount)
{
	ItemStackTags.OverrideStack(Tag, NewStackCount);
}

int32 UObsidianInventoryItemInstance::GetItemStackCount(const FGameplayTag Tag) const
{
	return ItemStackTags.GetStackCount(Tag);
}

bool UObsidianInventoryItemInstance::HasStackCountForTag(const FGameplayTag Tag) const
{
	return ItemStackTags.ContainsTag(Tag);
}

bool UObsidianInventoryItemInstance::HasAnyStacks() const
{
	bool bHasTags = false;
	for(const FGameplayTag& StackTag : ObsidianGameplayTags::StackTypes)
	{
		if(ItemStackTags.ContainsTag(StackTag))
		{
			bHasTags = true;
		}
	}
	return bHasTags;
}

bool UObsidianInventoryItemInstance::IsStackable() const
{
	return bStackable;
}

void UObsidianInventoryItemInstance::SetStackable(const bool InStackable)
{
	bStackable = InStackable;
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

UTexture2D* UObsidianInventoryItemInstance::GetItemImage() const
{
	return ItemImage;
}

void UObsidianInventoryItemInstance::SetItemImage(UTexture2D* ItemImageToSet)
{
	ItemImage = ItemImageToSet;
}

USkeletalMesh* UObsidianInventoryItemInstance::GetItemSkeletalMesh() const
{
	return ItemSkeletalMesh;
}

void UObsidianInventoryItemInstance::SetItemSkeletalMesh(USkeletalMesh* InItemSkeletalMesh)
{
	ItemSkeletalMesh = InItemSkeletalMesh;
}

UStaticMesh* UObsidianInventoryItemInstance::GetItemDroppedMesh() const
{
	return ItemDroppedMesh;
}

void UObsidianInventoryItemInstance::SetItemDroppedMesh(UStaticMesh* InItemDroppedMesh)
{
	ItemDroppedMesh = InItemDroppedMesh;
}

FText UObsidianInventoryItemInstance::GetItemDisplayName() const
{
	return ItemDisplayName;
}

void UObsidianInventoryItemInstance::SetItemDisplayName(const FText& InItemDisplayName)
{
	ItemDisplayName = InItemDisplayName;
}

FText UObsidianInventoryItemInstance::GetItemDescription() const
{
	return ItemDescription;
}

void UObsidianInventoryItemInstance::SetItemDescription(const FText& InItemDescription)
{
	ItemDescription = InItemDescription;
}

FText UObsidianInventoryItemInstance::GetItemAdditionalDescription() const
{
	return ItemAdditionalDescription;
}

void UObsidianInventoryItemInstance::SetItemAdditionalDescription(const FText& InItemAdditionalDescription)
{
	ItemAdditionalDescription = InItemAdditionalDescription;
}

FString UObsidianInventoryItemInstance::GetItemDebugName() const
{
	return DebugName;
}

void UObsidianInventoryItemInstance::SetItemDebugName(const FString& InItemDebugName)
{
	DebugName = InItemDebugName;
}

