// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "ObsidianMainWidgetBase.h"
#include "ObsidianMainOverlayWidgetBase.generated.h"

class UButton;

/** Fired when a widget is destroyed, used to fire logic when widget was destroyed via other method than it was opened with. */
DECLARE_MULTICAST_DELEGATE(FOnWidgetDestroyedSignature);

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianMainOverlayWidgetBase : public UObsidianMainWidgetBase
{
	GENERATED_BODY()

public:
	FOnWidgetDestroyedSignature OnWidgetDestroyedDelegate;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UFUNCTION()
	void OnCloseButtonClicked();
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Close_Button;
};
