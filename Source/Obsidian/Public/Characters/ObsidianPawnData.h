// Copyright 2024 Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ObsidianPawnData.generated.h"

class UObsidianAbilitySet;
class UObsidianInputConfig;
class  UInputMappingContext;
class UOAbilityTagRelationshipMapping;

/**
 * Struct that holds InputMapping, its Priority and if the InputMapping should be registered with settings.
 */
USTRUCT()
struct FInputMappingContextAndPriority
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMapping;

	/** Higher priority input mappings will be prioritized over mappings with a lower priority. */
	UPROPERTY(EditAnywhere, Category = "Input")
	int32 Priority = 0;
	
	/** If true, then this mapping context will be registered with the settings when this game feature action is registered. */
	UPROPERTY(EditAnywhere, Category = "Input")
	bool bRegisterWithSettings = true;
};

/**
 * Non-mutable data asset that contains properties used to define a pawn.
 */
UCLASS(BlueprintType, Const, meta = (DisplayName = "Obsidian Pawn Data", ShortToolTip = "Data asset used to define a Pawn"))
class OBSIDIAN_API UObsidianPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UObsidianPawnData(const FObjectInitializer& ObjectInitializer);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Pawn")
	TSubclassOf<APawn> PawnClass;

	/** Ability sets to grant to this pawn's ability system. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Abilities")
	TArray<TObjectPtr<UObsidianAbilitySet>> AbilitySets;

	/** What mapping of ability tags to use for actions taking by this pawn. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Abilities")
	TObjectPtr<UOAbilityTagRelationshipMapping> TagRelationshipMapping;

	/** Input Mappings used by player controlled pawns map the input. */
	UPROPERTY(EditAnywhere, Category = "Obsidian|Input")
	TArray<FInputMappingContextAndPriority> DefaultMappingContext;

	/** Input configuration used by player controlled pawns to create input mappings and bind input actions. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Input")
	TObjectPtr<UObsidianInputConfig> InputConfig;
	
	
};
