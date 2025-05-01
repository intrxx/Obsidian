// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ProgressBars/ProgressGlobe/ObsidianProgressGlobeBase.h"
#include "ObsidianProgressGlobe_Mana.generated.h"

class UObsidianRadialProgressBar;
/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianProgressGlobe_Mana : public UObsidianProgressGlobe
{
	GENERATED_BODY()
	
protected:
	virtual void HandleWidgetControllerSet() override;

	void InitializeSpecialResourceVisuals();

	UFUNCTION()
	void OnManaChanged(float NewMana);
	UFUNCTION()
	void OnMaxManaChanged(float NewMaxMana);
	UFUNCTION()
	void OnSpecialResourceChanged(float NewSpecialResource);
	UFUNCTION()
	void OnMaxSpecialResourceChanged(float NewMaxSpecialResource);
	
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|ManaProgressGlobe", meta=(BindWidget))
	TObjectPtr<UProgressBar> Mana_ProgressGlobe;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|HealthProgressGlobe", meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> ManaAttributeName_TextBlock;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|HealthProgressGlobe", meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> ManaAttributeCount_TextBlock;
	
	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|HealthProgressGlobe", meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> SpecialResourceAttributeName_TextBlock;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|HealthProgressGlobe", meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> SpecialResourceAttributeCount_TextBlock;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|HealthProgressGlobe", meta=(BindWidget))
	TObjectPtr<UProgressBar> SpecialResource_ProgressGlobe;

private:
	float Mana = 0.0f;
	float MaxMana = 0.0f;
	float SpecialResource = 0.0f;
	float MaxSpecialResource = 0.0f;
};
