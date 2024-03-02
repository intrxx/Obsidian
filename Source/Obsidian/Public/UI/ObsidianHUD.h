// Copyright 2024 Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ObsidianHUD.generated.h"

class UObsidianMainOverlay;
class UObsidianWidgetBase;
/**
 * 
 */
UCLASS()
class OBSIDIAN_API AObsidianHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	TObjectPtr<UObsidianWidgetBase> OverlayWidget;

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere, Category = "ObsidianUI|Main")
	TSubclassOf<UObsidianMainOverlay> MainOverlayWidgetClass;
};
