// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/ItemDrop/ObsidianItemDropComponent.h"

// ~ Core
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// ~ Project
#include "InventoryItems/ItemDrop/ObsidianDropItemManagerSubsystem.h"
#include "InventoryItems/ItemDrop/ObsidianTreasureList.h"
#include "InventoryItems/Items/ObsidianDroppableItem.h"

DEFINE_LOG_CATEGORY(LogDropComponent);

UObsidianItemDropComponent::UObsidianItemDropComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	
}


void UObsidianItemDropComponent::InitializeWithItemDropComponent()
{
	//TODO possibly initialize things like treasure class, area level etc.?
}

void UObsidianItemDropComponent::DropItems(const FVector& InOverrideDropLocation)
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
	
	TArray<FObsidianTreasureClass> TreasureClasses;
	
	bool bRollFromCommonSet = true;
	for (const FObsidianAdditionalTreasureList& AdditionalTreasureList : AdditionalTreasureLists) 
	{
		UObsidianTreasureList* TreasureListToAdd = AdditionalTreasureList.TreasureList.LoadSynchronous();
		if (TreasureListToAdd == nullptr)
		{
			continue;
		}

		const EObsidianAdditionalTreasureListPolicy Policy = AdditionalTreasureList.TreasureListPolicy;
		if (Policy == EObsidianAdditionalTreasureListPolicy::OverrideRoll)
		{
			
#if !UE_BUILD_SHIPPING
			if (TreasureClasses.IsEmpty() == false)
			{
				UE_LOG(LogDropComponent, Error, TEXT("AdditionalTreasureList contains an item with OverrideRoll "
										 "Policy along with more items that are set in the designer to roll from, this "
										 "setup is incorrect, please consider changing it as it will not work anyway."));
			}
#endif
			
			TreasureClasses.Empty(); // If the setup is ever wrong there could be added items, clear for safety and warn
			TreasureClasses.Append(TreasureListToAdd->GetAllTreasureClasses());
			bRollFromCommonSet = false;
			break;
		}
			
		if (Policy == EObsidianAdditionalTreasureListPolicy::TryToRoll)
		{
			TreasureClasses.Append(TreasureListToAdd->GetAllTreasureClasses());
		}
		else if (Policy == EObsidianAdditionalTreasureListPolicy::AlwaysRoll)
		{
			TreasureListToAdd->SetShouldAlwaysRoll(true);
			TreasureClasses.Append(TreasureListToAdd->GetAllTreasureClasses());
		}
	}

	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(World);
	if (GameInstance == nullptr)
	{
		UE_LOG(LogDropComponent, Error, TEXT("GameInstance is nullptr in [%hs]"), ANSI_TO_TCHAR(__FUNCDNAME__));
		return;
	}

	UObsidianDropItemManagerSubsystem* DropItemManager = GameInstance->GetSubsystem<UObsidianDropItemManagerSubsystem>();
	if (bRollFromCommonSet && DropItemManager)
	{
		TreasureClasses.Append(DropItemManager->GetAllTreasureClassesUpToQuality(/** Temp. */ 100));
	}

	if (TreasureClasses.IsEmpty())
	{
		return;
	}

	const AActor* OwningActor = GetOwner();
	if (OwningActor == nullptr)
	{
		UE_LOG(LogDropComponent, Error, TEXT("OwningActor of ItemDropComponent is null in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
	}

	// Roll for Treasure Class first.
	if (TSubclassOf<UObsidianInventoryItemDefinition> DroppedItemDef = TreasureClasses[0].GetRandomItemFromClass())
	{
		const FTransform ItemSpawnTransform = GetDropTransformAligned(OwningActor, InOverrideDropLocation);
		AObsidianDroppableItem* Item = World->SpawnActorDeferred<AObsidianDroppableItem>(AObsidianDroppableItem::StaticClass(), ItemSpawnTransform);
		Item->InitializeItem(DroppedItemDef);
		Item->FinishSpawning(ItemSpawnTransform);

		OnDroppingItemsFinishedDelegate.Broadcast();
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



