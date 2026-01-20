// Copyright 2026 out of sCope team - intrxx
#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "Components/WidgetComponent.h"
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
	void SetDamageTextProps(const FObsidianDamageTextProps& DamageTextProps);
};

