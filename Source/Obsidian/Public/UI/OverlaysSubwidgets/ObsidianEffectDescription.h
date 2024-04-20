// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetBase.h"
#include "ObsidianEffectDescription.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianEffectDescription : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Obsidian|EffectDesc")
	void InitEffectDesc(const FText& InEffectName, const FText& InEffectDesc);

	UFUNCTION(BlueprintCallable, Category = "Obsidian|EffectDesc")
	FVector2D GetDescriptionDesiredPosition();

public:
	UPROPERTY(EditAnywhere, Category = "Obsidian|EffectDescSetup")
	FVector2D DescPositionOffset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obsidian|EffectDescSetup")
	int32 WrapTextAtNumber = 200;
};
