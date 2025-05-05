// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/GameModeBase.h"
#include "ObsidianFrontEndGameMode.generated.h"

class AObsidianHero;
struct FGameplayTag;
class AObsidianCharacterCreationHero;
class UCommonActivatableWidget;

USTRUCT()
struct FObsidianHeroClassParams
{
	GENERATED_BODY()

public:
	FObsidianHeroClassParams(){};

	void Reset();
	
public:
	UPROPERTY()
	FText ObsidianPlayerName = FText();

	UPROPERTY()
	bool bIsHardcore = false;

	UPROPERTY()
	bool bIsOnline = false;

	UPROPERTY()
	TSoftClassPtr<AObsidianHero> HeroClass = nullptr;
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

	UFUNCTION(BlueprintCallable, Category = "Obsidian|CharacterCreation")
	void HighlightCharacterWithTag(const FGameplayTag& Tag);

	UFUNCTION(BlueprintCallable, Category = "Obsidian|CharacterCreation")
	void ResetHighlightForCharacterWithTag(const FGameplayTag& Tag);
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|CharacterCreation")
	AObsidianCharacterCreationHero* GetCreationHeroForTag(const FGameplayTag& Tag);

	void AssignPlayerName(const FText& InName);
	void AssignIsHardcore(const bool InIsHardcore);
	void CreateHeroClass(const FGameplayTag& InClassTag, const bool InIsOnline);
	
	void ResetCharacterCreation();
	
protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|FrontEnd") 
	TSubclassOf<UCommonActivatableWidget> MainMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|CharacterCreation")
	TMap<FGameplayTag, TSoftClassPtr<AObsidianHero>> ClassTagToClassMap;

	UPROPERTY()
	FObsidianHeroClassParams HeroClassParams = FObsidianHeroClassParams();

private:
	void GatherCreationHeroes();

private:
	UPROPERTY()
	TArray<TObjectPtr<AObsidianCharacterCreationHero>> CreationHeroes;
};
