// Copyright 2024 out of sCope team - Michał Ogiński


#include "InventoryItems/Equipment/OOSEquipmentComponent.h"

// Sets default values for this component's properties
UOOSEquipmentComponent::UOOSEquipmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	// ...
}


// Called when the game starts
void UOOSEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UOOSEquipmentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

