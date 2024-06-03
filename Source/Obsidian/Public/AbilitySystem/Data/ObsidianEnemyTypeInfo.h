// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ObsidianEnemyTypeInfo.generated.h"

class UObsidianAbilitySet;

UENUM(BlueprintType)
enum class EObsidianEnemyClass : uint8
{
	EEC_Zombie UMETA(DisplayName = "Zombie"),
	EEC_Goblin UMETA(DisplayName = "Goblin"),

	EEC_MAX UMETA(DisplayName = "Default MAX")
	
};

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
UCLASS(BlueprintType, Const)
class OBSIDIAN_API UObsidianEnemyTypeInfo : public UDataAsset
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "Player Defaults")
	TMap<EObsidianEnemyClass, FObsidianEnemyTypeDefaultInfo> EnemyTypeDefaultInfoMap;

	FObsidianEnemyTypeDefaultInfo GetEnemyTypeDefaultInfo(const EObsidianEnemyClass EnemyClass);
};
