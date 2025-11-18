// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>

#include <GameFramework/SaveGame.h>
#include "ObsidianSaveGame.generated.h"

/**
 * 
 */
USTRUCT()
struct FObsidianCharacterSaveData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	uint8 HeroLevel = 0;

	UPROPERTY()
	FText PlayerHeroName = FText();

	UPROPERTY()
	FVector CurrentLocation = FVector::ZeroVector;
};

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianSaveGame : public ULocalPlayerSaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FObsidianCharacterSaveData CharacterSaveData;
};
