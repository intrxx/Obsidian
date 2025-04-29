// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianActivatableWidget.h"
#include "ObsidianCharacterCreationScreen.generated.h"

class UCommonTextBlock;
/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianCharacterCreationScreen : public UObsidianActivatableWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Obsidian|CharacterCreation")
	void InitializeCharacterCreationScreen(const bool bIsOnline);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> TabName_TextBlock;
	
protected:
	bool bIsOnlineCharacter = false;
};
