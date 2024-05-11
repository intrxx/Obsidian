// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetBase.h"
#include "ObsidianEffectInfoBase.generated.h"

class UCommonTextBlock;
class UObsidianEffectDescription;
class UButton;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianEffectInfoBase : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Obsidian|EffectInfo")
	void InitEffectInfo(const FText& InEffectName, const FText& InEffectDesc, UTexture2D* InEffectImage);
	
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
	UFUNCTION()
	void OnEffectHovered();

	UFUNCTION()
	void OnEffectUnHovered();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|EffectInfo")
	TSubclassOf<UObsidianEffectDescription> EffectDescWidgetClass;

	UPROPERTY()
	TObjectPtr<UObsidianEffectDescription> EffectDescWidget;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Interact_Button;
	
};
