// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "ObsidianWidgetBase.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMouseEnterLeaveSignature, const bool bEnter);

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianWidgetBase : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	/** Function that sets widget controller, use this instead of setting it directly. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|UI")
	void SetWidgetController(UObject* InWidgetController);
	
public:
	/** Widget controller associated with this Widget */
	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|UI")
	TObjectPtr<UObject> WidgetController;

	FOnMouseEnterLeaveSignature OnMouseEnterLeaveDelegate;
	
protected:
	/** Function that gets called after WidgetController has been set */
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Handle Widget Controller Set", Category = "Obsidian|UI")
	void BP_HandleWidgetControllerSet();
	virtual void HandleWidgetControllerSet();

	/** Function that gets called after WidgetControllerSet functions for both cpp and BP. */
	virtual void PostHandleWidgetControllerSet();
};
