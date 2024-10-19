// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ProgressBars/ProgressGlobe/ObsidianProgressGlobeBase.h"
#include "ObsidianProgressGlobe_Mana.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianProgressGlobe_Mana : public UObsidianProgressGlobe
{
	GENERATED_BODY()

protected:
	virtual void HandleWidgetControllerSet() override;

	UFUNCTION()
	void OnManaChanged(float NewMana);
	UFUNCTION()
	void OnMaxManaChanged(float NewMaxMana);
	UFUNCTION()
	void OnSpecialResourceChanged(float NewSpecialResource);
	UFUNCTION()
	void OnMaxSpecialResourceChanged(float NewMaxSpecialResource);

private:
	float Mana = 0.0f;
	float MaxMana = 0.0f;
	float SpecialResource = 0.0f;
	float MaxSpecialResource = 0.0f;
};
