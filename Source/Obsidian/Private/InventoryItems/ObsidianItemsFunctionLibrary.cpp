// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/ObsidianItemsFunctionLibrary.h"


#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Characters/Player/ObsidianPlayerState.h"
#include "Core/ObsidianGameplayStatics.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianInventoryItemFragment.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Stacks.h"
#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "InventoryItems/PlayerStash/ObsidianPlayerStashComponent.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Obsidian_TwoHand, "TwoHand");

const UObsidianInventoryItemFragment* UObsidianItemsFunctionLibrary::FindItemDefinitionFragment(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const TSubclassOf<UObsidianInventoryItemFragment> FragmentClass)
{
	if((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<UObsidianInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}

bool UObsidianItemsFunctionLibrary::IsTheSameItem(const UObsidianInventoryItemInstance* InstanceA, const UObsidianInventoryItemInstance* InstanceB)
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

bool UObsidianItemsFunctionLibrary::IsTheSameItem_WithDef(const UObsidianInventoryItemInstance* Instance, const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef)
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

bool UObsidianItemsFunctionLibrary::GetItemStats(const AObsidianPlayerController* OwnerPC, const UObsidianInventoryItemInstance* ItemInstance,
	FObsidianItemStats& OutItemStats)
{
	if(ItemInstance == nullptr)
	{
		return false;
	}
	
	if(ItemInstance->IsStackable())
	{
		OutItemStats.SetStacks(ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current),
			 ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Max));
	}

	OutItemStats.SetItemImage(ItemInstance->GetItemImage(), ItemInstance->GetItemGridSpan());
	OutItemStats.SetDisplayName(ItemInstance->GetItemDisplayName());
	OutItemStats.SetRareDisplayNameAddition(ItemInstance->GetRareItemDisplayNameAddition());
	OutItemStats.SetMagicDisplayNameAddition(ItemInstance->GetMagicAffixMultiplierItemDisplayNameAddition());
	OutItemStats.SetDescription(ItemInstance->GetItemDescription());
	OutItemStats.SetAdditionalDescription(ItemInstance->GetItemAdditionalDescription());

	OutItemStats.ItemRarity = ItemInstance->GetItemRarity();
	
	const bool bIdentified = ItemInstance->IsItemIdentified();
	OutItemStats.SetIdentified(bIdentified);
	if(bIdentified)
	{
		OutItemStats.SetAffixDescriptionRows(FormatItemAffixes(ItemInstance->GetAllItemAffixes()));
	}
	else
	{
		OutItemStats.SetAffixDescriptionRows(FormatUnidentifiedItemAffixes(ItemInstance->GetAllItemAffixes()));
	}

	if (ItemInstance->HasEquippingRequirements())
	{
		FObsidianItemRequirementsUIDescription RequirementsUIDescription;
		if (GenerateItemEquippingRequirementsAsUIDesc(OwnerPC, ItemInstance->GetEquippingRequirements(), RequirementsUIDescription))
		{
			OutItemStats.SetItemEquippingRequirements(RequirementsUIDescription);
		}
	}
	
	return true;
}

bool UObsidianItemsFunctionLibrary::GetItemStats_WithDef(const AObsidianPlayerController* OwnerPC, const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef,
	const FObsidianItemGeneratedData& ItemGeneratedData, FObsidianItemStats& OutItemStats)
{
	if(IsValid(ItemDef) == false || OwnerPC == nullptr)
	{
		return false;
	}

	const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef);
	if(ItemDefault == nullptr)
	{
		return false;
	}
	
	if(ItemDefault->IsStackable())
	{
		if(const UOInventoryItemFragment_Stacks* StacksFrag = Cast<UOInventoryItemFragment_Stacks>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Stacks::StaticClass())))
		{
			OutItemStats.SetStacks(ItemGeneratedData.AvailableStackCount,
				 StacksFrag->GetItemStackNumberByTag(ObsidianGameplayTags::Item_StackCount_Max));
		}
	}

	if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
	{
		OutItemStats.SetItemImage(AppearanceFrag->GetItemImage(), AppearanceFrag->GetItemGridSpanFromDesc());
		OutItemStats.SetDisplayName(AppearanceFrag->GetItemDisplayName());
		OutItemStats.SetRareDisplayNameAddition(ItemGeneratedData.NameData.RareItemDisplayNameAddition);
		OutItemStats.SetMagicDisplayNameAddition(ItemGeneratedData.NameData.MagicItemDisplayNameAddition);
		OutItemStats.SetDescription(AppearanceFrag->GetItemDescription());
		OutItemStats.SetAdditionalDescription(AppearanceFrag->GetItemAdditionalDescription());
	}

	OutItemStats.ItemRarity = ItemGeneratedData.ItemRarity;
	
	const bool bIdentified = IsDefinitionIdentified(ItemDefault, ItemGeneratedData);
	OutItemStats.SetIdentified(bIdentified);
	if (bIdentified)
	{
		OutItemStats.SetAffixDescriptionRows(FormatItemAffixes(ItemGeneratedData.ItemAffixes));
	}

	if (HasEquippingRequirements(ItemGeneratedData.ItemEquippingRequirements))
	{
		FObsidianItemRequirementsUIDescription RequirementsUIDescription;
		if (GenerateItemEquippingRequirementsAsUIDesc(OwnerPC, ItemGeneratedData.ItemEquippingRequirements, RequirementsUIDescription))
		{
			OutItemStats.SetItemEquippingRequirements(RequirementsUIDescription);
		}
	}
	
	return true;
}

bool UObsidianItemsFunctionLibrary::GenerateItemEquippingRequirementsAsUIDesc(const AObsidianPlayerController* OwnerPC,
	const FObsidianItemRequirements& Requirements, FObsidianItemRequirementsUIDescription& OutRequirementsUIDescription)
{
	if (OwnerPC == nullptr)
	{
		return false;
	}
	
	if (Requirements.bInitialized == false || Requirements.bHasAnyRequirements == false)
	{
		return false;
	}
	
	OutRequirementsUIDescription.SetHeroClassRequirement(Requirements.HeroClassRequirement, OwnerPC->GetHeroClass());

	const AObsidianPlayerState* OwnerPS = OwnerPC->GetObsidianPlayerState();
	if (OwnerPS == nullptr)
	{
		return false;
	}
	
	OutRequirementsUIDescription.SetHeroLevelRequirement(Requirements.RequiredLevel, OwnerPS->GetHeroLevel());

	const UObsidianAbilitySystemComponent* OwnerASC = OwnerPS->GetObsidianAbilitySystemComponent();
	if (OwnerASC == nullptr)
	{
		return false;
	}

	for (const FObsidianAttributeRequirement& AttributeReq : Requirements.AttributeRequirements)
	{
		OutRequirementsUIDescription.SetAttributeRequirement(AttributeReq.RequiredAttribute,
												AttributeReq.RequiredAttributeMagnitude,
												OwnerASC->GetNumericAttribute(AttributeReq.RequiredAttribute));
	}

	return true;
}

TArray<FObsidianAffixDescriptionRow> UObsidianItemsFunctionLibrary::FormatItemAffixes(const TArray<FObsidianActiveItemAffix>& ItemAffixes)
{
	TArray<FObsidianAffixDescriptionRow> AffixDescriptionRows;
	AffixDescriptionRows.Reserve(ItemAffixes.Num());
	
	for(const FObsidianActiveItemAffix& Affix : ItemAffixes)
	{
		check(Affix);
		FObsidianAffixDescriptionRow Row;
		Row.AffixTag = Affix.AffixTag;
		Row.AffixRowDescription = FText::FromString(Affix.ActiveAffixDescription);
		Row.AffixItemNameAddition = Affix.AffixItemNameAddition;
		Row.SetAffixAdditionalDescription(Affix.AffixType, Affix.GetCurrentAffixTier());
		AffixDescriptionRows.Add(Row);
	}
	return AffixDescriptionRows;
}

TArray<FObsidianAffixDescriptionRow> UObsidianItemsFunctionLibrary::FormatUnidentifiedItemAffixes(const TArray<FObsidianActiveItemAffix>& ItemAffixes)
{
	TArray<FObsidianAffixDescriptionRow> AffixDescriptionRows;
	
	for(const FObsidianActiveItemAffix& Affix : ItemAffixes)
	{
		check(Affix);
		if (Affix.AffixType == EObsidianAffixType::SkillImplicit || Affix.AffixType == EObsidianAffixType::PrimaryItemAffix)
		{
			FObsidianAffixDescriptionRow Row;
			Row.AffixTag = Affix.AffixTag;
			Row.AffixRowDescription = FText::FromString(Affix.ActiveAffixDescription);
			Row.AffixItemNameAddition = Affix.AffixItemNameAddition;
			Row.SetAffixAdditionalDescription(Affix.AffixType, Affix.GetCurrentAffixTier());
			AffixDescriptionRows.Add(Row);
		}
	}
	return AffixDescriptionRows;
}

bool UObsidianItemsFunctionLibrary::HasEquippingRequirements(const FObsidianItemRequirements& Requirements)
{
	if (Requirements.RequiredLevel > 0)
	{
		return true;
	}

	if (Requirements.HeroClassRequirement > EObsidianHeroClass::None)
	{
		return true;
	}

	for (const FObsidianAttributeRequirement& AttributeReq : Requirements.AttributeRequirements)
	{
		if (AttributeReq.RequiredAttributeMagnitude > 0)
		{
			return true;
		}
	}
	
	return false;
}

FObsidianDynamicItemAffix UObsidianItemsFunctionLibrary::GetRandomDynamicAffix(const TArray<FObsidianDynamicItemAffix>& DynamicAffixes)
{
	if (DynamicAffixes.IsEmpty())
	{
		return FObsidianDynamicItemAffix();
	}

	uint32 TotalWeight = 0;
	for (const FObsidianDynamicItemAffix& Affix : DynamicAffixes)
	{
		TotalWeight += Affix.AffixWeight;
	}

	const uint32 Roll = FMath::RandRange(0, TotalWeight);
	uint32 Cumulative = 0;
	for (const FObsidianDynamicItemAffix& Affix : DynamicAffixes)
	{
		Cumulative += Affix.AffixWeight;
		if (Roll <= Cumulative)
		{
			return Affix;
		}
	}

	checkf(false, TEXT("No Affix was returned from GetRandomDynamicAffix."))
	return FObsidianDynamicItemAffix();
}

bool UObsidianItemsFunctionLibrary::FillItemGeneratedData(FObsidianItemGeneratedData& OutGeneratedData, const UObsidianInventoryItemInstance* FromInstance)
{
	if (FromInstance)
	{
		//NOTE(intrx) Do not initialize OutGeneratedData.AvailableStackCount here!! Stacks are handled in Stash/Inventory Components.
		OutGeneratedData.ItemAffixes = FromInstance->GetAllItemAffixes();
		OutGeneratedData.ItemRarity = FromInstance->GetItemRarity();
		OutGeneratedData.NameData = FObsidianItemGeneratedNameData(FromInstance->GetRareItemDisplayNameAddition(),
			FromInstance->GetMagicAffixMultiplierItemDisplayNameAddition());
		OutGeneratedData.ItemEquippingRequirements = FromInstance->GetEquippingRequirements();
		return true;
	}
	return false;
}

void UObsidianItemsFunctionLibrary::InitializeItemInstanceWithGeneratedData(UObsidianInventoryItemInstance* Instance,
	const FObsidianItemGeneratedData& GeneratedData)
{
	if (Instance)
	{
		//NOTE(intrx) Do not initialize GeneratedData.AvailableStackCount here!! Stacks are handled in Stash/Inventory Components.
		Instance->InitializeAffixes(GeneratedData.ItemAffixes);
		Instance->SetItemRarity(GeneratedData.ItemRarity);
		Instance->SetGeneratedNameAdditions(GeneratedData.NameData);
		Instance->InitializeEquippingRequirements(GeneratedData.ItemEquippingRequirements);
	}
}

int32 UObsidianItemsFunctionLibrary::GetAmountOfStacksAllowedToAddToItem(const AActor* Owner, const UObsidianInventoryItemInstance* AddingFromInstance, const UObsidianInventoryItemInstance* InstanceToAddTo)
{
	if(Owner == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Owner is nullptr in [%hs]"), __FUNCTION__);
		return 0; 
	}
	
	const int32 CurrentStackCount = InstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	if(CurrentStackCount == 0)
	{
		return 0;
	}

	int32 CombinedStacks = 0;
	const int32 LimitStackCount = InstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Limit);
	if(LimitStackCount > 0)
	{
		int32 StacksInInventory = 0;
		int32 StacksInStash = 0;
		if(UObsidianInventoryComponent* InventoryComp = UObsidianInventoryComponent::FindInventoryComponent(Owner))
		{
			StacksInInventory = InventoryComp->FindAllStacksForGivenItem(AddingFromInstance);
		}
		if(UObsidianPlayerStashComponent* PlayerStashComp = UObsidianPlayerStashComponent::FindPlayerStashComponent(Owner))
		{
			StacksInStash = PlayerStashComp->FindAllStacksForGivenItem(AddingFromInstance);
		}

		CombinedStacks = StacksInInventory + StacksInStash;
		checkf(CombinedStacks <= LimitStackCount, TEXT("Combined Stacks of held item is already bigger than Stacks Limit for this item, something went wrong."));
		if((CombinedStacks == 1) || (CombinedStacks == LimitStackCount))
		{
			return 0;
		}
	}
	
	const int32 AddingFromInstanceCurrentStacks = AddingFromInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	const int32 StacksThatCanBeAddedToInventory = LimitStackCount == 0 ? AddingFromInstanceCurrentStacks : LimitStackCount - CombinedStacks;
	if(StacksThatCanBeAddedToInventory <= 0)
	{
		return 0;
	}
			
	const int32 MaxStackCount = InstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Max);
	const int32 AmountThatCanBeAddedToInstance = FMath::Clamp<int32>((MaxStackCount - CurrentStackCount), 0, StacksThatCanBeAddedToInventory);
	return FMath::Min<int32>(AmountThatCanBeAddedToInstance, AddingFromInstanceCurrentStacks);
}

int32 UObsidianItemsFunctionLibrary::GetAmountOfStacksAllowedToAddToItem_WithDef(const AActor* Owner, const TSubclassOf<UObsidianInventoryItemDefinition>& AddingFromItemDef, const int32 AddingFromItemDefCurrentStacks, const UObsidianInventoryItemInstance* InstanceToAddTo)
{
	const int32 CurrentStackCount = InstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	if(CurrentStackCount == 0)
	{
		return 0;
	}

	int32 CombinedStacks = 0;
	const int32 LimitStackCount = InstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Limit);
	if(LimitStackCount > 0)
	{
		int32 StacksInInventory = 0;
		int32 StacksInStash = 0;
		if(UObsidianInventoryComponent* InventoryComp = UObsidianInventoryComponent::FindInventoryComponent(Owner))
		{
			StacksInInventory = InventoryComp->FindAllStacksForGivenItem(AddingFromItemDef);
		}
		if(UObsidianPlayerStashComponent* PlayerStashComp = UObsidianPlayerStashComponent::FindPlayerStashComponent(Owner))
		{
			StacksInStash = PlayerStashComp->FindAllStacksForGivenItem(AddingFromItemDef);
		}

		CombinedStacks = StacksInInventory + StacksInStash;
		checkf(CombinedStacks <= LimitStackCount, TEXT("Combined Stacks of held item is already bigger than Stacks Limit for this item, something went wrong."));
		if((CombinedStacks == 1) || (CombinedStacks == LimitStackCount))
		{
			return 0;
		}
	}
			
	const int32 StacksThatCanBeAddedToInventory = LimitStackCount == 0 ? AddingFromItemDefCurrentStacks : LimitStackCount - CombinedStacks;
	if(StacksThatCanBeAddedToInventory <= 0)
	{
		return 0;
	}
			
	const int32 MaxStackCount = InstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Max);
	const int32 AmountThatCanBeAddedToInstance = FMath::Clamp((MaxStackCount - CurrentStackCount), 0, StacksThatCanBeAddedToInventory);
	return FMath::Min(AmountThatCanBeAddedToInstance, AddingFromItemDefCurrentStacks);
}

bool UObsidianItemsFunctionLibrary::IsItemUnique(const UObsidianInventoryItemInstance* ItemInstance)
{
	return ItemInstance->GetItemRarity() == EObsidianItemRarity::Unique;
}

bool UObsidianItemsFunctionLibrary::IsTwoHanded(const UObsidianInventoryItemInstance* ItemInstance)
{
	if (ItemInstance)
	{
		return UObsidianGameplayStatics::DoesTagMatchesAnySubTag(ItemInstance->GetItemCategoryTag(),
			TAG_Obsidian_TwoHand);
	}
	return false;
}

bool UObsidianItemsFunctionLibrary::IsTwoHanded_WithCategory(const FGameplayTag& CategoryTag)
{
	if (CategoryTag.IsValid())
	{
		return UObsidianGameplayStatics::DoesTagMatchesAnySubTag(CategoryTag, TAG_Obsidian_TwoHand);
	}
	return false;
}

FGameplayTag UObsidianItemsFunctionLibrary::GetCategoryTagFromDraggedItem(const FDraggedItem& DraggedItem)
{
	if (DraggedItem.IsEmpty())
	{
		return FGameplayTag::EmptyTag;
	}

	if (const UObsidianInventoryItemInstance* DraggedInstance = DraggedItem.Instance)
	{
		return DraggedInstance->GetItemCategoryTag();
	}

	if (const TSubclassOf<UObsidianInventoryItemDefinition> DraggedItemDef = DraggedItem.ItemDef)
	{
		if (const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(DraggedItemDef))
		{
			return ItemDefault->GetItemCategoryTag();
		}
	}
	
	return FGameplayTag::EmptyTag;
}

FGameplayTag UObsidianItemsFunctionLibrary::GetBaseTypeTagFromDraggedItem(const FDraggedItem& DraggedItem)
{
	if (DraggedItem.IsEmpty())
	{
		return FGameplayTag::EmptyTag;
	}

	if (const UObsidianInventoryItemInstance* DraggedInstance = DraggedItem.Instance)
	{
		return DraggedInstance->GetItemBaseTypeTag();
	}

	if (const TSubclassOf<UObsidianInventoryItemDefinition> DraggedItemDef = DraggedItem.ItemDef)
	{
		if (const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(DraggedItemDef))
		{
			return ItemDefault->GetItemBaseTypeTag();
		}
	}
	
	return FGameplayTag::EmptyTag;
}

bool UObsidianItemsFunctionLibrary::GetItemCategoryAndBaseItemTypeTagsFromDraggedItem(const FDraggedItem& DraggedItem,
	FGameplayTag& OutCategoryTag, FGameplayTag& OutItemBaseTypeTag)
{
	if (DraggedItem.IsEmpty())
	{
		return false;
	}

	if (const UObsidianInventoryItemInstance* DraggedInstance = DraggedItem.Instance)
	{
		OutCategoryTag = DraggedInstance->GetItemCategoryTag();
		OutItemBaseTypeTag = DraggedInstance->GetItemBaseTypeTag();
		return true;
	}

	if (const TSubclassOf<UObsidianInventoryItemDefinition> DraggedItemDef = DraggedItem.ItemDef)
	{
		if (const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(DraggedItemDef))
		{
			OutCategoryTag = ItemDefault->GetItemCategoryTag();
			OutItemBaseTypeTag = ItemDefault->GetItemBaseTypeTag();
			return true;
		}
	}
	return false;
}

FIntPoint UObsidianItemsFunctionLibrary::GetGridSpanFromDraggedItem(const FDraggedItem& DraggedItem)
{
	if (DraggedItem.IsEmpty())
	{
		return FIntPoint::NoneValue;
	}
	
	if(const UObsidianInventoryItemInstance* Instance = DraggedItem.Instance)
	{
		return Instance->GetItemGridSpan();
	}
	
	if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem.ItemDef)
	{
		if(const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef))
		{
			if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(
				ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
			{
				return AppearanceFrag->GetItemGridSpanFromDesc();
			}
		}
	}
	return FIntPoint::NoneValue;
}

bool UObsidianItemsFunctionLibrary::IsDefinitionIdentified(const UObsidianInventoryItemDefinition* ItemDefault, const FObsidianItemGeneratedData& ItemGeneratedData)
{
	if (ItemDefault)
	{
		/** Add any other conditions form ItemGeneratedData (Corrupted?). */
		return ItemDefault->DoesStartIdentified() || (ItemGeneratedData.ItemRarity <= EObsidianItemRarity::Normal);
	}
	return false;
}

