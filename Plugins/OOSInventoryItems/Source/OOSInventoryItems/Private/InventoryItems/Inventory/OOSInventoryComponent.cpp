// Copyright 2024 out of sCope team - Michał Ogiński


#include "InventoryItems/Inventory/OOSInventoryComponent.h"

UOOSInventoryComponent::UOOSInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	// ...
}


// Called when the game starts
void UOOSInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UOOSInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

