// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "ObsidianTypes/ObsidianCoreTypes.h"

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
	FObsidianEnemyTypeDefaultInfo GetEnemyTypeDefaultInfo(const EObsidianEnemyClass InEnemyClass);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Types Info")
	TMap<EObsidianEnemyClass, FObsidianEnemyTypeDefaultInfo> EnemyTypeDefaultInfoMap;


};
