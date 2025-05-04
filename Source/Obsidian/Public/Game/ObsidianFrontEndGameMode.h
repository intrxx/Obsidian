// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ObsidianFrontEndGameMode.generated.h"

struct FGameplayTag;
class AObsidianCharacterCreationHero;
class UCommonActivatableWidget;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API AObsidianFrontEndGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AObsidianFrontEndGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Obsidian|FrontEnd")
	void HighlightCharacterWithTag(const FGameplayTag& Tag);

	UFUNCTION(BlueprintCallable, Category = "Obsidian|FrontEnd")
	void ResetHighlightForCharacterWithTag(const FGameplayTag& Tag);
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|FrontEnd")
	AObsidianCharacterCreationHero* GetCreationHeroForTag(const FGameplayTag& Tag);
	
protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|FrontEnd") 
	TSubclassOf<UCommonActivatableWidget> MainMenuWidgetClass;

private:
	void GatherCreationHeroes();

private:
	UPROPERTY()
	TArray<TObjectPtr<AObsidianCharacterCreationHero>> CreationHeroes;
};
