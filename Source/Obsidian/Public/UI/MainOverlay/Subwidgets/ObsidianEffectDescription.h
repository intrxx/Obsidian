// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "UI/Subwidgets/ObsidianToolTipBase.h"

#include "ObsidianEffectDescription.generated.h"

/**
 * 
 */
UCLASS(meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API UObsidianEffectDescription : public UObsidianToolTipBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Obsidian|EffectDesc")
	void InitEffectDesc(const FText& InEffectName, const FText& InEffectDesc);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obsidian|EffectDescSetup")
	int32 WrapTextAtNumber = 200;

protected:
	//~ Begin UUserWidget Interface
	virtual void NativePreConstruct() override;
	//~ End UUserWidget Interface

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> EffectName_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> EffectDesc_TextBlock;
};
