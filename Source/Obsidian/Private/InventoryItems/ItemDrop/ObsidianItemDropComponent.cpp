// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/ItemDrop/ObsidianItemDropComponent.h"

// ~ Core
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

// ~ Project
#include "InventoryItems/Fragments/OInventoryItemFragment_Affixes.h"
#include "InventoryItems/ItemDrop/ObsidianItemDataLoaderSubsystem.h"
#include "InventoryItems/ItemDrop/ObsidianItemManagerSubsystem.h"
#include "InventoryItems/ItemDrop/ObsidianTreasureList.h"

DEFINE_LOG_CATEGORY(LogDropComponent);

// ~ FObsidianAdditionalTreasureList

#if WITH_EDITOR
EDataValidationResult FObsidianAdditionalTreasureList::ValidateData(FDataValidationContext& Context, const int Index) const
{
	EDataValidationResult Result = EDataValidationResult::Valid;
	
	if(TreasureList.IsNull())
	{
		Result = EDataValidationResult::Invalid;

		const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("Treasure List at index [%i] is empty! \n"
			"Please fill Treasure List or delete this index entry in Additional Treasure Lists"), Index));

		Context.AddError(ErrorMessage);
	}
	
	return Result;
}
#endif

// ~ End of FObsidianAdditionalTreasureList

UObsidianItemDropComponent::UObsidianItemDropComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UObsidianItemDropComponent::DropItems(const EObsidianEntityRarity DroppingEntityRarity, const uint8 DroppingEntityLevel, const FVector& InOverrideDropLocation)
{
	checkf(DroppingEntityRarity != EObsidianEntityRarity::None, TEXT("Entity Rarity passed to DropItems is None, setup or run time logic is invalid."));
	
	const UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(World);
	if (GameInstance == nullptr)
	{
		UE_LOG(LogDropComponent, Error, TEXT("GameInstance is nullptr in [%hs]"), ANSI_TO_TCHAR(__FUNCDNAME__));
		return;
	}

	const UObsidianItemDataLoaderSubsystem* ItemDataLoader = GameInstance->GetSubsystem<UObsidianItemDataLoaderSubsystem>();
	if (ItemDataLoader == nullptr)
	{
		UE_LOG(LogDropComponent, Error, TEXT("ItemDataLoader is nullptr in [%hs]"), ANSI_TO_TCHAR(__FUNCDNAME__));
		return;
	}
	
	const uint8 TreasureQuality = FMath::Clamp(
		(DroppingEntityLevel + ObsidianTreasureStatics::DefaultRarityToAddedTreasureQualityMap[DroppingEntityRarity]),
		1, ObsidianTreasureStatics::MaxTreasureQuality);
	
	TArray<FObsidianTreasureClass> TreasureClasses;
	TArray<FObsidianTreasureClass> MustRollFromTreasureClasses;
	GetTreasureClassesToRollFrom(ItemDataLoader, TreasureQuality, TreasureClasses, MustRollFromTreasureClasses);
	
	if (TreasureClasses.IsEmpty() && MustRollFromTreasureClasses.IsEmpty())
	{
		UE_LOG(LogDropComponent, Warning, TEXT("TreasureClasses are empty after getting them from both the common"
									   " set and additional lists, is enemy level to low for drops or something is broken?"));
		return;
	}

	const AActor* OwningActor = GetOwner();
	if (OwningActor == nullptr)
	{
		UE_LOG(LogDropComponent, Error, TEXT("OwningActor of ItemDropComponent is null in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
	}
	
	uint8 DropRolls = ObsidianTreasureStatics::DefaultRarityToNumberOfDropRollsMap[DroppingEntityRarity];
	TArray<FObsidianDropItem> ItemsToDrop;
	
	if (MustRollFromTreasureClasses.IsEmpty() == false && DropRolls > 0)
	{
		//TODO(intrxx) Get Random TC in some weighted way?
		const uint16 RandomClassIndex = FMath::RandRange(0, (MustRollFromTreasureClasses.Num() - 1));
		FObsidianDropItem RolledItem = MustRollFromTreasureClasses[RandomClassIndex].GetRandomItemFromClass();
		if (RolledItem.IsValid())
		{
			RolledItem.DropStacks = RolledItem.GetRandomStackSizeToDropAdjusted(TreasureQuality); //TODO(intrxx) shouldn't I just set it in the function?
			RolledItem.DropTransform = GetDropTransformAligned(OwningActor, InOverrideDropLocation);
			RollItemAffixes(ItemDataLoader, RolledItem, TreasureQuality);
			ItemsToDrop.Add(RolledItem);
		}
		DropRolls--;
	}

	const int32 TreasureClassesCount = TreasureClasses.Num();
	for (uint8 i = 0; i < DropRolls; ++i)
	{
		//TODO(intrxx) Get Random TC in some weighted way?
		const uint16 RandomClassIndex = FMath::RandRange(0, (TreasureClassesCount - 1));
		FObsidianDropItem RolledItem = TreasureClasses[RandomClassIndex].GetRandomItemFromClass();
		if (RolledItem.IsValid())
		{
			RolledItem.DropStacks = RolledItem.GetRandomStackSizeToDropAdjusted(TreasureQuality); //TODO(intrxx) shouldn't I just set it in the function?
			RolledItem.DropTransform = GetDropTransformAligned(OwningActor, InOverrideDropLocation);
			RollItemAffixes(ItemDataLoader, RolledItem, TreasureQuality);
			ItemsToDrop.Add(RolledItem);
		}
	}

	if (ItemsToDrop.IsEmpty())
	{
		UE_LOG(LogTemp, Display, TEXT("No Items Were Rolled To Drop."));
		OnDroppingItemsFinishedDelegate.Broadcast(false);
		return;
	}

	for (const FObsidianDropItem& DropItem : ItemsToDrop)
	{
		if (!DropItem.SoftTreasureItemDefinitionClass.Get())
		{
			UE_LOG(LogDropComponent, Error, TEXT("SoftTreasureItemDefinitionClass was not loaded correctly."));
		}
	}
	
	if (const UObsidianItemManagerSubsystem* ManagerSubsystem = World->GetSubsystem<UObsidianItemManagerSubsystem>())
	{
		ManagerSubsystem->RequestDroppingItems(MoveTemp(ItemsToDrop));
		OnDroppingItemsFinishedDelegate.Broadcast(true);
	}
}

void UObsidianItemDropComponent::RollItemAffixes(const UObsidianItemDataLoaderSubsystem* FromItemDataLoader, const FObsidianDropItem& DropItem, const uint8 MaxTreasureClassQuality)
{
	if (FromItemDataLoader == nullptr)
	{
		UE_LOG(LogDropComponent, Error, TEXT("ItemDataLoader subsystem is invalid in [%hs]."), ANSI_TO_TCHAR(__FUNCTION__));
		return;
	}
	
	TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DropItem.SoftTreasureItemDefinitionClass.Get();
	if (ItemDef == nullptr)
	{
		ItemDef = DropItem.SoftTreasureItemDefinitionClass.LoadSynchronous();
		UE_LOG(LogDropComponent, Error, TEXT("SoftTreasureItemDefinitionClass wasn't loaded correctly, loading synchronously now."));
	}

	const UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject();
	if (DefaultObject == nullptr)
	{
		UE_LOG(LogDropComponent, Error, TEXT("DefaultObject of SoftTreasureItemDefinitionClass is invalid, abandoning [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
		return;
	}

	UOInventoryItemFragment_Affixes* AffixFragment = DefaultObject->GetAffixFragment_Mutable();
	if (AffixFragment == nullptr)
	{
		return;		
	}
	
	switch (AffixFragment->GetGenerationType())
	{
		case EObsidianAffixGenerationType::DefaultGeneration:
			{
				AffixFragment->RandomiseStaticAffixValues();

				const FGameplayTag ItemRarityTag = /* RollItemRarity(); */ ObsidianGameplayTags::Item_Rarity_Rare; 
				
				TArray<FObsidianDynamicItemAffix> PrefixAffixes;
				TArray<FObsidianDynamicItemAffix> SuffixAffixes;
				if (ItemRarityTag != ObsidianGameplayTags::Item_Rarity_Normal)
				{
					const bool bGatheredAffixes = FromItemDataLoader->GetAllAffixesUpToQualityForCategory_DefaultGeneration(
					MaxTreasureClassQuality, DefaultObject->GetItemCategoryTag(), PrefixAffixes, SuffixAffixes);
					if (bGatheredAffixes == false)
					{
						UE_LOG(LogDropComponent, Warning, TEXT("Could not find any Affixes for [%s] up to [%d] quality level."),
							*DefaultObject->GetItemCategoryTag().GetTagName().ToString(), MaxTreasureClassQuality);
						return;
					}
				}
				
				TArray<FObsidianDynamicItemAffix> RolledAffixes;
				uint8 AddedPrefixes = 0;
				uint8 AddedSuffixes = 0;
				const uint8 MaxPrefixCount = ObsidianTreasureStatics::GetMaxPrefixCountForRarity(ItemRarityTag);
				const uint8 MaxSuffixCount = ObsidianTreasureStatics::GetMaxSuffixCountForRarity(ItemRarityTag);
				for (uint8 i = 0; i < ObsidianTreasureStatics::GetMaxAffixCountForRarity(ItemRarityTag); ++i)
				{
					if (FMath::RandBool() && AddedPrefixes <= MaxPrefixCount) // Roll Prefix
					{
						if (PrefixAffixes.IsEmpty() == false)
						{
							//TODO(intrxx) Roll Affixes actually based on their Weights.
							FObsidianDynamicItemAffix DynamicItemAffix = PrefixAffixes[FMath::RandRange(0, PrefixAffixes.Num() - 1)];
							RolledAffixes.Add(DynamicItemAffix);
							++AddedPrefixes;
							UE_LOG(LogTemp, Warning, TEXT("Adding Affix: [%s], [%s]"), *DynamicItemAffix.AffixTag.GetTagName().ToString(),
								*DynamicItemAffix.AffixItemNameAddition.ToString());
						}
					}
					else if (AddedSuffixes <= MaxSuffixCount) // Roll Suffix
					{
						if (SuffixAffixes.IsEmpty() == false)
						{
							//TODO(intrxx) Roll Affixes actually based on their Weights.
							FObsidianDynamicItemAffix DynamicItemAffix = SuffixAffixes[FMath::RandRange(0, PrefixAffixes.Num() - 1)];
							RolledAffixes.Add(DynamicItemAffix);
							++AddedSuffixes;
							UE_LOG(LogTemp, Warning, TEXT("Adding Affix: [%s], [%s]"), *DynamicItemAffix.AffixTag.GetTagName().ToString(),
								*DynamicItemAffix.AffixItemNameAddition.ToString());
						}
					}
				}

				//TODO(intrxx) I shouldn't set it on fragment, that does not make any sense and its stupid, should be part of DroppedItem, just like stacks are.
				AffixFragment->InitializeDynamicAffixes(RolledAffixes, ItemRarityTag);
			} break;
		case EObsidianAffixGenerationType::FullGeneration:
			{
				TArray<FObsidianDynamicItemAffix> ImplicitAffixes;
				TArray<FObsidianDynamicItemAffix> PrefixAffixes;
				TArray<FObsidianDynamicItemAffix> SuffixAffixes;
				const bool bGatheredAffixes = FromItemDataLoader->GetAllAffixesUpToQualityForCategory_FullGeneration(
					MaxTreasureClassQuality, DefaultObject->GetItemCategoryTag(), PrefixAffixes, SuffixAffixes,
					ImplicitAffixes);
				if (bGatheredAffixes == false)
				{
					UE_LOG(LogDropComponent, Warning, TEXT("Could not find any Affixes for [%s] up to [%d] quality level."),
						*DefaultObject->GetItemCategoryTag().GetTagName().ToString(), MaxTreasureClassQuality);
					return;
				}
				
				FGameplayTag ItemRarityTag; 
				TArray<FObsidianDynamicItemAffix> RolledAffixes;
				
				//TODO(intrxx) roll dynamic affixes for both implicit and prefixes/suffixes

				//TODO(intrxx) I shouldn't set it on fragment, that does not make any sense and its stupid, should be part of DroppedItem, just like stacks are.
				AffixFragment->InitializeDynamicAffixes(RolledAffixes, ItemRarityTag);
			} break;
		case EObsidianAffixGenerationType::NoGeneration:
			{
				AffixFragment->RandomiseStaticAffixValues();
			} break;
			default:
			{} break;
	}
}

void UObsidianItemDropComponent::BeginPlay()
{
	Super::BeginPlay();

	LoadAdditionalTreasuresAsync();
}

void UObsidianItemDropComponent::LoadAdditionalTreasuresAsync()
{
	TArray<FSoftObjectPath> AdditionalTreasureListsPaths;
	for (const FObsidianAdditionalTreasureList& AdditionalTL : AdditionalTreasureLists)
	{
		AdditionalTreasureListsPaths.Add(AdditionalTL.TreasureList.ToSoftObjectPath());
	}
	
	UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(AdditionalTreasureListsPaths);
}

void UObsidianItemDropComponent::GetTreasureClassesToRollFrom(const UObsidianItemDataLoaderSubsystem* FromItemDataLoader, const uint8 MaxTreasureClassQuality, TArray<FObsidianTreasureClass>& OutTreasureClasses, TArray<FObsidianTreasureClass>& OutMustRollFromTreasureClasses)
{
	bool bRollFromCommonSet = true;
	for (const FObsidianAdditionalTreasureList& AdditionalTreasureList : AdditionalTreasureLists) 
	{
		UObsidianTreasureList* TreasureListToAdd = AdditionalTreasureList.TreasureList.Get();
		if (TreasureListToAdd == nullptr)
		{
			TreasureListToAdd = AdditionalTreasureList.TreasureList.LoadSynchronous();
			UE_LOG(LogDropComponent, Warning, TEXT("AdditionalTreasureList wasn't loaded correctly and needed to be loaded Synchronously."));
			if (TreasureListToAdd)
			{
				UE_LOG(LogDropComponent, Error, TEXT("AdditionalTreasureList was invalid in [%hs]."), __FUNCDNAME__);
				continue;
			}
		}

		const EObsidianAdditionalTreasureListPolicy Policy = AdditionalTreasureList.TreasureListPolicy;
		if (Policy == EObsidianAdditionalTreasureListPolicy::OverrideRoll)
		{
			TArray<FObsidianTreasureClass> ClassesToAdd = TreasureListToAdd->GetAllTreasureClasses();
			if (ClassesToAdd.IsEmpty() == false)
			{
				// This is safe as the AdditionalTreasureLists have strict Data Validation
				OutTreasureClasses.Append(ClassesToAdd);
				bRollFromCommonSet = false;
				continue;
			}
			UE_LOG(LogDropComponent, Error, TEXT("AdditionalTreasureLists contains List with OverrideRoll Policy but is empty."));
		}
			
		if (Policy == EObsidianAdditionalTreasureListPolicy::TryToRoll)
		{
			OutTreasureClasses.Append(TreasureListToAdd->GetAllTreasureClassesUpToQuality(MaxTreasureClassQuality));
		}
		else if (Policy == EObsidianAdditionalTreasureListPolicy::TryToAddAlwaysRoll)
		{
			OutMustRollFromTreasureClasses.Append(TreasureListToAdd->GetAllTreasureClassesUpToQuality(MaxTreasureClassQuality));
		}
		else if (Policy == EObsidianAdditionalTreasureListPolicy::AlwaysRoll)
		{
			OutMustRollFromTreasureClasses.Append(TreasureListToAdd->GetAllTreasureClasses());
		}
	}

	if (bRollFromCommonSet == false)
	{
		return;
	}
	
	if (FromItemDataLoader == nullptr)
	{
		UE_LOG(LogDropComponent, Error, TEXT("ItemDataLoader subsystem is invalid in [%hs]."), ANSI_TO_TCHAR(__FUNCTION__));
		return;
	}

	if (FromItemDataLoader->GetAllTreasureClassesUpToQuality(MaxTreasureClassQuality, OutTreasureClasses) == false)
	{
		UE_LOG(LogDropComponent, Error, TEXT("Gathering TreasureClasses failed in [%hs]."), ANSI_TO_TCHAR(__FUNCTION__));
	}
}

FTransform UObsidianItemDropComponent::GetDropTransformAligned(const AActor* DroppingActor, const FVector& InOverrideDropLocation) const
{
	FTransform InvalidTransform = FTransform::Identity;
	
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return InvalidTransform;
	}
	
	if (DroppingActor == nullptr)
	{
		UE_LOG(LogDropComponent, Error, TEXT("DroppingActor is null in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
		return InvalidTransform;
	}

	FVector DropLocation = FVector::ZeroVector;
	if (InOverrideDropLocation == FVector::ZeroVector)
	{
		const FVector OwnerLocation = DroppingActor->GetActorLocation();
		UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetCurrent(World);

		if(NavigationSystem == nullptr)
		{
			UE_LOG(LogDropComponent, Error, TEXT("NavigationSystem is null in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
			return InvalidTransform;
		}
		
		FNavLocation RandomPointLocation;
		const bool bFound = NavigationSystem->GetRandomPointInNavigableRadius(OwnerLocation, ItemDropRadius, RandomPointLocation);
		if (bFound == false)
		{
			//TODO Change the location to somewhere valid.
			UE_LOG(LogDropComponent, Error, TEXT("Could not initially find a valid drop location in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
		}
		
		DropLocation = RandomPointLocation.Location;
	}
	else
	{
		DropLocation = InOverrideDropLocation;
	}

	FHitResult GroundTraceResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(DroppingActor);
	const FVector GroundTraceEndLocation = FVector(DropLocation.X, DropLocation.Y, DropLocation.Z - 300.0f);
	World->LineTraceSingleByChannel(GroundTraceResult, DropLocation, GroundTraceEndLocation, ECC_Visibility, QueryParams);

	FRotator ItemRotation = FRotator::ZeroRotator;
	if(GroundTraceResult.bBlockingHit) // We are able to align the item to the ground better
	{
		FVector RandomisedRotationVector = FMath::VRand().GetSafeNormal();
		ItemRotation = UKismetMathLibrary::MakeRotFromZY(GroundTraceResult.ImpactNormal, RandomisedRotationVector);
		DropLocation = GroundTraceResult.Location;
	}

	return FTransform(ItemRotation, DropLocation, FVector(1.0f, 1.0f, 1.0f));
}

FGameplayTag UObsidianItemDropComponent::RollItemRarity(const FGameplayTag& MaxItemRarityTag)
{
	TMap<FGameplayTag, uint16> RarityToWeightMap = ObsidianTreasureStatics::DefaultRarityToWeightMap;
	//TODO(intrxx) Implement MaxItemRarity
	
	uint32 MaxWeight = 0;
	for (const TPair<FGameplayTag, uint16>& RarityWithWeight : RarityToWeightMap)
	{
		MaxWeight += RarityWithWeight.Value;
	}

	const uint32 RolledWeight = FMath::RandRange(0, MaxWeight);
	uint32 Cumulative = 0;
	for (const TPair<FGameplayTag, uint16>& RarityWithWeight : RarityToWeightMap)
	{
		Cumulative += RarityWithWeight.Value;
		if (RolledWeight <= Cumulative)
		{
			return RarityWithWeight.Key;
		}
	}

	ensureAlways(false);
	return ObsidianGameplayTags::Item_Rarity_Normal;
}

#if WITH_EDITOR
EDataValidationResult UObsidianItemDropComponent::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	uint16 TreasureClassesIndex = 0;
	TArray<EObsidianAdditionalTreasureListPolicy> Policies;
	bool bContainsOverridePolicy = false;
	for (const FObsidianAdditionalTreasureList& Class : AdditionalTreasureLists)
	{
		Result = CombineDataValidationResults(Result, Class.ValidateData(Context, TreasureClassesIndex));
		EObsidianAdditionalTreasureListPolicy ClassPolicy = Class.TreasureListPolicy;
		if (ClassPolicy == EObsidianAdditionalTreasureListPolicy::OverrideRoll)
		{
			bContainsOverridePolicy = true;
		}
		Policies.Add(ClassPolicy);
		TreasureClassesIndex++;
	}

	if (bContainsOverridePolicy)
	{
		for (const EObsidianAdditionalTreasureListPolicy Policy : Policies)
		{
			if (Policy != EObsidianAdditionalTreasureListPolicy::OverrideRoll)
			{
				Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
				
				const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("Additional Treasure Lists contains at least one different Policy (different than OverrideRoll) while containing OverrideRoll Policy! \n"
							"This is invalid and will lead to undefined behaviour, please make sure to change the setup")));

				Context.AddError(ErrorMessage);
			}
		}
	}
	
	return Result;
}
#endif



