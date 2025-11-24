// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"

#include "ObsidianTypes/ObsidianCoreTypes.h"

#include <GameFramework/GameModeBase.h>
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
	
public:
	UPROPERTY()
	FText ObsidianPlayerName = FText();

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
	int32 TempID = 0;
};

/**
 * 
 */
UCLASS()
class OBSIDIAN_API AObsidianFrontEndGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AObsidianFrontEndGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	FObsidianHeroClassParams CreateHeroClass(const EObsidianHeroClass InClass, const FText& InName, const bool InIsOnline,
		const bool InIsHardcore);
	bool DeleteHeroClass(const int32 WithID);

protected:
	virtual void BeginPlay() override;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|FrontEnd") 
	TSubclassOf<UCommonActivatableWidget> MainMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|CharacterCreation")
	TMap<EObsidianHeroClass, TSoftClassPtr<AObsidianHero>> ClassToSoftClassMap;

	UPROPERTY()
	TArray<FObsidianHeroClassParams> CreatedHeroes;
};
