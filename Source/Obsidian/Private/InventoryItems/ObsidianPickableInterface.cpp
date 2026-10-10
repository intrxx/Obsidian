// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/ObsidianPickableInterface.h"

#include "InventoryItems/ObsidianInventoryItemDefinition.h"


bool FObsidianPickupTemplate::IsValid() const
{
	return ItemDef != nullptr;
}

bool FObsidianPickupInstance::IsValid() const
{
	return Item != nullptr;
}

UObsidianPickableStatics::UObsidianPickableStatics(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
}

TScriptInterface<IObsidianPickableInterface> UObsidianPickableStatics::GetPickableFromActor(AActor* InActor)
{
	// If the actor is directly pickable, return that.
	TScriptInterface<IObsidianPickableInterface> PickupableActor(InActor);
	if(PickupableActor)
	{
		return PickupableActor;
	}

	// If the actor isn't pickable, it might have a component that has a pickupable interface.
	TArray<UActorComponent*> PickupableComponents = InActor ? InActor->GetComponentsByInterface(
		UObsidianPickableInterface::StaticClass()) : TArray<UActorComponent*>();
	if(PickupableComponents.IsEmpty() == false)
	{
		return TScriptInterface<IObsidianPickableInterface>(PickupableComponents[0]);
	}

	return TScriptInterface<IObsidianPickableInterface>();
}

