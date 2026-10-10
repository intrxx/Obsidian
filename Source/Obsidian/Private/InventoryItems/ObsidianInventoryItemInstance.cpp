// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/ObsidianInventoryItemInstance.h"

#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

#include "Game/Save/ObsidianHeroSaveGame.h"
#include "InventoryItems/Equipment/ObsidianSpawnedEquipmentPiece.h"
#include "InventoryItems/Fragments/Shards/ObsidianUsableShard.h"
#include "InventoryItems/ItemAffixes/ObsidianAffixAbilitySet.h"
#include "InventoryItems/ItemDrop/ObsidianItemDataDeveloperSettings.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemFragment.h"
#include "InventoryItems/ObsidianItemsFunctionLibrary.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "Obsidian/ObsidianLogCategories.h"


UObsidianInventoryItemInstance::UObsidianInventoryItemInstance(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
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
	DOREPLIFETIME(ThisClass, ItemNameAdditionsData);
	DOREPLIFETIME(ThisClass, ItemDroppedMesh);
	DOREPLIFETIME(ThisClass, ItemDescription);
	DOREPLIFETIME(ThisClass, ItemAdditionalDescription);
	DOREPLIFETIME(ThisClass, bCanHaveAffixes);
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

const UObsidianInventoryItemFragment* UObsidianInventoryItemInstance::FindFragmentByClass(
	const TSubclassOf<UObsidianInventoryItemFragment> InFragmentClass) const
{
	if((ItemDef != nullptr) && (InFragmentClass != nullptr))
	{
		return GetDefault<UObsidianInventoryItemDefinition>(ItemDef)->FindFragmentByClass(InFragmentClass);
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

int8 UObsidianInventoryItemInstance::GetItemLevel() const
{
#if !UE_BUILD_SHIPPING
	if (ItemLevel == INDEX_NONE)
	{
		UE_LOG(ObLogItems, Error, TEXT("Item [%s] has invalid Item Level."), *DebugName);
	}
#endif
	return ItemLevel;
}

void UObsidianInventoryItemInstance::SetItemLevel(const int8 InItemLevel)
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

bool UObsidianInventoryItemInstance::IsUniqueOrSet() const
{
	return ItemRarity == EObsidianItemRarity::Unique || ItemRarity == EObsidianItemRarity::Set;
}

bool UObsidianInventoryItemInstance::IsMagicOrRare() const
{
	return ItemRarity == EObsidianItemRarity::Magic || ItemRarity == EObsidianItemRarity::Rare;
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

void UObsidianInventoryItemInstance::SetUsable(const bool InIsUsable)
{
	bUsable = InIsUsable;
}

bool UObsidianInventoryItemInstance::IsItemUsable() const
{
	return bUsable;
}

void UObsidianInventoryItemInstance::SetUsableShard(UObsidianUsableShard* InUsableShard)
{
	UsableShard = InUsableShard;
}

bool UObsidianInventoryItemInstance::UseItem(AObsidianPlayerController* InItemOwner,
	UObsidianInventoryItemInstance* InUsingOntoInstance)
{
	if(UsableShard)
	{
		return UsableShard->OnItemUsed(InItemOwner, this, InUsingOntoInstance);
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

bool UObsidianInventoryItemInstance::FireItemUseUIContext(const TArray<UObsidianInventoryItemInstance*>& InAllItems,
	FObsidianItemsMatchingUsableContext& OutItemsMatchingContext) const
{
	if(UsableShard)
	{
		UsableShard->OnItemUsed_UIContext(InAllItems, OutItemsMatchingContext);
	}
	return OutItemsMatchingContext.HasAnyMatchingItems();
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

TArray<FObsidianActiveItemAffix> UObsidianInventoryItemInstance::GetAllItemPrefixesAndSuffixes() const
{
	return ItemAffixes.GetAllItemPrefixesAndSuffixes();
}

void UObsidianInventoryItemInstance::InitializeAffixes(const TArray<FObsidianActiveItemAffix>& InAffixesToInitialize)
{
	if (InAffixesToInitialize.IsEmpty())
	{
		return;
	}
	
	ItemAffixes.InitializeAffixes(this, InAffixesToInitialize);
}

void UObsidianInventoryItemInstance::AddAffix(const FObsidianActiveItemAffix& InAffixToAdd)
{
	ItemAffixes.AddAffix(this, InAffixToAdd);
}

bool UObsidianInventoryItemInstance::RemoveAffix(const FGameplayTag& InAffixTag)
{
	const bool bSuccess = ItemAffixes.RemoveAffix(InAffixTag);
	return bSuccess;
}

bool UObsidianInventoryItemInstance::RemoveSkillImplicitAffix()
{
	const bool bSuccess = ItemAffixes.RemoveSkillImplicitAffix();
	return bSuccess;
}

bool UObsidianInventoryItemInstance::RemoveAllPrefixesAndSuffixes()
{
	const bool bSuccess = ItemAffixes.RemoveAllPrefixesAndSuffixes();
	return bSuccess;
}

bool UObsidianInventoryItemInstance::CanAddPrefix() const
{
	return GetItemPrefixLimit() > GetItemAddedPrefixCount();
}

bool UObsidianInventoryItemInstance::CanAddSuffix() const
{
	return GetItemSuffixLimit() > GetItemAddedSuffixCount();
}

bool UObsidianInventoryItemInstance::CanAddPrefixOrSuffix() const
{
	return GetItemCombinedPrefixSuffixLimit() > (GetItemAddedPrefixCount() + GetItemAddedSuffixCount());
}

bool UObsidianInventoryItemInstance::HasSkillImplicitAffix() const
{
	return ItemAffixes.HasSkillImplicit();
}

bool UObsidianInventoryItemInstance::HasImplicitAffix() const
{
	return ItemAffixes.HasImplicit();
}

uint8 UObsidianInventoryItemInstance::GetItemCombinedAffixLimit() const
{
	if (const UObsidianItemDataDeveloperSettings* ItemDataSettings = GetDefault<UObsidianItemDataDeveloperSettings>())
	{
		const uint8 CombinedLimit = ItemDataSettings->GetMaxPrefixCountForRarity(ItemRarity) +
			ItemDataSettings->GetMaxSuffixCountForRarity(ItemRarity); //TODO(intrxx) Add Skill Affixes and Implicits
		return CombinedLimit;
	}
	
	ensureAlwaysMsgf(false, TEXT("Could not get ItemDataSettings! Returning INDEX_NONE from [%hs] for [%s]."),
		__FUNCTION__, *GetNameSafe(this));
	return INDEX_NONE;
}

uint8 UObsidianInventoryItemInstance::GetItemCombinedPrefixSuffixLimit() const
{
	if (const UObsidianItemDataDeveloperSettings* ItemDataSettings = GetDefault<UObsidianItemDataDeveloperSettings>())
	{
		const uint8 CombinedLimit = ItemDataSettings->GetMaxPrefixCountForRarity(ItemRarity) +
			ItemDataSettings->GetMaxSuffixCountForRarity(ItemRarity);
		return CombinedLimit;
	}
	
	ensureAlwaysMsgf(false, TEXT("Could not get ItemDataSettings! Returning INDEX_NONE from [%hs] for [%s]."),
		__FUNCTION__, *GetNameSafe(this));
	return INDEX_NONE;
}

uint8 UObsidianInventoryItemInstance::GetItemPrefixLimit() const
{
	if (const UObsidianItemDataDeveloperSettings* ItemDataSettings = GetDefault<UObsidianItemDataDeveloperSettings>())
	{
		return ItemDataSettings->GetMaxPrefixCountForRarity(ItemRarity);
	}
	
	ensureAlwaysMsgf(false, TEXT("Could not get ItemDataSettings! Returning INDEX_NONE from [%hs] for [%s]."),
		__FUNCTION__, *GetNameSafe(this));
	return INDEX_NONE;
}

uint8 UObsidianInventoryItemInstance::GetItemSuffixLimit() const
{
	if (const UObsidianItemDataDeveloperSettings* ItemDataSettings = GetDefault<UObsidianItemDataDeveloperSettings>())
	{
		return ItemDataSettings->GetMaxSuffixCountForRarity(ItemRarity);
	}
	
	ensureAlwaysMsgf(false, TEXT("Could not get ItemDataSettings! Returning INDEX_NONE from [%hs] for [%s]."),
		__FUNCTION__, *GetNameSafe(this));
	return INDEX_NONE;
}

uint8 UObsidianInventoryItemInstance::GetItemAddedTotalAffixCount() const
{
	return ItemAffixes.GetTotalAffixCount();
}

uint8 UObsidianInventoryItemInstance::GetItemAddedPrefixAndSuffixCount() const
{
	return ItemAffixes.GetPrefixAndSuffixCount();
}

uint8 UObsidianInventoryItemInstance::GetItemAddedSuffixCount() const
{
	return ItemAffixes.GetSuffixCount();
}

uint8 UObsidianInventoryItemInstance::GetItemAddedPrefixCount() const
{
	return ItemAffixes.GetPrefixCount();
}

void UObsidianInventoryItemInstance::AddItemStackCount(const FGameplayTag InToTag, const int32 InStackCount)
{
	ItemStackTags.AddStack(InToTag, InStackCount);
}

void UObsidianInventoryItemInstance::RemoveItemStackCount(const FGameplayTag InFromTag, const int32 InStackCount)
{
	ItemStackTags.RemoveStack(InFromTag, InStackCount);
}

void UObsidianInventoryItemInstance::OverrideItemStackCount(const FGameplayTag InTag, const int32 InNewStackCount)
{
	ItemStackTags.OverrideStack(InTag, InNewStackCount);
}

int32 UObsidianInventoryItemInstance::GetItemStackCount(const FGameplayTag InTag) const
{
	return ItemStackTags.GetStackCount(InTag);
}

bool UObsidianInventoryItemInstance::HasStackCountForTag(const FGameplayTag InTag) const
{
	return ItemStackTags.ContainsTag(InTag);
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

void UObsidianInventoryItemInstance::SetItemGridSpan(const FIntPoint& InGridSpanToSet)
{
	ItemGridSpan = InGridSpanToSet;
}

FObsidianItemPosition UObsidianInventoryItemInstance::GetItemCurrentPosition() const
{
	return ItemCurrentPosition;
}

void UObsidianInventoryItemInstance::SetItemCurrentPosition(const FObsidianItemPosition& InCurrentPositionToSet)
{
	ItemCurrentPosition = InCurrentPositionToSet;	
}

void UObsidianInventoryItemInstance::ResetItemCurrentPosition()
{
	ItemCurrentPosition = FIntPoint::NoneValue;
}

UTexture2D* UObsidianInventoryItemInstance::GetItemImage() const
{
	return ItemImage;
}

void UObsidianInventoryItemInstance::SetItemImage(UTexture2D* InItemImageToSet)
{
	ItemImage = InItemImageToSet;
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

void UObsidianInventoryItemInstance::SetEquipmentActors(const TArray<FObsidianEquipmentActor>& InEquipmentActors)
{
	if(InEquipmentActors.IsEmpty()) // This is a valid case since I don't plan to support any body armor equipment.
	{
		return;
	}
	
	ActorsToSpawn.Empty(InEquipmentActors.Num());
	ActorsToSpawn.Append(InEquipmentActors);
}

void UObsidianInventoryItemInstance::SpawnEquipmentActors(const FGameplayTag& InSlotTag)
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
			NewActor->AssociatedSlotTag = InSlotTag;
			if(SpawnInfo.bOverrideAttachSocket)
			{
				SpawnInfo.OverrideAttachSocket(InSlotTag);
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

bool UObsidianInventoryItemInstance::HasEquippingRequirements() const
{
	if (EquippingRequirements.bInitialized)
	{
		return EquippingRequirements.bHasAnyRequirements;
	}

	return UObsidianItemsFunctionLibrary::HasEquippingRequirements(EquippingRequirements);
}

FObsidianItemRequirements UObsidianInventoryItemInstance::GetEquippingRequirements() const
{
	return EquippingRequirements;
}

void UObsidianInventoryItemInstance::InitializeEquippingRequirements(const FObsidianItemRequirements& InRequirements)
{
	EquippingRequirements = InRequirements;
}

void UObsidianInventoryItemInstance::SetCanHaveAffixes(const bool bInCanHaveAffixes)
{
	bCanHaveAffixes = bInCanHaveAffixes;
}

bool UObsidianInventoryItemInstance::CanHaveAffixes() const
{
	return bCanHaveAffixes;
}

TArray<UObsidianAffixAbilitySet*> UObsidianInventoryItemInstance::GetAffixAbilitySetsFromItem() const
{
	TArray<UObsidianAffixAbilitySet*> SetsToReturn;
	for (const FObsidianActiveItemAffix& Affix : GetAllItemAffixes())
	{
		check(Affix);
		//TODO(intrxx) Preload at item drop?
		SetsToReturn.Add(Affix.SoftAbilitySetToApply.LoadSynchronous());
	}
	return SetsToReturn;
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

void UObsidianInventoryItemInstance::SetGeneratedNameAdditions(const FObsidianItemGeneratedNameData& InNameData)
{
	ItemNameAdditionsData = InNameData;
}

FString UObsidianInventoryItemInstance::GetRareItemDisplayNameAddition() const
{
	return ItemNameAdditionsData.RareItemDisplayNameAddition;
}

void UObsidianInventoryItemInstance::SetRareItemDisplayNameAddition(const FString& InItemNameAddition)
{
	ItemNameAdditionsData.RareItemDisplayNameAddition = InItemNameAddition;
}

FString UObsidianInventoryItemInstance::GetMagicAffixMultiplierItemDisplayNameAddition() const
{
	return ItemNameAdditionsData.MagicItemDisplayNameAddition;
}

void UObsidianInventoryItemInstance::SetMagicAffixMultiplierItemDisplayNameAddition(const FString& InItemNameAddition)
{
	ItemNameAdditionsData.MagicItemDisplayNameAddition = InItemNameAddition;
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

void UObsidianInventoryItemInstance::ConstructSaveItem(FObsidianSavedItem& OutSavedItem)
{
	// General
	OutSavedItem.UniqueItemID = ItemUniqueID.ToString();
	OutSavedItem.ItemLevel = ItemLevel;
	OutSavedItem.SoftItemDef = ItemDef;
	OutSavedItem.ItemCategory = ItemCategory;
	OutSavedItem.ItemBaseType = ItemBaseType;
	OutSavedItem.ItemRarity = ItemRarity;
	OutSavedItem.ItemCurrentPosition = ItemCurrentPosition;

	// Usability
	OutSavedItem.bUsable = bUsable;
	if (OutSavedItem.bUsable)
	{
		OutSavedItem.UsableItemType = UsableItemType;
		OutSavedItem.UsableShard = UsableShard;
	}
	
	// Equipping
	OutSavedItem.bEquippable = bEquippable;
	if (OutSavedItem.bEquippable)
	{
		OutSavedItem.bNeedsTwoSlots = bNeedsTwoSlots;
		OutSavedItem.SavedEquipmentPieces.Append(ActorsToSpawn);
		OutSavedItem.EquippingRequirements = EquippingRequirements;
	}

	// Affixes
	OutSavedItem.bIdentified = bIdentified;
	OutSavedItem.SavedAffixes = GetAllItemAffixes();

	// Stacks
	OutSavedItem.bStackable = bStackable;
	if (OutSavedItem.bStackable)
	{
		OutSavedItem.ItemCurrentStacks = GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
		OutSavedItem.ItemMaxStacks = GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Max);
		OutSavedItem.ItemLimitStacks = GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Limit);
	}
	else
	{
		OutSavedItem.ItemCurrentStacks = GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	}
	
	// Appearance
	OutSavedItem.ItemGridSpan = ItemGridSpan;
	OutSavedItem.SoftItemImage = ItemImage;
	OutSavedItem.SoftItemDroppedMesh = ItemDroppedMesh;
	OutSavedItem.ItemDisplayName = ItemDisplayName.ToString();
	OutSavedItem.ItemNameAdditionsData_RareItemDisplayNameAddition = ItemNameAdditionsData.RareItemDisplayNameAddition;
	OutSavedItem.ItemNameAdditionsData_MagicItemDisplayNameAddition = ItemNameAdditionsData.MagicItemDisplayNameAddition;
	OutSavedItem.ItemDescription = ItemDescription.ToString();
	OutSavedItem.ItemAdditionalDescription = ItemAdditionalDescription.ToString();
	OutSavedItem.ItemSlotPadding = ItemSlotPadding;
	
	// Debug
	OutSavedItem.DebugItemName = DebugName;
}

void UObsidianInventoryItemInstance::ConstructFromSavedItem(const FObsidianSavedItem& InSavedItem)
{
	// General
	if (FGuid::Parse(InSavedItem.UniqueItemID, ItemUniqueID) == false)
	{
		UE_LOG(ObLogItems, Error, TEXT("Fail to load unique item ID from string [%s]"), *InSavedItem.UniqueItemID);
	}
	ItemLevel = InSavedItem.ItemLevel;
	ItemDef = InSavedItem.SoftItemDef.LoadSynchronous();
	ItemCategory = InSavedItem.ItemCategory;
	ItemBaseType = InSavedItem.ItemBaseType;
	ItemRarity = InSavedItem.ItemRarity;
	ItemCurrentPosition = InSavedItem.ItemCurrentPosition;

	// Usability
	bUsable = InSavedItem.bUsable;
	if (bUsable)
	{
		UsableItemType = InSavedItem.UsableItemType;
		UsableShard = InSavedItem.UsableShard.LoadSynchronous();
	}

	// Equipping
	bEquippable = InSavedItem.bEquippable;
	if (bEquippable)
	{
		bNeedsTwoSlots = InSavedItem.bNeedsTwoSlots;
		ActorsToSpawn.Append(InSavedItem.SavedEquipmentPieces);
		EquippingRequirements = InSavedItem.EquippingRequirements;
	}

	// Affixes
	bIdentified = InSavedItem.bIdentified;
	InitializeAffixes(InSavedItem.SavedAffixes);

	// Stacks
	bStackable = InSavedItem.bStackable;
	if (bStackable)
	{
		ItemStackTags.AddStack(ObsidianGameplayTags::Item::StackCount::Current, InSavedItem.ItemCurrentStacks);

		if (InSavedItem.ItemMaxStacks > 0)
		{
			ItemStackTags.AddStack(ObsidianGameplayTags::Item::StackCount::Max, InSavedItem.ItemMaxStacks);
		}
		if (InSavedItem.ItemLimitStacks > 0)
		{
			ItemStackTags.AddStack(ObsidianGameplayTags::Item::StackCount::Limit, InSavedItem.ItemLimitStacks);
		}
	}
	else
	{
		ItemStackTags.AddStack(ObsidianGameplayTags::Item::StackCount::Current, InSavedItem.ItemCurrentStacks);
	}

	// Appearance
	ItemGridSpan = InSavedItem.ItemGridSpan;
	ItemImage = InSavedItem.SoftItemImage.LoadSynchronous();
	ItemDroppedMesh = InSavedItem.SoftItemDroppedMesh.LoadSynchronous();
	ItemDisplayName = FText::FromString(InSavedItem.ItemDisplayName);
	ItemNameAdditionsData = FObsidianItemGeneratedNameData(
		InSavedItem.ItemNameAdditionsData_RareItemDisplayNameAddition,
		InSavedItem.ItemNameAdditionsData_MagicItemDisplayNameAddition);
	ItemDescription = FText::FromString(InSavedItem.ItemDescription);
	ItemAdditionalDescription = FText::FromString(InSavedItem.ItemAdditionalDescription);
	ItemSlotPadding = InSavedItem.ItemSlotPadding;
	
	// Debug
	DebugName = InSavedItem.DebugItemName;
}

