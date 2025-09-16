// Copyright 2024 out of sCope team - Michał Ogiński

// ~ Core

// ~ Project
#include "InventoryItems/ItemDrop/ObsidianItemDropComponent.h"

#include "NavigationSystem.h"
#include "InventoryItems/Items/ObsidianDroppableItem.h"
#include "Kismet/KismetMathLibrary.h"

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
	
	//TODO Get the item

	const AActor* OwningActor = GetOwner();
	if (OwningActor == nullptr)
	{
		UE_LOG(LogDropComponent, Error, TEXT("OwningActor of ItemDropComponent is null in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
	}
	
	const FTransform ItemSpawnTransform = GetDropTransformAligned(OwningActor, InOverrideDropLocation);

	UE_LOG(LogDropComponent, Error, TEXT("Dropped Item! (Debug Sphere) in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
	DrawDebugSphere(World, ItemSpawnTransform.GetLocation(), 32.0f, 16, FColor::Orange, false, 8.0f);
	
	if (!TempItemDropTable.IsEmpty())
	{
		AObsidianDroppableItem* Item = World->SpawnActorDeferred<AObsidianDroppableItem>(AObsidianDroppableItem::StaticClass(), ItemSpawnTransform);
		Item->InitializeItem(TempItemDropTable[0]);
		Item->FinishSpawning(ItemSpawnTransform);
	}

	//TODO Broadcast it when actually finished
	OnDroppingItemsFinishedDelegate.Broadcast();
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



