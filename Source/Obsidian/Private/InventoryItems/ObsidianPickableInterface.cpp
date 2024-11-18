// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/ObsidianPickableInterface.h"
#include "InventoryItems/ObsidianInventoryComponent.h"

UObsidianPickableStatics::UObsidianPickableStatics(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TScriptInterface<IObsidianPickableInterface> UObsidianPickableStatics::GetFirstPickableFromActor(AActor* Actor)
{
	// If the actor is directly pickable, return that.
	TScriptInterface<IObsidianPickableInterface> PickupableActor(Actor);
	if (PickupableActor)
	{
		return PickupableActor;
	}

	// If the actor isn't pickable, it might have a component that has a pickupable interface.
	TArray<UActorComponent*> PickupableComponents = Actor ? Actor->GetComponentsByInterface(UObsidianPickableInterface::StaticClass()) : TArray<UActorComponent*>();
	if (PickupableComponents.Num() > 0)
	{
		// Get first pickable, if the user needs more sophisticated pickup distinction, will need to be solved elsewhere.
		return TScriptInterface<IObsidianPickableInterface>(PickupableComponents[0]);
	}

	return TScriptInterface<IObsidianPickableInterface>();
}

void UObsidianPickableStatics::AddPickupToInventory(UObsidianInventoryComponent* InventoryComponent, const TScriptInterface<IObsidianPickableInterface> Pickup)
{
	if (InventoryComponent && Pickup)
	{
		const FPickupContent& PickupContent = Pickup->GetPickupContent();

		for (const FPickupTemplate& Template : PickupContent.Templates)
		{
			InventoryComponent->AddItemDefinition(Template.ItemDef, Template.StackCount);
		}

		for (const FPickupInstance& Instance : PickupContent.Instances)
		{
			InventoryComponent->AddItemInstance(Instance.Item);
		}
	}
}
