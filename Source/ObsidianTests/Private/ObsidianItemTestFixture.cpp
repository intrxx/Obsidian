// Copyright 2026 out of sCope team - intrxx

#include "ObsidianItemTestFixture.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Characters/Heroes/ObsidianHero.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Characters/Player/ObsidianPlayerState.h"
#include "InventoryItems/Equipment/ObsidianEquipmentComponent.h"
#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/PlayerStash/ObsidianPlayerStashComponent.h"
#include "InventoryItems/PlayerStash/ObsidianStashTabsConfig.h"
#include "InventoryItems/PlayerStash/Tabs/ObsidianStashTab_Grid.h"
#include "Obsidian/ObsidianGameplayTags.h"


namespace ObsidianItemTestFixture
{
	/** Stash Tabs are normally configured in a Data Asset set on the Player Controller Blueprint, tests build one in code. */
	UObsidianStashTabsConfig* CreateStashTabsConfig(UObject* InOuter)
	{
		UObsidianStashTabsConfig* Config = NewObject<UObsidianStashTabsConfig>(InOuter);

		const FArrayProperty* StashTabsProperty = FindFProperty<FArrayProperty>(UObsidianStashTabsConfig::StaticClass(), TEXT("StashTabs"));
		checkf(StashTabsProperty, TEXT("StashTabs was not found on UObsidianStashTabsConfig, was it renamed?"));
		TArray<FObsidianStashTabDefinition>* StashTabs = StashTabsProperty->ContainerPtrToValuePtr<TArray<FObsidianStashTabDefinition>>(Config);

		FObsidianStashTabDefinition PersonalTab;
		PersonalTab.StashTabType = EObsidianStashTabType::STT_GridType;
		PersonalTab.StashTabAccessabilityType = EObsidianStashTabAccessability::Personal;
		PersonalTab.StashTabName = TEXT("Test Personal");
		PersonalTab.StashTabClass = UObsidianStashTab_Grid::StaticClass();
		PersonalTab.StashTag = FObsidianItemTestEnvironment::PersonalStashTab();
		StashTabs->Add(PersonalTab);

		FObsidianStashTabDefinition SharedTab;
		SharedTab.StashTabType = EObsidianStashTabType::STT_GridType;
		SharedTab.StashTabAccessabilityType = EObsidianStashTabAccessability::Shared;
		SharedTab.StashTabName = TEXT("Test Shared");
		SharedTab.StashTabClass = UObsidianStashTab_Grid::StaticClass();
		SharedTab.StashTag = FObsidianItemTestEnvironment::SharedStashTab();
		StashTabs->Add(SharedTab);

		return Config;
	}

	void SetStashTabsConfig(UObsidianPlayerStashComponent* InStashComponent, UObsidianStashTabsConfig* InConfig)
	{
		const FObjectProperty* ConfigProperty = FindFProperty<FObjectProperty>(UObsidianPlayerStashComponent::StaticClass(), TEXT("StashTabsConfig"));
		checkf(ConfigProperty, TEXT("StashTabsConfig was not found on UObsidianPlayerStashComponent, was it renamed?"));
		ConfigProperty->SetObjectPropertyValue_InContainer(InStashComponent, InConfig);
	}
}

FObsidianItemTestEnvironment::FObsidianItemTestEnvironment()
{
	// The item lists broadcast their changes through the Gameplay Message Subsystem, which lives on the Game Instance.
	Spawner.InitializeGameSubsystems();

	// The Player Stash creates its tabs in PostInitializeComponents, so the config needs to be there before the
	// construction of the Player Controller finishes.
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.bDeferConstruction = true;
	PlayerController = &Spawner.SpawnActor<AObsidianPlayerController>(SpawnParameters);
	ObsidianItemTestFixture::SetStashTabsConfig(PlayerController->GetPlayerStashComponent(),
		ObsidianItemTestFixture::CreateStashTabsConfig(PlayerController));
	PlayerController->FinishSpawning(FTransform::Identity);

	// The test world has no Game Mode to create the Player State, which owns the Ability System Component that the
	// components ask if they can be modified.
	PlayerState = &Spawner.SpawnActor<AObsidianPlayerState>();
	PlayerController->PlayerState = PlayerState;

	// Equipping grants the item's affixes with the Hero as their source, the Hero does not need to be fully possessed for that.
	Hero = &Spawner.SpawnActor<AObsidianHero>();
	PlayerController->SetPawn(Hero);
}

UObsidianInventoryComponent& FObsidianItemTestEnvironment::Inventory() const
{
	return *PlayerController->GetInventoryComponent();
}

UObsidianEquipmentComponent& FObsidianItemTestEnvironment::Equipment() const
{
	return *PlayerController->GetEquipmentComponent();
}

UObsidianPlayerStashComponent& FObsidianItemTestEnvironment::Stash() const
{
	return *PlayerController->GetPlayerStashComponent();
}

void FObsidianItemTestEnvironment::AddOwnerTag(const FGameplayTag& InTag) const
{
	PlayerState->GetObsidianAbilitySystemComponent()->AddLooseGameplayTag(InTag);
}

void FObsidianItemTestEnvironment::RemoveOwnerTag(const FGameplayTag& InTag) const
{
	PlayerState->GetObsidianAbilitySystemComponent()->RemoveLooseGameplayTag(InTag);
}

void FObsidianItemTestEnvironment::SetHeroLevel(const uint8 InHeroLevel) const
{
	PlayerState->SetHeroLevel(InHeroLevel);
}

FGameplayTag FObsidianItemTestEnvironment::PersonalStashTab()
{
	return ObsidianGameplayTags::StashTab::Grid_1;
}

FGameplayTag FObsidianItemTestEnvironment::SharedStashTab()
{
	return ObsidianGameplayTags::StashTab::Grid_2;
}

FObsidianItemPosition FObsidianItemTestEnvironment::PersonalStashPosition(const int32 InX, const int32 InY)
{
	return FObsidianItemPosition(FIntPoint(InX, InY), PersonalStashTab());
}

FObsidianItemPosition FObsidianItemTestEnvironment::SharedStashPosition(const int32 InX, const int32 InY)
{
	return FObsidianItemPosition(FIntPoint(InX, InY), SharedStashTab());
}

FObsidianItemGeneratedData FObsidianItemTestEnvironment::MakeItemData(const int32 InStackCount)
{
	return FObsidianItemGeneratedData(InStackCount);
}

int32 FObsidianItemTestEnvironment::Stacks(const UObsidianInventoryItemInstance* InInstance)
{
	return InInstance ? InInstance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current) : 0;
}

UObsidianInventoryItemInstance* FObsidianItemTestEnvironment::MakeHeldItem(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef,
	const int32 InStackCount) const
{
	UObsidianInventoryComponent& InventoryComponent = Inventory();

	// Added on its own, so it does not get stacked onto the items the test already put in the Inventory.
	const FIntPoint FreeCorner(InventoryComponent.GetInventoryGridWidth() - 2, 0);
	const FObsidianItemOperationResult AddingResult = InventoryComponent.AddItemDefinitionToSpecifiedSlot(InItemDef, FreeCorner,
		MakeItemData(InStackCount));
	UObsidianInventoryItemInstance* HeldItem = AddingResult.AffectedInstance;
	checkf(HeldItem, TEXT("Could not create the held item, the top right corner of the Inventory needs to stay free for it."));

	const FObsidianItemOperationResult RemovingResult = InventoryComponent.RemoveItemInstance(HeldItem);
	checkf(RemovingResult.bActionSuccessful, TEXT("Could not take the held item out of the Inventory."));
	return HeldItem;
}

#endif // WITH_DEV_AUTOMATION_TESTS
