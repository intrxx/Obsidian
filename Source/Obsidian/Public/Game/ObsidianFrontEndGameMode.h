// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ObsidianFrontEndGameMode.generated.h"

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
	
protected:
	virtual void BeginPlay() override;

	virtual void OnPostLogin(AController* NewPlayer) override;

	void TryToShowMainMenu();

protected:
	void OnLocalPlayerAdded(ULocalPlayer* NewPlayer);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian") 
	TSubclassOf<UCommonActivatableWidget> MainMenuWidgetClass;
};
