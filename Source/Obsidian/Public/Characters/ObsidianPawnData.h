// Copyright 2024 Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ObsidianPawnData.generated.h"

class UObsidianInputConfig;
class  UInputMappingContext;

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
class OBSIDIAN_API UObsidianPawnData : public UDataAsset
{
	GENERATED_BODY()

public:
	UObsidianPawnData(const FObjectInitializer& ObjectInitializer);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn")
	TSubclassOf<APawn> PawnClass;

	UPROPERTY(EditAnywhere, Category = "Input")
	TArray<FInputMappingContextAndPriority> DefaultMappingContext;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UObsidianInputConfig> InputConfig;
	
};