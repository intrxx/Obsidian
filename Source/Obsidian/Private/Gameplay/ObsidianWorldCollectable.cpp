// Copyright 2024 out of sCope team - Michał Ogiński

#include "Gameplay/ObsidianWorldCollectable.h"

// ~ Core
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

// ~ Project
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"

AObsidianWorldCollectable::AObsidianWorldCollectable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
	
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AObsidianWorldCollectable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PickupContent);
}

FObsidianPickupContent AObsidianWorldCollectable::GetPickupContent() const
{
	return PickupContent;
}

FObsidianPickupInstance AObsidianWorldCollectable::GetPickupInstanceFromPickupContent() const
{
	FObsidianPickupInstance PickupInstance = GetPickupContent().Instance;
	if(!PickupInstance.IsValid())
	{
		return FObsidianPickupInstance(nullptr);
	}
	return PickupInstance;
}

FObsidianPickupTemplate AObsidianWorldCollectable::GetPickupTemplateFromPickupContent() const
{
	FObsidianPickupTemplate PickupTemplate = GetPickupContent().Template;
	if(!PickupTemplate.IsValid())
	{
		return FObsidianPickupTemplate(nullptr, -1);
	}
	return PickupTemplate;
}

void AObsidianWorldCollectable::AddItemInstance(UObsidianInventoryItemInstance* InstanceToAdd)
{
	checkf(InstanceToAdd, TEXT("Provided InstanceToAdd is invalid in AObsidianWorldCollectable::AddItemInstance."));
	PickupContent.Instance = FObsidianPickupInstance(InstanceToAdd);
	
	if(InstanceToAdd && IsUsingRegisteredSubObjectList())
	{
		AddReplicatedSubObject(InstanceToAdd);
	}
}

void AObsidianWorldCollectable::AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const FObsidianItemGeneratedData& InGeneratedData)
{
	checkf(ItemDef, TEXT("Provided ItemDef is invalid in AObsidianWorldCollectable::AddItemDefinition."));
	PickupContent.Template = FObsidianPickupTemplate(ItemDef, InGeneratedData);

	if(ItemDef && IsUsingRegisteredSubObjectList())
	{
		AddReplicatedSubObject(ItemDef);
	}
}

void AObsidianWorldCollectable::OverrideTemplateStacks(const int32 NewItemStacks)
{
	if(PickupContent.Template.IsValid())
	{
		PickupContent.Template.ItemGeneratedData.StackCount = NewItemStacks;
	}
}

bool AObsidianWorldCollectable::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething =  Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	UObsidianInventoryItemInstance* Instance = PickupContent.Instance.Item;
	if(Instance && IsValid(Instance))
	{
		WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
	}
	
	const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = PickupContent.Template.ItemDef;
	if(ItemDef && IsValid(ItemDef))
	{
		WroteSomething |= Channel->ReplicateSubobject(ItemDef, *Bunch, *RepFlags);
	}

	return WroteSomething;
}

void AObsidianWorldCollectable::OnRep_PickupContent()
{
}

bool AObsidianWorldCollectable::CarriesItemInstance() const
{
	if(PickupContent.Instance.IsValid())
	{
		return true;
	}
	return false;
}

bool AObsidianWorldCollectable::CarriesItemDef() const
{
	if(PickupContent.Template.IsValid())
	{
		return true;
	}
	return false;
}

bool AObsidianWorldCollectable::CarriesBoth() const
{
	const FObsidianPickupContent Content = GetPickupContent();
	if((Content.Instance.IsValid()) && (Content.Template.IsValid()))
	{
		return true;
	}
	return false;
}


