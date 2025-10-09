// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/ObsidianInventoryItemInstance.h"

// ~ Core
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"

// ~ Project
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemFragment.h"
#include "InventoryItems/Fragments/Shards/ObsidianUsableShard.h"
#include "InventoryItems/Equipment/ObsidianSpawnedEquipmentPiece.h"
#include "Obsidian/ObsidianGameplayTags.h"

UObsidianInventoryItemInstance::UObsidianInventoryItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UObsidianInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//TODO(intrxx) Test which of these needs replicating, most of them will need to get only replicated once as they will never change
	DOREPLIFETIME(ThisClass, ItemUniqueID);
	DOREPLIFETIME(ThisClass, ItemLevel);
	DOREPLIFETIME(ThisClass, ItemDef);
	DOREPLIFETIME(ThisClass, ItemStackTags);
	DOREPLIFETIME(ThisClass, bStackable);
	DOREPLIFETIME(ThisClass, ItemRarity);
	DOREPLIFETIME(ThisClass, ItemCategory);
	DOREPLIFETIME(ThisClass, ItemAffixes);
	DOREPLIFETIME(ThisClass, SpawnedActors);
	DOREPLIFETIME(ThisClass, ActorsToSpawn);
	DOREPLIFETIME(ThisClass, ItemGridSpan);
	DOREPLIFETIME(ThisClass, ItemCurrentPosition);
	DOREPLIFETIME(ThisClass, ItemImage);
	DOREPLIFETIME(ThisClass, ItemDisplayName);
	DOREPLIFETIME(ThisClass, ItemDroppedMesh);
	DOREPLIFETIME(ThisClass, ItemDescription);
	DOREPLIFETIME(ThisClass, ItemAdditionalDescription);
	DOREPLIFETIME(ThisClass, bStartsIdentified);
	DOREPLIFETIME(ThisClass, bIdentified);
	DOREPLIFETIME(ThisClass, bEquippable);
	DOREPLIFETIME(ThisClass, bUsable);
	DOREPLIFETIME(ThisClass, UsableShard);
	DOREPLIFETIME(ThisClass, bNeedsTwoSlots);
	DOREPLIFETIME(ThisClass, ItemSlotPadding);
	DOREPLIFETIME(ThisClass, UsableItemType);
}

void UObsidianInventoryItemInstance::OnInstanceCreatedAndInitialized()
{
	if(IsItemEquippable())
	{
		if(const FGameplayTagContainer* AcceptedCategories = ObsidianGameplayTags::GetSisterSlotAcceptedCategoriesMap().Find(ItemCategory))
		{
			bNeedsTwoSlots = AcceptedCategories->IsEmpty();
		}
	}
}

APawn* UObsidianInventoryItemInstance::GetPawn() const
{
	const APlayerController* OwningController = Cast<APlayerController>(GetOuter());
	return OwningController->GetPawn();
}

UWorld* UObsidianInventoryItemInstance::GetWorld() const
{
	if(const APawn* Pawn = GetPawn())
	{
		return Pawn->GetWorld();
	}
	return nullptr;
}

const UObsidianInventoryItemFragment* UObsidianInventoryItemInstance::FindFragmentByClass(const TSubclassOf<UObsidianInventoryItemFragment> FragmentClass) const
{
	if((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<UObsidianInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}

FGuid UObsidianInventoryItemInstance::GetUniqueItemID() const
{
	return ItemUniqueID;
}

void UObsidianInventoryItemInstance::GenerateUniqueItemID()
{
	if (!ItemUniqueID.IsValid())
	{
		ItemUniqueID = FGuid::NewGuid();
	}
}

int32 UObsidianInventoryItemInstance::GetItemLevel() const
{
#if !UE_BUILD_SHIPPING
	if (ItemLevel == INDEX_NONE)
	{
		UE_LOG(LogItems, Error, TEXT("Item [%s] has invalid Item Level."), *DebugName);
	}
#endif
	return ItemLevel;
}

void UObsidianInventoryItemInstance::SetItemLevel(const int32 InItemLevel)
{
	ItemLevel = InItemLevel;
}

TSubclassOf<UObsidianInventoryItemDefinition> UObsidianInventoryItemInstance::GetItemDef() const
{
	return ItemDef;
}

void UObsidianInventoryItemInstance::SetItemDef(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef)
{
	ItemDef = InItemDef;
}

EObsidianItemRarity UObsidianInventoryItemInstance::GetItemRarity() const
{
	return ItemRarity;
}

void UObsidianInventoryItemInstance::SetItemRarity(const EObsidianItemRarity InItemRarity)
{
	ItemRarity = InItemRarity;
}

FGameplayTag UObsidianInventoryItemInstance::GetItemCategoryTag() const
{
	return ItemCategory;
}

void UObsidianInventoryItemInstance::SetItemCategory(const FGameplayTag& InItemCategoryTag)
{
	ItemCategory = InItemCategoryTag;
}

FGameplayTag UObsidianInventoryItemInstance::GetItemBaseTypeTag() const
{
	return ItemBaseType;
}

void UObsidianInventoryItemInstance::SetItemBaseType(const FGameplayTag& InItemBaseTypeTag)
{
	ItemBaseType = InItemBaseTypeTag;
}

void UObsidianInventoryItemInstance::SetUsable(const bool IsUsable)
{
	bUsable = IsUsable;
}

bool UObsidianInventoryItemInstance::IsItemUsable() const
{
	return bUsable;
}

void UObsidianInventoryItemInstance::SetUsableShard(UObsidianUsableShard* InUsableShard)
{
	UsableShard = InUsableShard;
}

bool UObsidianInventoryItemInstance::UseItem(AObsidianPlayerController* ItemOwner, UObsidianInventoryItemInstance* UsingOntoInstance)
{
	if(UsableShard)
	{
		return UsableShard->OnItemUsed(ItemOwner, this, UsingOntoInstance);
	}
	return false;
}

void UObsidianInventoryItemInstance::SetUsableItemType(const EObsidianUsableItemType InUsableItemTyp)
{
	UsableItemType = InUsableItemTyp;
}

EObsidianUsableItemType UObsidianInventoryItemInstance::GetUsableItemType() const
{
	return UsableItemType;
}

FObsidianItemsMatchingUsableContext UObsidianInventoryItemInstance::FireItemUseUIContext(const TArray<UObsidianInventoryItemInstance*>& AllItems) const
{
	if(UsableShard)
	{
		return UsableShard->OnItemUsed_UIContext(AllItems);
	}
	return FObsidianItemsMatchingUsableContext();
}

void UObsidianInventoryItemInstance::SetStartsIdentified(const bool InStartsIdentified)
{
	bStartsIdentified = InStartsIdentified;
}

void UObsidianInventoryItemInstance::SetIdentified(const bool InIdentified)
{
	bIdentified = InIdentified;
}

bool UObsidianInventoryItemInstance::IsItemIdentified() const
{
	return bStartsIdentified || bIdentified;
}

TArray<FObsidianActiveItemAffix> UObsidianInventoryItemInstance::GetAllItemAffixes() const
{
	return ItemAffixes.GetAllItemAffixes();
}

void UObsidianInventoryItemInstance::InitializeAffixes(const TArray<FObsidianActiveItemAffix>& AffixesToInitialize)
{
	if (AffixesToInitialize.IsEmpty())
	{
		return;
	}
	
	ItemAffixes.InitializeAffixes(this, AffixesToInitialize);
}

void UObsidianInventoryItemInstance::AddAffix(const FObsidianActiveItemAffix& AffixToAdd)
{
	ItemAffixes.AddAffix(this, AffixToAdd);
}

void UObsidianInventoryItemInstance::RemoveAffix(const FGameplayTag& AffixTag)
{
	ItemAffixes.RemoveAffix(AffixTag);
}

int32 UObsidianInventoryItemInstance::GetItemCombinedAffixLimit() const
{
	return 1;
}

int32 UObsidianInventoryItemInstance::GetItemAddedAffixCount() const
{
	return GetItemAddedPrefixCount() + GetItemAddedSuffixCount();
}

int32 UObsidianInventoryItemInstance::GetItemAddedSuffixCount() const
{
	return 0;
}

int32 UObsidianInventoryItemInstance::GetItemAddedPrefixCount() const
{
	return 0;
}

void UObsidianInventoryItemInstance::SetItemAffixesCountLimit(const int32 InAffixesLimit)
{
	
}

void UObsidianInventoryItemInstance::SetItemAddedSuffixCount(const int32 InAddedSuffixes)
{
}

void UObsidianInventoryItemInstance::SetItemAddedPrefixCount(const int32 InAddedPrefixes)
{
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

FIntPoint UObsidianInventoryItemInstance::GetItemGridSpan() const
{
	return ItemGridSpan;
}

void UObsidianInventoryItemInstance::SetItemGridSpan(const FIntPoint& GridSpanToSet)
{
	ItemGridSpan = GridSpanToSet;
}

FObsidianItemPosition UObsidianInventoryItemInstance::GetItemCurrentPosition() const
{
	return ItemCurrentPosition;
}

void UObsidianInventoryItemInstance::SetItemCurrentPosition(const FObsidianItemPosition& CurrentPositionToSet)
{
	ItemCurrentPosition = CurrentPositionToSet;	
}

void UObsidianInventoryItemInstance::ResetItemCurrentPosition()
{
	ItemCurrentPosition = FIntPoint::NoneValue;
}

UTexture2D* UObsidianInventoryItemInstance::GetItemImage() const
{
	return ItemImage;
}

void UObsidianInventoryItemInstance::SetItemImage(UTexture2D* ItemImageToSet)
{
	ItemImage = ItemImageToSet;
}

void UObsidianInventoryItemInstance::SetEquippable(const bool InEquippable)
{
	bEquippable = InEquippable;
}

bool UObsidianInventoryItemInstance::IsItemEquippable() const
{
	return bEquippable;
}

void UObsidianInventoryItemInstance::SetItemNeedTwoSlots(const bool InNeedsTwoSlots)
{
	bNeedsTwoSlots = InNeedsTwoSlots;
}

bool UObsidianInventoryItemInstance::DoesItemNeedTwoSlots() const
{
	return bNeedsTwoSlots;
}

TArray<AObsidianSpawnedEquipmentPiece*> UObsidianInventoryItemInstance::GetSpawnedActors() const
{
	return SpawnedActors;
}

void UObsidianInventoryItemInstance::SetEquipmentActors(const TArray<FObsidianEquipmentActor>& EquipmentActors)
{
	if(EquipmentActors.IsEmpty()) // This is a valid case since I don't plan to support any body armor equipment.
	{
		return;
	}
	
	ActorsToSpawn.Empty(EquipmentActors.Num());
	ActorsToSpawn.Append(EquipmentActors);
}

void UObsidianInventoryItemInstance::SpawnEquipmentActors(const FGameplayTag& SlotTag)
{
	if(ActorsToSpawn.IsEmpty()) // This is a valid case since I don't plan to support any body armor equipment.
	{
		return;
	}
	
	if(APawn* OwningPawn = GetPawn())
	{ 
		USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
		if(ACharacter* Char = Cast<ACharacter>(OwningPawn))
		{
			AttachTarget = Char->GetMesh();
		}

		for(FObsidianEquipmentActor& SpawnInfo : ActorsToSpawn)
		{
			AObsidianSpawnedEquipmentPiece* NewActor = GetWorld()->SpawnActorDeferred<AObsidianSpawnedEquipmentPiece>(SpawnInfo.ActorToSpawn, FTransform::Identity, OwningPawn);
			NewActor->FinishSpawning(FTransform::Identity, true);
			NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);
			NewActor->AssociatedSlotTag = SlotTag;
			if(SpawnInfo.bOverrideAttachSocket)
			{
				SpawnInfo.OverrideAttachSocket(SlotTag);
			}
			NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, SpawnInfo.AttachSocket);

			SpawnedActors.Add(NewActor);
		}
	}
}

void UObsidianInventoryItemInstance::DestroyEquipmentActors()
{
	for(AObsidianSpawnedEquipmentPiece* Actor : SpawnedActors)
	{
		if(Actor)
		{
			Actor->Destroy();
		}
	}
	
	SpawnedActors.Empty();
}

void UObsidianInventoryItemInstance::SetAbilitySets(const TArray<UObsidianAbilitySet*>& InAbilitySets)
{
	if(InAbilitySets.IsEmpty())
	{
		return;
	}

	AbilitySetsToGive.Empty(InAbilitySets.Num());
	AbilitySetsToGive.Append(InAbilitySets);
}

TArray<UObsidianAbilitySet*> UObsidianInventoryItemInstance::GetOwningAbilitySets() const
{
	return AbilitySetsToGive;
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

float UObsidianInventoryItemInstance::GetItemSlotPadding() const
{
	return ItemSlotPadding;
}

void UObsidianInventoryItemInstance::SetItemSlotPadding(const float InItemSlotPadding)
{
	ItemSlotPadding = InItemSlotPadding;
}

FString UObsidianInventoryItemInstance::GetItemDebugName() const
{
	return DebugName;
}

void UObsidianInventoryItemInstance::SetItemDebugName(const FString& InItemDebugName)
{
	DebugName = InItemDebugName;
}

