// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CommonButtonBase.h"
#include "CoreMinimal.h"

#include "ObsidianButtonBase.generated.h"

/**
 * Blueprint Base Class for Common Button in Obsidian.
 */
UCLASS(Abstract, BlueprintType, Blueprintable, meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API UObsidianButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetButtonText(const FText& InText);

protected:
	void RefreshButtonText();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateButtonText(const FText& InText);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateButtonStyle();

	//~ Begin UUserWidget Interface
	virtual void NativePreConstruct() override;
	//~ End UUserWidget Interface

	//~ Begin UCommonButtonBase Interface
	virtual void UpdateInputActionWidget() override;
	virtual void OnInputMethodChanged(ECommonInputType InCurrentInputType) override;
	//~ End UCommonButtonBase Interface

private:
	UPROPERTY(EditAnywhere, Category = "Obsidian|Button", meta = (InlineEditConditionToggle))
	uint8 bOverride_ButtonText:1;

	UPROPERTY(EditAnywhere, Category = "Obsidian|Button", meta=(EditCondition="bOverride_ButtonText"))
	FText ButtonText;
};
