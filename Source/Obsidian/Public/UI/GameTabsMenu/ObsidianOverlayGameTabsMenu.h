// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetBase.h"
#include "ObsidianOverlayGameTabsMenu.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnTabButtonClicked);

class UObsidianGameTabButton;
class UOverlay;
/**
 *  Parent class for Gameplay Tabs Menu like Character Status, Inventory etc. which will be placed in the main Overlay.
 */
UCLASS()
class OBSIDIAN_API UObsidianOverlayGameTabsMenu : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnCharacterStatusButtonClicked();

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianGameTabButton> CharacterStatus_GameTabButton;

	FOnTabButtonClicked OnCharacterStatusButtonClickedDelegate;
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Root_Overlay;

	
};