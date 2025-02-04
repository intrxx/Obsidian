// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/ObsidianPickableInterface.h"
#include "InventoryItems/ObsidianInventoryComponent.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"

bool FPickupTemplate::IsValid() const
{
	return ItemDef != nullptr;
}

bool FPickupInstance::IsValid() const
{
	return Item != nullptr;
}

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

		if(const FPickupTemplate Template = PickupContent.Template; Template.IsValid())
		{
			int32 OutStacksLeft = 0;
			InventoryComponent->AddItemDefinition(Template.ItemDef, /** OUT */ OutStacksLeft, Template.StackCount);
		}

		if(const FPickupInstance Instance = PickupContent.Instance; Instance.IsValid())
		{
			int32 OutStacksLeft = 0;
			InventoryComponent->AddItemInstance(Instance.Item, /** OUT */ OutStacksLeft);
		}
	}
}
