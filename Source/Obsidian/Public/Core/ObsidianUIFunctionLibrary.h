// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ObsidianUIFunctionLibrary.generated.h"

class UOCharacterStatusWidgetController;
class UMainOverlayWidgetController;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianUIFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "ObsidianUIFunctionLibrary|WidgetControllers")
	static UMainOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "ObsidianUIFunctionLibrary|WidgetControllers")
	static UOCharacterStatusWidgetController* GetCharacterStatusWidgetController(const UObject* WorldContextObject);
};