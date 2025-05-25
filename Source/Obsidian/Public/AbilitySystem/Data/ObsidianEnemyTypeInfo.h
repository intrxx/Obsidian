// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "ObsidianTypes/ObsidianCoreTypes.h"

#include "Engine/DataAsset.h"
#include "ObsidianEnemyTypeInfo.generated.h"

class UObsidianAbilitySet;

USTRUCT(BlueprintType)
struct FObsidianEnemyTypeDefaultInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UObsidianAbilitySet> DefaultAbilitySet;
};

/**
 * 
 */
UCLASS(BlueprintType)
class OBSIDIAN_API UObsidianEnemyTypeInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	FObsidianEnemyTypeDefaultInfo GetEnemyTypeDefaultInfo(const EObsidianEnemyClass EnemyClass);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Types Info")
	TMap<EObsidianEnemyClass, FObsidianEnemyTypeDefaultInfo> EnemyTypeDefaultInfoMap;


};
