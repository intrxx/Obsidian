// Copyright 2024 out of sCope team - Michał Ogiński
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

