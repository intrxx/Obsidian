// Copyright 2026 out of sCope team - intrxx
#pragma once

#include "Components/WidgetComponent.h"
#include "CoreMinimal.h"

#include "ObsidianDamageNumberWidgetComp.generated.h"

/**
*
 */
UCLASS()
class OBSIDIAN_API UObsidianDamageNumberWidgetComp : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetDamageTextProps(const FObsidianDamageTextProps& InDamageTextProps);
};

