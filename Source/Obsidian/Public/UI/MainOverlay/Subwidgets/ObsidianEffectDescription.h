// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "UI/Subwidgets/ObsidianToolTipBase.h"
#include "ObsidianEffectDescription.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianEffectDescription : public UObsidianToolTipBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Obsidian|EffectDesc")
	void InitEffectDesc(const FText& InEffectName, const FText& InEffectDesc);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obsidian|EffectDescSetup")
	int32 WrapTextAtNumber = 200;

protected:
	virtual void NativePreConstruct() override;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> EffectName_TextBlock;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> EffectDesc_TextBlock;
};
