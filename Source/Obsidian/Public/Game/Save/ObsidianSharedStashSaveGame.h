// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "ObsidianTypes/ObsidianSavedTypes.h"

#include "ObsidianSharedStashSaveGame.generated.h"

/**
 * 
 */
USTRUCT()
struct FObsidianSavedStashData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FGameplayTag StashTabTag = FGameplayTag::EmptyTag;

	UPROPERTY()
	FString StashTabName = FString();
};

/**
 * 
 */
USTRUCT()
struct FObsidianSharedStashData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FObsidianSavedItem> StashedSavedItems; 

	UPROPERTY()
	TArray<FObsidianSavedStashData> StashesData;
};

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianSharedStashSaveGame : public ULocalPlayerSaveGame
{
	GENERATED_BODY()

public:
	UObsidianSharedStashSaveGame(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY()
	FObsidianSharedStashData SharedStashData = FObsidianSharedStashData();
};
