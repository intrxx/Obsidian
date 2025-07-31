// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/PlayerStash/ObsidianPlayerStashComponent.h"

// ~ Core
#include "Net/UnrealNetwork.h"

// ~ Project

UObsidianPlayerStashComponent::UObsidianPlayerStashComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, StashItemList(this)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
	
}

void UObsidianPlayerStashComponent::BeginPlay()
{
	Super::BeginPlay();

	StashItemList.InitializeStashTabs(StashTabsConfig);
}

void UObsidianPlayerStashComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, StashItemList);
}

int32 UObsidianPlayerStashComponent::FindAllStacksForGivenItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef)
{
	//TODO Implement
	return 0;
}

int32 UObsidianPlayerStashComponent::FindAllStacksForGivenItem(const UObsidianInventoryItemInstance* ItemInstance)
{
	//TODO Implement
	return 0;
}



