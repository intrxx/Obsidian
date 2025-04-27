// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "ObsidianButtonBase.generated.h"

/**
 * Blueprint Base Class for Common Button in Obsidian.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class OBSIDIAN_API UObsidianButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetButtonText(const FText& InText);

protected:
	//~ Start of UUserWidget interface
	virtual void NativePreConstruct() override;
	//~ End of UUserWidget interface

	//~ Start of UCommonButtonBase interface
	virtual void UpdateInputActionWidget() override;
	virtual void OnInputMethodChanged(ECommonInputType CurrentInputType) override;
	//~ End of UCommonButtonBase interface

	void RefreshButtonText();
	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateButtonText(const FText& InText);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateButtonStyle();
	
private:
	UPROPERTY(EditAnywhere, Category = "Obsidian|Button", meta = (InlineEditConditionToggle))
	uint8 bOverride_ButtonText : 1;

	UPROPERTY(EditAnywhere, Category = "Obsidian|Button", meta=(EditCondition="bOverride_ButtonText"))
	FText ButtonText;
};
