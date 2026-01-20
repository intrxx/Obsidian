// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "ObsidianProgressBarBase.h"
#include "ObsidianBasicHealthBar.generated.h"

class UVerticalBox;
class USizeBox;
class UOverlay;
class UProgressBar;

/**
 * Basic Health Bar meant to be used by regular enemies
 */
UCLASS()
class OBSIDIAN_API UObsidianBasicHealthBar : public UObsidianProgressBarBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UOverlay> Main_Overlay;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UProgressBar> Health_ProgressBar;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UProgressBar> EnergyShield_ProgressBar;

protected:
	float Health = 0.f;
	float MaxHealth = 0.f;
	float EnergyShield = 0.f;
	float MaxEnergyShield = 0.f;
};
