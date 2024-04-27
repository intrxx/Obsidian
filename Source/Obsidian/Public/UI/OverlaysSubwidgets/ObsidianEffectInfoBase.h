// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetBase.h"
#include "ObsidianEffectInfoBase.generated.h"

class UCommonTextBlock;
class UObsidianEffectDescription;
/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianEffectInfoBase : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Obsidian|EffectInfo")
	void InitEffectInfo(const FText& InEffectName, const FText& InEffectDesc, UTexture2D* InEffectImage, bool bIsDurational, const float& Duration);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obsidian|EffectInfo")
	FVector2D EffectImageSize;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|EffectInfo")
	FText EffectDesc;
	
	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|EffectInfo")
	FText EffectName;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|EffectInfo")
	float EffectDuration;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|EffectInfo")
	bool bIsEffectDurational;

protected:
	UFUNCTION(BlueprintCallable, Category = "Obsidian|EffectInfo")
	void OnEffectHovered();

	UFUNCTION(BlueprintCallable, Category = "Obsidian|EffectInfo")
	void OnEffectUnHovered();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|EffectInfo")
	TSubclassOf<UObsidianEffectDescription> EffectDescWidgetClass;

	UPROPERTY()
	TObjectPtr<UObsidianEffectDescription> EffectDescWidget;
	
};
