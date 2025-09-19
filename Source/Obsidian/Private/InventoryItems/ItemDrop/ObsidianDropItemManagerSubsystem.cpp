// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/ItemDrop/ObsidianDropItemManagerSubsystem.h"

// ~ Core
#include "Engine/AssetManager.h"

// ~ Project
#include "InventoryItems/ItemDrop/ObsidianTreasureConfig.h"
#include "InventoryItems/ItemDrop/ObsidianTreasureConfigDeveloperSettings.h"

void UObsidianDropItemManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadTreasureConfig();
}

void UObsidianDropItemManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

TArray<FObsidianTreasureClass> UObsidianDropItemManagerSubsystem::GetAllTreasureClassesUpToQuality(const int32 TreasureQuality) const
{
	TArray<FObsidianTreasureClass> GatheredClasses;
	if (TreasureConfig == nullptr)
	{
		return GatheredClasses;
	}
	
	for (TSoftObjectPtr<UObsidianTreasureList> TreasureList : TreasureConfig->CommonTreasureLists)
	{
		if (UObsidianTreasureList* LoadedList = TreasureList.LoadSynchronous())
		{
			GatheredClasses.Append(LoadedList->GetAllTreasureClassesUpToQuality(TreasureQuality));
		}
	}
	
	return GatheredClasses;
}

void UObsidianDropItemManagerSubsystem::LoadTreasureConfig()
{
	const UObsidianTreasureConfigDeveloperSettings* Settings = GetDefault<UObsidianTreasureConfigDeveloperSettings>();
	if (Settings == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ObsidianTreasureConfigDeveloperSettings not found!"));
		return;
	}

	const TSoftObjectPtr<UObsidianTreasureConfig>& ConfigRef = Settings->TreasureConfig;
	if (ConfigRef.IsNull() == false)
	{
		FSoftObjectPath TreasureConfigPath = Settings->TreasureConfig.ToSoftObjectPath();
		
		UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
				TreasureConfigPath,
				FStreamableDelegate::CreateUObject(this, &UObsidianDropItemManagerSubsystem::OnTreasureConfigLoaded)
			);
	}
}

void UObsidianDropItemManagerSubsystem::OnTreasureConfigLoaded()
{
	const UObsidianTreasureConfigDeveloperSettings* Settings = GetDefault<UObsidianTreasureConfigDeveloperSettings>();
	if (Settings && Settings->TreasureConfig)
	{
		TreasureConfig = Settings->TreasureConfig.Get();
		UE_LOG(LogTemp, Log, TEXT("Loaded asset: %s"), *TreasureConfig->GetName());
	}
}
