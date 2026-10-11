// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "ObsidianTypes/ObsidianCoreTypes.h"

#include "ObsidianFrontEndGameMode.generated.h"

class AObsidianHero;
struct FGameplayTag;
class AObsidianCharacterCreationHero;
class UCommonActivatableWidget;

/**
 * 
 */
USTRUCT()
struct FObsidianHeroClassParams
{
	GENERATED_BODY()

public:
	FObsidianHeroClassParams(){};

	void Reset();
	bool IsValid() const;

	UPROPERTY()
	FString ObsidianPlayerName = FString();

	UPROPERTY()
	bool bIsHardcore = false;

	UPROPERTY()
	bool bIsOnline = false;

	UPROPERTY()
	TSoftClassPtr<AObsidianHero> HeroObjectClass = nullptr;

	// For convenience
	UPROPERTY()
	EObsidianHeroClass Class = EObsidianHeroClass::None;

	UPROPERTY()
	int32 HeroID = 0;
};

/**
 * 
 */
UCLASS(meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API AObsidianFrontEndGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AObsidianFrontEndGameMode(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	FObsidianHeroClassParams CreateHeroClass(const EObsidianHeroClass InClass, const FString& InName, const bool InIsOnline,
		const bool InIsHardcore);
	bool DeleteHeroClass(const int32 InWithID);

protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	//~ End AActor Interface

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|FrontEnd") 
	TSubclassOf<UCommonActivatableWidget> MainMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|CharacterCreation")
	TMap<EObsidianHeroClass, TSoftClassPtr<AObsidianHero>> ClassToSoftClassMap;

	UPROPERTY()
	TArray<FObsidianHeroClassParams> CreatedHeroes;
};
